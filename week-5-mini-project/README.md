# 🛠️ Mini Project: Smart Systems Using ESP32

## ESP32 MCU, IoT, TinyML, BLE, MQTT, and Embedded Security

---

## 🧩 1. Introduction

This mini-project integrates the major concepts of **embedded systems, IoT, real-time processing, wireless communication, TinyML, and embedded security** using the ESP32 microcontroller.

Students select and develop one smart-system application that combines sensing, computation, communication, and intelligent decision-making.

The general architecture is:

```text
Physical Environment
        │
        ▼
     Sensors
        │
        ▼
┌─────────────────────┐
│        ESP32        │
│                     │
│ Data Acquisition    │
│ Signal Processing   │
│ Decision / TinyML   │
│ Communication       │
│ Security            │
└──────────┬──────────┘
           │
     ┌─────┴─────┐
     ▼           ▼
 Actuator     Network
                 │
          ┌──────┴──────┐
          ▼             ▼
      MQTT/Cloud     Smartphone
```

The project emphasizes a complete embedded intelligence pipeline:

$$
\boxed{
\text{Sense}
\rightarrow
\text{Process}
\rightarrow
\text{Decide}
\rightarrow
\text{Communicate}
\rightarrow
\text{Act}
}
$$

---

# 🎯 2. Objectives

After completing the mini-project, students should be able to:

- design an ESP32-based smart embedded system;
- interface sensors and actuators with a microcontroller;
- acquire and process real-world sensor data;
- implement Wi-Fi, MQTT, BLE, or REST communication;
- apply FreeRTOS concepts where appropriate;
- integrate TinyML inference into an embedded application;
- evaluate latency, memory, power, and communication performance;
- apply basic embedded-security principles;
- design and test an end-to-end IoT system;
- document and demonstrate a working engineering prototype.

---

# 🧠 3. Smart System Concept

A **smart system** combines sensing, computation, communication, and intelligent decision-making.

A general model is:

$$
\boxed{
\text{Smart System} =
\text{Sensing}
+
\text{Embedded Computing}
+
\text{Connectivity}
+
\text{Intelligence}
+
\text{Actuation}
}
$$

For example:

```text
Temperature Sensor
       │
       ▼
     ESP32
       │
       ├── Process Data
       ├── Detect Condition
       └── Generate Decision
               │
        ┌──────┴──────┐
        ▼             ▼
      MQTT          Actuator
        │
        ▼
   IoT Dashboard
```

---

# ⚙️ 4. Recommended Hardware and Software

| Component | Purpose |
|---|---|
| **ESP32 Development Board** | Main embedded controller |
| **Arduino IDE / ESP-IDF** | Firmware development |
| **Wi-Fi** | Internet/network connectivity |
| **BLE** | Short-range wireless communication |
| **MQTT Broker** | IoT publish/subscribe communication |
| **Node-RED / IoT Dashboard** | Data visualization and control |
| **MPU6050** | Accelerometer and gyroscope |
| **DHT11/DHT22/BME280** | Environmental sensing |
| **Soil Moisture Sensor** | Plant monitoring |
| **PIR/Ultrasonic Sensor** | Presence or occupancy detection |
| **LED/Relay/Servo** | Actuation |
| **Current/Voltage Sensor** | Energy monitoring |
| **TinyML Framework** | Edge-AI inference |

Not every project requires all components.

---

# 📋 5. Mini-Project Options

Students select **one** project from the following ten smart-system applications.

| No. | Mini Project | Main Technologies |
|:---:|---|---|
| 1 | Smart Environment Monitor | Sensors, Wi-Fi, MQTT |
| 2 | Gesture-Controlled Light | MPU6050, TinyML, LED |
| 3 | Smart Door Lock | BLE, authentication, servo |
| 4 | IoT Plant Monitor | Soil sensor, Wi-Fi, MQTT |
| 5 | Activity Recognition Wearable | IMU, TinyML |
| 6 | Smart Energy Meter | ADC, power sensing, IoT |
| 7 | Secure OTA Update System | Wi-Fi, OTA, security |
| 8 | BLE Weather Station | Environmental sensors, BLE |
| 9 | Smart Parking Sensor | Distance sensing, MQTT |
| 10 | TinyML Fall Detector | IMU, TinyML, alerts |

---

# 🌡️ 6. Project 1: Smart Environment Monitor

