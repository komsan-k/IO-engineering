# 📘 Theory: BLE Topology, GAP, and GATT Concepts  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

**Bluetooth Low Energy (BLE)** is a short-range wireless communication technology designed for applications requiring **low power consumption, moderate data rates, and intermittent communication**. BLE is widely used in embedded and Internet of Things (IoT) systems such as wearable devices, healthcare sensors, smart-home devices, industrial sensors, beacons, and mobile-connected embedded systems.

The **ESP32** family provides integrated Bluetooth capabilities on supported variants, making it suitable for developing BLE-enabled embedded systems.

A basic BLE system can be represented as:

```text
Sensor
   │
   ▼
 ESP32
BLE Device
   │
   │ Bluetooth Low Energy
   ▼
Smartphone / Gateway
   │
   ▼
Application
```

Three fundamental concepts are particularly important when learning BLE:

$$
\boxed{
\text{BLE System}
=
\text{Topology}
+
\text{GAP}
+
\text{GATT}
}
$$

where:

- **Topology** describes how BLE devices communicate.
- **GAP** defines device discovery, advertising, connection, and device roles.
- **GATT** defines how application data is organized and exchanged.

---

# 📡 2. Bluetooth Low Energy

BLE was introduced as part of the Bluetooth specification to support applications where devices need to operate with limited energy.

Typical BLE applications include:

- wearable sensors,
- heart-rate monitors,
- environmental sensors,
- smart locks,
- proximity devices,
- asset tracking,
- industrial monitoring,
- wireless configuration,
- smart-home devices,
- human-machine interfaces.

A typical BLE sensor system is:

```text
Physical Sensor
      │
      ▼
     ESP32
      │
      │ BLE
      ▼
  Smartphone
      │
      ▼
 Mobile Application
```

---

# ⚡ 3. Why BLE for Embedded Systems?

BLE is attractive for embedded systems because it provides:

- low-power wireless communication,
- support in smartphones and tablets,
- standardized device discovery,
- standardized data organization,
- short-range connectivity,
- security mechanisms,
- event-driven communication,
- interoperability between compatible devices.

A simplified design objective is:

$$
\boxed{
\text{BLE}
\rightarrow
\text{Low Power}
+
\text{Wireless Connectivity}
+
\text{Interoperability}
}
$$

BLE is especially useful when an ESP32 needs to communicate directly with a smartphone without requiring a Wi-Fi access point.

---

# 🆚 4. BLE vs. Bluetooth Classic

Bluetooth Classic and BLE are designed for different communication requirements.

| Characteristic | Bluetooth Classic | BLE |
|---|---|---|
| Primary focus | Continuous data communication | Low-power communication |
| Power consumption | Generally higher | Generally lower |
| Typical applications | Audio, continuous streams | Sensors, IoT, wearables |
| Discovery | Bluetooth discovery | BLE advertising/scanning |
| Application data model | Profile dependent | GATT-based |
| Smartphone support | Yes | Yes |
| ESP32 applications | Audio/serial-like applications | Sensors/control/IoT |

BLE should not simply be considered a slower version of Bluetooth Classic. Its architecture and communication model are designed around energy-efficient device interaction.

---

# 🧱 5. BLE Protocol Architecture

BLE consists of several protocol layers.

A simplified architecture is:

```text
┌──────────────────────────┐
│       Application        │
├──────────────────────────┤
│          GATT            │
├──────────────────────────┤
│          ATT             │
├──────────────────────────┤
│          L2CAP           │
├──────────────────────────┤
│      Link Layer (LL)     │
├──────────────────────────┤
│       BLE PHY/Radio      │
└──────────────────────────┘
```

Two higher-level concepts commonly encountered by application developers are:

```text
GAP
│
├── Advertising
├── Scanning
├── Discovery
├── Connections
└── Roles

GATT
│
├── Services
├── Characteristics
├── Descriptors
└── Data exchange
```

---

# 🌐 6. BLE Topology

BLE supports several communication relationships depending on the Bluetooth version and application architecture.

For introductory ESP32 applications, the most important topology is a connection between two devices:

```text
BLE Peripheral
      │
      │ BLE Connection
      ▼
BLE Central
```

For example:

```text
ESP32 Sensor
 Peripheral
      │
      │ BLE
      ▼
 Smartphone
   Central
```

The ESP32 typically advertises its presence, while the smartphone scans for available devices and initiates the connection.

---

# 📢 7. Advertising and Scanning

Before establishing a connection, a BLE device can transmit **advertising packets**.

