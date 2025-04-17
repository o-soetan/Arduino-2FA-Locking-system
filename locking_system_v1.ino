// v1 uses RFID module and the Arduino UNO R4 Wifi for Pin input
// The lines of code that involve serial print are meant for debugging
#include <SPI.h>
#include <MFRC522.h>

// Pin Definitions
#define RST_PIN 9       // Reset pin for MFRC522
#define SS_PIN 10       // Slave Select pin for MFRC522
#define GREEN_LED 2     // Green LED pin
#define RED_LED 3       // Red LED pin

// Create an MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

//----------------------------------------------------------------------------------
// Predefined authorized RFID tag (replace with your actual RFID tag UID)
byte authorizedUID[] ={0xD4, 0xA9, 0xFD, 0x3};
//SHOULD LOOK LIKE: byte authorizedUID[] = {0x14, 0x26, 0x78, 0x45};
//----------------------------------------------------------------------------------
/*********************************************************************************************************/
//LCD Initializations
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//Size of the LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*********************************************************************************************************/
//TO save the passwords in ROM
#include <EEPROM.h>

#define EEPROM_FLAG_ADDR  0  // Address to store the flag
#define EEPROM_PASS_ADDR  1  // Address where the password starts
#define DEFAULT_PASSWORD  "0000" 

void savePasswordToEEPROM(char* password) {
    for (int i = 0; i < 4; i++) { 
        EEPROM.write(EEPROM_PASS_ADDR + i, password[i]);
    }
    EEPROM.write(EEPROM_PASS_ADDR + 4, '\0'); 
}

void loadPasswordFromEEPROM(char* password) {
    for (int i = 0; i < 5; i++) {
        password[i] = EEPROM.read(EEPROM_PASS_ADDR + i);
    }
}

void initializeEEPROM() {
    if (EEPROM.read(EEPROM_FLAG_ADDR) != 0xAA) {
        savePasswordToEEPROM(DEFAULT_PASSWORD); // Save default password
        EEPROM.write(EEPROM_FLAG_ADDR, 0xAA); 
    }
}

/*********************************************************************************************************/
//initializations to use Ardiono WiFi
#include <WiFiS3.h>

//WiFi Access Point Credentials
const char* ssid = "ArduinoWiFi";       
const char* password = "12345678";      

WiFiServer server(80);                   // Create server on port 80

/*********************************************************************************************************/
//initializations to Servo
#include <Servo.h>
Servo myServo;

//Default Password Index
const int MAX = 4;

#define resetPin 13 

// Function to reset Arduino
void(* resetFunc) (void) = 0; 


void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  while (!Serial); // Wait for the Serial Monitor to open
  
  // Initialize SPI and MFRC522
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("MFRC522 Initialized");  //Debug Code

  lcd.init();
  lcd.backlight();

//Linking Servo to Pin
  myServo.attach(6);        

//Start Wi-Fi access point
  WiFi.beginAP(ssid, password);
  Serial.println("WiFi Access Point Started");
  Serial.print("Connect to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  // Set LED pins as OUTPUT
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Turn off LEDs initially
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // EEPROM
  initializeEEPROM();
  Serial.println("EEPROM has been Initialized."); 
    
  // ask for an RFID key.
  Serial.println("Place an RFID tag near the reader.");
  //Preset Instructions
  lcd.setCursor(0, 0);
  lcd.print("Scan RFID");

}

void loop() {

  myServo.write(90);  // Move to position 90

  // Check if a new RFID card is present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Compare the scanned UID with the authorized UID
  if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
    // Authorized RFID tag detected
    // Debug Code
    Serial.print("RFID UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      Serial.print(" ");
       
    }
    Serial.println();

    if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) 
    {
      Serial.println("Correct RFID");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Use WiFi. . .");

      //Load stored password
      char setPassword[5];
      loadPasswordFromEEPROM(setPassword);
      Serial.print("Stored Password: ");
      Serial.println(setPassword);

      int passwordAttempt = 0;
      while (passwordAttempt < 1000)
      {
        IPAddress ip = WiFi.localIP();       // Get the IP address
        String ipStr = ip.toString();        // Convert to string
        lcd.setCursor(0, 1);
        lcd.print(ipStr);                    // Display on LCD

        WiFiClient client = server.available();   // Wait for client to connect
        if (client) {
          String request = client.readStringUntil('\r');  // Read the request
          client.flush();
          if (request.indexOf("/?pin=") != -1) {
            int pinIndex = request.indexOf("/?pin=") + 6;
            String pin = request.substring(pinIndex, pinIndex + 4);  // Read 4-digit PIN

            Serial.print("PIN entered: ");
            Serial.println(pin);
            
            // HTML code for webpage
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<!DOCTYPE html><html><body>");
            client.println("<h2>Enter PIN</h2>");
            client.println("<form action='/' method='GET'>");
            client.println("<input type='password' name='pin' maxlength='4'>");
            client.println("<input type='submit' value='Submit'></form>");
            client.println("</body></html>");
            client.println("<br><a href='/reset'><button>Reset System</button></a>");
            client.println("</body></html>");
            client.stop();

            //Checking if Pin is correct
            if (pin.equals(String(setPassword))) 
            {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Access Granted");

              // Servo movement
              myServo.write(0);  // Open the lock or perform action
              delay(20000);
              myServo.write(90);   // Return the servo to the initial position

              digitalWrite(GREEN_LED, HIGH);
              delay(5000);
              digitalWrite(GREEN_LED, LOW);
              return;
            } 
            else 
            {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Att " +  String(passwordAttempt));
              lcd.setCursor(0, 1);
              lcd.print("Access Denied");
              passwordAttempt++;
              delay(1000);
            }
          }

          //For Reset Button
          if (request.indexOf("/reset") != -1) 
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("System Resetting");
            digitalWrite(GREEN_LED, HIGH);
            digitalWrite(RED_LED, HIGH);
            myServo.write(90);  // Move to position 90
            resetFunc();  
          }

        }
      }
      // If 3 failed attempts
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too Many Attempts");
      lcd.setCursor(0, 1);
      lcd.print("Reset and try again");
      delay(10000);
    }
    //-------------------------------------------------------------------------------
    Serial.println("Access Granted"); 
    //-------------------------------------------------------------------------------

  } 
  else {
    // Unauthorized RFID tag detected

    //-------------------------------------------------------------------------------
    Serial.println("Access Denied: Wrong RFID tag!"); //replace this message with a wrong ID message of your choice
    //-------------------------------------------------------------------------------
    Serial.println("Wrong RFID");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unauthorized Tag");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
    delay(2000);
  }

  // Halt PICC (Passive Communication Card)
  mfrc522.PICC_HaltA();
}


//Other Functions
// Function to compare the scanned UID with the authorized UID
bool isAuthorized(byte *scannedUID, byte length) {
  if (length != sizeof(authorizedUID)) {
    return false; // Length mismatch
  }

  for (byte i = 0; i < length; i++) {
    if (scannedUID[i] != authorizedUID[i]) {
      return false; // UID mismatch
    }
  }

  return true; // UID matches
}