## 6.1 Project Description

Develop an ESP32-based environmental monitoring node that measures environmental conditions and transmits the measurements to an IoT platform using Wi-Fi and MQTT.

Possible parameters include:

- temperature;
- humidity;
- atmospheric pressure;
- light intensity;
- air-quality indicators.

### System Architecture

```text
Temperature ─┐
Humidity ────┤
Pressure ────┤
Light ───────┤
             ▼
           ESP32
             │
          Wi-Fi
             │
             ▼
        MQTT Broker
             │
             ▼
         Node-RED
             │
             ▼
        Dashboard
```

### Example MQTT Topics

```text
smart/environment/temperature
smart/environment/humidity
smart/environment/pressure
```

### Core Tasks

1. Connect environmental sensors.
2. Acquire sensor measurements.
3. Connect ESP32 to Wi-Fi.
4. Establish an MQTT connection.
5. Publish measurements periodically.
6. Visualize data on a dashboard.
7. Implement threshold-based alerts.

### Extension

Implement:

$$
T>T_{\text{threshold}}
\Rightarrow
\text{Alert}
$$

---

# 💡 7. Project 2: Gesture-Controlled Light

## 7.1 Project Description

Develop an intelligent lighting controller that recognizes hand gestures using IMU measurements and TinyML.

An MPU6050 can provide:

$$
(a_x,a_y,a_z)
$$

and:

$$
(\omega_x,\omega_y,\omega_z)
$$

### Architecture

```text
Hand Gesture
     │
     ▼
  MPU6050
     │
     ▼
   ESP32
     │
     ▼
Feature Extraction
     │
     ▼
TinyML Model
     │
     ▼
Gesture Class
     │
     ▼
LED Controller
```

Example classes:

```text
Gesture 0 → Light OFF
Gesture 1 → Light ON
Gesture 2 → Brightness UP
Gesture 3 → Brightness DOWN
```

### TinyML Pipeline

$$
\boxed{
\text{IMU}
\rightarrow
\text{Sampling}
\rightarrow
\text{Features}
\rightarrow
\text{TinyML}
\rightarrow
\text{Gesture}
\rightarrow
\text{LED}
}
$$

### Core Tasks

1. Interface MPU6050.
2. Collect gesture datasets.
3. Label the gestures.
4. Extract motion features.
5. Train a classification model.
6. Quantize the model if required.
7. Deploy the model to ESP32.
8. Perform real-time inference.
9. Control LEDs according to predictions.

---

# 🔐 8. Project 3: Smart Door Lock

## 8.1 Project Description

Develop an ESP32 smart door-lock prototype using BLE-based authentication.

### Architecture

```text
Smartphone
    │
    │ BLE
    ▼
  ESP32
    │
    ▼
Authentication
    │
 ┌──┴──┐
 ▼     ▼
Valid Invalid
 │      │
 ▼      ▼
Servo  Reject
Unlock
```

### Core Functions

- BLE device communication;
- user authentication;
- servo-based locking mechanism;
- LED status indication;
- access-event logging.

### Security Model

$$
\text{Access}
=
\begin{cases}
\text{Granted}, & C=C_{\text{authorized}}\\
\text{Denied}, & C\neq C_{\text{authorized}}
\end{cases}
$$

where $C$ represents the presented credential.

### Extension

Add:

- time-limited credentials;
- encrypted BLE communication;
- access logs;
- failed-attempt detection;
- remote notifications.

---

# 🌱 9. Project 4: IoT Plant Monitor

## 9.1 Project Description

Develop an IoT system that monitors soil moisture and environmental conditions and generates watering alerts.

### Architecture

```text
Soil Moisture ──┐
Temperature ────┤
Humidity ───────┤
                ▼
              ESP32
                │
          Decision Logic
                │
        ┌───────┴───────┐
        ▼               ▼
     MQTT Alert       Water Pump
```

### Watering Decision

Let $M$ represent soil moisture.

$$
\text{Watering Alert} =
\begin{cases}
1,&M<M_{\min}\\
0,&M\ge M_{\min}
\end{cases}
$$

### Core Tasks

- acquire soil-moisture data;
- calibrate dry and wet conditions;
- define a watering threshold;
- publish measurements using MQTT;
- display historical measurements;
- generate low-moisture alerts.

