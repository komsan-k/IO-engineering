# Lab — Simple ESP32 Mesh Network Using painlessMesh

## Objective

In this lab, students will create a simple **self-organizing mesh network** using multiple ESP32 boards and the **painlessMesh** library.

Each ESP32 node can:

- join the mesh automatically;
- discover other nodes;
- send messages to all nodes;
- receive messages from other nodes.

This lab demonstrates a simple alternative to manually configuring ESP-NOW peers and routing.

---

## Learning Outcomes

After completing this lab, students should be able to:

- explain the basic concept of a mesh network;
- initialize a painlessMesh network;
- send broadcast messages;
- receive messages from other ESP32 nodes;
- observe nodes joining and leaving the network.

---

## System Architecture

Example with three ESP32 nodes:

```text
        ESP32 Node 2
       /            \
      /              \
ESP32 Node 1 ------ ESP32 Node 3
```

Each node can communicate through the mesh.

The mesh automatically manages the network connections.

---

## Required Library

Install the **painlessMesh** library from the Arduino Library Manager.

In Arduino IDE:

```text
Sketch
  |
  v
Include Library
  |
  v
Manage Libraries
  |
  v
Search: painlessMesh
```

Install:

```text
painlessMesh
```

The library may also install required dependencies such as:

```text
TaskScheduler
ArduinoJson
AsyncTCP
```

---

## Mesh Configuration

All nodes must use the same:

```cpp
MESH_PREFIX
MESH_PASSWORD
MESH_PORT
```

Example:

```cpp
#define MESH_PREFIX     "ESP32-Mesh"
#define MESH_PASSWORD   "meshpassword"
#define MESH_PORT       5555
```

---

# Complete Arduino Code

Upload the same program to all ESP32 boards.

```cpp
#include "painlessMesh.h"

// --------------------------------
// Mesh Configuration
// --------------------------------

#define MESH_PREFIX \
  "ESP32-Mesh"

#define MESH_PASSWORD \
  "meshpassword"

#define MESH_PORT \
  5555


// --------------------------------
// Mesh Objects
// --------------------------------

Scheduler userScheduler;

painlessMesh mesh;


// ========================================
// Task: Send Message
// ========================================

void sendMessage();

Task taskSendMessage(
  TASK_SECOND * 5,
  TASK_FOREVER,
  &sendMessage
);


// ========================================
// Send Broadcast Message
// ========================================

void sendMessage()
{
  String message =
    "Hello from Node ";

  message +=
    mesh.getNodeId();


  mesh.sendBroadcast(
    message
  );


  Serial.print(
    "Sent: "
  );

  Serial.println(
    message
  );
}


// ========================================
// Receive Callback
// ========================================

void receivedCallback(
  uint32_t from,
  String &msg
)
{
  Serial.print(
    "Received from Node "
  );

  Serial.print(
    from
  );

  Serial.print(
    ": "
  );

  Serial.println(
    msg
  );
}


// ========================================
// New Connection Callback
// ========================================

void newConnectionCallback(
  uint32_t nodeId
)
{
  Serial.print(
    "New node connected: "
  );

  Serial.println(
    nodeId
  );
}


// ========================================
// Changed Connection Callback
// ========================================

void changedConnectionCallback()
{
  Serial.println(
    "Mesh topology changed"
  );
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  delay(1000);


  Serial.println(
    "Starting painlessMesh..."
  );


  // --------------------------------
  // Initialize Mesh
  // --------------------------------

  mesh.init(
    MESH_PREFIX,
    MESH_PASSWORD,
    &userScheduler,
    MESH_PORT
  );


  // --------------------------------
  // Register Callbacks
  // --------------------------------

  mesh.onReceive(
    &receivedCallback
  );

  mesh.onNewConnection(
    &newConnectionCallback
  );

  mesh.onChangedConnections(
    &changedConnectionCallback
  );


  // --------------------------------
  // Add Scheduled Task
  // --------------------------------

  userScheduler.addTask(
    taskSendMessage
  );

  taskSendMessage.enable();


  Serial.print(
    "My Node ID: "
  );

  Serial.println(
    mesh.getNodeId()
  );


  Serial.println(
    "Mesh node ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  // Must be called continuously
  mesh.update();
}
```

---

## How the Program Works

Each node joins the same mesh network using:

```cpp
mesh.init(
  MESH_PREFIX,
  MESH_PASSWORD,
  &userScheduler,
  MESH_PORT
);
```

Every 5 seconds, the node sends:

