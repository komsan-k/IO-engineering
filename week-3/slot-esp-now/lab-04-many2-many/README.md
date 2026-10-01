# Lab 4 — ESP-NOW Many-to-Many Communication

## Objective

In this lab, multiple ESP32 boards communicate with each other using **ESP-NOW**.

Each ESP32 can:

- send data to multiple peer nodes;
- receive data from multiple peer nodes;
- identify the source of received data using a `nodeID`.

This is a simple **many-to-many** communication pattern.

---

## System Architecture

```text
ESP32 #1  <------>  ESP32 #2
   ^  \              /  ^
   |   \            /   |
   |    \          /    |
   |     \        /     |
   v      \      /      v
ESP32 #3  <------>  ESP32 #4
```

For a simple experiment, this lab uses three ESP32 nodes:

```text
Node 1
Node 2
Node 3
```

Each node sends data to the other two nodes.

---

## Important Concept

Each ESP32 node has:

- a unique `nodeID`;
- two peer MAC addresses;
- a send callback;
- a receive callback.

Each node can both transmit and receive data.

---

## Node Configuration

For three ESP32 boards:

```text
Node 1:
nodeID = 1
peer1 = MAC of Node 2
peer2 = MAC of Node 3

Node 2:
nodeID = 2
peer1 = MAC of Node 1
peer2 = MAC of Node 3

Node 3:
nodeID = 3
peer1 = MAC of Node 1
peer2 = MAC of Node 2
```

---

## Step 1 — Get Wi-Fi MAC Addresses

Before configuring the peers, upload a simple program to each ESP32:

```cpp
#include <WiFi.h>

void setup()
{
  Serial.begin(115200);

  WiFi.mode(
    WIFI_STA
  );

  Serial.print(
    "Wi-Fi MAC: "
  );

  Serial.println(
    WiFi.macAddress()
  );
}

void loop()
{
}
```

Record the MAC address of each node.

Example:

```text
Node 1 -> 24:6F:28:11:22:33
Node 2 -> 24:6F:28:44:55:66
Node 3 -> 24:6F:28:77:88:99
```

---

# Arduino Code

Use the same code on all nodes.

Only change:

```cpp
int nodeID = 1;
```

and the peer MAC addresses.

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Node ID
// --------------------------------

// Change for each ESP32
int nodeID = 1;


// --------------------------------
// Peer MAC Addresses
// --------------------------------

// Replace with real Wi-Fi MAC addresses

uint8_t peer1[] =
{
  0x24, 0x6F, 0x28,
  0x11, 0x22, 0x33
};

uint8_t peer2[] =
{
  0x24, 0x6F, 0x28,
  0x44, 0x55, 0x66
};


// --------------------------------
// Data Structure
// --------------------------------

typedef struct
{
  int nodeID;
  int value;
}
DataPacket;

DataPacket data;
DataPacket receivedData;


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
    "Received from Node "
  );

  Serial.print(
    receivedData.nodeID
  );

  Serial.print(
    " -> Value: "
  );

  Serial.println(
    receivedData.value
  );
}


// ========================================
// Add Peer
// ========================================