### Advanced Extension

Add automatic irrigation:

```text
Dry Soil
   │
   ▼
ESP32
   │
   ▼
Relay
   │
   ▼
Water Pump
```

---

# 🏃 10. Project 5: Activity Recognition Wearable

## 10.1 Project Description

Develop a wearable motion-recognition system that classifies physical activities using an IMU and TinyML.

Possible classes include:

$$
\{
\text{Standing},
\text{Walking},
\text{Running},
\text{Sitting}
\}
$$

### Architecture

```text
Human Motion
     │
     ▼
  MPU6050
     │
     ▼
   ESP32
     │
     ▼
Signal Window
     │
     ▼
Feature Extraction
     │
     ▼
TinyML Classifier
     │
     ▼
Activity Class
```

### Example Features

For a sampling window containing $N$ measurements:

$$
\mu_x=\frac{1}{N}\sum_{i=1}^{N}x_i
$$

$$
\sigma_x=
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}(x_i-\mu_x)^2
}
$$

The acceleration magnitude can be calculated as:

$$
a_m=\sqrt{a_x^2+a_y^2+a_z^2}
$$

### Evaluation

Use:

$$
\text{Accuracy} =
\frac{\text{Correct Predictions}}
{\text{Total Predictions}}
\times100\%
$$

Students should also report inference latency and model memory requirements.

---

# ⚡ 11. Project 6: Smart Energy Meter

## 11.1 Project Description

Develop an ESP32-based energy-monitoring prototype that measures electrical quantities and transmits usage information to an IoT system.

> Use appropriate isolated sensing modules and low-voltage laboratory setups. Students should not directly connect experimental circuits to hazardous mains voltage.

### Architecture

```text
Voltage Sensor ─┐
Current Sensor ─┤
                ▼
              ESP32
                │
                ▼
        Power Calculation
                │
                ▼
             Wi-Fi
                │
                ▼
          MQTT / Cloud
```

For a simplified DC laboratory system:

$$
P=VI
$$

Energy can be estimated as:

$$
E=\int P(t)\,dt
$$

or discretely:

$$
E\approx\sum_{k=1}^{N}P_k\Delta t
$$

### Core Tasks

- measure voltage/current safely;
- calculate power;
- accumulate energy usage;
- publish measurements;
- display real-time consumption;
- detect abnormal consumption.

---

# 🔒 12. Project 7: Secure OTA Update System

## 12.1 Project Description

Develop an ESP32 firmware-update architecture demonstrating secure Over-the-Air update principles.

### Architecture

```text
Firmware Server
      │
      ▼
Signed Firmware
      │
      ▼
TLS Connection
      │
      ▼
    ESP32
      │
      ▼
Verification
      │
   ┌──┴──┐
   ▼     ▼
 Valid  Invalid
   │      │
Install Reject
```

### Security Requirements

Students should investigate:

- HTTPS/TLS;
- firmware integrity;
- firmware authenticity;
- digital signatures;
- secure boot;
- rollback protection;
- Root of Trust.

A conceptual verification model is:

$$
V(K_{\text{pub}},F,\sigma)
=
\begin{cases}
1,&\text{Accept firmware}\\
0,&\text{Reject firmware}
\end{cases}
$$

### Important Laboratory Note

Students should avoid irreversible security provisioning or eFuse changes on shared teaching hardware unless specifically instructed and appropriate recovery procedures exist.

---

# 🌤️ 13. Project 8: BLE Weather Station

## 13.1 Project Description

Develop a portable ESP32 weather station that broadcasts environmental measurements to a smartphone using BLE.

### Architecture

```text
BME280 / DHT
      │
      ▼
    ESP32
      │
      ▼
  BLE GATT Server
      │
      ▼
   Smartphone
```

Possible BLE characteristics:

```text
Weather Service
     │
     ├── Temperature
     ├── Humidity
     └── Pressure
```

### Core Tasks

1. Configure the sensor.
2. Configure ESP32 as a BLE peripheral.
3. Create a GATT service.
4. Define sensor characteristics.
5. Advertise the device.
6. Connect using a smartphone.
7. Transmit measurements using BLE notifications.

---

# 🚗 14. Project 9: Smart Parking Sensor

## 14.1 Project Description

Develop a parking-space occupancy detector using a distance or proximity sensor.

### Architecture

