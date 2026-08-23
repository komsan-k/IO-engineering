# 📘 Theory: MQTT Publish/Subscribe, Brokers, and QoS Levels
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

The **Message Queuing Telemetry Transport (MQTT)** protocol is a lightweight messaging protocol widely used in **Internet of Things (IoT)** and embedded systems. It is particularly suitable for microcontrollers such as the **ESP32**, where network bandwidth, memory, processing power, and energy may be constrained.

Unlike HTTP/REST, which commonly follows a **request–response** communication model, MQTT uses a **publish/subscribe** model.

```text
Temperature Sensor
        │
        ▼
      ESP32
    Publisher
        │
        ▼
   ┌──────────┐
   │   MQTT   │
   │  Broker  │
   └────┬─────┘
        │
        ▼
   Subscriber
        │
        ▼
 Dashboard / App
```

$$
\boxed{
\text{MQTT System}
=
\text{Publisher}
+
\text{Broker}
+
\text{Subscriber}
}
$$

This chapter focuses on:

1. **MQTT publish/subscribe communication**
2. **MQTT brokers**
3. **MQTT topics**
4. **Quality of Service (QoS) levels**

---

## 🌐 2. Why MQTT for Embedded Systems?

MQTT is well suited to applications involving:

- sensor telemetry,
- remote monitoring,
- smart homes,
- industrial IoT,
- environmental monitoring,
- connected vehicles,
- smart agriculture,
- smart traffic,
- edge computing,
- digital twins.

Typical messages include:

```text
Temperature = 28.5 °C
Humidity = 70 %
LED = ON
Motor Speed = 1200 rpm
Vehicle Count = 15
```

---

## 📡 3. MQTT Communication Model

MQTT introduces an intermediary called a **broker**.

```text
Publisher
    │
    ▼
 MQTT Broker
    │
    ▼
Subscriber
```

The publisher and subscriber do not need direct knowledge of each other.

$$
\boxed{
\text{Publisher}
\rightarrow
\text{Broker}
\rightarrow
\text{Subscriber}
}
$$

---

## 🧠 4. Publish/Subscribe Architecture

The MQTT publish/subscribe architecture consists of:

- **Publisher**
- **Subscriber**
- **Broker**
- **Topic**
- **Message**

```text
                  MQTT Broker
               ┌───────────────┐
ESP32 Sensor ──►│               │──► Dashboard
  Publisher     │               │──► Smartphone
                │               │──► Database
               └───────────────┘
```

A single published message can be distributed to multiple subscribers.

---

## 📤 5. Publisher

A **publisher** sends messages to a topic.

```text
Topic:
building/room1/temperature

Payload:
28.5
```

```text
Temperature Sensor
       │
       ▼
     ESP32
       │
       ▼
building/room1/temperature
       │
       ▼
    MQTT Broker
```

---

## 📥 6. Subscriber

A subscriber registers interest in one or more topics.

```text
Subscribe:
building/room1/temperature
```

```text
Publisher
    │
    ▼
Broker
    │
    ├────────► Dashboard
    ├────────► Database
    └────────► Mobile App
```

---

## 🖥️ 7. MQTT Broker

The broker can:

- accept client connections,
- authenticate clients,
- receive published messages,
- match topics and subscriptions,
- forward messages,
- manage sessions,
- handle retained messages,
- implement QoS behavior.

```text
             ┌──────────────┐
Publisher ──►│              │
             │ MQTT Broker  │──► Subscriber A
Publisher ──►│              │──► Subscriber B
             │              │──► Subscriber C
             └──────────────┘
```

---

## 🧰 8. Examples of MQTT Brokers

Common brokers include:

- Eclipse Mosquitto
- EMQX
- HiveMQ
- VerneMQ

A simple laboratory setup:

```text
ESP32
  │
  ▼
Wi-Fi
  │
  ▼
Laptop / Raspberry Pi
  │
  ▼
Mosquitto Broker
```

---

## 🏷️ 9. MQTT Topics

MQTT messages are organized using hierarchical **topics**.

```text
home/livingroom/temperature
```

```text
home
 └── livingroom
      └── temperature
```

Another example:

```text
university/lab1/esp32/temperature
```

---

## 🧩 10. Topic Design

Example:

```text
building/room1/temperature
building/room1/humidity
building/room1/light

building/room2/temperature
building/room2/humidity
building/room2/light
```

A generalized structure is:

$$
\boxed{
\text{Location}/\text{Device}/\text{Measurement}
}
$$

---

## 📦 11. MQTT Message

An MQTT application message can be viewed as:

```text
Topic
+
Payload
+
QoS
+
Retain Setting
```

Example:

```text
Topic:
lab/esp32/temperature

Payload:
28.7

QoS:
1
```

Payloads may contain text, numbers, JSON, or binary data.

---

## 📊 12. JSON Payload

```json
{
  "temperature": 28.7,
  "humidity": 68
}
```

Published to:

```text
lab/esp32/sensor
```

---

## 🔄 13. MQTT Publish Process

```text
Sensor
   │
   ▼
ESP32 reads sensor
   │
   ▼
Create payload
   │
   ▼
Select topic
   │
   ▼
Publish message
   │
   ▼
MQTT Broker
   │
   ▼
Matching Subscribers
```

$$
\boxed{
\text{Sensor Data}
\rightarrow
\text{MQTT Publish}
\rightarrow
\text{Broker}
\rightarrow
\text{Subscriber}
}
$$

---

## 📥 14. MQTT Subscribe Process

```text
Subscriber
    │
    ▼
Connect Broker
    │
    ▼
Subscribe Topic
    │
    ▼
Wait for Messages
    │
    ▼
Receive Matching Message
```

---

## 🔀 15. Many-to-Many Communication

```text
ESP32-1 ──┐
ESP32-2 ──┼──► MQTT Broker
ESP32-3 ──┘
```

```text
MQTT Broker
    │
    ├──► Dashboard
    ├──► Database
    ├──► Mobile App
    └──► Analytics
```

---

## ⭐ 16. Topic Wildcards

MQTT supports:

```text
+
#
```

- `+` = one topic level
- `#` = multiple topic levels

---

## ➕ 17. Single-Level Wildcard

Topics:

```text
home/room1/temperature
home/room2/temperature
home/room3/temperature
```

Subscription:

```text
home/+/temperature
```

---

## #️⃣ 18. Multi-Level Wildcard

Subscription:

```text
home/#
```

Matches:

```text
home/room1/temperature
home/room1/humidity
home/room2/light
home/security/door/front
```

---

## ⚙️ 19. MQTT over TCP/IP

```text
MQTT
 │
 ▼
TCP
 │
 ▼
IP
 │
 ▼
Wi-Fi
```

$$
\boxed{
\text{MQTT}
\rightarrow
\text{TCP}
\rightarrow
\text{IP}
\rightarrow
\text{Wi-Fi}
}
$$

---

## 🚪 20. MQTT Ports

| Port | Typical Use |
|---:|---|
| `1883` | MQTT over TCP without TLS |
| `8883` | MQTT over TLS |

---

## 📊 21. MQTT Quality of Service

MQTT defines three QoS levels:

| QoS | Delivery Semantics |
|---:|---|
| `0` | At most once |
| `1` | At least once |
| `2` | Exactly once |

---

## 🚀 22. QoS 0 — At Most Once

$$
\boxed{
\text{QoS 0}
=
\text{At Most Once}
}
$$

```text
Publisher
    │
    │ PUBLISH
    ▼
Broker
```

Advantages:

- lowest protocol overhead,
- low latency,
- suitable for frequent telemetry.

Disadvantage:

- message delivery is not guaranteed at the MQTT QoS level.

---

## 🔁 23. QoS 1 — At Least Once

$$
\boxed{
\text{QoS 1}
=
\text{At Least Once}
}
$$

```text
Publisher
    │
    │ PUBLISH
    ▼
Receiver
    │
    │ PUBACK
    ▼
Publisher
```

If an acknowledgment is lost, retransmission may occur.

$$
\boxed{
\text{QoS 1}
=
\text{Delivery Expected}
+
\text{Duplicates Possible}
}
$$

---

## 🛡️ 24. QoS 2 — Exactly Once

