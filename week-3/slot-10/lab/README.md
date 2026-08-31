# LAB 10 — MQTT Telemetry & Remote Control Using JSON

## 1. Objective

The objective of this lab is to:

1. Understand the **MQTT publish/subscribe communication model**.
2. Connect an ESP32 to an **MQTT broker** through Wi-Fi.
3. Publish sensor measurements as **JSON-formatted MQTT messages**.
4. Subscribe to MQTT topics for receiving **remote control commands**.
5. Control an ESP32 output, such as an LED, from MQTT messages.
6. Understand MQTT **topics, clients, brokers, payloads, and Quality of Service (QoS)**.
7. Integrate sensor acquisition and MQTT communication into an IoT architecture.
8. Evaluate message latency and communication reliability.

---

## 2. Background

HTTP communication from the previous lab follows a **request-response** architecture. The client sends a request and waits for a server response.

MQTT uses a different communication model known as **publish/subscribe**.

Instead of communicating directly with each other, MQTT devices exchange messages through an **MQTT broker**.

```text
                    MQTT Broker
                  ┌───────────────┐
                  │               │
 Sensor Data ────►│    Topics     │────► Subscriber
   Publisher      │               │
                  └───────────────┘
                         ▲
                         │
                  Control Command
```

For an ESP32 IoT system:

```text
Sensor
   ↓
ESP32
   ↓
Publish JSON
   ↓
MQTT Broker
   ↓
Dashboard / Application
```

Remote commands can travel in the opposite direction:

```text
Dashboard / Application
          ↓
    Publish Command
          ↓
      MQTT Broker
          ↓
     ESP32 Subscribe
          ↓
        Actuator
```

This makes MQTT particularly useful for **IoT telemetry and remote control**.

### Key Concepts

- **MQTT:** Message Queuing Telemetry Transport.
- **Broker:** Server responsible for receiving and distributing MQTT messages.
- **Client:** Device or application connected to the MQTT broker.
- **Publisher:** Client that sends messages.
- **Subscriber:** Client that receives messages.
- **Topic:** Hierarchical name used to route MQTT messages.
- **Payload:** Data contained in an MQTT message.
- **QoS:** Quality of Service level controlling message-delivery guarantees.
- **Telemetry:** Data transmitted from devices, such as sensor measurements.
- **JSON:** Structured text format commonly used for IoT payloads.

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Wi-Fi access point
- LED and resistor
- Analog sensor or potentiometer
- Optional temperature/humidity sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- MQTT client application

### Arduino Libraries

The examples use:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
```

For structured JSON messages:

```cpp
#include <ArduinoJson.h>
```

Install the required libraries through the Arduino Library Manager.

---

## 4. MQTT System Architecture

A complete IoT architecture can be represented as:

```text
                 ┌─────────────────┐
                 │   MQTT Broker   │
                 └────────┬────────┘
                          │
            ┌─────────────┴─────────────┐
            │                           │
            ▼                           ▼
       Sensor Topic               Control Topic
            ▲                           │
            │                           ▼
      ESP32 Publisher             ESP32 Subscriber
            │                           │
            ▼                           ▼
         Sensor                       LED
```

The ESP32 acts as both:

```text
Publisher  → Sensor Telemetry
Subscriber → Remote Commands
```

---

## 5. MQTT Topics

MQTT organizes communication using **topics**.

For this lab, use:

```text
esp32/sensor
esp32/control
esp32/status
```

Their functions are:

| Topic | Direction | Purpose |
|---|---|---|
| `esp32/sensor` | ESP32 → Broker | Sensor telemetry |
| `esp32/control` | Broker → ESP32 | Remote commands |
| `esp32/status` | ESP32 → Broker | Device status |

A hierarchical topic structure can also be used:

```text
lab/esp3201/sensor/temperature
lab/esp3201/sensor/humidity
lab/esp3201/control/led
lab/esp3201/status
```

This approach becomes useful when multiple devices are connected.

---

## 6. MQTT Quality of Service

MQTT defines three QoS levels:

| QoS | Delivery | Description |
|---:|---|---|
| **0** | At most once | Fast, no delivery guarantee |
| **1** | At least once | Message delivery acknowledged |
| **2** | Exactly once | Highest reliability, greater overhead |

A simplified relationship is:

```text
QoS 0
Low Overhead
     ↓