```text
Vehicle
   │
   ▼
Distance Sensor
   │
   ▼
 ESP32
   │
   ▼
Occupancy Detection
   │
 ┌─┴──────────┐
 ▼            ▼
LED       MQTT Status
              │
              ▼
          Dashboard
```

Let measured distance be $d$.

$$
\text{Occupancy} =
\begin{cases}
1,&d<d_{\text{threshold}}\\
0,&d\ge d_{\text{threshold}}
\end{cases}
$$

Example messages:

```text
parking/slot1/status = OCCUPIED
parking/slot2/status = AVAILABLE
```

### Extension

Develop a multi-space system:

```text
Slot 1 ─┐
Slot 2 ─┤
Slot 3 ─┤
Slot 4 ─┤
        ▼
     Gateway
        │
        ▼
 MQTT Broker
        │
        ▼
Parking Dashboard
```

---

# 🚨 15. Project 10: TinyML Fall Detector

## 15.1 Project Description

Develop an edge-AI fall-detection system using ESP32, an IMU, and a TinyML classifier.

### Architecture

```text
Human Motion
     │
     ▼
   MPU6050
     │
     ▼
    ESP32
     │
     ▼
Motion Window
     │
     ▼
TinyML Model
     │
 ┌───┴────┐
 ▼        ▼
Normal   Fall
           │
           ▼
         Alert
```

Possible classes:

$$
C=
\{
\text{Normal},
\text{Walking},
\text{Sitting},
\text{Fall}
\}
$$

### Processing Pipeline

$$
\boxed{
\text{IMU}
\rightarrow
\text{Windowing}
\rightarrow
\text{Features}
\rightarrow
\text{TinyML}
\rightarrow
\text{Fall Decision}
}
$$

### Alert Architecture

```text
Fall Detected
      │
      ├──► Buzzer
      │
      ├──► LED
      │
      └──► Wi-Fi/MQTT
                 │
                 ▼
              Alert
```

### Evaluation

Important metrics include:

$$
\text{Precision} =
\frac{TP}{TP+FP}
$$

$$
\text{Recall} =
\frac{TP}{TP+FN}
$$

$$
F_1=
2\frac{\text{Precision}\times\text{Recall}}
{\text{Precision}+\text{Recall}}
$$

For fall detection, students should pay particular attention to **false negatives** and **false alarms**.

---

# 🧭 16. General Project Development Workflow

All projects should follow a structured engineering process.

```text
Problem Definition
       │
       ▼
System Requirements
       │
       ▼
Architecture Design
       │
       ▼
Hardware Integration
       │
       ▼
Firmware Development
       │
       ▼
Communication / TinyML
       │
       ▼
System Integration
       │
       ▼
Testing
       │
       ▼
Performance Evaluation
       │
       ▼
Final Demonstration
```

---

# 📝 17. Phase 1 — Problem Definition

Students should clearly define:

- What problem does the system solve?
- Who are the intended users?
- What physical parameters must be measured?
- What decisions must the system make?
- What communication method is required?
- What output or actuation is required?

The project should have a clearly stated objective.

Example:

> The objective is to develop an ESP32-based TinyML fall-detection system capable of classifying motion locally and transmitting an alert through MQTT when a fall is detected.

---

# 🏗️ 18. Phase 2 — System Architecture

Students should create a block diagram.

Example:

```text
Sensor
  │
  ▼
ESP32
  │
  ├── Data Acquisition
  ├── Processing
  ├── TinyML
  └── Communication
          │
          ▼
      MQTT Broker
          │
          ▼
       Dashboard
```

Clearly identify:

- inputs;
- processing;
- outputs;
- communication;
- storage;
- security mechanisms.

---

# 🔌 19. Phase 3 — Hardware Integration

Create a hardware table.

| Component | ESP32 Interface | Function |
|---|---|---|
| MPU6050 | I²C | Motion sensing |
| BME280 | I²C | Environment sensing |
| LED | GPIO/PWM | Indicator |
| Servo | PWM | Mechanical actuation |
| Soil Sensor | ADC | Moisture sensing |

Students should include a circuit diagram or wiring diagram in the final report.

---

# 💻 20. Phase 4 — Firmware Development

The firmware should be modular.

Example:

