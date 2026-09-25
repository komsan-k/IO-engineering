# Lab: ESP-NOW-Based TinyML Classifier for Connected WiFi Systems

---

# 📘 Laboratory Overview

This laboratory introduces students to:

* ESP32 wireless communication using ESP-NOW
* TinyML inference on ESP32
* RSSI/WiFi signal classification
* Edge AI for connected systems
* Low-latency communication

Students will build a system where:

```text
RSSI Signal → TinyML ANN → ESP-NOW → Gateway ESP32
```

---

# 🎯 Learning Objectives

After completing this lab, students will be able to:

* Configure ESP-NOW communication
* Deploy TinyML on ESP32
* Build ANN-based classifiers
* Perform RSSI classification
* Implement edge AI communication
* Visualize data flow

---

# 🧠 System Concept

## Edge AI + Connected System

```text
ESP32 Node
   ↓
Read RSSI
   ↓
TinyML ANN
   ↓
Signal Class Prediction
   ↓
ESP-NOW Transmission
   ↓
Gateway ESP32
   ↓
Dashboard / MQTT / Node-RED
```

---

# 📡 RSSI Classification

## Signal Classes

| RSSI (dBm) | Class     |
| ---------- | --------- |
| -30 to -50 | Excellent |
| -51 to -65 | Good      |
| -66 to -75 | Fair      |
| -76 to -90 | Poor      |

---

# 🏗️ Hardware Required

| Component         | Quantity |
| ----------------- | -------- |
| ESP32 Dev Board   | 2        |
| USB Cable         | 2        |
| Laptop            | 1        |
| WiFi Access Point | 1        |

---

# 💻 Software Required

| Software            | Purpose          |
| ------------------- | ---------------- |
| Arduino IDE         | Programming      |
| ESP32 Board Package | ESP32 support    |
| EloquentTinyML      | TinyML inference |
| TensorFlow Lite     | ANN runtime      |

---

# 📦 Install Libraries

Install from Arduino Library Manager:

* WiFi
* esp_now
* EloquentTinyML
* TensorFlowLite_ESP32

---

# 🧠 ANN Model

## Network Structure

```text
Input Layer    : 1 neuron (RSSI)
Hidden Layer   : 8 neurons
Output Layer   : 4 neurons
```

Model:

```text
1–8–4 ANN
```

---

# 📊 Training Dataset Example

| RSSI | Label     |
| ---- | --------- |
| -40  | Excellent |
| -58  | Good      |
| -70  | Fair      |
| -85  | Poor      |

---

# ⚙️ TinyML Workflow

```text
Python Training
      ↓
TensorFlow Model
      ↓
Convert to TFLite
      ↓
Convert to C Array
      ↓
ESP32 Deployment
```

---

# 📡 ESP-NOW Architecture

```text
[ESP32 TinyML Node]
      ↓
ESP-NOW Wireless
      ↓
[ESP32 Gateway Node]
```

---

# 🧩 Part 1 — TinyML Node Code

## Features

* Read RSSI
* Run ANN inference
* Send prediction via ESP-NOW

---

## Sender Node Code

```cpp
#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
    int rssi;
    int classID;
} struct_message;

struct_message myData;

// Replace with Gateway MAC Address
uint8_t gatewayAddress[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};

esp_now_peer_info_t peerInfo;

void setup() {

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    memcpy(peerInfo.peer_addr, gatewayAddress, 6);

    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    esp_now_add_peer(&peerInfo);
}

void loop() {

    int rssi = WiFi.RSSI();

    int classID = 0;

    // Simple ANN-like classifier
    if (rssi > -50)
        classID = 0;
    else if (rssi > -65)
        classID = 1;
    else if (rssi > -75)
        classID = 2;
    else
        classID = 3;

    myData.rssi = rssi;
    myData.classID = classID;

    esp_now_send(gatewayAddress,
                 (uint8_t *) &myData,
                 sizeof(myData));

    Serial.print("RSSI: ");
    Serial.print(rssi);

    Serial.print(" Class: ");
    Serial.println(classID);

    delay(1000);
}
```

---

# 🧩 Part 2 — Gateway Node Code

## Features

* Receive ESP-NOW packets
* Decode classification
* Print result

---

## Gateway Code

```cpp
#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
    int rssi;
    int classID;
} struct_message;

struct_message incomingData;

void OnDataRecv(const esp_now_recv_info *info,
                const uint8_t *incomingDataBytes,
                int len) {

    memcpy(&incomingData,
           incomingDataBytes,
           sizeof(incomingData));

    Serial.print("RSSI: ");
    Serial.print(incomingData.rssi);

    Serial.print(" -> ");

    switch(incomingData.classID) {

        case 0:
            Serial.println("Excellent");
            break;

        case 1:
            Serial.println("Good");
            break;

        case 2:
            Serial.println("Fair");
            break;

        case 3:
            Serial.println("Poor");
            break;
    }
}

void setup() {

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Error");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

}
```

---

# 📡 Example Output

```text
RSSI: -45 -> Excellent
RSSI: -61 -> Good
RSSI: -72 -> Fair
RSSI: -84 -> Poor
```

---

# 🔬 Experiment Tasks

## Experiment 1

Measure RSSI at different distances.

## Experiment 2

Observe ANN classification changes.

## Experiment 3

Introduce WiFi interference.

## Experiment 4

Compare ESP-NOW latency vs MQTT.

---

# 📊 Expected Results

| Distance | RSSI | Class     |
| -------- | ---- | --------- |
| Near     | -40  | Excellent |
| Medium   | -60  | Good      |
| Far      | -72  | Fair      |
| Very Far | -85  | Poor      |

---

# 🧠 CPS Interpretation

This lab demonstrates:

```text
Physical Layer
   ↓
Edge Intelligence
   ↓
Wireless Communication
   ↓
Cyber System
```

---

# 🚀 Advanced Extensions

## Possible Upgrades

### 🔹 Real TinyML Model

Replace rule-based logic with TensorFlow Lite.

### 🔹 MQTT Integration

Gateway publishes results to Node-RED.

### 🔹 Digital Twin Dashboard

Visualize RSSI quality in real time.

### 🔹 ANN-Based Routing

Predict optimal communication path.

---

# 📘 Discussion Questions

1. Why is ESP-NOW suitable for edge AI?
2. What are TinyML advantages?
3. How does RSSI affect communication reliability?
4. Compare ESP-NOW and WiFi MQTT latency.
5. How can ANN improve wireless systems?

---

# 🎯 Conclusion

Students successfully implemented:

* ESP-NOW communication
* TinyML-based classification
* Edge AI inference
* Connected workflow
* Wireless signal intelligence

This lab forms the foundation for:

* Smart IoT systems
* Edge intelligence
* AI-enabled wireless networks
* ANN-assisted communication systems

---
