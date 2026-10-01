# Lab — Simple ESP32 BLE Mesh with 4 Devices
## One Provisioner and Three Light Nodes

## Objective

In this lab, four ESP32 boards form a simple **Bluetooth Low Energy Mesh** network.

The devices are:

- **ESP32 #1** — BLE Mesh Provisioner / Generic OnOff Client
- **ESP32 #2** — Light Node 1 / Generic OnOff Server
- **ESP32 #3** — Light Node 2 / Generic OnOff Server
- **ESP32 #4** — Light Node 3 / Generic OnOff Server

The provisioner adds the three light nodes to the BLE Mesh network and sends **ON/OFF** commands to individual lights or to all lights as a group.

> This lab uses **ESP-IDF ESP-BLE-MESH**, not the simple Arduino BLE client/server API.

---

## Learning Outcomes

After completing this lab, students should be able to:

- explain the roles of a BLE Mesh provisioner and mesh node;
- provision multiple BLE Mesh devices;
- identify unicast addresses;
- control individual mesh nodes;
- use group addressing;
- understand basic relay and multi-hop concepts.

---

## System Architecture

```text
                 BLE Mesh Network

                    ESP32 #1
                  Provisioner
                Generic OnOff
                    Client
                       |
          +------------+------------+
          |            |            |
          v            v            v
     ESP32 #2      ESP32 #3      ESP32 #4
     Light Node 1  Light Node 2  Light Node 3
     OnOff Server  OnOff Server  OnOff Server
        LED            LED            LED
```

---

## Hardware

Required:

```text
4 × ESP32 development boards
4 × USB cables
3 × LEDs
3 × 220 Ω resistors
```

Example LED wiring:

```text
GPIO 2 ---- 220 Ω ---- LED ---- GND
```

Use one LED on each light-node ESP32.

---

## Software

Use:

```text
ESP-IDF
ESP-BLE-MESH
```

Recommended BLE Mesh examples:

```text
Provisioner:
examples/bluetooth/esp_ble_mesh/provisioner

Light Nodes:
examples/bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

---

# Part 1 — Prepare the Three Light Nodes

Use the same **Generic OnOff Server** application on:

```text
ESP32 #2
ESP32 #3
ESP32 #4
```

Each device starts as an unprovisioned BLE Mesh node.

---

## Light Node Concept

```text
ESP32 Light Node
      |
      v
Generic OnOff Server
      |
      v
LED
```

A received state of:

```text
0
```

means:

```text
LED OFF
```

A received state of:

```text
1
```

means:

```text
LED ON
```

---

## Build the Light-Node Example

Open an ESP-IDF terminal:

```bash
cd $IDF_PATH/examples/bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

Set the target:

```bash
idf.py set-target esp32
```

Configure:

```bash
idf.py menuconfig
```

Build:

```bash
idf.py build
```

Flash the first light node:

```bash
idf.py -p COM5 flash monitor
```

Flash the second light node:

```bash
idf.py -p COM6 flash monitor
```

Flash the third light node:

```bash
idf.py -p COM7 flash monitor
```

Replace the COM ports with the ports used by your ESP32 boards.

---

# Part 2 — Prepare the Provisioner

ESP32 #1 acts as the provisioner.

The provisioner performs:

```text
Device Discovery
      |
      v
Provisioning
      |
      v
Address Assignment
      |
      v
AppKey Configuration
      |
      v
Generic OnOff Control
```

---

## Build the Provisioner

Open:

```bash
cd $IDF_PATH/examples/bluetooth/esp_ble_mesh/provisioner
```

Set the target:

```bash
idf.py set-target esp32
```

Configure:

```bash
idf.py menuconfig
```

Build:

```bash
idf.py build
```

Flash:

```bash
idf.py -p COM4 flash monitor
```

---

# Part 3 — Start the BLE Mesh Network

Power all four ESP32 boards.

The network initially looks like:

```text
Provisioner
    |
    | scans for unprovisioned devices
    |
    +---- Light Node 1
    |
    +---- Light Node 2
    |
    +---- Light Node 3
```

The provisioner adds each device to the mesh.

---

## Provisioning Flow

```text
Unprovisioned Node
       |
       v
Provisioner Detects Node
       |
       v
Provisioning
       |
       v
Network Key Added
       |
       v
Unicast Address Assigned
       |
       v
Provisioned Mesh Node
```

---

# Part 4 — Record Node Addresses

Each light node receives a unique unicast address.

Example:

```text
Provisioner  -> 0x0001
Light Node 1 -> 0x0005
Light Node 2 -> 0x0006
Light Node 3 -> 0x0007
```

> Actual addresses depend on the configuration and provisioning sequence.

Record the addresses shown on the provisioner Serial Monitor.

---

## Address Table

| Device | Role | Example Address |
|---|---|---|
| ESP32 #1 | Provisioner | 0x0001 |
| ESP32 #2 | Light Node 1 | 0x0005 |
| ESP32 #3 | Light Node 2 | 0x0006 |
| ESP32 #4 | Light Node 3 | 0x0007 |

---

# Part 5 — Configure the Application Key

After provisioning, the Generic OnOff models need an **AppKey**.

Conceptually:

```text
Provisioned Node
      |
      v
Add AppKey
      |
      v
Bind AppKey
      |
      v
Generic OnOff Server Ready
```

The Configuration Client in the provisioner performs this configuration.

---

# Part 6 — Individual Light Control

The provisioner sends Generic OnOff messages to each node.

---

## Control Light Node 1

