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
| Component         |   Marks |
| ----------------- | ------: |
| Week 1 Assessment |      20 |
| Week 2 Assessment |      20 |
| Week 3 Assessment |      20 |
| Week 4 Assessment |      20 |
| Mini Project      |      20 |
| **Total**         | **100** |

---

## 📚 Teaching Book
A teaching textbook

📖 [Vasseur JP, Dunkels A. Interconnecting smart objects with IP. 2010.](https://www.sciencedirect.com/book/monograph/9780123751652/interconnecting-smart-objects-with-ip)

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

| Assessment Item                                      |  Marks |
| ---------------------------------------------------- | -----: |
| Quiz (GPIO, Registers, Interrupts, ADC/PWM, I²C/SPI) |      5 |
| Laboratory Completion (Labs 1–4)                     |      8 |
| Code Quality & Documentation                         |      3 |
| Demonstration & Oral Explanation                     |      4 |
| **Total**                                            | **20** |

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

  | Assessment Item                  |  Marks |
| -------------------------------- | -----: |
| Quiz (FreeRTOS Concepts)         |      5 |
| Laboratory Completion (Labs 5–8) |      8 |
| RTOS Program Design & Debugging  |      3 |
| Demonstration & Discussion       |      4 |
| **Total**                        | **20** |

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

| Assessment Item                       |  Marks |
| ------------------------------------- | -----: |
| Quiz (Wi-Fi, MQTT, BLE, LPWAN)        |      5 |
| Laboratory Completion (Labs 9–12)     |      8 |
| IoT Connectivity & Data Communication |      3 |
| Live Demonstration                    |      4 |
| **Total**                             | **20** |

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

| Assessment Item                         |  Marks |
| --------------------------------------- | -----: |
| Quiz (TinyML & Embedded Security)       |      5 |
| Laboratory Completion (Labs 13–16)      |      8 |
| AI Deployment & Security Implementation |      3 |
| Final Demonstration                     |      4 |
| **Total**                               | **20** |

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

## 🎯 Mini Project

**Smart System**

1. Smart Environment Monitor - Monitor environmental conditions and upload data via Wi-Fi/MQTT.

2. Gesture-Controlled Light - Use TinyML and IMU data to control LEDs with hand gestures.

3. Smart Door Lock - Implement BLE-based authentication and secure access control.
  
4. IoT Plant Monitor - Measure soil moisture and send watering alerts.

5. Activity Recognition Wearable - Classify user activities using TinyML and motion sensors.

6. Smart Energy Meter - Monitor power consumption and transmit usage data.

7. Secure OTA Update System - Enable encrypted and authenticated firmware updates.

8. BLE Weather Station - Broadcast sensor data to smartphones via BLE.

9. Smart Parking Sensor - Detect vehicle occupancy and report status remotely.

10. TinyML Fall Detector - Detect falls and trigger alerts using edge AI.

**Recommended Capstone Projects**
* Gesture-Controlled Light
* Activity Recognition Wearable
* Smart Environment Monitor
* Secure OTA Update System
* TinyML Fall Detector

| Criteria                      |  Marks |
| ----------------------------- | -----: |
| System Design & Innovation    |      4 |
| Hardware Integration          |      4 |
| Software Implementation       |      4 |
| IoT/AI/Security Features      |      3 |
| Demonstration & Functionality |      3 |
| Report & Presentation         |      2 |
| **Total**                     | **20** |

---

## 📌 Tools & Platforms

* ESP32-S3 Development Board
* MPU6050 Sensor
* Breadboard + LEDs + Push Buttons
* VS Code + PlatformIO
* FreeRTOS (included with ESP-IDF)
* HiveMQ MQTT Broker
* nRF Connect Mobile App
* Edge Impulse for TinyML

---

## 📖 Outcome

### Stage 1: Embedded Hardware Foundations

**(Week 1: Foundations & Hardware Interfacing)**

Students learn microcontroller fundamentals, including GPIO, interrupts, timers, analog interfaces, and serial communication, enabling them to control peripherals and acquire sensor data.

**Skills Gained:**

* Bare-metal programming
* Register-level control
* Sensor interfacing
* ADC/PWM operation
* UART, I²C, and SPI communication

⬇️

### Stage 2: Real-Time Embedded Systems

**(Week 2: RTOS & Concurrency)**

Students progress from single-loop programs to FreeRTOS-based multitasking systems, learning task scheduling, synchronization, communication, and memory management for real-time applications.

**Skills Gained:**

* Multitasking design
* Task synchronization
* Queue-based communication
* Software timers
* Resource management

⬇️

### Stage 3: Connected IoT Devices

**(Week 3: Connectivity & IoT Protocols)**

Students enable embedded devices to communicate with sensors, cloud services, and mobile apps using Wi-Fi, MQTT, BLE, and low-power networking.

**Skills Gained:**

* Wi-Fi networking
* HTTP and REST APIs
* MQTT telemetry
* BLE applications
* Low-power IoT design

⬇️

### Stage 4: Edge Intelligence & Security

**(Week 4: Edge AI & Embedded Security)**

Students collect data, deploy TinyML models on microcontrollers, and secure embedded systems using cryptography and secure boot.

**Skills Gained:**

* TinyML workflow
* Dataset collection
* Model deployment
* Edge inference
* Embedded cybersecurity

⬇️

### Final System Integration Outcome


Students can design intelligent, connected, and secure embedded systems for IoT, Industry 4.0, robotics, and cyber-physical applications.

---

## 👨‍🏫 Instructor

Komsan Kanjanasit

College of Computing, Prince of Songkla University, Thailand  

---
