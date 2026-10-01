# Lab 5 — Simple ESP-NOW Mesh Communication

## Objective

In this lab, three ESP32 boards form a simple **mesh-like ESP-NOW network** with multi-hop forwarding.

The network uses:

- **Node 1** as the source
- **Node 2** as the relay
- **Node 3** as the destination

Node 1 sends a packet to Node 3 through Node 2.

---

## System Architecture

```text
Node 1  <---->  Node 2  <---->  Node 3
```

Node 1 cannot directly send to Node 3 in this example.

Instead:

```text
Node 1
   |
   | packet for Node 3
   v
Node 2
   |
   | forwards packet
   v
Node 3
```

This demonstrates a simple **static multi-hop mesh**.

---

## Important Concept

A many-to-many network becomes more mesh-like when it includes:

```text
Multiple Nodes
    +
Packet Forwarding
    +
Destination Address
    +
TTL
    +
Routing Logic
```

This example uses a simple static route:

```text
Node 1 -> Node 2 -> Node 3
```

---

## Node Configuration

Use the same Arduino program on all three ESP32 boards.

Change only:

```cpp
#define NODE_ID 1
```

to:

```text
Node 1 -> NODE_ID 1
Node 2 -> NODE_ID 2
Node 3 -> NODE_ID 3
```

Also replace the MAC addresses with the real Wi-Fi MAC addresses of your ESP32 boards.

---

## Get the Wi-Fi MAC Address

Use this code on each ESP32:

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

Record:

```text
Node 1 MAC
Node 2 MAC
Node 3 MAC
```

---

# Complete Arduino Code

```cpp
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ========================================
// Node Configuration
// ========================================

// Change for each ESP32
#define NODE_ID 1

#define WIFI_CHANNEL 1


// --------------------------------
// Wi-Fi MAC Addresses
// Replace with real addresses
// --------------------------------

uint8_t node1Mac[] =
{
  0x24, 0x6F, 0x28,
  0x11, 0x22, 0x33
};

uint8_t node2Mac[] =
{
  0x24, 0x6F, 0x28,
  0x44, 0x55, 0x66
};

uint8_t node3Mac[] =
{
  0x24, 0x6F, 0x28,
  0x77, 0x88, 0x99
};


// ========================================
// Packet Structure
// ========================================

typedef struct
{
  int source;
  int destination;
  int value;
  int ttl;
}
MeshPacket;


// ========================================
// Add ESP-NOW Peer
// ========================================

void addPeer(
  uint8_t *mac
)
{
  if (
    esp_now_is_peer_exist(
      mac
    )
  )
  {
    return;
  }

  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    mac,
    6
  );

  peerInfo.channel =
    WIFI_CHANNEL;

  peerInfo.encrypt =
    false;

  esp_now_add_peer(
    &peerInfo
  );
}


// ========================================
// Send Packet to Next Hop
// ========================================

void sendPacket(
  MeshPacket packet
)
{
  uint8_t *nextHop =
    nullptr;


#if NODE_ID == 1

  // Node 1 sends through Node 2
  nextHop =
    node2Mac;

#elif NODE_ID == 2

  if (
    packet.destination == 1
  )
  {
    nextHop =
      node1Mac;
  }

  else if (
    packet.destination == 3
  )
  {
    nextHop =
      node3Mac;
  }

#elif NODE_ID == 3

  // Node 3 sends through Node 2
  nextHop =
    node2Mac;

#endif


  if (
    nextHop != nullptr
  )
  {
    esp_now_send(
      nextHop,
      (uint8_t *)&packet,
      sizeof(packet)
    );

    Serial.print(
      "Forwarding packet to Node "
    );

    Serial.println(
      packet.destination
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
  if (
    len != sizeof(MeshPacket)
  )
  {
    return;
  }


  MeshPacket packet;

  memcpy(
    &packet,
    incomingData,
    sizeof(packet)
  );


  Serial.println();

  Serial.println(
    "Packet received"
  );


  Serial.print(
    "Source: "
  );

  Serial.println(
    packet.source
  );


  Serial.print(
    "Destination: "
  );

  Serial.println(
    packet.destination
  );


  Serial.print(
    "Value: "
  );

  Serial.println(
    packet.value
  );


  Serial.print(
    "TTL: "
  );

  Serial.println(
    packet.ttl
  );


  // --------------------------------
  // Packet is for this node
  // --------------------------------

  if (
    packet.destination
    == NODE_ID
  )
  {
    Serial.println(
      "Packet reached destination"
    );

    return;
  }


  // --------------------------------
  // Forward packet
  // --------------------------------

  if (
    packet.ttl > 0
  )
  {
    packet.ttl--;

    Serial.println(
      "Forwarding packet..."
    );

    sendPacket(
      packet
    );
  }

  else
  {
    Serial.println(
      "TTL expired"
    );
  }
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  delay(1000);


  // --------------------------------
  // Wi-Fi Setup
  // --------------------------------

  WiFi.mode(
    WIFI_STA
  );


  esp_wifi_set_channel(
    WIFI_CHANNEL,
    WIFI_SECOND_CHAN_NONE
  );


  Serial.print(
    "Node ID: "
  );

  Serial.println(
    NODE_ID
  );


  Serial.print(
    "MAC: "
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


  esp_now_register_recv_cb(
    onDataRecv
  );


  // --------------------------------
  // Add Neighbor Nodes
  // --------------------------------

#if NODE_ID == 1

  addPeer(
    node2Mac
  );

#elif NODE_ID == 2

  addPeer(
    node1Mac
  );

  addPeer(
    node3Mac
  );

#elif NODE_ID == 3

  addPeer(
    node2Mac
  );

#endif


  Serial.println(
    "Mesh node ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{

#if NODE_ID == 1

  MeshPacket packet;


  packet.source =
    1;


  packet.destination =
    3;


  packet.value =
    random(0, 4096);


  packet.ttl =
    5;


  Serial.println();

  Serial.print(
    "Node 1 sending value: "
  );

  Serial.println(
    packet.value
  );


  sendPacket(
    packet
  );


  delay(5000);

#else

  delay(1000);

#endif
}
```

