# Lab — Simple ESP32 Wi-Fi Mesh with 4 Devices
## One Controller and Three Light Nodes Using Arduino + painlessMesh

## Objective

In this lab, four ESP32 boards form a simple **Wi-Fi mesh network** using the Arduino-compatible **painlessMesh** library.

The network contains:

- **ESP32 #1** — Controller Node
- **ESP32 #2** — Light Node 1
- **ESP32 #3** — Light Node 2
- **ESP32 #4** — Light Node 3

The Controller Node sends commands through the mesh to control the LEDs on the three Light Nodes.

---

## Important Note

This lab uses:

```text
Arduino
+
ESP32
+
painlessMesh
```

It is a **self-organizing Wi-Fi mesh-style network** managed by the painlessMesh library.

It is not the same API as Espressif's native:

```text
ESP-WIFI-MESH
```

The painlessMesh approach is simpler for an Arduino teaching lab.

---

# 1. System Architecture

```text
                   ESP32 #1
                 Controller
                     |
             Wi-Fi Mesh Network
          +----------+----------+
          |          |          |
          v          v          v
      ESP32 #2    ESP32 #3    ESP32 #4
      Light 1     Light 2     Light 3
        LED         LED         LED
```

Because the mesh is self-organizing, the physical radio path may also look like:

```text
Controller
    |
    v
Light 1
    |
    v
Light 2
    |
    v
Light 3
```

Messages can be forwarded through intermediate nodes when required by the mesh topology.

---

# 2. Hardware

Required:

```text
4 × ESP32 development boards
4 × USB cables
3 × LEDs
3 × 220 Ω resistors
```

For each Light Node:

```text
GPIO 2 ---- 220 Ω ---- LED ---- GND
```

If your ESP32 board uses another onboard LED GPIO, change:

```cpp
#define LED_PIN 2
```

---

# 3. Required Arduino Libraries

Install these libraries using **Arduino Library Manager**:

```text
painlessMesh
ArduinoJson
TaskScheduler
AsyncTCP
```

The exact dependencies installed automatically may depend on the painlessMesh version.

---

# 4. Mesh Configuration

All four ESP32 devices must use the same:

```cpp
#define MESH_PREFIX   "ESP32-WIFI-MESH"
#define MESH_PASSWORD "meshpassword"
#define MESH_PORT     5555
```

The mesh configuration must match on every node.

---

# 5. Communication Protocol

The controller broadcasts JSON commands.

Example:

```json
{
  "type": "control",
  "target": 1,
  "state": 1
}
```

Meaning:

```text
target = 1
state  = 1

Light Node 1 -> ON
```

Another example:

```json
{
  "type": "control",
  "target": 3,
  "state": 0
}
```

Meaning:

```text
Light Node 3 -> OFF
```

Use:

```text
target = 0
```

to control all lights.

Example:

```json
{
  "type": "control",
  "target": 0,
  "state": 1
}
```

Result:

```text
Light 1 -> ON
Light 2 -> ON
Light 3 -> ON
```

---

# 6. ESP32 #1 — Controller Code

Save as:

```text
ESP32_Mesh_Controller.ino
```

## Complete Arduino Code

```cpp
#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

// --------------------------------
// Mesh Configuration
// --------------------------------

#define MESH_PREFIX \
  "ESP32-WIFI-MESH"

#define MESH_PASSWORD \
  "meshpassword"

#define MESH_PORT \
  5555


Scheduler userScheduler;

painlessMesh mesh;


// ========================================
// Send Control Command
// ========================================

void sendControl(
  int target,
  int state
)
{
  JsonDocument doc;

  doc["type"] =
    "control";

  doc["target"] =
    target;

  doc["state"] =
    state;


  String message;

  serializeJson(
    doc,
    message
  );


  mesh.sendBroadcast(
    message
  );


  Serial.print(
    "Broadcast: "
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
    "Received from "
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
// Changed Connections Callback
// ========================================

void changedConnectionCallback()
{
  Serial.println(
    "Mesh topology changed"
  );

  Serial.println(
    mesh.subConnectionJson()
  );
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(
    115200
  );

  delay(
    1000
  );


  Serial.println();
  Serial.println(
    "ESP32 Wi-Fi Mesh Controller"
  );


  mesh.setDebugMsgTypes(
    ERROR |
    STARTUP |
    CONNECTION
  );


  mesh.init(
    MESH_PREFIX,
    MESH_PASSWORD,
    &userScheduler,
    MESH_PORT
  );


  mesh.onReceive(
    &receivedCallback
  );


  mesh.onNewConnection(
    &newConnectionCallback
  );


  mesh.onChangedConnections(
    &changedConnectionCallback
  );


  Serial.print(
    "Controller Node ID: "
  );

  Serial.println(
    mesh.getNodeId()
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  mesh.update();


  static unsigned long
    lastAction = 0;


  static int step = 0;


  if (
    millis() - lastAction
    >= 3000
  )
  {
    lastAction =
      millis();


    switch (step)
    {
      case 0:

        sendControl(
          1,
          1
        );

        break;


      case 1:

        sendControl(
          1,
          0
        );

        break;


      case 2:

        sendControl(
          2,
          1
        );

        break;


      case 3:

        sendControl(
          2,
          0
        );

        break;


      case 4:

        sendControl(
          3,
          1
        );

        break;


      case 5:

        sendControl(
          3,
          0
        );

        break;


      case 6:

        sendControl(
          0,
          1
        );

        break;


      case 7:

        sendControl(
          0,
          0
        );

        break;
    }


    step++;


    if (
      step > 7
    )
    {
      step =
        0;
    }
  }
}
```

