# Advanced Embedded Systems Lab 01

## Introduction to Arduino WiFi

**Course:** Advanced Embedded Systems Lab
**University:** Hochschule Hamm-Lippstadt
**Semester:** Summer Term 2025
**Lab Date:** 05.05.2025
**Professor:** Ali Hayek

---

# Overview

This laboratory exercise introduces the basics of the Arduino ecosystem with a focus on the **Arduino Uno WiFi Rev2** board. The lab covers installation and configuration of the Arduino development environment, exploration of the WiFiNINA library, and implementation of wireless communication using an embedded web server.

The final objective of this lab is to wirelessly control the onboard LED of the Arduino Uno WiFi Rev2 using a web interface.

---

# Objectives

After completing this lab, students should be able to:

* Understand the Arduino development ecosystem
* Configure and program the Arduino Uno WiFi Rev2
* Install and use the WiFiNINA library
* Establish WiFi communication with the Arduino board
* Create a simple HTTP web server
* Control hardware peripherals wirelessly

---

# Team Organization

* Each team organizes all labs and projects in a single GitHub repository.
* Teams must upload all documentation and source code to GitHub.
* Hardware provided by the university remains the responsibility of the team.
* All hardware must be returned in proper working condition after the final presentation.

---

# Introduction to Arduino

Arduino is an open-source electronics platform designed for embedded systems prototyping and development.

## Features of Arduino

* Easy-to-use development environment
* Inexpensive hardware platform
* Large online community and documentation
* Portable C/C++ programming environment
* Expandable using shields and modules
* Suitable for rapid prototyping

---

# Arduino Uno WiFi Rev2

The Arduino Uno WiFi Rev2 extends the standard Arduino Uno by integrating WiFi capabilities.

## Main Features

* Microcontroller: ATmega4809
* Integrated ESP32 u-blox NINA-W13 WiFi Module
* 14 Digital I/O Pins
* 6 Analog Inputs
* 5 PWM Outputs
* Built-in IMU (Inertial Measurement Unit)
* USB Programming Interface

---

# Comparison: Arduino Uno R3 vs Uno WiFi Rev2

| Feature         | Arduino Uno R3 | Arduino Uno WiFi Rev2 |
| --------------- | -------------- | --------------------- |
| Microcontroller | ATmega328P     | ATmega4809            |
| Clock Frequency | 16 MHz         | 16 MHz                |
| Flash Memory    | 32 KB          | 48 KB                 |
| SRAM            | 2 KB           | 6 KB                  |
| EEPROM          | 1 KB           | 256 KB                |
| WiFi            | No             | Yes                   |

---

# Required Software

## Arduino IDE

Download and install the latest Arduino IDE:

[Arduino IDE Download](https://www.arduino.cc/en/software?utm_source=chatgpt.com)

---

# Board Configuration

After installing the Arduino IDE:

1. Open Arduino IDE
2. Navigate to:

```text
Tools → Board → Board Manager
```

3. Install:

```text
Arduino megaAVR Boards
```

4. Select the board:

```text
Arduino Uno WiFi Rev2
```

---

# Required Libraries

## WiFiNINA Library

Install the WiFiNINA library:

```text
Tools → Manage Libraries → Search "WiFiNINA"
```

Install:

```text
WiFiNINA by Arduino
```

---

# Documentation and References

## Official Arduino Documentation

* [Arduino Uno WiFi Rev2 Guide](https://www.arduino.cc/en/Guide/ArduinoUnoWiFiRev2?utm_source=chatgpt.com)
* [WiFiNINA Library Reference](https://www.arduino.cc/reference/en/libraries/wifinina/?utm_source=chatgpt.com)
* [WiFiNINA Examples and Tutorials](https://docs.arduino.cc/tutorials/communication/wifi-nina-examples/?utm_source=chatgpt.com)

---

# Datasheets

* [ATmega4808/4809 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf?utm_source=chatgpt.com)
* [NINA-W10/W13 Datasheet](https://content.arduino.cc/assets/Arduino_NINA-W10_DataSheet_%28UBX-17065507%29.pdf?utm_source=chatgpt.com)

---

# First Task: Blink Example

The first programming task is to upload the classic Blink example to verify that the board is functioning correctly.

## Steps

1. Open Arduino IDE
2. Navigate to:

```text
File → Examples → 01.Basics → Blink
```

3. Select the correct COM port
4. Upload the sketch to the Arduino board

---

# Lab Tasks

## Task 1 — Install WiFiNINA Library

Install the WiFiNINA library through the Arduino Library Manager.

---

## Task 2 — Understand the WiFiNINA Library

Study the WiFiNINA documentation and examples to understand:

* WiFi initialization
* Network connection
* IP addressing
* HTTP communication
* Web server implementation

---

## Task 3 — Create WiFi Communication

Establish a wireless connection between the Arduino Uno WiFi Rev2 and a local WiFi network.

Expected functionality:

* Connect to a WiFi network
* Print IP address to Serial Monitor
* Verify successful communication

---

## Task 4 — Wireless LED Control Using Web Server

Create a web server hosted on the Arduino board to control the onboard LED (GPIO 25).

### Functional Requirements

* Start an HTTP server
* Access the server through a browser
* Turn LED ON
* Turn LED OFF
* Display current LED status

---

# Example Workflow

1. Arduino connects to WiFi
2. Arduino starts web server
3. User enters Arduino IP address in browser
4. Browser sends HTTP request
5. Arduino processes request
6. LED state changes

---

# Expected Learning Outcomes

By the end of this lab, students should understand:

* Embedded networking basics
* HTTP communication in embedded systems
* Wireless hardware control
* Arduino WiFi architecture
* Client-server interaction in IoT systems

---

# Repository Structure

```text
Lab01/
│
├── README.md
├── src/
│   └── main.ino
├── documentation/
│   └── screenshots/
└── report/
    └── Lab01_Report.pdf
```

---

# Notes

* The Arduino Uno WiFi Rev2 is not fully compatible with Arduino Uno R3 sketches due to architectural differences.
* Existing projects may require modifications before running on the Uno WiFi Rev2.
* Ensure correct board selection before uploading sketches.

---

# Conclusion

This lab introduces the fundamental concepts required for IoT-based embedded systems development using Arduino. Through WiFi communication and web server implementation, students gain practical experience in wireless embedded applications and hardware-software integration.

---

# Authors

* Team Members: *Add Names Here*
* Course: Advanced Embedded Systems Lab
* Supervisor: Prof. Dr.-Ing. Ali Hayek