---

## Node Setup

### Node 1

```cpp
#define NODE_ID 1
```

Neighbor:

```text
Node 2
```

### Node 2

```cpp
#define NODE_ID 2
```

Neighbors:

```text
Node 1
Node 3
```

### Node 3

```cpp
#define NODE_ID 3
```

Neighbor:

```text
Node 2
```

---

## Communication Flow

```text
Node 1
Source = 1
Destination = 3
Value = 2450
TTL = 5
    |
    v
Node 2
Destination != 2
TTL -> 4
Forward
    |
    v
Node 3
Destination = 3
Packet accepted
```

---

## Expected Output — Node 1

```text
Node ID: 1
Mesh node ready

Node 1 sending value: 2450
Forwarding packet to Node 3
```

---

## Expected Output — Node 2

```text
Node ID: 2
Mesh node ready

Packet received
Source: 1
Destination: 3
Value: 2450
TTL: 5
Forwarding packet...
Forwarding packet to Node 3
```

---

## Expected Output — Node 3

```text
Node ID: 3
Mesh node ready

Packet received
Source: 1
Destination: 3
Value: 2450
TTL: 4
Packet reached destination
```

---

## Packet Structure

```cpp
typedef struct
{
  int source;
  int destination;
  int value;
  int ttl;
}
MeshPacket;
```

### source

Identifies the original sender.

### destination

Identifies the final destination.

### value

Contains application data.

### ttl

Limits how many times the packet can be forwarded.

---

## TTL Concept

```text
TTL = 5
   |
   v
TTL = 4
   |
   v
TTL = 3
   |
   v
...
   |
   v
TTL = 0
   |
   v
Drop Packet
```

TTL prevents packets from being forwarded forever.

---

## Important Commands

### Initialize ESP-NOW

```cpp
esp_now_init();
```

### Add a Neighbor

```cpp
esp_now_add_peer(
  &peerInfo
);
```

### Send a Packet

```cpp
esp_now_send(
  nextHop,
  (uint8_t *)&packet,
  sizeof(packet)
);
```

### Receive a Packet

```cpp
esp_now_register_recv_cb(
  onDataRecv
);
```

### Set Wi-Fi Channel

```cpp
esp_wifi_set_channel(
  WIFI_CHANNEL,
  WIFI_SECOND_CHAN_NONE
);
```

All nodes should use the same channel.

---

## Experiment 1 — Reverse Direction

Modify Node 3 so that it sends a packet to Node 1.

Expected route:

```text
Node 3
   |
   v
Node 2
   |
   v
Node 1
```

---

## Experiment 2 — Change TTL

Change:

```cpp
packet.ttl =
  5;
```

to:

```cpp
packet.ttl =
  1;
```

Observe whether the packet can still reach the destination.

---

## Experiment 3 — Send Real Sensor Data

Replace:

```cpp
packet.value =
  random(0, 4096);
```

with:

```cpp
packet.value =
  analogRead(
    SENSOR_PIN
  );
```

The mesh can then forward sensor data across multiple nodes.

---

## Checkpoint Questions

1. What is a mesh network?
2. What is multi-hop communication?
3. What is the role of Node 2 in this lab?
4. Why is a destination field required?
5. What does TTL mean?
6. Why is TTL important?
7. Why must neighboring nodes be added as ESP-NOW peers?
8. Why should all nodes use the same Wi-Fi channel?
9. What happens when a packet reaches its destination?
10. Is this example a full dynamic mesh network?

---

## Important Note

This lab demonstrates a **simple static mesh**.

It includes:

```text
Multi-Hop Forwarding
+
Destination Addressing
+
TTL
+
Static Routing
```

It does not yet include:

```text
Dynamic Route Discovery
Self-Healing
Neighbor Discovery
Automatic Path Selection
Routing Metrics
```

These features can be added in more advanced mesh experiments.

---

## Simple Assignment

Extend the network to four nodes:

```text
Node 1
   |
   v
Node 2
   |
   v
Node 3
   |
   v
Node 4
```

Send a simulated sensor value from Node 1 to Node 4.

The expected route is:

```text
Node 1
   |
   v
Node 2
   |
   v
Node 3
   |
   v
Node 4
```

Each relay node should:

1. receive the packet;
2. check the destination;
3. decrease the TTL;
4. forward the packet to the next hop.