QoS 1
Higher Reliability
     ↓
QoS 2
Highest Reliability
```

For basic sensor telemetry, **QoS 0 or QoS 1** is commonly sufficient depending on application requirements.

---

## 7. Experiment 1 — Connecting to Wi-Fi

Start by connecting the ESP32 to Wi-Fi.

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
```

### Expected Output

```text
Connecting.....
Wi-Fi connected
IP Address: 192.168.1.105
```

---

## 8. Experiment 2 — Connecting to an MQTT Broker

The ESP32 MQTT client can be created using:

```cpp
WiFiClient espClient;
PubSubClient mqttClient(espClient);
```

The broker is configured using:

```cpp
mqttClient.setServer(
    mqtt_server,
    1883
);
```

Example:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid =
    "YOUR_WIFI_NAME";

const char* password =
    "YOUR_WIFI_PASSWORD";

const char* mqtt_server =
    "YOUR_MQTT_BROKER";

WiFiClient espClient;

PubSubClient mqttClient(
    espClient
);

void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.println(
      "Connecting to MQTT..."
    );

    if (mqttClient.connect(
          "ESP32Client")) {

      Serial.println(
        "MQTT connected"
      );

    } else {

      Serial.print(
        "Failed, state = "
      );

      Serial.println(
        mqttClient.state()
      );

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  WiFi.begin(
    ssid,
    password
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);
  }

  mqttClient.setServer(
    mqtt_server,
    1883
  );

  connectMQTT();
}

void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();
}
```

The connection process is:

```text
ESP32
   ↓
Wi-Fi
   ↓
TCP Connection
   ↓
MQTT Broker
   ↓
MQTT Session
```

---

## 9. Experiment 3 — Publishing a Simple MQTT Message

Use:

```cpp
mqttClient.publish();
```

to publish data.

Example:

```cpp
mqttClient.publish(
    "esp32/sensor",
    "25.5"
);
```

The communication becomes:

```text
ESP32
  │
  │ "25.5"
  ▼
esp32/sensor
  │
  ▼
MQTT Broker
  │
  ▼
Subscribers
```

---

## 10. Experiment 4 — Publishing Sensor Data

Connect an analog sensor to an ADC-capable pin.

```cpp
#define SENSOR_PIN 34
```

Example:

```cpp
void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  int sensorValue =
      analogRead(SENSOR_PIN);

  char message[20];

  snprintf(
    message,
    sizeof(message),
    "%d",
    sensorValue
  );

  mqttClient.publish(
    "esp32/sensor",
    message
  );

  Serial.print(
    "Published: "
  );

  Serial.println(message);

  delay(2000);
}
```

### Expected Output

```text
Published: 2048
Published: 2102
Published: 2150
```

---

## 11. Experiment 5 — Creating a JSON Sensor Payload

Instead of sending a single value:

```text
2048
```

the ESP32 can send structured data:

```json
{
  "device": "ESP32-01",
  "sensor": 2048,
  "rssi": -55
}
```

Using ArduinoJson:

```cpp
#include <ArduinoJson.h>

void publishSensorData() {

  int sensorValue =
      analogRead(SENSOR_PIN);

  JsonDocument doc;

  doc["device"] =
      "ESP32-01";

  doc["sensor"] =
      sensorValue;

  doc["rssi"] =
      WiFi.RSSI();

  char jsonBuffer[256];

  serializeJson(
    doc,
    jsonBuffer
  );

  mqttClient.publish(
    "esp32/sensor",
    jsonBuffer
  );

  Serial.println(
    jsonBuffer
  );
}
```

The telemetry flow becomes:

```text
Sensor
   ↓
ADC Reading
   ↓
JSON Encoding
   ↓
MQTT Publish
   ↓
Broker
```

---

## 12. Experiment 6 — Temperature and Humidity JSON

A more realistic environmental telemetry payload may be:

```json
{
  "device": "ESP32-01",
  "temperature": 25.7,
  "humidity": 61.2,
  "rssi": -52
}
```

Example:

```cpp
JsonDocument doc;