```text
ESP32
Peripheral
   │
   │ Advertising
   │ Advertising
   │ Advertising
   ▼

 Smartphone
   Central
```

The central device performs **scanning**:

```text
ESP32
   │
   │ Advertisement
   ▼
Smartphone
   │
   ▼
Device discovered
```

Therefore:

$$
\boxed{
\text{Advertising}
+
\text{Scanning}
\rightarrow
\text{Device Discovery}
}
$$

---

# 📣 8. Advertising Device

An advertising device periodically transmits information that may include:

- device name,
- service UUIDs,
- manufacturer-specific data,
- flags,
- device capabilities,
- other advertising data.

For example:

```text
Device Name:
ESP32-SENSOR

Service:
Environmental Sensor

Status:
Available
```

Advertising allows other BLE devices to discover the ESP32 without first establishing a connection.

---

# 🔍 9. Scanner

A BLE scanner listens for advertisements.

For example:

```text
             BLE Advertisements

ESP32-01 ──────────────┐
ESP32-02 ──────────────┼──► Smartphone
Beacon-01 ─────────────┘       Scanner
```

The scanner may obtain information such as:

```text
ESP32-SENSOR
RSSI = -58 dBm
Service UUID = ...
```

The application can then decide whether to connect.

---

# 📶 10. Connection-Oriented BLE

After discovering the desired device, a connection may be established.

```text
ESP32
Peripheral
   │
   │ Advertisement
   ▼
Smartphone
 Central
   │
   │ Connection Request
   ▼
ESP32
   │
   ▼
Connected
```

Once connected, the devices can exchange application data through mechanisms such as GATT.

---

# 🌟 11. Connectionless BLE

Not every BLE application requires a persistent connection.

A device can broadcast information through advertisements:

```text
BLE Beacon
    │
    ├────► Smartphone A
    │
    ├────► Smartphone B
    │
    └────► Gateway
```

This model is useful for applications such as:

- proximity detection,
- beaconing,
- simple status broadcasting,
- location-related services.

---

# 🧠 12. Generic Access Profile — GAP

The **Generic Access Profile (GAP)** defines fundamental procedures related to BLE device discovery and connection behavior.

GAP deals with questions such as:

```text
How is the device discovered?

How does it advertise?

How does another device scan for it?

Which device initiates a connection?

How is a connection established?
```

Conceptually:

$$
\boxed{
\text{GAP}
=
\text{Discovery}
+
\text{Advertising}
+
\text{Scanning}
+
\text{Connection Management}
}
$$

---

# 🎭 13. GAP Roles

Common GAP roles include:

- **Broadcaster**
- **Observer**
- **Peripheral**
- **Central**

These roles describe how a BLE device participates in discovery and connection procedures.

---

# 📣 14. Broadcaster

A **Broadcaster** transmits advertising information without requiring a connection.

```text
Broadcaster
     │
     ├────► Observer 1
     ├────► Observer 2
     └────► Observer 3
```

Possible examples include:

- BLE beacons,
- proximity transmitters,
- simple status broadcasters.

---

# 👀 15. Observer

An **Observer** scans for BLE advertising packets.

```text
Broadcaster A ──┐
Broadcaster B ──┼──► Observer
Broadcaster C ──┘
```

The Observer can collect advertising information without establishing a connection.

---

# 📱 16. Peripheral

A **Peripheral** typically advertises and can accept a connection initiated by a Central.

Example:

```text
ESP32 Sensor
 Peripheral
     │
     │ Advertising
     ▼
 Smartphone
   Central
```

Typical peripheral devices include:

- temperature sensors,
- wearable sensors,
- smart switches,
- ESP32 sensor nodes.

---

# 🖥️ 17. Central

A **Central** scans for peripherals and can initiate connections.

```text
             ┌──► ESP32 Sensor 1
             │
Smartphone ──┼──► ESP32 Sensor 2
 Central     │
             └──► ESP32 Sensor 3
```

Typical central devices include:

- smartphones,
- tablets,
- gateways,
- computers,
- another capable ESP32.

---

# 🔄 18. GAP Role Relationships

A simplified relationship is:

```text
Broadcaster ─────► Observer

Peripheral  ◄────► Central
```

For a common ESP32 laboratory:

```text
ESP32
Peripheral
   │
   │ BLE
   ▼
Smartphone
 Central
```

---

# 📋 19. GAP and Device Discovery

A typical discovery sequence is:

```text
ESP32
   │
   │ Start Advertising
   ▼
BLE Advertisement
   │
   ▼
Smartphone Scanning
   │
   ▼
ESP32 Detected
   │
   ▼
User Selects ESP32
   │
   ▼
Connection
```

This process is largely associated with **GAP behavior**.

---

# 🏷️ 20. BLE Device Name

A BLE device can expose a human-readable name such as:

```text
ESP32-BLE-SENSOR
```

A smartphone scanning application may display:

```text
Available Devices

ESP32-BLE-SENSOR
FitnessTracker
SmartWatch
BLE-Light
```

The name helps users identify devices, although BLE applications commonly use UUIDs and addresses internally.

---

# 📊 21. RSSI

During scanning, BLE applications often report **Received Signal Strength Indicator (RSSI)**.

Example:

```text
Device: ESP32-BLE-SENSOR
RSSI: -52 dBm
```

RSSI provides an indication of received signal strength.

Generally:

```text
-40 dBm → strong signal
-60 dBm → moderate signal
-90 dBm → weak signal
```

However, RSSI should not automatically be interpreted as an exact distance because it is affected by:

- obstacles,
- antenna orientation,
- multipath propagation,
- transmit power,
- environmental conditions.

---

# 🧩 22. Generic Attribute Profile — GATT

After devices establish a suitable BLE connection, application data is commonly organized using the **Generic Attribute Profile (GATT)**.

GATT defines a hierarchical data model based on:

```text
Profile
   │
   ▼
Service
   │
   ▼
Characteristic
   │
   ▼
Descriptor
```

A simplified expression is:

$$
\boxed{
\text{GATT}
=
\text{Services}
+
\text{Characteristics}
+
\text{Descriptors}
}
$$

---

# 🆚 23. GAP vs. GATT

A useful distinction is:

| GAP | GATT |
|---|---|
| Device discovery | Application data organization |
| Advertising | Services |
| Scanning | Characteristics |
| Connection roles | Descriptors |
| Connection establishment | Data access |
| Central/Peripheral | Client/Server |

A useful mental model is:

$$
\boxed{
\text{GAP}
\rightarrow
\text{Find and Connect}
}
$$

$$
\boxed{
\text{GATT}
\rightarrow
\text{Organize and Exchange Data}
}
$$

---

# 🗄️ 24. GATT Server

The **GATT Server** stores and exposes attribute data.

In a common ESP32 sensor application:

```text
ESP32
GATT Server
    │
    ├── Temperature Service
    │       │
    │       └── Temperature Characteristic
    │
    └── LED Service
            │
            └── LED Characteristic
```

The ESP32 can therefore provide sensor values and control attributes.

---

# 📱 25. GATT Client

The **GATT Client** accesses data exposed by the GATT Server.

For example:

```text
Smartphone
GATT Client
     │
     │ Read / Write
     ▼
ESP32
GATT Server
```

The client may:

- discover services,
- discover characteristics,
- read values,
- write values,
- subscribe to notifications or indications.

---

# ⚠️ 26. GAP Roles vs. GATT Roles

GAP and GATT roles describe different aspects of BLE.

For example:

```text
ESP32
│
├── GAP Role: Peripheral
│
└── GATT Role: Server
```

and:

```text
Smartphone
│
├── GAP Role: Central
│
└── GATT Role: Client
```

This combination is common, but the concepts should not be treated as identical.

---

# 🧱 27. GATT Hierarchy

A typical hierarchy is:

```text
GATT Server
    │
    ├── Service 1
    │      │
    │      ├── Characteristic A
    │      │       └── Descriptor
    │      │
    │      └── Characteristic B
    │
    └── Service 2
           │
           └── Characteristic C
```

---

# 🧰 28. Service

A **Service** groups related characteristics.

For example:

```text
Environmental Service
      │
      ├── Temperature
      ├── Humidity
      └── Pressure
```

This provides a logical organization for related device functionality.

---

# 🌡️ 29. Characteristic

A **Characteristic** represents an application-level value and associated metadata/properties.

For example:

```text
Temperature Characteristic

Value:
28.5 °C
```

A characteristic can support operations such as:

```text
READ
WRITE
NOTIFY
INDICATE
```

depending on how it is configured.

---

# 📝 30. Descriptor

A **Descriptor** provides additional information or configuration associated with a characteristic.

A common example is the **Client Characteristic Configuration Descriptor (CCCD)**, used by a client to configure notifications or indications.

```text
Temperature Characteristic
       │
       ├── Value
       │
       └── CCCD
```

---

# 🆔 31. UUID

