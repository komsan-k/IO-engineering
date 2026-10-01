# Lab — Simple ESP32 BLE Mesh Light Control
## One Provisioner and Two Light Nodes

## Objective

In this lab, three ESP32 boards form a small **Bluetooth Low Energy Mesh** network:

- **ESP32 #1** — BLE Mesh Provisioner / Generic OnOff Client
- **ESP32 #2** — Light Node 1 / Generic OnOff Server
- **ESP32 #3** — Light Node 2 / Generic OnOff Server

The provisioner discovers and provisions the two light nodes, then sends **ON/OFF** commands through the BLE Mesh network.

> This lab uses **ESP-IDF ESP-BLE-MESH**, not the simple Arduino BLE client/server API.

---

## System Architecture

```text
                 BLE Mesh
        +-----------------------+
        |                       |
        v                       v
+---------------+       +---------------+
| ESP32 #2      |       | ESP32 #3      |
| Light Node 1  |       | Light Node 2  |
| OnOff Server  |       | OnOff Server  |
| LED           |       | LED           |
+-------^-------+       +-------^-------+
        |                       |
        +-----------+-----------+
                    |
                    v
             +-------------+
             | ESP32 #1    |
             | Provisioner |
             | OnOff Client|
             +-------------+
```

---

## BLE Mesh Concepts

### Provisioner

The provisioner adds unprovisioned devices into the mesh network.

```text
Unprovisioned Device
        |
        v
Provisioner
        |
        v
Provisioned Mesh Node
```

### Generic OnOff Client

The client sends:

```text
ON
OFF
```

commands.

### Generic OnOff Server

The server receives the command and changes the LED state.

---

## Hardware

You need:

```text
3 × ESP32 development boards
3 × USB cables
2 × LEDs
2 × 220 Ω resistors
```

Example LED connections:

```text
ESP32 #2 GPIO 2 ---- 220 Ω ---- LED ---- GND

ESP32 #3 GPIO 2 ---- 220 Ω ---- LED ---- GND
```

> If your ESP32 board already has an onboard LED, you may use the onboard LED GPIO instead.

---

## Software

Use:

```text
ESP-IDF
ESP-BLE-MESH
```

This lab is based on the official ESP-BLE-MESH examples:

```text
bluetooth/esp_ble_mesh/provisioner
bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

---

# Part 1 — Prepare the Light Nodes

Use the official:

```text
examples/bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

for both Light Node 1 and Light Node 2.

The same server application can be flashed to both ESP32 boards.

---

## Build the OnOff Server Example

Open an ESP-IDF terminal and go to:

```bash
cd $IDF_PATH/examples/bluetooth/esp_ble_mesh/onoff_models/onoff_server
```

Set the target:

```bash
idf.py set-target esp32
```

Configure the example:

```bash
idf.py menuconfig
```

Then build:

```bash
idf.py build
```

Flash Light Node 1:

```bash
idf.py -p COM5 flash monitor
```

Flash Light Node 2 using the same binary:

```bash
idf.py -p COM6 flash monitor
```

Replace `COM5` and `COM6` with the ports used by your boards.

---

## Light Node Behavior

At startup, each light node is:

```text
Unprovisioned
```

After provisioning, each node contains a:

```text
Generic OnOff Server
```

which can receive:

```text
ON
OFF
```

commands.

---

# Part 2 — Prepare the Provisioner

Use the official example:

```text
examples/bluetooth/esp_ble_mesh/provisioner
```

Open the example:

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

Flash the provisioner:

```bash
idf.py -p COM4 flash monitor
```

Replace `COM4` with the provisioner ESP32 port.

---

# Part 3 — Provision the Two Light Nodes

Power all three ESP32 boards.

Expected startup:

```text
ESP32 #1
Provisioner
   |
   | scans
   v
Finds unprovisioned devices
   |
   +---- ESP32 #2
   |
   +---- ESP32 #3
```

The provisioner adds both nodes to the mesh.

Conceptually:

```text
Light Node 1
Unprovisioned
     |
     v
Provisioning
     |
     v
Assigned Mesh Address


Light Node 2
Unprovisioned
     |
     v
Provisioning
     |
     v
Assigned Mesh Address
```

---

## Typical Address Example

The exact addresses depend on the example configuration.

A simple example is:

```text
Provisioner   -> 0x0001
Light Node 1  -> 0x0005
Light Node 2  -> 0x0006
```