$$
\boxed{
\text{QoS 2}
=
\text{Exactly Once}
}
$$

```text
Sender
  │
  │ PUBLISH
  ▼
Receiver
  │
  │ PUBREC
  ▼
Sender
  │
  │ PUBREL
  ▼
Receiver
  │
  │ PUBCOMP
  ▼
Sender
```

---

## 📊 25. QoS Comparison

| Feature | QoS 0 | QoS 1 | QoS 2 |
|---|---|---|---|
| Delivery semantics | At most once | At least once | Exactly once |
| Acknowledgment | No | Yes | Multi-step |
| Duplicate delivery | No retransmission | Possible | Prevented by QoS flow |
| Protocol overhead | Lowest | Medium | Highest |
| Latency | Generally lowest | Moderate | Generally highest |
| Resource demand | Low | Medium | Higher |

---

## 🎯 26. Selecting a QoS Level

| Application | Possible QoS |
|---|---:|
| Periodic temperature telemetry | 0 |
| Device alarm | 1 |
| Important control message | 1 |
| Exactly-once MQTT delivery requirement | 2 |

$$
\boxed{
\text{QoS Selection}
=
f(
\text{Reliability},
\text{Latency},
\text{Bandwidth},
\text{Resources}
)
}
$$

---

## ⚠️ 27. QoS Does Not Mean End-to-End Application Success

MQTT QoS concerns protocol-level message delivery. It does not automatically guarantee that a physical actuator completed an action or that a database stored a record successfully.

```text
MQTT Command Delivered
        │
        ▼
ESP32 Receives Command
        │
        ▼
Motor Driver
        │
        ▼
Physical Motor
```

Application-level confirmation may still be required.

---

## 💾 28. Retained Messages

A retained publication causes the broker to store the retained message for the topic.

Example:

```text
Topic:
home/room1/status

Payload:
ONLINE

Retain:
true
```

A new matching subscriber can immediately receive the latest retained value.

---

## 📴 29. Last Will and Testament

MQTT provides a **Last Will and Testament (LWT)** mechanism.

Example:

```text
Topic:
devices/esp32-01/status

Payload:
OFFLINE
```

If the ESP32 disconnects unexpectedly, the broker can publish this message.

---

## ❤️ 30. Device Status Monitoring

When connected:

```text
devices/esp32-01/status
ONLINE
```

Unexpected disconnect:

```text
devices/esp32-01/status
OFFLINE
```

---

## 🔌 31. MQTT Client Connection

```text
Start ESP32
    │
    ▼
Connect Wi-Fi
    │
    ▼
Connect MQTT Broker
    │
    ▼
MQTT Session Established
    │
    ├── Publish
    └── Subscribe
```

---

## 🔄 32. MQTT Reconnection

```text
Wi-Fi Connected?
    │
    ├── No → Reconnect Wi-Fi
    │
    ▼
MQTT Connected?
    │
    ├── No → Reconnect Broker
    │
    ▼
Publish / Subscribe
```

---

## ❤️ 33. MQTT Keep Alive

```text
ESP32
   │
   │ PINGREQ
   ▼
Broker
   │
   │ PINGRESP
   ▼
ESP32
```

Keep Alive helps detect failed connections.

---

## 🧠 34. ESP32 MQTT Architecture

```text
┌───────────────────┐
│      Sensors      │
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│       ESP32       │
│ Sensor Processing │
│ MQTT Client       │
│ Wi-Fi             │
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│    MQTT Broker    │
└───────┬─────┬─────┘
        │     │
        ▼     ▼
 Dashboard  Database
```

---

## 🧪 35. ESP32 MQTT Publisher Example

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    client.setServer(mqtt_server, 1883);
}

void loop()
{
    if (!client.connected())
    {
        // Reconnect logic
    }

    client.loop();

    client.publish(
        "lab/esp32/temperature",
        "28.5"
    );

    delay(5000);
}
```

---

## 📥 36. ESP32 MQTT Subscriber Example

```cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Payload: ");

    for (unsigned int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }

    Serial.println();
}
```

Register:

```cpp
client.setCallback(callback);
```

Subscribe:

```cpp
client.subscribe("lab/esp32/led");
```

---

## 💡 37. MQTT LED Control

```text
Dashboard
    │
    │ Publish "ON"
    ▼