BLE services and characteristics are identified using **Universally Unique Identifiers (UUIDs)**.

For example:

```text
Service UUID:
12345678-1234-1234-1234-123456789000

Characteristic UUID:
12345678-1234-1234-1234-123456789001
```

A UUID allows the client to identify the intended service or characteristic.

---

# 📖 32. Read Operation

A characteristic configured for reading can be accessed by the client.

```text
Smartphone
 GATT Client
     │
     │ READ
     ▼
ESP32
 GATT Server
     │
     │ Temperature = 28.5
     ▼
Smartphone
```

Conceptually:

$$
\boxed{
\text{Client Read Request}
\rightarrow
\text{Server Value}
}
$$

---

# ✍️ 33. Write Operation

A client can write to a writable characteristic.

For example:

```text
Smartphone
     │
     │ WRITE "ON"
     ▼
ESP32
     │
     ▼
LED ON
```

This can be used for:

- LED control,
- motor commands,
- configuration parameters,
- threshold settings.

---

# 🔔 34. Notification

Notifications allow the server to send characteristic updates to a subscribed client without requiring the client to repeatedly read the value.

```text
Sensor
   │
   ▼
ESP32
GATT Server
   │
   │ Notification
   ▼
Smartphone
GATT Client
```

For example:

```text
28.5
28.7
28.9
29.0
```

This is particularly useful for changing sensor data.

---

# 📩 35. Indication

An **indication** is similar to a notification but requires confirmation at the ATT/GATT protocol level.

```text
Server
   │
   │ INDICATION
   ▼
Client
   │
   │ CONFIRMATION
   ▼
Server
```

Therefore:

| Feature | Notification | Indication |
|---|---|---|
| Server sends update | Yes | Yes |
| ATT confirmation | No | Yes |
| Protocol overhead | Lower | Higher |
| Delivery assurance | Lower | Higher |

---

# 🔄 36. Polling vs. Notification

Without notification:

```text
Client
 │
 ├── Read?
 ├── Read?
 ├── Read?
 └── Read?
```

With notification:

```text
Server
 │
 └──► Send update when required
```

For suitable sensor applications, notifications can reduce unnecessary application-level polling.

---

# 🧠 37. Example ESP32 GATT Model

Consider an ESP32 environmental monitor:

```text
ESP32 GATT Server
      │
      └── Environmental Service
             │
             ├── Temperature Characteristic
             │      ├── Read
             │      └── Notify
             │
             ├── Humidity Characteristic
             │      ├── Read
             │      └── Notify
             │
             └── LED Characteristic
                    ├── Read
                    └── Write
```

This allows the smartphone to:

- read temperature,
- receive temperature updates,
- read humidity,
- receive humidity updates,
- control an LED.

---

# 🔁 38. Complete BLE Communication Process

A typical BLE application follows:

```text
ESP32 starts
    │
    ▼
Initialize BLE
    │
    ▼
Create GATT Server
    │
    ▼
Create Service
    │
    ▼
Create Characteristics
    │
    ▼
Start Advertising
    │
    ▼
Smartphone Scans
    │
    ▼
Device Discovered
    │
    ▼
Connection Established
    │
    ▼
Service Discovery
    │
    ▼
Characteristic Discovery
    │
    ▼
Read / Write / Notify
```

---

# 🧭 39. GAP and GATT Interaction

The overall relationship can be represented as:

```text
                GAP
                 │
      ┌──────────┴──────────┐
      │                     │
Advertising              Scanning
      │                     │
      └──────────┬──────────┘
                 │
             Connection
                 │
                 ▼
                GATT
                 │
          Service Discovery
                 │
          Characteristic
                 │
       Read / Write / Notify
```

Therefore:

$$
\boxed{
\text{GAP}
\rightarrow
\text{Connection}
\rightarrow
\text{GATT}
\rightarrow
\text{Application Data}
}
$$

---

# 💻 40. ESP32 BLE Example — Arduino IDE

A simplified ESP32 BLE server can be implemented using a BLE library appropriate for the selected ESP32 board/core.

Conceptually:

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID \
"12345678-1234-1234-1234-123456789000"

#define CHARACTERISTIC_UUID \
"12345678-1234-1234-1234-123456789001"

void setup()
{
    Serial.begin(115200);

    BLEDevice::init("ESP32-BLE");

    BLEServer *server =
        BLEDevice::createServer();

    BLEService *service =
        server->createService(SERVICE_UUID);

    BLECharacteristic *characteristic =
        service->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ
        );

    characteristic->setValue("Hello BLE");

    service->start();

    BLEAdvertising *advertising =
        BLEDevice::getAdvertising();

    advertising->start();
}