---

# 7. ESP32 Light-Node Code

Use the same code on:

```text
ESP32 #2
ESP32 #3
ESP32 #4
```

Only change:

```cpp
#define LIGHT_ID 1
```

to:

```text
ESP32 #2 -> LIGHT_ID 1
ESP32 #3 -> LIGHT_ID 2
ESP32 #4 -> LIGHT_ID 3
```

Save as:

```text
ESP32_Mesh_Light_Node.ino
```

## Complete Arduino Code

```cpp
#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

// --------------------------------
// Mesh Configuration
// --------------------------------

#define MESH_PREFIX \
  "ESP32-WIFI-MESH"

#define MESH_PASSWORD \
  "meshpassword"

#define MESH_PORT \
  5555


// --------------------------------
// Light Configuration
// --------------------------------

// Change for each light node
#define LIGHT_ID 1

#define LED_PIN 2


Scheduler userScheduler;

painlessMesh mesh;


// ========================================
// Send Status
// ========================================

void sendStatus(
  int state
)
{
  JsonDocument doc;

  doc["type"] =
    "status";

  doc["light"] =
    LIGHT_ID;

  doc["state"] =
    state;


  String message;

  serializeJson(
    doc,
    message
  );


  mesh.sendBroadcast(
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
    "Received from "
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


  JsonDocument doc;


  DeserializationError error =
    deserializeJson(
      doc,
      msg
    );


  if (error)
  {
    Serial.println(
      "JSON parse error"
    );

    return;
  }


  const char *type =
    doc["type"];


  if (
    type == nullptr
  )
  {
    return;
  }


  if (
    String(type)
    != "control"
  )
  {
    return;
  }


  int target =
    doc["target"];

  int state =
    doc["state"];


  // --------------------------------
  // Check Destination
  // --------------------------------

  if (
    target == LIGHT_ID
    ||
    target == 0
  )
  {
    digitalWrite(
      LED_PIN,
      state
        ? HIGH
        : LOW
    );


    Serial.print(
      "Light "
    );

    Serial.print(
      LIGHT_ID
    );

    Serial.print(
      " = "
    );

    Serial.println(
      state
        ? "ON"
        : "OFF"
    );


    sendStatus(
      state
    );
  }
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
// Changed Connections Callback
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
  Serial.begin(
    115200
  );

  delay(
    1000
  );


  pinMode(
    LED_PIN,
    OUTPUT
  );


  digitalWrite(
    LED_PIN,
    LOW
  );


  Serial.println();
  Serial.println(
    "ESP32 Wi-Fi Mesh Light Node"
  );


  Serial.print(
    "Logical Light ID: "
  );

  Serial.println(
    LIGHT_ID
  );


  mesh.setDebugMsgTypes(
    ERROR |
    STARTUP |
    CONNECTION
  );


  mesh.init(
    MESH_PREFIX,
    MESH_PASSWORD,
    &userScheduler,
    MESH_PORT
  );


  mesh.onReceive(
    &receivedCallback
  );


  mesh.onNewConnection(
    &newConnectionCallback
  );


  mesh.onChangedConnections(
    &changedConnectionCallback
  );


  Serial.print(
    "Mesh Node ID: "
  );

  Serial.println(
    mesh.getNodeId()
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  mesh.update();
}
```

---

# 8. Configure Each Light Node

## ESP32 #2

```cpp
#define LIGHT_ID 1
```

## ESP32 #3

```cpp
#define LIGHT_ID 2
```

## ESP32 #4

```cpp
#define LIGHT_ID 3
```

All other code remains the same.

---

# 9. How the Network Works

The Controller sends:

```text
target = 1
state = 1
```

All mesh nodes may receive the broadcast.

Only:

```text
LIGHT_ID = 1
```

accepts the command.

Therefore:

```text
Controller
     |
     | Broadcast
     v
+----------+----------+----------+
|          |          |          |
v          v          v
Light 1    Light 2    Light 3
MATCH      ignore     ignore
|
v
LED ON
```

---

# 10. Expected Controller Output

```text
ESP32 Wi-Fi Mesh Controller
Controller Node ID: 274839102

New node connected: 129384750
New node connected: 387221934
New node connected: 198475620

Broadcast:
{"type":"control","target":1,"state":1}

Received:
{"type":"status","light":1,"state":1}

Broadcast:
{"type":"control","target":1,"state":0}

Broadcast:
{"type":"control","target":2,"state":1}
```

---

# 11. Expected Light Node 1 Output