```text
Hello from Node <Node ID>
```

using:

```cpp
mesh.sendBroadcast(
  message
);
```

When another node sends a message, the callback:

```cpp
receivedCallback()
```

is executed.

---

## Program Flow

```text
ESP32 Starts
    |
    v
Initialize painlessMesh
    |
    v
Join Mesh Network
    |
    v
Get Node ID
    |
    v
Send Broadcast Every 5 s
    |
    v
Receive Messages
    |
    v
mesh.update()
    |
    v
Repeat
```

---

## Expected Serial Monitor

Example for Node 1:

```text
Starting painlessMesh...
My Node ID: 284735219
Mesh node ready

New node connected: 142376512
Mesh topology changed

Sent: Hello from Node 284735219

Received from Node 142376512:
Hello from Node 142376512
```

Example for Node 2:

```text
Starting painlessMesh...
My Node ID: 142376512
Mesh node ready

New node connected: 284735219
Mesh topology changed

Sent: Hello from Node 142376512

Received from Node 284735219:
Hello from Node 284735219
```

---

## Important Commands

### Initialize Mesh

```cpp
mesh.init(
  MESH_PREFIX,
  MESH_PASSWORD,
  &userScheduler,
  MESH_PORT
);
```

### Get Node ID

```cpp
mesh.getNodeId();
```

### Send Broadcast

```cpp
mesh.sendBroadcast(
  message
);
```

### Receive Messages

```cpp
mesh.onReceive(
  &receivedCallback
);
```

### Detect New Connection

```cpp
mesh.onNewConnection(
  &newConnectionCallback
);
```

### Maintain Mesh Operation

```cpp
mesh.update();
```

This function should run continuously inside:

```cpp
void loop()
```

---

## Experiment 1 — Two-Node Mesh

Use two ESP32 boards.

Upload the same program to both boards.

Observe:

```text
Node 1 <------> Node 2
```

Check that both nodes:

- obtain different Node IDs;
- detect each other;
- exchange broadcast messages.

---

## Experiment 2 — Three-Node Mesh

Add a third ESP32:

```text
        Node 2
       /      \
      /        \
   Node 1 ---- Node 3
```

Observe the Serial Monitors.

Each ESP32 should receive messages from the other nodes.

---

## Experiment 3 — Simulated Sensor Data

Replace:

```cpp
String message =
  "Hello from Node ";
```

with simulated sensor data:

```cpp
int sensorValue =
  random(0, 4096);

String message =
  "Node ";

message +=
  mesh.getNodeId();

message +=
  " Sensor: ";

message +=
  sensorValue;
```

Example output:

```text
Node 284735219 Sensor: 1760
```

---

## Experiment 4 — Node Leaving the Mesh

1. Start three ESP32 nodes.
2. Observe the connections.
3. Disconnect power from one node.
4. Observe:

```text
Mesh topology changed
```

on the remaining nodes.

Reconnect the node and observe it joining again.

---

## ESP-NOW Mesh vs painlessMesh

```text
Manual ESP-NOW Mesh
-------------------
MAC addresses
Static peers
Manual forwarding
Manual routing logic
Manual TTL

painlessMesh
------------
Automatic node discovery
Automatic connection management
Automatic mesh organization
Node IDs
Broadcast messaging
```

---

## Checkpoint Questions

1. What is a mesh network?
2. What library is used in this lab?
3. What values must be the same on all nodes?
4. What is a Node ID?
5. What function sends a broadcast message?
6. What function receives messages?
7. Why must `mesh.update()` run continuously?
8. What happens when a new node joins?
9. What happens when a node leaves?
10. What is one advantage of painlessMesh compared with manually configured ESP-NOW peers?

---

## Simple Assignment

Create a three-node mesh where:

```text
Node 1 -> Simulated Light Sensor
Node 2 -> Simulated Temperature Sensor
Node 3 -> Simulated Humidity Sensor
```

Each node should broadcast:

```text
Node ID
Sensor Type
Sensor Value
```

Example:

```text
Node 284735219
Sensor: Light
Value: 1720
```

The other nodes should display the received messages on the Serial Monitor.

---

## Key Takeaway

painlessMesh simplifies ESP32 mesh networking by automatically managing node discovery and mesh connections.

Conceptually:

```text
Multiple ESP32 Nodes
        |
        v
Self-Organizing Mesh
        |
        v
Automatic Connections
        |
        v
Broadcast / Node Messaging
```

It is useful when students need to focus on mesh applications rather than manually implementing routing and peer management.