void loop()
{
}
```

> Exact APIs can vary with the ESP32 Arduino core, BLE library, and ESP32 variant.

---

# 📱 41. Smartphone BLE Testing

A smartphone BLE utility can be used to inspect the ESP32.

Typical procedure:

```text
Start ESP32
    │
    ▼
ESP32 Advertises
    │
    ▼
Open BLE Scanner
    │
    ▼
Scan
    │
    ▼
Select ESP32-BLE
    │
    ▼
Connect
    │
    ▼
Discover Services
    │
    ▼
Select Characteristic
    │
    ▼
Read / Write / Subscribe
```

---

# 💡 42. ESP32 BLE LED Control

A characteristic can represent an LED state:

```text
LED Service
    │
    └── LED Characteristic
            │
            └── Write
```

Data flow:

```text
Smartphone
     │
     │ Write "1"
     ▼
ESP32
     │
     ▼
GPIO HIGH
     │
     ▼
LED ON
```

Similarly:

```text
Write "0"
    │
    ▼
LED OFF
```

---

# 🌡️ 43. ESP32 BLE Sensor Monitoring

A sensor application might use:

```text
Temperature Sensor
       │
       ▼
     ESP32
       │
       ▼
Temperature Characteristic
       │
       │ Notify
       ▼
   Smartphone
```

The ESP32 can periodically update the characteristic:

```text
28.1 °C
28.3 °C
28.4 °C
28.7 °C
```

---

# ⚙️ 44. BLE with FreeRTOS

Because ESP32 commonly uses FreeRTOS internally, application functionality can be separated into tasks.

```text
Sensor Task
     │
     ▼
    Queue
     │
     ▼
BLE Task
     │
     ▼
GATT Characteristic
     │
     ▼
BLE Client
```

This architecture separates:

- sensor acquisition,
- data processing,
- BLE communication.

---

# 📦 45. BLE and FreeRTOS Queue

A sensor task can generate data:

```cpp
struct SensorData
{
    float temperature;
    float humidity;
};
```

The architecture becomes:

```text
Sensor
   │
   ▼
Sensor Task
   │
   ▼
FreeRTOS Queue
   │
   ▼
BLE Processing
   │
   ▼
GATT Characteristic
   │
   ▼
Notification
```

This helps decouple sensing from wireless communication.

---

# 🔒 46. BLE Security

BLE supports security mechanisms such as:

- pairing,
- bonding,
- authentication,
- encryption,
- key generation and distribution,
- privacy mechanisms.

A simplified secure connection is:

```text
ESP32
   │
   │ Pairing
   ▼
Smartphone
   │
   ▼
Encrypted BLE Connection
```

Security requirements depend on the application.

---

# 🔑 47. Pairing

**Pairing** establishes security information between BLE devices.

Conceptually:

```text
Device A
   │
   │ Pairing Procedure
   ▼
Device B
   │
   ▼
Security Keys Established
```

Pairing can enable encrypted and authenticated communication depending on the selected security procedure and device capabilities.

---

# 💾 48. Bonding

**Bonding** stores security information generated during pairing for future connections.

```text
First Connection
      │
      ▼
Pairing
      │
      ▼
Store Keys
      │
      ▼
Bonded
      │
      ▼
Future Reconnection
```

This can avoid repeating the full pairing process for every connection.

---

# 🔋 49. BLE Power Consumption

BLE power consumption is influenced by parameters such as:

- advertising interval,
- connection interval,
- radio transmit power,
- packet frequency,
- notification frequency,
- scanning behavior,
- sleep behavior.

Conceptually:

$$
P_{\text{avg}}
=
\frac{
E_{\text{active}}+E_{\text{sleep}}
}{
T
}
$$

Reducing radio activity can reduce average energy consumption.

---

# ⏱️ 50. Advertising Interval

The advertising interval determines how frequently advertising events occur.

Conceptually:

```text
Advertisement
     │
     │      Advertisement
     │           │
─────●───────────●───────────●────► Time
```

A shorter advertising interval generally improves discovery responsiveness but increases radio activity.

A longer interval generally reduces radio activity but may increase discovery time.

Thus:

$$
\boxed{
\text{Advertising Interval}
\leftrightarrow
\text{Discovery Latency / Energy Trade-off}
}
$$

---

# 🔄 51. Connection Interval

During a BLE connection, devices communicate during scheduled connection events.

```text
Connection
 Event
   │
   │      Connection
   │        Event
   ▼          ▼