```text
Destination = 0x0005
Value = 1
```

Result:

```text
Light Node 1 -> ON
```

Then:

```text
Destination = 0x0005
Value = 0
```

Result:

```text
Light Node 1 -> OFF
```

---

## Control Light Node 2

```text
Destination = 0x0006
Value = 1
```

Result:

```text
Light Node 2 -> ON
```

---

## Control Light Node 3

```text
Destination = 0x0007
Value = 1
```

Result:

```text
Light Node 3 -> ON
```

---

# Part 7 — Group Control

Create a BLE Mesh group address.

Example:

```text
0xC001
```

Subscribe all three Generic OnOff Servers to this group.

```text
Group 0xC001
    |
    +---- Light Node 1
    |
    +---- Light Node 2
    |
    +---- Light Node 3
```

Send:

```text
Destination = 0xC001
Generic OnOff = ON
```

Expected result:

```text
Light Node 1 -> ON
Light Node 2 -> ON
Light Node 3 -> ON
```

Then send:

```text
Destination = 0xC001
Generic OnOff = OFF
```

Expected result:

```text
Light Node 1 -> OFF
Light Node 2 -> OFF
Light Node 3 -> OFF
```

---

# Part 8 — Relay / Multi-Hop Concept

BLE Mesh can relay messages through intermediate nodes.

Conceptually:

```text
Provisioner
    |
    v
Light Node 1
   Relay
    |
    v
Light Node 2
   Relay
    |
    v
Light Node 3
```

This allows a destination to receive a message even when it is outside the direct radio range of the provisioner.

---

## TTL Concept

BLE Mesh messages use a TTL value.

```text
TTL = 5
   |
   v
Relay
   |
   v
TTL = 4
   |
   v
Relay
   |
   v
TTL = 3
```

TTL prevents indefinite forwarding.

---

# Expected Serial Output

Example provisioner output:

```text
Provisioner started

Found unprovisioned device
Provisioning Light Node 1
Assigned address: 0x0005

Found unprovisioned device
Provisioning Light Node 2
Assigned address: 0x0006

Found unprovisioned device
Provisioning Light Node 3
Assigned address: 0x0007

All light nodes configured
```

Example control output:

```text
Send ON -> 0x0005
Send OFF -> 0x0005

Send ON -> 0x0006
Send OFF -> 0x0006

Send ON -> 0x0007
Send OFF -> 0x0007
```

Group control:

```text
Send ON -> Group 0xC001
All lights ON

Send OFF -> Group 0xC001
All lights OFF
```

---

# Experiment 1 — Sequential Light Control

Control the lights in sequence:

```text
Light 1 ON
   |
   v
Light 1 OFF

Light 2 ON
   |
   v
Light 2 OFF

Light 3 ON
   |
   v
Light 3 OFF
```

---

# Experiment 2 — All Lights ON/OFF

Use group address:

```text
0xC001
```

Test:

```text
ALL ON
```

then:

```text
ALL OFF
```

---

# Experiment 3 — Different Group

Create another group:

```text
0xC002
```

Subscribe only:

```text
Light Node 1
Light Node 2
```

Then:

```text
Destination = 0xC002
Value = ON
```

Expected:

```text
Light Node 1 -> ON
Light Node 2 -> ON
Light Node 3 -> unchanged
```

---

# Experiment 4 — Node Removal

1. Start all four ESP32 boards.
2. Control all three lights.
3. Power off Light Node 3.
4. Send a group command.
5. Observe Light Nodes 1 and 2.
6. Power Light Node 3 back on.

Discuss how the mesh behaves when a node disappears.

---

# Important BLE Mesh Models

| Model | Function |
|---|---|
| Configuration Client | Configures nodes |
| Configuration Server | Receives configuration |
| Generic OnOff Client | Sends ON/OFF commands |
| Generic OnOff Server | Controls LED state |

---

# BLE Mesh Addressing

BLE Mesh supports:

```text
Unicast Address
-> one node

Group Address
-> several nodes

Virtual Address
-> logical application group
```

For this lab:

```text
0x0005 -> Light Node 1
0x0006 -> Light Node 2
0x0007 -> Light Node 3

0xC001 -> All Lights
```

---

# Checkpoint Questions

1. What is the role of the provisioner?
2. How many light nodes are used in this lab?
3. What model receives the ON/OFF command?
4. What is a unicast address?
5. What is a group address?
6. Why is an AppKey required?
7. What is the purpose of the Configuration Client?
8. What is the purpose of a relay node?
9. What does TTL mean?
10. What advantage does group addressing provide?

---

# Simple Assignment

Modify the system so the four devices behave as:

```text
ESP32 #1 -> Provisioner

ESP32 #2 -> Room A Light
ESP32 #3 -> Room B Light
ESP32 #4 -> Room C Light
```

Create:

```text
Group 0xC001 -> All Rooms
Group 0xC002 -> Room A + Room B
```

Test:

```text
1. Turn Room A ON individually.
2. Turn Room B ON individually.
3. Turn Room C ON individually.
4. Turn all rooms OFF using Group 0xC001.
5. Turn Room A and Room B ON using Group 0xC002.
```

---

# Key Takeaway

A four-device BLE Mesh lighting system can be represented as:

```text
Provisioner
     |
     v
Provision 3 Nodes
     |
     v
Configure AppKeys
     |
     v
Assign Addresses
     |
     v
Individual Control
     +
Group Control
     +
Relay / Multi-Hop
```

BLE Mesh is useful for scalable lighting, building automation, sensor networks, and distributed control systems.