MQTT Broker
    │
    ▼
ESP32 Subscriber
    │
    ▼
GPIO HIGH
    │
    ▼
LED ON
```

Topic:

```text
lab/esp32/led
```

---

## 🌡️ 38. MQTT Temperature Monitoring

```text
Topic:
lab/esp32/temperature

Payload:
29.2
```

```text
Sensor
  │
  ▼
ESP32
  │
  ▼
Broker
  │
  ▼
Dashboard
```

---

## 📦 39. Multiple Sensor Values

```json
{
  "temperature": 29.2,
  "humidity": 68,
  "pressure": 1008
}
```

Topic:

```text
lab/esp32/environment
```

---

## 🏠 40. Smart Home MQTT Architecture

```text
Temperature ESP32 ──┐
Light ESP32 ─────────┼──► MQTT Broker
Door ESP32 ──────────┘
                         │
             ┌───────────┼───────────┐
             ▼           ▼           ▼
         Dashboard    Database    Mobile App
```

Possible topics:

```text
home/room1/temperature
home/room1/light
home/frontdoor/status
home/room1/fan/set
```

---

## 🚦 41. Smart Traffic MQTT Example

Topics:

```text
traffic/intersection1/vehicle_count
traffic/intersection1/light_state
traffic/intersection1/congestion
```

Example JSON:

```json
{
  "vehicles": 24,
  "state": "GREEN",
  "congestion": "MEDIUM"
}
```

---

## 🏭 42. Industrial IoT MQTT Example

```text
factory/machine01/temperature
factory/machine01/vibration
factory/machine01/status
```

```text
ESP32
  │
  ▼
MQTT Broker
  │
  ├──► Monitoring System
  ├──► Database
  └──► Predictive Maintenance
```

---

## 🆚 43. MQTT vs. HTTP/REST

| Characteristic | MQTT | HTTP/REST |
|---|---|---|
| Communication model | Publish/subscribe | Request/response |
| Main intermediary | Broker | Web/API server |
| Push messaging | Natural | Requires additional mechanisms |
| IoT telemetry | Very suitable | Suitable |
| Browser integration | Less direct | Excellent |
| Protocol overhead | Generally lower | Generally higher |
| Topic-based routing | Yes | No |
| Resource-oriented API | No | Yes |

---

## 🔄 44. MQTT and REST Together

```text
ESP32 Sensors
     │
     │ MQTT
     ▼
MQTT Broker
     │
     ▼
Backend
     │
     ├── Database
     └── REST API
              │
              ▼
         Web Application
```

---

## ⚙️ 45. MQTT with FreeRTOS

```text
Sensor Task
     │
     ▼
    Queue
     │
     ▼
MQTT Task
     │
     ▼
Wi-Fi
     │
     ▼
MQTT Broker
```

---

## 📤 46. Queue-Based MQTT Publishing

```cpp
struct SensorData
{
    float temperature;
    float humidity;
};
```

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
MQTT Task
  │
  ▼
JSON Encoding
  │
  ▼
MQTT Publish
```

---

## 📥 47. MQTT Commands and FreeRTOS

```text
MQTT Broker
     │
     ▼
MQTT Callback
     │
     ▼
Command Queue
     │
     ▼
Control Task
     │
     ▼
Actuator
```

---

## 🔒 48. MQTT Security

Security mechanisms may include:

- TLS,
- username/password authentication,
- client certificates,
- access-control lists,
- topic authorization,
- secure credential storage,
- network segmentation.

```text
ESP32
  │
  │ MQTT + TLS
  ▼
Authenticated Broker
  │
  ▼
Authorized Subscribers
```

---

## 🔐 49. MQTT over TLS

$$
\boxed{
\text{Secure MQTT Transport}
=
\text{MQTT over TLS}
}
$$

TLS can provide:

- confidentiality,
- integrity,
- server authentication,
- optional client authentication.

---

## 🔑 50. MQTT Authentication

A broker may require:

```text
Client ID
Username
Password
```

Authentication identifies clients; authorization controls topic access.