───●──────────●──────────●────► Time
```

The connection interval affects:

- responsiveness,
- throughput,
- radio activity,
- power consumption.

---

# 📊 52. BLE Throughput Considerations

BLE application throughput depends on multiple parameters, including:

- PHY,
- connection interval,
- ATT MTU,
- packet size,
- data length,
- protocol overhead,
- number of packets per connection event,
- device implementation.

A simplified conceptual relationship is:

$$
R_{\text{application}}
=
\frac{
\text{Useful Application Data}
}{
\text{Time}
}
$$

The theoretical radio data rate should therefore not be confused with application-level throughput.

---

# 📏 53. ATT MTU

The **ATT Maximum Transmission Unit (MTU)** determines the maximum size of an ATT protocol data unit exchanged between devices.

A larger negotiated MTU can allow more application data to be carried per suitable GATT operation, reducing relative overhead for larger transfers.

Conceptually:

```text
Small MTU
[Header][Small Payload]

Larger MTU
[Header][      Larger Payload      ]
```

---

# 📡 54. BLE Range

BLE communication range depends on:

- transmit power,
- receiver sensitivity,
- PHY mode,
- antenna design,
- obstacles,
- interference,
- propagation environment.

Therefore:

$$
\boxed{
\text{BLE Range}
=
f(
P_{TX},
\text{Sensitivity},
\text{PHY},
\text{Antenna},
\text{Environment}
)
}
$$

There is no single fixed BLE range applicable to all ESP32 deployments.

---

# 📶 55. BLE Frequency Band

BLE operates in the **2.4 GHz ISM band**.

BLE divides its spectrum into multiple RF channels and uses dedicated advertising channels for fundamental discovery procedures.

Because Wi-Fi also commonly uses the 2.4 GHz band, coexistence and interference can be relevant when an ESP32 uses both technologies.

---

# 📡 56. ESP32 BLE + Wi-Fi

Many ESP32 variants support both Wi-Fi and BLE.

This enables architectures such as:

```text
BLE Sensor
    │
    ▼
   ESP32
    │
    │ Wi-Fi
    ▼
MQTT Broker
    │
    ▼
Cloud / Dashboard
```

The ESP32 acts as a bridge between local BLE devices and an IP-based network.

---

# 🌉 57. BLE-to-MQTT Gateway

A useful IoT architecture is:

```text
BLE Sensor
     │
     │ BLE
     ▼
ESP32 Gateway
     │
     │ Wi-Fi
     ▼
MQTT Broker
     │
     ├──► Dashboard
     ├──► Database
     └──► Cloud
```

This combines:

$$
\boxed{
\text{BLE}
+
\text{ESP32 Gateway}
+
\text{MQTT}
=
\text{Connected IoT System}
}
$$

---

# 🪞 58. BLE and Digital Twins

BLE can provide local physical-device data to a digital-twin architecture.

```text
Physical Sensor
      │
      │ BLE
      ▼
ESP32 Gateway
      │
      │ Wi-Fi / MQTT
      ▼
Digital Twin
      │
      ▼
Visualization / Analytics
```

The GATT data model can represent physical parameters such as:

- temperature,
- position,
- acceleration,
- device status,
- battery level.

---

# 🧠 59. BLE and Embedded Intelligence

BLE can also transport results generated by embedded AI.

```text
Sensor
  │
  ▼
ESP32
  │
  ▼
TinyML Model
  │
  ▼
Inference
  │
  ▼
GATT Characteristic
  │
  │ Notification
  ▼
Smartphone
```

Example inference result:

```text
Activity = WALKING
Confidence = 0.92
```

---

# 🚨 60. BLE Error Handling

An ESP32 BLE application should account for conditions such as:

```text
No client found
Connection lost
Characteristic unavailable
Invalid data
Write rejected
Notification disabled
Pairing failure
Communication timeout
```

A robust design can follow:

```text
Initialize
    │
    ▼
Advertise
    │
    ▼
Connect
    │
    ▼
Exchange Data
    │
    ▼
Disconnected?
    │
    ├── Yes → Restart Advertising
    │
    └── No  → Continue
```

---

# 🔄 61. BLE Reconnection

When a client disconnects:

```text
ESP32
   │
   ▼
Connection Lost
   │
   ▼
Restart Advertising
   │
   ▼
Wait for Central
   │
   ▼