```text
setup()
   │
   ├── initializeSensor()
   ├── initializeNetwork()
   ├── initializeCommunication()
   └── initializeModel()
              │
              ▼
            loop()
              │
      ┌───────┼─────────┐
      ▼       ▼         ▼
   Sense   Process   Communicate
```

Recommended software modules include:

```text
sensor.cpp
network.cpp
mqtt.cpp
ble.cpp
tinyml.cpp
security.cpp
main.cpp
```

The exact structure depends on project complexity and development environment.

---

# 📡 21. Phase 5 — Communication

Students should select an appropriate communication technology.

| Technology | Suitable Application |
|---|---|
| Wi-Fi | Internet/cloud connectivity |
| MQTT | IoT telemetry |
| HTTP/REST | Web-service interaction |
| BLE | Smartphone/device communication |
| I²C | Local sensors |
| SPI | High-speed peripherals |
| UART | Serial modules/debugging |

Selection should be justified according to:

- range;
- bandwidth;
- energy consumption;
- topology;
- security;
- latency.

---

# 🤖 22. Phase 6 — TinyML Integration

Projects 2, 5, and 10 strongly emphasize TinyML.

A typical development process is:

```text
Data Collection
      │
      ▼
Data Labeling
      │
      ▼
Preprocessing
      │
      ▼
Feature Extraction
      │
      ▼
Model Training
      │
      ▼
Model Evaluation
      │
      ▼
INT8 Quantization
      │
      ▼
ESP32 Deployment
      │
      ▼
Real-Time Inference
```

The trained model represents:

$$
\hat{y}=f(x;\theta)
$$

where:

- $x$ = sensor input;
- $\theta$ = trained model parameters;
- $\hat{y}$ = predicted class.

---

# 📊 23. Dataset Design

For TinyML projects, dataset quality strongly affects model performance.

Students should consider:

$$
\boxed{
\text{Good Dataset} =
\text{Representative}
+
\text{Balanced}
+
\text{Clean}
+
\text{Correctly Labeled}
}
$$

A dataset table should be included.

| Class | Training Samples | Test Samples |
|---|---:|---:|
| Class 1 | | |
| Class 2 | | |
| Class 3 | | |
| Class 4 | | |

Avoid testing the model only with the same samples used during training.

---

# ⚡ 24. Real-Time Performance

Smart systems often have timing constraints.

The end-to-end response time can be modeled as:

$$
T_{\text{response}} =
T_{\text{sensing}}
+
T_{\text{processing}}
+
T_{\text{inference}}
+
T_{\text{communication}}
+
T_{\text{actuation}}
$$

Students should measure relevant components where possible.

For TinyML:

$$
T_{\text{inference}} =
t_{\text{end}}-t_{\text{start}}
$$

---

# 💾 25. Memory Evaluation

Embedded applications must operate within limited memory.

Students should report:

- program/flash usage;
- SRAM usage;
- model size;
- runtime buffers.

Conceptually:

$$
M_{\text{total}} =
M_{\text{firmware}}
+
M_{\text{model}}
+
M_{\text{buffers}}
+
M_{\text{runtime}}
$$

---

# 🔋 26. Power Considerations

For battery-operated projects, students should consider:

- Wi-Fi duty cycle;
- BLE advertising interval;
- sensor sampling frequency;
- CPU frequency;
- sleep modes.

Average current can be approximated by:

$$
I_{\text{avg}} =
\frac{
\sum I_iT_i
}{
\sum T_i
}
$$

Estimated battery life is:

$$
T_{\text{battery}}
\approx
\frac{C_{\text{battery}}}{I_{\text{avg}}}
$$

under simplified assumptions.

---

# 🔐 27. Security Requirements

Every connected smart system should include a brief security analysis.

Students should identify:

```text
Assets
   │
   ▼
Threats
   │
   ▼
Vulnerabilities
   │
   ▼
Countermeasures
```

Examples:

| Threat | Possible Countermeasure |
|---|---|
| Wi-Fi eavesdropping | TLS |
| Unauthorized BLE access | Authentication |
| MQTT impersonation | Broker authentication |
| Firmware modification | Signed firmware |
| Credential extraction | Protected storage |
| Malicious update | Secure OTA |

---

# 🧪 28. Testing Requirements

Each project should include at least three categories of testing.

### Functional Testing

Does the system perform its intended function?

### Communication Testing

Does information reach the intended receiver correctly?

### Performance Testing