---

## 🛡️ 51. Topic Authorization

A device may be permitted to publish:

```text
building/room1/temperature
```

but not:

```text
building/admin/command
```

---

## 🧠 52. Client ID

Examples:

```text
ESP32-LAB-01
ESP32-LAB-02
ESP32-ROOM1-01
```

A naming strategy may use:

$$
\boxed{
\text{Device Type}
+
\text{Location}
+
\text{Device Number}
}
$$

---

## 🧹 53. Session State

MQTT can preserve certain session information across reconnects depending on MQTT version and connection settings.

```text
Client Disconnects
       │
       ▼
Session State?
       │
       ├── Preserved → Resume
       └── Removed   → New state
```

---

## 📊 54. MQTT Performance Considerations

Important factors include:

- message frequency,
- payload size,
- QoS,
- number of subscribers,
- network latency,
- broker performance,
- Wi-Fi quality,
- TLS overhead,
- reconnection frequency.

Message rate:

$$
R_m=
\frac{N_m}{T}
$$

---

## 📦 55. MQTT Data Rate

Approximate application payload rate:

$$
R_{payload}=S f
$$

where:

- $S$ = payload bytes,
- $f$ = messages per second.

Example:

$$
S=100~\text{bytes},\qquad f=10~\text{messages/s}
$$

$$
R_{payload}=1000~\text{bytes/s}
$$

---

## ⚖️ 56. QoS and Resource Trade-Off

$$
\boxed{
\text{Higher QoS}
\Rightarrow
\text{Greater Delivery Assurance}
+
\text{More Protocol Overhead}
}
$$

---

## 🔋 57. MQTT and Power Consumption

```text
Measure
   │
   ▼
Buffer
   │
   ▼
Publish periodically
   │
   ▼
Sleep
```

Reducing radio activity can reduce average power consumption.

---

## 🧠 58. Edge Processing Before MQTT

```text
Raw Sensor
    │
    ▼
ESP32
    │
    ├── Filtering
    ├── Averaging
    ├── Feature Extraction
    └── AI Inference
            │
            ▼
       MQTT Publish
```

Example:

```json
{
  "machine": "M01",
  "state": "abnormal",
  "rms": 0.74
}
```

---

## 🤖 59. MQTT + Embedded AI

```text
Sensor
  │
  ▼
ESP32
  │
  ▼
TinyML / AI
  │
  ▼
Inference Result
  │
  ▼
MQTT Broker
  │
  ▼
Monitoring System
```

$$
\boxed{
\text{Embedded Intelligence}
+
\text{MQTT Connectivity}
=
\text{Connected Intelligence}
}
$$

---

## 🪞 60. MQTT and Digital Twins

```text
Physical Sensor
      │
      ▼
    ESP32
      │
      │ MQTT
      ▼
 MQTT Broker
      │
      ▼
Digital Twin
      │
      ▼
Dashboard / Analytics
```

---

## 🚨 61. MQTT Error Handling

An ESP32 should handle:

```text
Wi-Fi unavailable
Broker unavailable
Authentication rejected
Connection lost
Publish failure
Subscription failure
TLS failure
Malformed payload
```

---

## 🔁 62. MQTT Reconnection Strategy

Exponential backoff:

$$
T_n=
\min(T_{\max},T_0 2^n)
$$

Example:

```text
1 s
2 s
4 s
8 s
16 s
...
```

---

## 📋 63. Recommended ESP32 MQTT Architecture

```text
┌─────────────────┐
│   Sensor Task   │
└────────┬────────┘
         │
         ▼
       Queue
         │
         ▼
┌─────────────────┐
│ Processing Task │
└────────┬────────┘
         │
         ▼
       Queue
         │
         ▼
┌─────────────────┐
│    MQTT Task    │
└────────┬────────┘
         │
         ▼
       Wi-Fi
         │
         ▼
┌─────────────────┐
│   MQTT Broker   │
└─────────────────┘
```

Incoming commands:

```text
MQTT Broker
     │
     ▼
MQTT Task
     │
     ▼
Command Queue
     │
     ▼
Control Task
     │
     ▼
Actuator
```

---