Reconnect
```

This behavior is important for sensor systems that must remain discoverable after disconnection.

---

# 🧪 62. Exercise 1 — BLE Scanning

Use a smartphone BLE scanner to identify nearby BLE devices.

Record:

| Parameter | Observation |
|---|---|
| Device name | |
| RSSI | |
| Connectable? | |
| Advertised services | |
| Manufacturer data | |

Observe how RSSI changes as the distance between the smartphone and ESP32 changes.

---

# 📣 63. Exercise 2 — ESP32 Advertising

Configure the ESP32 to advertise as:

```text
ESP32-BLE-LAB
```

Verify that the smartphone detects the device.

Record:

```text
Device Name:
RSSI:
Service UUID:
Connection Status:
```

---

# 🔗 64. Exercise 3 — BLE Connection

Perform:

```text
ESP32 Advertising
       │
       ▼
Smartphone Scanning
       │
       ▼
Device Selected
       │
       ▼
Connect
       │
       ▼
Service Discovery
```

Observe the available services and characteristics.

---

# 📖 65. Exercise 4 — Read Characteristic

Create a characteristic containing:

```text
Hello ESP32
```

Configure it with the **Read** property.

From the smartphone:

```text
Connect
  │
  ▼
Select Service
  │
  ▼
Select Characteristic
  │
  ▼
Read
```

Expected value:

```text
Hello ESP32
```

---

# ✍️ 66. Exercise 5 — Write Characteristic

Create an LED characteristic with the **Write** property.

Use:

```text
1 → LED ON
0 → LED OFF
```

Architecture:

```text
Smartphone
     │
     │ GATT Write
     ▼
ESP32
     │
     ▼
LED
```

---

# 🔔 67. Exercise 6 — Notification

Configure a temperature characteristic with:

```text
READ
NOTIFY
```

Generate a new temperature value periodically.

```text
ESP32
   │
   │ Notification
   ▼
Smartphone
```

Record:

| Time | Temperature |
|---:|---:|
| 0 s | |
| 1 s | |
| 2 s | |
| 3 s | |
| 4 s | |

---

# 📊 68. Exercise 7 — Compare Read and Notify

Compare:

| Feature | Read | Notify |
|---|---|---|
| Initiated by | Client | Server |
| Repeated polling needed | Yes | No |
| Suitable for changing sensor data | Possible | Very suitable |
| Client subscription required | No | Yes |

Discuss which mechanism is preferable for continuous sensor monitoring.

---

# 🚀 69. Advanced Exercise — BLE Environmental Sensor

Develop an ESP32 BLE environmental monitoring node.

Required GATT structure:

```text
ESP32
GATT Server
     │
     └── Environmental Service
             │
             ├── Temperature
             │      ├── Read
             │      └── Notify
             │
             ├── Humidity
             │      ├── Read
             │      └── Notify
             │
             └── LED Control
                    ├── Read
                    └── Write