doc["device"] =
    "ESP32-01";

doc["temperature"] =
    25.7;

doc["humidity"] =
    61.2;

doc["rssi"] =
    WiFi.RSSI();

char buffer[256];

serializeJson(
    doc,
    buffer
);

mqttClient.publish(
    "esp32/sensor",
    buffer
);
```

This structure allows several sensor variables to be transported in one MQTT message.

---

## 13. Experiment 7 — Subscribing to Remote Commands

The ESP32 can subscribe to:

```text
esp32/control
```

using:

```cpp
mqttClient.subscribe(
    "esp32/control"
);
```

A callback function processes incoming messages:

```cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length
) {

  Serial.print(
    "Message received: "
  );

  String message = "";

  for (
    unsigned int i = 0;
    i < length;
    i++
  ) {

    message +=
      (char)payload[i];
  }

  Serial.println(message);
}
```

Configure the callback:

```cpp
mqttClient.setCallback(
    callback
);
```

The architecture becomes:

```text
Remote Application
        ↓
Publish Command
        ↓
esp32/control
        ↓
MQTT Broker
        ↓
ESP32 Subscriber
        ↓
callback()
```

---

## 14. Experiment 8 — Remote LED Control

Connect an LED or use the ESP32 onboard LED where supported.

```cpp
#define LED_PIN 2
```

The remote application publishes:

```text
ON
```

or:

```text
OFF
```

to:

```text
esp32/control
```

The ESP32 callback becomes:

```cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length
) {

  String message = "";

  for (
    unsigned int i = 0;
    i < length;
    i++
  ) {

    message +=
      (char)payload[i];
  }

  Serial.print(
    "Command: "
  );

  Serial.println(message);

  if (message == "ON") {

    digitalWrite(
      LED_PIN,
      HIGH
    );

  } else if (
    message == "OFF"
  ) {

    digitalWrite(
      LED_PIN,
      LOW
    );
  }
}
```

The control path is:

```text
Phone / PC
    ↓
"ON"
    ↓
MQTT Broker
    ↓
esp32/control
    ↓
ESP32
    ↓
LED ON
```

---

## 15. Experiment 9 — JSON Remote Control Commands

Remote commands can also use JSON.

Example command:

```json
{
  "led": "ON"
}
```

or:

```json
{
  "led": "OFF"
}
```

The ESP32 can parse the JSON payload:

```cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length
) {

  JsonDocument doc;

  DeserializationError error =
      deserializeJson(
        doc,
        payload,
        length
      );

  if (error) {

    Serial.println(
      "JSON parsing failed"
    );

    return;
  }

  const char* command =
      doc["led"];

  if (
    strcmp(command, "ON") == 0
  ) {

    digitalWrite(
      LED_PIN,
      HIGH
    );

  } else if (
    strcmp(command, "OFF") == 0
  ) {

    digitalWrite(
      LED_PIN,
      LOW
    );
  }
}
```

This produces a fully structured command system:

```text
Remote Application
       ↓
JSON Command
       ↓
MQTT
       ↓
ESP32
       ↓
JSON Parsing
       ↓
Actuator Control
```

---

## 16. Experiment 10 — Publishing Device Status

After executing a command, the ESP32 can publish its current status.

For example:

```json
{
  "device": "ESP32-01",
  "led": "ON"
}
```

Publish to:

```text
esp32/status
```

Example:

```cpp
void publishLEDStatus(
    bool state
) {

  JsonDocument doc;

  doc["device"] =
      "ESP32-01";

  doc["led"] =
      state ? "ON" : "OFF";

  char buffer[128];

  serializeJson(
    doc,
    buffer
  );

  mqttClient.publish(
    "esp32/status",
    buffer
  );
}
```

The complete control loop becomes:

```text
User
  ↓
Command
  ↓
MQTT Broker
  ↓
ESP32
  ↓
Actuator
  ↓
Status Message
  ↓
MQTT Broker
  ↓
