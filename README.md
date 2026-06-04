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

## 🌐 Week 2: Real-Time Operating Systems (RTOS)
#### Focus: Concurrency, multitasking, and resource management.

### 🔹 Slot 5: Concurrency & Multitasking Basics

* Theory: FreeRTOS fundamentals, scheduling, preemption, and task management.
* Lab 5: Create multiple FreeRTOS tasks with different priorities and observe preemptive scheduling.

### 🔹 Slot 6: Task Synchronization: Semaphores & Mutexes

* Theory: Race conditions, critical sections, semaphores, and mutexes.
* Lab 6: Demonstrate a race condition on a shared resource and resolve it using a FreeRTOS mutex.

### 🔹 Slot 7: Inter-Task Communication via Queues

* Theory: Thread-safe data exchange using message queues.
* Lab 7: Implement a producer-consumer system where one task acquires sensor data and another processes it via a FreeRTOS queue.

### 🔹 Slot 8: Software Timers & Memory Management

* Theory: Software timers, RTOS memory management, and dynamic vs. static allocation.
* Lab 8: Use FreeRTOS timers for periodic tasks and monitor heap usage under varying workloads.
---

## 🔗 Week 3: Connectivity & IoT Protocols
#### Focus: Wireless networking, edge-to-cloud communication, and low-power operation.

### 🔹 Slot 9: Wi-Fi Networking & HTTP Clients

* Theory: Wi-Fi modes, embedded networking, and REST APIs.
* Lab 9: Connect to Wi-Fi, synchronize time via NTP, and perform HTTP GET/POST requests.

### 🔹 Slot 10: Event-Driven IoT Telemetry via MQTT

* Theory: MQTT publish/subscribe, brokers, and QoS levels.
* Lab 10: Publish sensor data in JSON to an MQTT broker and subscribe to remote control commands.

### 🔹 Slot 11: Bluetooth Low Energy (BLE) Fundamentals

* Theory: BLE topology, GAP, and GATT concepts.
* Lab 11: Create a BLE GATT server, expose sensor data, and monitor notifications using a mobile app.

### 🔹 Slot 12: Low-Power Optimization & LPWAN

* Theory: Power management, sleep modes, and LPWAN technologies.
* Lab 12: Publish sensor data, enter deep sleep, and wake via timer or GPIO interrupt for low-power operation.

---

## ⚡ Week 4: Edge AI & Embedded Security
#### Focus: Edge AI deployment and embedded system security.

### 🔹 Slot 13: Data Collection for Machine Learning

* Theory: TinyML fundamentals, feature extraction, and dataset quality.
* Lab 13: Collect and export IMU gesture data to build a machine-learning training dataset.

### 🔹 Slot 14: Model Quantization & TinyML Deployment

* Theory: Model optimization (INT8 quantization, pruning) and TensorFlow Lite for Microcontrollers.
* Lab 14: Train, optimize, and deploy a TinyML gesture-recognition model on the MCU.

### 🔹 Slot 15: Edge Inference & Local Actuation

* Theory: Real-time ML inference on MCUs, focusing on latency, execution efficiency, and memory management.
* Lab 15: Run the ML model on the MCU, process live sensor data, and control LEDs based on gesture classification results.

### 🔹 Slot 16: Embedded Cryptography & Secure Boot
* Theory: Hardware security, symmetric/asymmetric encryption, secure boot, and Root of Trust.
* Lab 16: Encrypt data with AES-256, verify firmware using SHA-256 hashes, and implement a basic secure OTA firmware update.

---

## 🔁 Learning Flow

```
Embedded Hardware
        ↓
Peripheral Interfacing
        ↓
Real-Time Multitasking (RTOS)
        ↓
Wireless IoT Connectivity
        ↓
Low-Power Edge Devices
        ↓
TinyML Model Development
        ↓
Real-Time Edge AI Inference
        ↓
Secure Intelligent Embedded Systems
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