```

Requirements:

1. Configure ESP32 as a BLE peripheral.
2. Configure ESP32 as a GATT server.
3. Create a custom service.
4. Create temperature and humidity characteristics.
5. Create an LED control characteristic.
6. Advertise the service.
7. Connect using a smartphone.
8. Read sensor values.
9. Enable notifications.
10. Control the LED through GATT Write.
11. Detect disconnection.
12. Restart advertising automatically.

---

# 💬 70. Discussion Questions

1. What is Bluetooth Low Energy?
2. Why is BLE suitable for embedded systems?
3. How does BLE differ from Bluetooth Classic?
4. What is BLE advertising?
5. What is BLE scanning?
6. What is GAP?
7. What is GATT?
8. What is the difference between GAP and GATT?
9. What is a GAP Broadcaster?
10. What is a GAP Observer?
11. What is a GAP Peripheral?
12. What is a GAP Central?
13. What is a GATT Server?
14. What is a GATT Client?
15. Are Peripheral and GATT Server identical concepts?
16. What is a BLE Service?
17. What is a Characteristic?
18. What is a Descriptor?
19. What is a UUID?
20. What is the difference between Read and Write?
21. What is a BLE notification?
22. What is the difference between notification and indication?
23. What is the CCCD?
24. What is RSSI?
25. Why is RSSI not an exact distance measurement?
26. What is pairing?
27. What is bonding?
28. How does advertising interval influence power consumption?
29. How can BLE and Wi-Fi be integrated on an ESP32?
30. How can an ESP32 operate as a BLE-to-MQTT gateway?

---

# 🧾 71. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain BLE architecture,
- explain basic BLE topology,
- distinguish connection-oriented and advertising-based communication,
- describe BLE advertising and scanning,
- explain GAP,
- identify Broadcaster, Observer, Peripheral, and Central roles,
- explain GATT,
- distinguish GATT Client and Server,
- distinguish GAP roles from GATT roles,
- explain Services, Characteristics, and Descriptors,
- explain UUIDs,
- perform GATT Read and Write operations,
- explain notifications and indications,
- describe the CCCD,
- explain BLE pairing and bonding,
- discuss BLE power-consumption considerations,
- explain advertising and connection intervals,
- describe ATT MTU at a conceptual level,
- implement an ESP32 BLE peripheral,
- design an ESP32 GATT server,
- connect an ESP32 to a smartphone,
- design BLE sensor and control applications,
- integrate BLE with FreeRTOS,
- describe BLE-to-Wi-Fi/MQTT gateway architectures.

---

# 🔑 72. Key Concepts

### BLE Architecture

$$
\boxed{
\text{BLE Application}
=
\text{GAP}
+
\text{GATT}
}
$$

### GAP

$$
\boxed{
\text{GAP}
=
\text{Advertising}
+
\text{Scanning}
+
\text{Discovery}
+
\text{Connection}
}
$$

### GATT

$$
\boxed{
\text{GATT}
=
\text{Services}
+
\text{Characteristics}
+
\text{Descriptors}
}
$$

### Common ESP32 Configuration

$$
\boxed{
\text{ESP32}
=
\text{GAP Peripheral}
+
\text{GATT Server}
}
$$

### Common Smartphone Configuration

$$
\boxed{
\text{Smartphone}
=
\text{GAP Central}
+
\text{GATT Client}
}
$$

### Typical Communication Flow

$$
\boxed{
\text{Advertise}
\rightarrow
\text{Scan}
\rightarrow
\text{Connect}
\rightarrow
\text{Discover}
\rightarrow
\text{Read/Write/Notify}
}
$$

---

# 🎯 73. Summary

Bluetooth Low Energy provides an energy-conscious short-range communication architecture suitable for **ESP32-based embedded and IoT systems**. Understanding BLE requires distinguishing between device connectivity behavior and application-data organization.

**GAP** defines important aspects of device discovery and connection behavior. Its common roles include **Broadcaster, Observer, Peripheral, and Central**. In a typical ESP32 sensor application, the ESP32 operates as a **Peripheral**, advertises its presence, and accepts a connection from a smartphone operating as the **Central**.

**GATT**, in contrast, defines how application information is structured using **Services, Characteristics, and Descriptors**. In a common configuration, the ESP32 acts as the **GATT Server**, while the smartphone acts as the **GATT Client**.

The complete process can be summarized as:

$$
\boxed{
\text{ESP32 Advertising}
\rightarrow
\text{Smartphone Scanning}
\rightarrow
\text{BLE Connection}
\rightarrow
\text{GATT Discovery}
\rightarrow
\text{Data Exchange}
}
$$

GATT characteristics can support **Read, Write, Notify, and Indicate** operations, allowing the ESP32 to provide sensor information and receive control commands.

BLE can also be combined with **FreeRTOS, Wi-Fi, MQTT, digital twins, and embedded AI**, enabling architectures such as:

$$
\boxed{
\text{BLE Sensor}
\rightarrow
\text{ESP32 Gateway}
\rightarrow
\text{Wi-Fi/MQTT}
\rightarrow
\text{Cloud or Digital Twin}
}
$$

These concepts provide the foundation for developing **wearable systems, wireless sensors, smart-home devices, industrial monitoring nodes, BLE gateways, and connected intelligent embedded systems**.

---

# 📘 References

1. Bluetooth SIG, *Bluetooth Core Specification*.
2. Bluetooth SIG, *Generic Access Profile (GAP)*.
3. Bluetooth SIG, *Generic Attribute Profile (GATT)*.
4. Bluetooth SIG, *Attribute Protocol (ATT)*.
5. Espressif Systems, *ESP-IDF Programming Guide — Bluetooth Low Energy*.
6. Espressif Systems, *ESP32 Technical Reference Manual*.
7. K. Townsend, C. Cufí, R. Davidson, and A. Akiba, *Getting Started with Bluetooth Low Energy*, O'Reilly Media.
8. R. Heydon, *Bluetooth Low Energy: The Developer's Handbook*, Prentice Hall.
9. FreeRTOS, *FreeRTOS Kernel Documentation*.
10. N. Gupta, *Inside Bluetooth Low Energy*, Artech House.

---