## 🧪 64. Exercise 1 — MQTT Broker

Identify:

```text
Broker IP
Broker Port
Username
Password
TLS Setting
```

Local lab architecture:

```text
ESP32
  │
  ▼
Wi-Fi Router
  │
  ▼
Laptop
  │
  ▼
Mosquitto
```

---

## 📤 65. Exercise 2 — ESP32 Publisher

Publish:

```text
Topic:
lab/esp32/temperature
```

every:

$$
5~s
$$

Example payload:

```text
28.5
```

---

## 📥 66. Exercise 3 — ESP32 Subscriber

Subscribe to:

```text
lab/esp32/led
```

Use:

```text
ON
OFF
```

to control an LED.

---

## 📦 67. Exercise 4 — JSON Sensor Data

Publish:

```json
{
  "temperature": 28.5,
  "humidity": 70
}
```

to:

```text
lab/esp32/environment
```

---

## ⭐ 68. Exercise 5 — Topic Wildcards

Create:

```text
lab/room1/temperature
lab/room1/humidity
lab/room2/temperature
lab/room2/humidity
```

Test:

```text
lab/+/temperature
```

and:

```text
lab/#
```

---

## 📊 69. Exercise 6 — Compare QoS

Compare:

| Metric | QoS 0 | QoS 1 | QoS 2 |
|---|---:|---:|---:|
| Delivery semantics | At most once | At least once | Exactly once |
| Protocol interaction | Low | Medium | High |
| Observed latency | | | |
| Messages received | | | |
| Duplicate possibility | Low-level retransmission not used | Possible | Prevented by QoS flow |

---

## 📌 70. Exercise 7 — Retained Messages

Publish:

```text
Topic:
lab/esp32/status

Payload:
ONLINE
```

with retain enabled. Reconnect a subscriber and observe the retained value.

---

## 📴 71. Exercise 8 — Last Will

Configure:

```text
Topic:
lab/esp32/status

Payload:
OFFLINE
```

Disconnect the ESP32 unexpectedly and observe the will message.

---

## 🔄 72. Exercise 9 — Broker Failure

1. Stop the MQTT broker.
2. Observe ESP32 behavior.
3. Restart the broker.
4. Measure reconnection time.
5. Verify subscriptions.

| Test | Result |
|---|---|
| Broker stopped | |
| Failure detected | |
| Retry interval | |
| Broker restarted | |
| Reconnected | |
| Subscription restored | |

---

## 🚀 73. Advanced Exercise — ESP32 MQTT IoT Node

Requirements:

1. Connect ESP32 to Wi-Fi.
2. Connect to an MQTT broker.
3. Read sensor data every 1 s.
4. Publish every 5 s.
5. Encode data using JSON.
6. Subscribe to an LED-control topic.
7. Implement Wi-Fi reconnection.
8. Implement MQTT reconnection.
9. Select an appropriate QoS.
10. Implement retained status where useful.
11. Configure an LWT message.
12. Use a FreeRTOS queue between sensor and MQTT tasks.

---

## 💬 74. Discussion Questions

1. What does MQTT stand for?
2. Why is MQTT suitable for IoT?
3. What is publish/subscribe communication?
4. What is an MQTT publisher?
5. What is an MQTT subscriber?
6. What is an MQTT broker?
7. What is an MQTT topic?
8. Why are hierarchical topics useful?
9. What does `+` mean?
10. What does `#` mean?
11. What is MQTT QoS?
12. Explain QoS 0.
13. Explain QoS 1.
14. Explain QoS 2.
15. Why can QoS 1 produce duplicates?
16. Why does QoS 2 have greater overhead?
17. Is QoS 2 always the best choice?
18. What is a retained message?
19. What is Last Will and Testament?
20. What is MQTT Keep Alive?
21. Why does an ESP32 need reconnection logic?
22. What is a Client ID?
23. How does MQTT differ from REST?
24. Why should MQTT Internet deployments use TLS?
25. How can FreeRTOS queues improve MQTT architecture?

---

## 🧾 75. Expected Learning Outcomes

Students should be able to:

- explain MQTT architecture,
- describe publish/subscribe communication,
- identify publishers, subscribers, and brokers,
- design hierarchical topics,
- use topic wildcards,
- explain MQTT payloads,
- distinguish QoS 0, 1, and 2,
- select suitable QoS,
- explain retained messages,
- explain LWT,
- describe Keep Alive,
- connect ESP32 to an MQTT broker,
- publish sensor data,
- subscribe to control topics,
- process MQTT messages,
- use JSON payloads,
- implement Wi-Fi and MQTT reconnection,
- explain MQTT security,
- integrate MQTT with FreeRTOS tasks and queues,
- compare MQTT with HTTP/REST,
- design MQTT-based IoT architectures.

---

## 🔑 76. Key Concepts

### MQTT Architecture

$$
\boxed{
\text{Publisher}
\rightarrow
\text{Broker}
\rightarrow
\text{Subscriber}
}
$$

### MQTT Stack

$$
\boxed{
\text{MQTT}
\rightarrow
\text{TCP}
\rightarrow
\text{IP}
\rightarrow
\text{Wi-Fi}
}
$$

### QoS 0

$$
\boxed{
\text{QoS 0}
=
\text{At Most Once}
}
$$

### QoS 1

$$
\boxed{
\text{QoS 1}
=
\text{At Least Once}
}
$$

### QoS 2

$$
\boxed{
\text{QoS 2}
=
\text{Exactly Once}
}
$$

### IoT MQTT Architecture

$$
\boxed{
\text{Sensor}
\rightarrow
\text{ESP32}
\rightarrow
\text{MQTT Broker}
\rightarrow
\text{Application}
}
$$

### FreeRTOS MQTT Architecture

$$
\boxed{
\text{Sensor Task}
\rightarrow
\text{Queue}
\rightarrow
\text{MQTT Task}
\rightarrow
\text{Broker}
}
$$

---

## 🎯 77. Summary

MQTT is a lightweight messaging protocol particularly useful for **embedded systems and IoT applications**. It employs a **publish/subscribe architecture** centered around a broker.

Publishers send messages to topics, subscribers register topic filters, and the broker routes messages to matching subscribers. This decoupling makes MQTT suitable for systems involving many sensors, dashboards, databases, control systems, and cloud applications.

MQTT defines three QoS levels:

$$
\boxed{
\begin{aligned}
\text{QoS 0} &: \text{At most once} \\
\text{QoS 1} &: \text{At least once} \\
\text{QoS 2} &: \text{Exactly once}
\end{aligned}
}
$$

Higher QoS provides stronger delivery guarantees but requires more protocol interaction and resources. QoS should therefore be selected according to the application requirements.

MQTT also provides useful IoT mechanisms such as **retained messages, Last Will and Testament, Keep Alive, session state, topic wildcards, authentication, and TLS support**.

For ESP32 FreeRTOS applications, a useful architecture is:

$$
\boxed{
\text{Sensor}
\rightarrow
\text{Sensor Task}
\rightarrow
\text{Queue}
\rightarrow
\text{MQTT Task}
\rightarrow
\text{Wi-Fi}
\rightarrow
\text{Broker}
}
$$

This provides a strong foundation for **IoT telemetry, smart homes, industrial monitoring, smart traffic, digital twins, edge AI, and connected intelligence**.

---

## 📘 References

1. OASIS, *MQTT Version 5.0*.
2. OASIS, *MQTT Version 3.1.1*.
3. Espressif Systems, *ESP-IDF Programming Guide — MQTT*.
4. Espressif Systems, *ESP-IDF Programming Guide — Wi-Fi*.
5. Eclipse Foundation, *Eclipse Mosquitto Documentation*.
6. FreeRTOS, *FreeRTOS Kernel Documentation*.
7. A. Banks, E. Briggs, K. Borgendale, and R. Gupta, *MQTT Version 5.0*, OASIS Standard.
8. J. F. Kurose and K. W. Ross, *Computer Networking: A Top-Down Approach*.
9. A. S. Tanenbaum and D. J. Wetherall, *Computer Networks*.
10. P. Varga et al., “Making System of Systems Interoperable — The Core Components of the Arrowhead Framework,” *Journal of Network and Computer Applications*.

---