Evaluate relevant metrics such as:

- latency;
- accuracy;
- packet reliability;
- memory;
- power;
- sensor error.

For TinyML projects, also evaluate the classification performance.

---

# 📊 29. Experimental Results

Students should present quantitative results where applicable.

Example:

| Metric | Result |
|---|---:|
| Sensor sampling rate | ___ Hz |
| MQTT latency | ___ ms |
| Inference latency | ___ ms |
| Classification accuracy | ___ % |
| Model size | ___ KB |
| Flash usage | ___ % |
| RAM usage | ___ % |

Results should be obtained from the implemented system rather than estimated whenever practical.

---

# 📈 30. TinyML Confusion Matrix

TinyML projects should consider presenting a confusion matrix.

Example:

| Actual \ Predicted | Walking | Sitting | Running | Fall |
|---|---:|---:|---:|---:|
| Walking | | | | |
| Sitting | | | | |
| Running | | | | |
| Fall | | | | |

The diagonal elements represent correct predictions.

---

# 🧮 31. Classification Metrics

For a classification problem:

$$
\text{Accuracy} =
\frac{TP+TN}
{TP+TN+FP+FN}
$$

$$
\text{Precision} =
\frac{TP}{TP+FP}
$$

$$
\text{Recall} =
\frac{TP}{TP+FN}
$$

$$
F_1=
2
\frac{
\text{Precision}\times\text{Recall}
}{
\text{Precision}+\text{Recall}
}
$$

Students should select metrics appropriate to their application.

---

# 📋 32. Project-Specific Minimum Requirements

| Project | Minimum Demonstration |
|---|---|
| Environment Monitor | Sensor → ESP32 → MQTT → Dashboard |
| Gesture Light | IMU → classifier → LED |
| Door Lock | BLE → authentication → lock actuator |
| Plant Monitor | Soil sensor → decision → IoT alert |
| Activity Wearable | IMU → TinyML → activity |
| Energy Meter | Measurement → calculation → IoT |
| Secure OTA | Update → authentication → firmware |
| BLE Weather | Sensor → GATT → smartphone |
| Parking Sensor | Distance → occupancy → remote status |
| Fall Detector | IMU → TinyML → alert |

---

# 📄 33. Final Report Structure

The final project report should contain:

1. **Title**
2. **Abstract**
3. **Introduction**
4. **Problem Statement**
5. **Objectives**
6. **Background Theory**
7. **System Requirements**
8. **System Architecture**
9. **Hardware Design**
10. **Software Design**
11. **Communication Architecture**
12. **TinyML Model**, if applicable
13. **Security Considerations**
14. **Experimental Setup**
15. **Results**
16. **Discussion**
17. **Limitations**
18. **Conclusion**
19. **Future Work**
20. **References**
21. **Appendix / Source Code**

---

# 🎤 34. Final Presentation

Each group should demonstrate:

```text
Problem
   ↓
Architecture
   ↓
Hardware
   ↓
Software
   ↓
Live Demonstration
   ↓
Results
   ↓
Discussion
```

Students should be prepared to explain both **how the system works** and **why particular engineering choices were made**.

---

# 🎬 35. Demonstration Requirements

The demonstration should show the complete signal flow.

For example:

```text
Real-World Event
      │
      ▼
Sensor Measurement
      │
      ▼
ESP32 Processing
      │
      ▼
Decision / TinyML
      │
      ▼
Communication
      │
      ▼
Dashboard / Actuator / Alert
```

A prerecorded backup demonstration is recommended in case of network or hardware failure during presentation.

---

# 📊 36. Suggested Assessment Rubric

| Assessment Component | Weight |
|---|---:|
| Problem definition and objectives | 10% |
| System architecture | 10% |
| Hardware implementation | 15% |
| Firmware/software implementation | 15% |
| Communication / IoT integration | 10% |
| Intelligence / TinyML / decision logic | 10% |
| Testing and experimental results | 10% |
| Security and reliability considerations | 5% |
| Final report | 5% |
| Demonstration and presentation | 10% |
| **Total** | **100%** |

For projects that do not use TinyML, the intelligence component can be evaluated through decision logic, automation, data processing, or another approved intelligent function.

---

# ⭐ 37. Bonus Features

Additional credit may be considered for well-implemented extensions such as:

- Node-RED dashboard;
- InfluxDB data logging;
- digital-twin integration;
- FreeRTOS multitasking;
- deep-sleep power management;
- encrypted MQTT;
- HTTPS;
- secure OTA;
- mobile application;
- web dashboard;
- anomaly detection;
- INT8 TinyML model;
- cloud integration;
- edge analytics.

---

# 💬 38. Discussion Questions

1. What makes an embedded system a **smart system**?
2. Why is ESP32 suitable for smart-system development?
3. When should Wi-Fi be selected instead of BLE?
4. Why is MQTT suitable for IoT telemetry?
5. What is the role of a sensor-sampling rate?
6. Why is data preprocessing important?
7. What is TinyML?
8. Why perform ML inference directly on an MCU?
9. What is inference latency?
10. How does model size affect an embedded implementation?
11. Why is INT8 quantization useful?
12. Why is dataset quality important?
13. What is a confusion matrix?
14. Why can accuracy alone be misleading?
15. What is the difference between sensing and actuation?
16. What is end-to-end latency?
17. How does Wi-Fi affect energy consumption?
18. Why is BLE suitable for wearable applications?
19. Why should connected embedded systems use authentication?
20. Why should firmware updates be authenticated?
21. What is secure OTA?
22. What is a Root of Trust?
23. How can FreeRTOS improve a smart-system architecture?
24. Why should sensor, ML, and network operations sometimes run independently?
25. How would you extend your project into a commercial product?

---

# 🧾 39. Expected Learning Outcomes

After completing the mini-project, students should be able to:

- integrate sensors with ESP32;
- acquire and process real-time data;
- design embedded decision-making algorithms;
- implement Wi-Fi and BLE communication;
- use MQTT for IoT applications;
- create basic IoT dashboards;
- implement actuator control;
- collect and analyze sensor datasets;
- deploy TinyML models where applicable;
- evaluate classification performance;
- measure inference latency;
- analyze MCU memory requirements;
- consider power-management requirements;
- identify embedded-system security threats;
- apply basic authentication and communication security;
- design modular embedded software;
- evaluate an end-to-end smart system;
- communicate engineering results through a technical report and demonstration.

---

# 🎯 40. Summary

The **Smart System Mini Project** integrates the major topics of modern MCU-based embedded-system development:

$$
\boxed{
\text{Smart System} =
\text{MCU}
+
\text{Sensors}
+
\text{Connectivity}
+
\text{Intelligence}
+
\text{Security}
}
$$

The ten proposed projects provide different application pathways:

```text
                    Smart Systems
                         │
       ┌─────────────────┼─────────────────┐
       ▼                 ▼                 ▼
      IoT             TinyML           Security
       │                 │                 │
 Environment         Gesture           Door Lock
 Plant Monitor       Activity          Secure OTA
 Parking             Fall Detection
 Energy
       │                 │                 │
       └─────────────────┼─────────────────┘
                         ▼
                       ESP32
```

Regardless of the selected project, students should demonstrate the complete engineering cycle:

$$
\boxed{
\text{Design}
\rightarrow
\text{Implement}
\rightarrow
\text{Integrate}
\rightarrow
\text{Test}
\rightarrow
\text{Evaluate}
}
$$

The goal is not simply to connect a sensor to an ESP32, but to develop a functioning **smart embedded system** in which sensing, processing, connectivity, intelligence, actuation, and security work together as an integrated application.

---

# 📘 References

1. Espressif Systems, *ESP32 Series Datasheets and ESP-IDF Programming Guide*.  
2. Espressif Systems, *ESP-IDF Programming Guide — Wi-Fi, Bluetooth, FreeRTOS, OTA, and Security*.  
3. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*, O'Reilly Media, 2019.  
4. J.-P. Vasseur and A. Dunkels, *Interconnecting Smart Objects with IP: The Next Internet*, Morgan Kaufmann, 2010.  
5. FreeRTOS, *FreeRTOS Kernel Documentation*.  
6. OASIS, *MQTT Version 5.0 Specification*.  
7. Bluetooth SIG, *Bluetooth Core Specification*.  
8. TensorFlow, *TensorFlow Lite for Microcontrollers Documentation*.  
9. Espressif Systems, *ESP-DL and ESP32 Machine-Learning Documentation*.  
10. NIST, *IoT Device Cybersecurity Guidance*.

---