User
```

This provides **command feedback**.

---

## 17. Complete MQTT Sensor and Remote-Control Example

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_PIN 2
#define SENSOR_PIN 34

const char* ssid =
    "YOUR_WIFI_NAME";

const char* password =
    "YOUR_WIFI_PASSWORD";

const char* mqtt_server =
    "YOUR_MQTT_BROKER";

WiFiClient espClient;

PubSubClient mqttClient(
    espClient
);

unsigned long previousMillis = 0;

void callback(
    char* topic,
    byte* payload,
    unsigned int length
) {

  JsonDocument doc;

  DeserializationError error =
      deserializeJson(
        doc,
        payload,
        length
      );

  if (error) {

    Serial.println(
      "Invalid JSON command"
    );

    return;
  }

  const char* command =
      doc["led"];

  if (command == NULL) {
    return;
  }

  if (
    strcmp(command, "ON") == 0
  ) {

    digitalWrite(
      LED_PIN,
      HIGH
    );

    Serial.println(
      "LED ON"
    );

  } else if (
    strcmp(command, "OFF") == 0
  ) {

    digitalWrite(
      LED_PIN,
      LOW
    );

    Serial.println(
      "LED OFF"
    );
  }
}

void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print(
      "Connecting to MQTT..."
    );

    if (
      mqttClient.connect(
        "ESP32Client"
      )
    ) {

      Serial.println(
        "connected"
      );

      mqttClient.subscribe(
        "esp32/control"
      );

    } else {

      Serial.println(
        "failed"
      );

      delay(2000);
    }
  }
}

void publishSensor() {

  int sensorValue =
      analogRead(SENSOR_PIN);

  JsonDocument doc;

  doc["device"] =
      "ESP32-01";

  doc["sensor"] =
      sensorValue;

  doc["rssi"] =
      WiFi.RSSI();

  char buffer[256];

  serializeJson(
    doc,
    buffer
  );

  mqttClient.publish(
    "esp32/sensor",
    buffer
  );

  Serial.print(
    "Published: "
  );

  Serial.println(buffer);
}

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  WiFi.begin(
    ssid,
    password
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "Wi-Fi connected"
  );

  mqttClient.setServer(
    mqtt_server,
    1883
  );

  mqttClient.setCallback(
    callback
  );
}

void loop() {

  if (
    !mqttClient.connected()
  ) {

    connectMQTT();
  }

  mqttClient.loop();

  unsigned long currentMillis =
      millis();

  if (
    currentMillis -
    previousMillis >= 5000
  ) {

    previousMillis =
        currentMillis;

    publishSensor();
  }
}
```

---

## 18. Expected MQTT Operation

After successful connection, the Serial Monitor should display messages similar to:

```text
Wi-Fi connected
Connecting to MQTT...connected

Published:
{"device":"ESP32-01","sensor":2054,"rssi":-51}

Published:
{"device":"ESP32-01","sensor":2071,"rssi":-52}

LED ON

Published:
{"device":"ESP32-01","sensor":2103,"rssi":-53}

LED OFF
```

---

## 19. Experiment 11 — FreeRTOS MQTT Architecture

The system can be extended using the concepts from previous labs.

A recommended architecture is:

```text
┌─────────────┐
│ Sensor Task │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│    Queue    │
└──────┬──────┘
       │
       ▼
┌──────────────────┐
│ MQTT Publish Task│
└──────┬───────────┘
       │
       ▼
   MQTT Broker
       │
       ▼
Remote Application
       │
       │ Control Command
       ▼
   MQTT Broker
       │
       ▼
┌──────────────────┐
│ MQTT Client Task │
└──────┬───────────┘
       │
       ▼
    Actuator
```

This architecture separates:

- Sensor acquisition
- Data buffering
- Network communication
- Command processing
- Actuator control

---

## 20. MQTT Telemetry Timing

Suppose sensor measurements are generated every:

$$
T_s = 1~\text{s}
$$

while MQTT telemetry is published every:

$$
T_p = 5~\text{s}.
$$

The corresponding rates are:

$$
f_s = \frac{1}{T_s}
$$

and

$$
f_p = \frac{1}{T_p}.
$$

Therefore:

$$
f_s = 1~\text{Hz}
$$

and

$$
f_p = 0.2~\text{Hz}.
$$

This demonstrates that **sensor sampling rate and network transmission rate do not need to be identical**.

---

