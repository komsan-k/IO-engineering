# Lab 3 — ESP-NOW Many-to-One Communication

## Objective

In this lab, multiple ESP32 sender nodes transmit data to a single ESP32 receiver using **ESP-NOW**.

The system uses:

- **ESP32 #1** as Sender Node 1
- **ESP32 #2** as Sender Node 2
- **ESP32 #3** as Sender Node 3
- **ESP32 #4** as the Receiver

Each sender includes a unique `nodeID` so the receiver can identify the source of each message.

---

## System Architecture

```text
ESP32 #1 Sender
Node ID = 1
        \
         \
ESP32 #2 Sender ----> ESP32 #4 Receiver
Node ID = 2              |
         /               v
        /          Serial Monitor
ESP32 #3 Sender
Node ID = 3
```

---

## Important Concept

ESP-NOW uses the ESP32 Wi-Fi radio for direct wireless communication.

All sender nodes must know the **Wi-Fi MAC address** of the receiver.

First, upload the receiver code and read its MAC address:

```cpp
Serial.println(
  WiFi.macAddress()
);
```

Example receiver MAC address:

```text
24:6F:28:12:34:56
```

Convert it to:

```cpp
uint8_t receiverMac[] =
{
  0x24, 0x6F, 0x28,
  0x12, 0x34, 0x56
};
```

Use the same receiver MAC address in all sender programs.

---

# Part A — ESP32 Sender Code

Use this same sender code on all sender ESP32 boards.

Only change:

```cpp
int nodeID = 1;
```

to the correct node number.

For example:

```text
ESP32 #1 -> nodeID = 1
ESP32 #2 -> nodeID = 2
ESP32 #3 -> nodeID = 3
```

## Arduino Code

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Receiver MAC Address
// --------------------------------

uint8_t receiverMac[] =
{
  0x24, 0x6F, 0x28,
  0x12, 0x34, 0x56
};

// --------------------------------
// Node ID
// --------------------------------

// Change this for each sender
int nodeID = 1;


// --------------------------------
// Data Structure
// --------------------------------

typedef struct
{
  int nodeID;
  int sensorValue;
}
DataPacket;

DataPacket data;


// ========================================
// Send Callback
// ========================================

void onDataSent(
  const uint8_t *macAddr,
  esp_now_send_status_t status
)
{
  Serial.print(
    "Send Status: "
  );

  if (
    status == ESP_NOW_SEND_SUCCESS
  )
  {
    Serial.println(
      "Success"
    );
  }
  else
  {
    Serial.println(
      "Failed"
    );
  }
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  WiFi.mode(
    WIFI_STA
  );

  Serial.print(
    "Sender MAC: "
  );

  Serial.println(
    WiFi.macAddress()
  );


  // --------------------------------
  // Initialize ESP-NOW
  // --------------------------------

  if (
    esp_now_init()
    != ESP_OK
  )
  {
    Serial.println(
      "ESP-NOW initialization failed"
    );

    return;
  }


  // --------------------------------
  // Register Send Callback
  // --------------------------------

  esp_now_register_send_cb(
    onDataSent
  );


  // --------------------------------
  // Add Receiver as Peer
  // --------------------------------

  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    receiverMac,
    6
  );

  peerInfo.channel = 0;
  peerInfo.encrypt = false;


  if (
    esp_now_add_peer(
      &peerInfo
    )
    != ESP_OK
  )
  {
    Serial.println(
      "Failed to add receiver"
    );

    return;
  }


  Serial.println(
    "ESP-NOW Sender ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  data.nodeID =
    nodeID;

  // Simulated 12-bit ADC-like value
  data.sensorValue =
    random(0, 4096);


  Serial.print(
    "Node "
  );

  Serial.print(
    data.nodeID
  );

  Serial.print(
    " sending: "
  );

  Serial.println(
    data.sensorValue
  );


  esp_now_send(
    receiverMac,
    (uint8_t *)&data,
    sizeof(data)
  );


  delay(3000);
}
```

---

## Expected Serial Monitor — Sender

Example for Node 1:

```text
Sender MAC: 24:6F:28:AA:BB:01
ESP-NOW Sender ready

Node 1 sending: 1450
Send Status: Success

