# 📘 Module 977-331 : Intelligence Object Engineering 
## Part-2: Intelligent Connected & Embedded Systems - Course Plan

# 🗂 Course Overview

This 4-week intensive course covers:

* Foundations & Hardware Interfacing
* Real-Time Operating Systems (RTOS)
* Connectivity & IoT Protocols
* Edge AI & Embedded Security

Each week consists of **4 time slots**, each **3 hours**.

---

## 🗓️ Week 1: Foundations & Hardware Interfacing

### 🔹 Slot 1: GPIO, Registers & Bare-Metal Control
#### Focus: Bare-metal programming, peripherals, and communication protocols.

* Theory: MCU architecture, memory mapping, register-level programming, and HAL basics.
* Lab 1: Set up the development environment and create a bare-metal program to blink an LED and read a push button.

### 🔹 Slot 2: Hardware Interrupts & Timers

* Theory: Polling vs. interrupts, ISRs, debouncing, and hardware timers.
* Lab 2: Implement a button interrupt, debounce the input, and use a timer to blink an LED at a fixed frequency.


### 🔹 Slot 3: Analog Interfacing & PWM

* Theory: ADC operation, resolution, sampling, and PWM duty-cycle control.
* Lab 3: Read sensor values via ADC and use PWM to control LED brightness or a servo motor.

### 🔹 Slot 4: Serial Protocols (I²C & SPI)

* Theory: Synchronous communication, master-slave operation, addressing, and timing.
* Lab 4: Interface an I²C sensor, read register data, and display formatted results via UART serial output.
---

## 🌐 Week 2: Connected Systems (Communication Layer)

### 🔹 Slot 5: Connected Systems – Technology

* IoT architecture
* Cloud communication 

### 🔹 Slot 6: Connected Systems – HTTP

* REST API
* Client-server model

### 🔹 Slot 7: Connected Systems – WiFi

* TCP/IP basics
* ESP32 WiFi integration

### 🔹 Slot 8: Connected Systems – Bluetooth

* BLE fundamentals
* Device communication

---

## 🔗 Week 3: Advanced Connectivity & Integration

### 🔹 Slot 9: Device-to-Device (ESP-NOW) 

* D2D communication basics
* Non-IP communication introduction

### 🔹 Slot 10: Mesh Network (ESP-Mesh)

* WiFI Mesh architecture
* Multi-Hop Network

### 🔹 Slot 11: WiFi–Bluetooth Cooperation 

* Hybrid communication models
* System design

### 🔹 Slot 12: ANN for Connected WiFi

* ANN Structure
* RSSI Signal Classifier

---

## ⚡ Week 4: FPGA & High-Level Synthesis (HLS)

### 🔹 Slot 13: FPGA – HLS 1

* FPGA fundamentals
* HLS workflow (C → HDL)

### 🔹 Slot 14: FPGA – HLS 2

* Loop optimization
* Pipelining

### 🔹 Slot 15: Edge Inference & Local Actuation

* Theory: Real-time ML inference on MCUs, focusing on latency, execution efficiency, and memory management.
* Lab 15: Run the ML model on the MCU, process live sensor data, and control LEDs based on gesture classification results.

### 🔹 Slot 16: Embedded Cryptography & Secure Boot
* Theory: Hardware security, symmetric/asymmetric encryption, secure boot, and Root of Trust.
* Lab 16: Encrypt data with AES-256, verify firmware using SHA-256 hashes, and implement a basic secure OTA firmware update.

---

## 🔁 Learning Flow

```
Embedded → Connectivity → ML Integration → FPGA Acceleration
```

---

## 🧪 Labs

| Week   | Lab                              |
| ------ | -------------------------------- |
| Week 1 | ESP32 + Sensor Interface         |
| Week 2 | HTTP + WiFi Dashboard (Node-RED) |
| Week 3 | WiFi + TinyML Classifier             |
| Week 4 | FPGA HLS            |
| Week 5 | Mini Project            |

---

## 🎯 Mini Project

**Smart System**

* ESP32 Sensor Node
* MQTT Communication
* visualization and analyticals 
* FPGA Acceleration

---

## 📌 Tools & Platforms

* ESP32 (Arduino / ESP-IDF)
* Node-RED + MQTT (Mosquitto)
* Vivado HLS / Vitis HLS
* Python (AI integration)

---

## 📖 Outcome

Students will be able to:

* Design embedded systems
* Build IoT architectures
* Implement real-time communication
* Accelerate systems using FPGA

---

## 👨‍🏫 Instructor

Komsan Kanjanasit

College of Computing, Prince of Songkla University, Thailand  

---