```text
ESP32 Wi-Fi Mesh Light Node
Logical Light ID: 1

Mesh Node ID: 129384750

Received:
{"type":"control","target":1,"state":1}

Light 1 = ON

Received:
{"type":"control","target":1,"state":0}

Light 1 = OFF
```

---

# 12. Expected Control Sequence

The controller automatically generates:

```text
Light 1 ON
     |
     v
Light 1 OFF
     |
     v
Light 2 ON
     |
     v
Light 2 OFF
     |
     v
Light 3 ON
     |
     v
Light 3 OFF
     |
     v
ALL ON
     |
     v
ALL OFF
```

---

# 13. Group Control

A special target is used:

```text
target = 0
```

Example:

```cpp
sendControl(
  0,
  1
);
```

Result:

```text
Light 1 -> ON
Light 2 -> ON
Light 3 -> ON
```

To turn all OFF:

```cpp
sendControl(
  0,
  0
);
```

---

# 14. Mesh Topology

The application does not need to know the physical forwarding path.

For example:

```text
Controller
     |
     v
Light 1
     |
     v
Light 2
     |
     v
Light 3
```

or:

```text
        Controller
        /       \
       v         v
   Light 1     Light 2
                  |
                  v
               Light 3
```

painlessMesh manages the connectivity automatically.

---

# 15. Important Commands

## Initialize Mesh

```cpp
mesh.init(
  MESH_PREFIX,
  MESH_PASSWORD,
  &userScheduler,
  MESH_PORT
);
```

## Get Node ID

```cpp
mesh.getNodeId();
```

## Broadcast Message

```cpp
mesh.sendBroadcast(
  message
);
```

## Receive Messages

```cpp
mesh.onReceive(
  &receivedCallback
);
```

## Maintain Mesh

```cpp
mesh.update();
```

This should execute frequently.

Avoid long blocking delays in the application.

---

# 16. Experiment 1 — Individual LED Control

Use:

```cpp
sendControl(
  1,
  1
);
```

Expected:

```text
Light 1 -> ON
Light 2 -> unchanged
Light 3 -> unchanged
```

Then:

```cpp
sendControl(
  2,
  1
);
```

Expected:

```text
Light 2 -> ON
```

---

# 17. Experiment 2 — All LEDs ON/OFF

Send:

```cpp
sendControl(
  0,
  1
);
```

Then:

```cpp
sendControl(
  0,
  0
);
```

Observe all three LEDs.

---

# 18. Experiment 3 — Running Light

Modify the controller sequence:

```text
Light 1 ON
Light 1 OFF
Light 2 ON
Light 2 OFF
Light 3 ON
Light 3 OFF
```

Reduce the interval from:

```cpp
3000
```

to:

```cpp
500
```

to create a faster running-light effect.

---

# 19. Experiment 4 — Node Failure

1. Start all four nodes.
2. Wait until the network forms.
3. Disconnect one Light Node.
4. Observe:

```text
Mesh topology changed
```

5. Reconnect the node.
6. Observe it rejoin the mesh.

---

# 20. Experiment 5 — Simulated Sensor Feedback

Add a simulated sensor value to the status message.

Example:

```cpp
int sensorValue =
  random(
    0,
    4096
  );
```

Then:

```cpp
doc["sensor"] =
  sensorValue;
```

Example message:

```json
{
  "type": "status",
  "light": 2,
  "state": 1,
  "sensor": 2384
}
```

---

# 21. Wi-Fi Mesh vs ESP-NOW

```text
ESP-NOW
-------
Peer MAC addresses
Direct wireless packets
Manual routing if mesh is required
Low overhead
```

```text
painlessMesh
------------
Wi-Fi based
Automatic connection management
Node IDs
Message forwarding
Broadcast messaging
Self-organizing topology
```

---

# 22. Checkpoint Questions

1. What is the role of the Controller Node?
2. Why must all four ESP32 boards use the same mesh prefix?
3. What is the purpose of `LIGHT_ID`?
4. What is the difference between `LIGHT_ID` and `mesh.getNodeId()`?
5. What does `mesh.sendBroadcast()` do?
6. Why does each light inspect the `target` value?
7. What does `target = 0` mean in this lab?
8. Why should `mesh.update()` execute frequently?
9. What happens if a mesh node disconnects?
10. How is painlessMesh different from manually routed ESP-NOW?

---

# 23. Simple Assignment

Modify the four-device mesh system to represent three rooms:

```text
ESP32 #1 -> Building Controller

ESP32 #2 -> Room A Light
ESP32 #3 -> Room B Light
ESP32 #4 -> Room C Light
```

Create commands for:

```text
Room A ON/OFF
Room B ON/OFF
Room C ON/OFF
ALL ROOMS ON/OFF
```

Each light node should send a status response after changing its LED.

---

# 24. Key Takeaway

The system can be summarized as:

```text
Arduino
   |
   v
painlessMesh
   |
   v
Self-Organizing Wi-Fi Network
   |
   +---- Controller
   |
   +---- Light Node 1
   |
   +---- Light Node 2
   |
   +---- Light Node 3
```

The application uses logical IDs and JSON messages while painlessMesh manages Wi-Fi mesh connectivity between the ESP32 devices.