void addPeer(
  uint8_t *mac
)
{
  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    mac,
    6
  );

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (
    esp_now_add_peer(
      &peerInfo
    )
    == ESP_OK
  )
  {
    Serial.println(
      "Peer added"
    );
  }
  else
  {
    Serial.println(
      "Failed to add peer"
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
    "Node MAC: "
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
  // Register Callbacks
  // --------------------------------

  esp_now_register_send_cb(
    onDataSent
  );

  esp_now_register_recv_cb(
    onDataRecv
  );


  // --------------------------------
  // Add Peers
  // --------------------------------

  addPeer(
    peer1
  );

  addPeer(
    peer2
  );


  Serial.println(
    "ESP-NOW Node ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  data.nodeID =
    nodeID;

  // Simulated ADC-like value
  data.value =
    random(0, 4096);


  Serial.print(
    "Node "
  );

  Serial.print(
    nodeID
  );

  Serial.print(
    " sending value: "
  );

  Serial.println(
    data.value
  );


  // Send to Peer 1
  esp_now_send(
    peer1,
    (uint8_t *)&data,
    sizeof(data)
  );


  // Send to Peer 2
  esp_now_send(
    peer2,
    (uint8_t *)&data,
    sizeof(data)
  );


  delay(3000);
}
```

---

## Example Configuration — Node 1

```cpp
int nodeID = 1;

uint8_t peer1[] =
{
  // MAC of Node 2
};

uint8_t peer2[] =
{
  // MAC of Node 3
};
```

---

## Example Configuration — Node 2

```cpp
int nodeID = 2;

uint8_t peer1[] =
{
  // MAC of Node 1
};

uint8_t peer2[] =
{
  // MAC of Node 3
};
```

---

## Example Configuration — Node 3

```cpp
int nodeID = 3;

uint8_t peer1[] =
{
  // MAC of Node 1
};

uint8_t peer2[] =
{
  // MAC of Node 2
};
```

---

## Expected Serial Monitor

Example output on Node 1:

```text
Node MAC: 24:6F:28:11:22:33
Peer added
Peer added
ESP-NOW Node ready

Node 1 sending value: 1450
Send Status: Success
Send Status: Success

Received from Node 2 -> Value: 2780
Received from Node 3 -> Value: 920
```

Example output on Node 2:

```text
Node 2 sending value: 2100
Send Status: Success
Send Status: Success

Received from Node 1 -> Value: 1450
Received from Node 3 -> Value: 920
```

---

## Communication Flow

```text
Node 1
  | \
  |  \
  v   v
Node 2 <----> Node 3
  ^           ^
  |           |
  +-----------+
```

Each node can:

```text
Send to multiple peers
        +
Receive from multiple peers
```

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

### Add a Peer

```cpp
esp_now_add_peer(
  &peerInfo
);
```

### Send Data

```cpp
esp_now_send(
  peer1,
  (uint8_t *)&data,
  sizeof(data)
);
```

### Register Send Callback

```cpp
esp_now_register_send_cb(
  onDataSent
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

1. Record the Wi-Fi MAC address of each ESP32.
2. Configure `nodeID` for each node.
3. Configure the two peer MAC addresses for each node.
4. Upload the program to Node 1.
5. Upload the program to Node 2.
6. Upload the program to Node 3.
7. Open the Serial Monitor for each node.
8. Observe data being sent and received between all nodes.

---

## Experiment 1 — Different Simulated Ranges

Assign different simulated data ranges.

For Node 1:

```cpp
data.value =
  random(0, 1000);
```

For Node 2:

```cpp
data.value =
  random(1000, 2000);
```

For Node 3:

```cpp
data.value =
  random(2000, 3000);
```

This makes it easier to identify the source of each value.

---

## Experiment 2 — Use Real Sensors

Replace:

```cpp
data.value =
  random(0, 4096);
```

with:

```cpp
data.value =
  analogRead(
    SENSOR_PIN
  );
```

Each node can represent a different sensor device.

---

## Experiment 3 — Add a Fourth Node

Extend the network:

```text
Node 1 <----> Node 2
  ^             ^
  |             |
  v             v
Node 3 <----> Node 4
```

Each node will need the MAC addresses of the peers it should communicate with.

---

## Checkpoint Questions

1. What does many-to-many communication mean?
2. Can each ESP32 act as both sender and receiver?
3. Why does each node need a unique `nodeID`?
4. Why are peer MAC addresses required?
5. What function initializes ESP-NOW?
6. What function adds a peer?
7. What function sends data?
8. What callback receives data?
9. What callback reports send status?
10. Is this a full routing mesh network?

---

## Important Note

This example is a **mesh-like many-to-many communication pattern**, but it is not a full routing mesh.

The peers are manually configured using MAC addresses.

```text
Many-to-Many ESP-NOW
=
Direct Peer Communication
```

A full mesh network would additionally require routing, forwarding, path selection, and dynamic topology management.

---

## Simple Assignment

Create a three-node ESP-NOW network where:

```text
Node 1 -> Simulated Light Value
Node 2 -> Simulated Temperature Value
Node 3 -> Simulated Humidity Value
```

Each node should:

- send its own value to the other two nodes;
- receive data from the other two nodes;
- display the sender `nodeID`;
- display the received value.

Target behavior:

```text
Node 1
  |
  +---- Send Light Data
  |
  +---- Receive Temperature
  |
  +---- Receive Humidity
```