Node 1 sending: 2870
Send Status: Success
```

Example for Node 2:

```text
Node 2 sending: 2510
Send Status: Success
```

Example for Node 3:

```text
Node 3 sending: 920
Send Status: Success
```

---

# Part B — ESP32 Receiver Code

Upload this program to the single receiver ESP32.

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Data Structure
// --------------------------------

typedef struct
{
  int nodeID;
  int sensorValue;
}
DataPacket;

DataPacket receivedData;


// ========================================
// Receive Callback
// ========================================

void onDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *incomingData,
  int len
)
{
  memcpy(
    &receivedData,
    incomingData,
    sizeof(receivedData)
  );


  Serial.print(
    "Node "
  );

  Serial.print(
    receivedData.nodeID
  );

  Serial.print(
    " -> Sensor Value: "
  );

  Serial.println(
    receivedData.sensorValue
  );
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  WiFi.mode(
    WIFI_STA
  );


  Serial.print(
    "Receiver MAC: "
  );

  Serial.println(
    WiFi.macAddress()
  );


  // --------------------------------
  // Initialize ESP-NOW
  // --------------------------------

  if (
    esp_now_init()
    != ESP_OK
  )
  {
    Serial.println(
      "ESP-NOW initialization failed"
    );

    return;
  }


  // --------------------------------
  // Register Receive Callback
  // --------------------------------

  esp_now_register_recv_cb(
    onDataRecv
  );


  Serial.println(
    "ESP-NOW Receiver ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
}
```

---

## Expected Serial Monitor — Receiver

```text
Receiver MAC: 24:6F:28:12:34:56
ESP-NOW Receiver ready

Node 1 -> Sensor Value: 1450
Node 2 -> Sensor Value: 2780
Node 3 -> Sensor Value: 920
Node 1 -> Sensor Value: 2015
Node 2 -> Sensor Value: 3320
```

---

## Communication Flow

```text
Sender Node 1
ID = 1
      \
       \
Sender Node 2 -----> Receiver
ID = 2                |
       /              v
      /          Serial Monitor
Sender Node 3
ID = 3
```

Each sender uses the same receiver MAC address.

The `nodeID` field identifies the source of the data.

---

## Important Commands

### Set Wi-Fi Station Mode

```cpp
WiFi.mode(
  WIFI_STA
);
```

### Initialize ESP-NOW

```cpp
esp_now_init();
```

### Add the Receiver as a Peer

```cpp
esp_now_add_peer(
  &peerInfo
);
```

### Send Data

```cpp
esp_now_send(
  receiverMac,
  (uint8_t *)&data,
  sizeof(data)
);
```

### Register Receive Callback

```cpp
esp_now_register_recv_cb(
  onDataRecv
);
```

---

## How to Run

1. Upload the receiver program to ESP32 #4.
2. Open its Serial Monitor.
3. Record the receiver Wi-Fi MAC address.
4. Put the same receiver MAC address into each sender program.
5. Set `nodeID = 1` on ESP32 #1.
6. Set `nodeID = 2` on ESP32 #2.
7. Set `nodeID = 3` on ESP32 #3.
8. Upload the sender program to all three sender boards.
9. Open the receiver Serial Monitor.
10. Observe data arriving from all sender nodes.

---

## Experiment 1 — Different Data from Each Node

Assign a different simulated range to each node.

Example:

```cpp
if (nodeID == 1)
{
  data.sensorValue =
    random(0, 1000);
}

else if (nodeID == 2)
{
  data.sensorValue =
    random(1000, 2000);
}

else if (nodeID == 3)
{
  data.sensorValue =
    random(2000, 3000);
}
```

This makes it easy to distinguish data sources.

---

## Experiment 2 — Replace Simulated Data with Real Sensors

Replace:

```cpp
data.sensorValue =
  random(0, 4096);
```

with:

```cpp
data.sensorValue =
  analogRead(
    SENSOR_PIN
  );
```

Each sender can then represent a different physical sensor node.

---

## Checkpoint Questions

1. What does many-to-one communication mean?
2. How many sender nodes are used in this lab?
3. How many receiver nodes are used?
4. Why must all senders know the receiver MAC address?
5. Why is `nodeID` required?
6. What function initializes ESP-NOW?
7. What function sends data?
8. What function receives data?
9. Can all senders use the same receiver MAC address?
10. How could this architecture be used in a wireless sensor network?

---

## Simple Assignment

Modify the system so three ESP32 sensor nodes send different types of data to one receiver.

Example:

```text
Node 1 -> Light Sensor
Node 2 -> Temperature Sensor
Node 3 -> Humidity Sensor
```

Target architecture:

```text
Light Node
    \
     \
Temperature Node ----> ESP32 Receiver
     /                     |
    /                      v
Humidity Node         Serial Monitor
```

The receiver should identify both:

- the node ID;
- the received sensor value.