## 21. Measuring MQTT Command Latency

Remote-control latency can be defined as:

$$
T_{\text{control}}
= t_{\text{actuator}}
- t_{\text{command}}.
$$

where:

- $t_{\text{command}}$ = time the command is transmitted.
- $t_{\text{actuator}}$ = time the ESP32 executes the command.

A practical experiment can record:

| Test | Command | MQTT QoS | Latency (ms) | Success |
|---:|---|---:|---:|---|
| 1 | ON | 0 |  |  |
| 2 | OFF | 0 |  |  |
| 3 | ON | 1 |  |  |
| 4 | OFF | 1 |  |  |

---

## 22. MQTT vs. HTTP

| Feature | HTTP | MQTT |
|---|---|---|
| Communication model | Request-response | Publish-subscribe |
| Main architecture | Client-server | Broker-based |
| Device coupling | More direct | Decoupled |
| Message overhead | Generally higher | Generally lower |
| Continuous telemetry | Possible | **Well suited** |
| Remote commands | Possible | **Well suited** |
| Topic-based routing | No | **Yes** |
| QoS levels | Not MQTT-style | **0, 1, 2** |
| Typical IoT use | REST APIs | Telemetry/control |

The conceptual difference is:

```text
HTTP

ESP32
  ↓
Request
  ↓
Server
  ↓
Response
```

versus:

```text
MQTT

Publisher
    ↓
  Broker
    ↓
Subscriber
```

---

## 23. Experiments

### Experiment 1 — MQTT Connection

Connect the ESP32 to an MQTT broker.

Record:

- Wi-Fi IP address
- Broker address
- MQTT connection state
- Connection time

### Experiment 2 — Basic Publish

Publish:

```text
Hello MQTT
```

to:

```text
esp32/test
```

Verify the message using another MQTT client.

### Experiment 3 — Sensor Telemetry

Publish an ADC measurement every 2 seconds.

Record:

- Sensor value
- MQTT topic
- Message size
- Transmission interval

### Experiment 4 — JSON Telemetry

Publish:

```json
{
  "device": "ESP32-01",
  "sensor": 2048,
  "rssi": -55
}
```

Verify that the subscriber receives valid JSON.

### Experiment 5 — Remote LED Control

Subscribe to:

```text
esp32/control
```

Send:

```text
ON
```

and:

```text
OFF
```

Verify LED operation.

### Experiment 6 — JSON Remote Control

Send:

```json
{
  "led": "ON"
}
```

and:

```json
{
  "led": "OFF"
}
```

Parse the messages on the ESP32 and control the LED.

### Experiment 7 — Bidirectional MQTT

Implement:

```text
ESP32
 │
 ├──── Sensor JSON ────► MQTT Broker
 │
 │
 └◄── Control JSON ───── MQTT Broker
```

Verify simultaneous telemetry and remote control.

### Experiment 8 — Network Failure

Disconnect Wi-Fi temporarily.

Observe:

- MQTT disconnection
- Reconnection behavior
- Telemetry interruption
- Command availability after reconnection

---

## 24. Exercises

### Exercise 1 — Environmental Telemetry

Create a JSON payload containing:

```json
{
  "device": "ESP32-01",
  "temperature": 25.5,
  "humidity": 60.5,
  "rssi": -50
}
```

Publish the message every 5 seconds.

### Exercise 2 — Multiple Control Commands

Extend the remote command to:

```json
{
  "led": "ON",
  "brightness": 128
}
```

Use PWM to control LED brightness.

### Exercise 3 — Device Status

After receiving a command, publish:

```json
{
  "device": "ESP32-01",
  "led": "ON",
  "status": "OK"
}
```

to:

```text
esp32/status
```

### Exercise 4 — FreeRTOS MQTT Pipeline

Implement:

```text
Sensor Task
      ↓
Queue
      ↓
Processing Task
      ↓
Queue
      ↓
MQTT Task
      ↓
Broker
```

The MQTT task should publish processed sensor data in JSON format.

### Exercise 5 — Communication Reliability

Publish 100 MQTT messages.

Record:

- Number sent
- Number received
- Number lost
- Average latency

Calculate the message delivery ratio:

