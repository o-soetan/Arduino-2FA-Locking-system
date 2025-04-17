# Arduino 2FA Locking System
# 🔐 Arduino 2FA Locking System (Wi-Fi + RFID)
![Arduino](https://img.shields.io/badge/Made%20with-Arduino-blue)
![WiFi](https://img.shields.io/badge/Connectivity-WiFi-lightblue)
![Status](https://img.shields.io/badge/Status-Working-brightgreen)


A Two-Factor Authentication (2FA) locking system built with an **Arduino Uno R4 WiFi**. This system requires both an RFID tag **and** a correct PIN (entered via a web interface over Wi-Fi) to unlock a servo-controlled mechanism. Designed for extra security and learning hands-on embedded systems!

## 📦 Features

- ✅ Two-Factor Authentication using:
  - RFID tag (RC522)
  - Web-based PIN entry (via onboard Wi-Fi)
- 📟 LCD screen displays system status and prompts
- 🔒 Servo motor controls physical locking mechanism
- 🌐 Hosted locally on Arduino via Wi-Fi server
- 🔌 Power efficient and fully standalone (it is powered by a breadboard power module)

## 🛠️ Components Used

| Component           | Model / Specs         |
|---------------------|------------------------|
| Arduino Board       | Arduino Uno R4 WiFi   |
| RFID Module         | RC522 RFID Reader     |
| LCD Display         | 16x2 I2C LCD          |
| Servo Motor         | SG90 Micro Servo      |
| Misc.               | Breadboard, jumpers, resistor, etc. |

## ⚙️ How It Works
1. **Scans their RFID tag** on the reader.
2. If an unregistered or incorrect RFID tag is scanned, the system halts and denies access to the next step.
3. **User accesses** the web interface served by the Arduino.
4. **Enters a 4-digit PIN** via the webpage.
5. If both factors are correct, the **servo unlocks the system** and LCD shows confirmation.
6. If any factor fails, access is denied and the LCD displays an error.

> This replaces the traditional keypad entry method due to hardware failure, offering a smoother and more modern PIN entry process via Wi-Fi.

## 📸 Screenshots / Demo

*Add screenshots or a GIF of your project in action here (e.g., RFID scan → PIN entry → unlock)*

> Tip: A short video or GIF demonstrating the flow greatly improves clarity and interest.

## 🧠 Challenges Faced

- The keypad module failed during testing, so I switched to using the built-in Wi-Fi to serve a web interface for PIN entry.
- Learning to set up a lightweight web server on the Arduino R4 WiFi was tricky but rewarding.
- Integrating the LCD, RFID, Wi-Fi server, and servo motor to work together reliably was more complex than expected.

---

## 📁 Project Structure

*Include a wiring diagram (Tinkercad or Fritzing) here to show circuit connections.*


## 🚀 Future Improvements

- Find and incorporate a keyPad that works
- Add buzzer for alerts
- Log access attempts with timestamps (possibly using an SD card or serial monitor)
- Encrypt PIN transmission between browser and board
- Develop a more sophisticated web interface that allows users to change the password, view system status, and perform additional actions
- Possibly design a custom PCB for a cleaner build