Record the addresses shown by the provisioner monitor.

---

# Part 4 — Bind the Application Key

After provisioning, the Generic OnOff model must use an application key.

Conceptually:

```text
Provision Node
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

The official provisioner example performs configuration through the BLE Mesh Configuration Client model.

---

# Part 5 — Control Light Node 1

The provisioner sends a Generic OnOff SET message.

Conceptually:

```text
Provisioner
     |
     | Generic OnOff SET
     | value = 1
     v
Light Node 1
     |
     v
LED ON
```

For OFF:

```text
Provisioner
     |
     | Generic OnOff SET
     | value = 0
     v
Light Node 1
     |
     v
LED OFF
```

---

# Part 6 — Control Light Node 2

The same operation is repeated using the address of Light Node 2.

```text
Provisioner
     |
     | destination = Light Node 2
     v
Generic OnOff Server
     |
     v
LED ON / OFF
```

---

# Expected Operation

Example:

```text
Provisioner Monitor

Node 1 provisioned
Address: 0x0005

Node 2 provisioned
Address: 0x0006

Sending ON to 0x0005
Light Node 1 -> ON

Sending OFF to 0x0005
Light Node 1 -> OFF

Sending ON to 0x0006
Light Node 2 -> ON

Sending OFF to 0x0006
Light Node 2 -> OFF
```

---

# BLE Mesh Message Flow

```text
ESP32 Provisioner
       |
       | Generic OnOff SET
       v
BLE Mesh Network
       |
       +------> Light Node 1
       |
       +------> Light Node 2
```

---

# Experiment 1 — Individual Light Control

Control each node separately.

```text
Node 1 -> ON
Node 2 -> OFF
```

Then:

```text
Node 1 -> OFF
Node 2 -> ON
```

---

# Experiment 2 — Group Control

Create a group address such as:

```text
0xC001
```

Bind both light nodes to the same group.

Then send:

```text
Generic OnOff SET
Destination = 0xC001
Value = ON
```

Both nodes should turn ON.

```text
Provisioner
     |
     | Group Address 0xC001
     v
+------------+------------+
|                         |
v                         v
Light Node 1          Light Node 2
LED ON                LED ON
```

---

# Experiment 3 — Relay Concept

Move Light Node 2 farther away.

If relay functionality is enabled on an intermediate mesh node, a message may be relayed:

```text
Provisioner
     |
     v
Light Node 1
   Relay
     |
     v
Light Node 2
```

This demonstrates the multi-hop concept of BLE Mesh.

---

# Important BLE Mesh Models

| Model | Role |
|---|---|
| Configuration Client | Configures mesh nodes |
| Configuration Server | Receives configuration |
| Generic OnOff Client | Sends ON/OFF commands |
| Generic OnOff Server | Controls light state |

---

# BLE Mesh vs Normal BLE

```text
Normal BLE
----------
Central
   |
Peripheral

Usually direct connection
```

```text
BLE Mesh
--------
Node <--> Node <--> Node
          |
          v
       Relay
```

BLE Mesh adds:

```text
Provisioning
Addressing
Models
Application Keys
Relay
Group Messaging
Multi-hop Communication
```

---

# Checkpoint Questions

1. What is the role of the provisioner?
2. What is an unprovisioned device?
3. What model controls the LEDs?
4. What is the difference between Generic OnOff Client and Server?
5. Why does each mesh node need an address?
6. What is an AppKey?
7. What is a group address?
8. Why is group addressing useful for lighting?
9. What is the purpose of a relay node?
10. How is BLE Mesh different from normal BLE client/server communication?

---

# Simple Assignment

Extend the system to three light nodes:

```text
                  Provisioner
                      |
          +-----------+-----------+
          |           |           |
          v           v           v
       Light 1     Light 2     Light 3
```

Requirements:

```text
1. Provision all three nodes.
2. Record each unicast address.
3. Control each light individually.
4. Create one group address.
5. Add all three lights to the group.
6. Send one group ON command.
7. Send one group OFF command.
```

---

# Key Takeaway

A BLE Mesh lighting system can be summarized as:

```text
Provisioner
    |
    v
Provision Devices
    |
    v
Configure Models
    |
    v
Bind AppKey
    |
    v
Send Generic OnOff Commands
    |
    v
Control Multiple Lights
```

This provides a standardized way to build large-scale Bluetooth-based lighting and sensor networks.