$$
\text{Delivery Ratio}
= \frac{N_{\text{received}}}
     {N_{\text{sent}}}
\times 100\%.
$$

### Exercise 6 — Complete IoT Node

Develop an ESP32 system that simultaneously:

1. Samples a sensor.
2. Creates a JSON telemetry message.
3. Publishes telemetry to MQTT.
4. Subscribes to remote commands.
5. Controls an LED.
6. Publishes device status.

The final architecture should be:

```text
                   ┌───────────────┐
                   │  MQTT Broker  │
                   └───────┬───────┘
                           │
             ┌─────────────┴─────────────┐
             │                           │
             ▼                           ▼
        Telemetry                    Commands
             ▲                           │
             │                           ▼
        ┌────┴───────────────────────────┴────┐
        │                ESP32                │
        │                                     │
        │ Sensor → JSON → MQTT     MQTT → LED │
        └─────────────────────────────────────┘
```

---

## 25. Checkpoint Questions

1. What is MQTT?
2. What is the role of an MQTT broker?
3. What is the difference between a publisher and a subscriber?
4. What is an MQTT topic?
5. What is an MQTT payload?
6. What is telemetry?
7. What is the difference between MQTT and HTTP?
8. Why is MQTT suitable for IoT applications?
9. What is MQTT QoS?
10. What is the difference between QoS 0, QoS 1, and QoS 2?
11. What does `mqttClient.publish()` do?
12. What does `mqttClient.subscribe()` do?
13. What is the purpose of `mqttClient.loop()`?
14. Why is JSON useful for MQTT telemetry?
15. How can an ESP32 act as both publisher and subscriber?
16. Why should MQTT reconnection be implemented?
17. What happens to telemetry when the broker becomes unavailable?
18. Why are separate telemetry and control topics useful?
19. How can MQTT be combined with FreeRTOS queues?
20. How can MQTT command latency be measured?

---

## 26. Expected Results

Students should successfully establish:

```text
ESP32
  ↓
Wi-Fi
  ↓
MQTT Broker
```

and publish structured telemetry:

```text
Sensor
   ↓
JSON
   ↓
MQTT Publish
   ↓
Broker
   ↓
Subscriber
```

Students should also demonstrate remote actuator control:

```text
Remote Client
     ↓
MQTT Publish
     ↓
Broker
     ↓
ESP32 Subscribe
     ↓
LED / Actuator
```

The final system therefore provides **bidirectional IoT communication**:

$$
\boxed{
\text{Telemetry}
\longrightarrow
\text{Broker}
\longrightarrow
\text{Application}
}
$$

and

$$
\boxed{
\text{Application}
\longrightarrow
\text{Broker}
\longrightarrow
\text{Control}
}
$$

---

## 27. Submission

Submit the following:

- ESP32 MQTT source code.
- Screenshot showing successful MQTT connection.
- Sensor telemetry output.
- JSON telemetry payload.
- MQTT topic configuration.
- Remote LED-control demonstration.
- JSON command implementation.
- Device-status feedback.
- MQTT reliability measurements.
- Short discussion comparing HTTP and MQTT.

Suggested telemetry results table:

| Test | Sensor Value | RSSI (dBm) | JSON Size (bytes) | Publish Success |
|---:|---:|---:|---:|---|
| 1 |  |  |  |  |
| 2 |  |  |  |  |
| 3 |  |  |  |  |
| 4 |  |  |  |  |

Suggested remote-control results table:

| Test | Command | LED State | Latency (ms) | Success |
|---:|---|---|---:|---|
| 1 | ON |  |  |  |
| 2 | OFF |  |  |  |
| 3 | ON |  |  |  |
| 4 | OFF |  |  |  |

---

## 28. Conclusion

In this lab, the ESP32 is developed into a **bidirectional MQTT-based IoT node**. Sensor measurements are encoded as JSON and published to an MQTT broker, while the ESP32 simultaneously subscribes to control topics for receiving remote actuator commands.

Compared with the HTTP request-response approach from the previous lab, MQTT provides a **publish/subscribe architecture** that decouples devices and applications through a broker. This architecture is particularly suitable for continuous telemetry, event-driven communication, and remote control.
