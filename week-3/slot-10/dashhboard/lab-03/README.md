# Lab 3 — Structured IoT Data Using JSON, Multiple Sensors, MQTT, and GitHub Pages

## 1. Introduction

In Lab 1, the ESP32 published a single temperature value to an MQTT broker and displayed it on a GitHub Pages dashboard. In Lab 2, the system was extended to support bidirectional communication, allowing the dashboard to monitor sensor data and remotely control an LED.

In this laboratory, the system is extended again to support **multiple sensor variables using structured JSON messages**.

Instead of publishing several independent values as simple text messages, the ESP32 combines multiple measurements into a single JSON payload.

For example:

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

JSON is widely used in practical IoT applications because it is flexible, lightweight, human-readable, and easy to process in web applications.

The system architecture becomes:

```text
+----------------------+
|   Multiple Sensors   |
| Temperature          |
| Humidity             |
| Light                |
+----------+-----------+
           |
           v
+----------------------+
|        ESP32         |
| Read Sensor Values   |
| Build JSON Payload   |
+----------+-----------+
           |
           | MQTT Publish
           v
+----------------------+
|     MQTT Broker      |
+----------+-----------+
           |
           | MQTT over WSS
           v
+----------------------+
|    GitHub Pages      |
| HTML + JavaScript    |
| MQTT.js              |
+----------+-----------+
           |
           v
+----------------------+
| Structured Dashboard |
+----------------------+
```

---

## 2. Objectives

After completing this laboratory, students should be able to:

1. Explain why structured data is useful in IoT systems.
2. Describe the basic structure of JSON.
3. Generate JSON messages on an ESP32.
4. Publish multiple sensor variables in one MQTT message.
5. Parse JSON data using JavaScript.
6. Display multiple sensor values on a GitHub Pages dashboard.
7. Compare separate MQTT topics with a single structured JSON topic.
8. Design MQTT topics for structured IoT communication.
9. Add device metadata to an MQTT payload.
10. Extend an IoT dashboard to display multiple measurements.

---

## 3. System Architecture

```text
+---------------------+
|       Sensors       |
|                     |
| Temperature         |
| Humidity            |
| Light               |
+----------+----------+
           |
           v
+---------------------+
|       ESP32         |
|                     |
| Read Sensors        |
| Build JSON          |
| MQTT Publish        |
+----------+----------+
           |
           | MQTT/TCP
           v
+---------------------+
|    MQTT Broker      |
+----------+----------+
           |
           | MQTT over WSS
           v
+---------------------+
|    GitHub Pages     |
|     Dashboard       |
|                     |
| Temperature         |
| Humidity            |
| Light               |
+---------------------+
```

---

## 4. Why Use JSON?

In earlier laboratories, an MQTT message may contain only one value.

Example:

```text
Topic:   iot/lab/temperature
Payload: 28.5
```

If three measurements are required, one possible design is:

```text
iot/lab/temperature -> 28.5
iot/lab/humidity    -> 72.0
iot/lab/light       -> 640
```

This works, but the application must manage several MQTT topics.

Another approach is to publish all related measurements together:

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Using JSON keeps related measurements together as one sensor record.

---

## 5. JSON Structure

JSON stands for **JavaScript Object Notation**.

A JSON object contains key-value pairs:

```json
{
  "temperature": 28.5,
  "humidity": 72.0
}
```

The keys are:

```text
temperature
humidity
```

The values are:

```text
28.5
72.0
```

JSON can also contain strings:

```json
{
  "device": "ESP32-01",
  "status": "ONLINE"
}
```

and Boolean values:

```json
{
  "led": true
}
```

---

## 6. MQTT Topic Design

For this laboratory, use one main sensor topic:

```text
iot/lab/sensors
```

Example payload:

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Communication:

```text
ESP32
  |
  | Publish JSON
  v
iot/lab/sensors
  |
  v
MQTT Broker
  |
  | WSS
  v
GitHub Pages
```

For a shared public broker, students should use a unique topic, for example:

```text
iot/lab/student01/sensors
```

> Replace `student01` with a unique student or group identifier.

---

# Part A — Simulated Multiple Sensors

## 7. Simulated Sensor Values

Physical sensors are optional in this laboratory. The ESP32 can generate simulated values:

```cpp
float temperature = random(250, 351) / 10.0;
float humidity    = random(400, 901) / 10.0;
int light         = random(0, 1024);
```

Example output:

```text
Temperature: 28.4 °C
Humidity:    71.2 %
Light:       640
```

---

## 8. Sensor Data Model

| Variable | Example | Unit |
|---|---:|---|
| Temperature | 28.5 | °C |
| Humidity | 72.0 | % |
| Light | 640 | ADC level |

These measurements will be encoded into one JSON message.

---

# Part B — ESP32 JSON Publisher

## 9. JSON Message Construction

A simple JSON message can be created manually:

```cpp
String json =
  "{"
  "\"temperature\":" + String(temperature, 1) + ","
  "\"humidity\":"    + String(humidity, 1) + ","
  "\"light\":"       + String(light) +
  "}";
```

The resulting payload may look like:

```json
{
  "temperature": 28.5,
  "humidity": 72.3,
  "light": 640
}
```

---

## 10. Complete ESP32 Program

This example uses the public HiveMQ broker.

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

// --------------------------------------
// Wi-Fi Configuration
// --------------------------------------

const char* ssid =
  "YOUR_WIFI_NAME";

const char* password =
  "YOUR_WIFI_PASSWORD";

// --------------------------------------
// MQTT Configuration
// --------------------------------------

const char* mqtt_server =
  "broker.hivemq.com";

const int mqtt_port = 1883;

const char* mqtt_topic =
  "iot/lab/student01/sensors";

// Publish every 2 seconds
const unsigned long PUBLISH_INTERVAL =
  2000;

// --------------------------------------
// MQTT Client
// --------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;

// --------------------------------------
// Connect Wi-Fi
// --------------------------------------

void connectWiFi()
{
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(
    ssid,
    password
  );

  while (
    WiFi.status() != WL_CONNECTED
  )
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// --------------------------------------
// Connect MQTT
// --------------------------------------

void connectMQTT()
{
  while (!client.connected())
  {
    Serial.print(
      "Connecting to MQTT..."
    );

    String clientId =
      "ESP32-Lab3-" +
      String(
        random(0xffff),
        HEX
      );

    if (
      client.connect(
        clientId.c_str()
      )
    )
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.println(
        client.state()
      );

      delay(2000);
    }
  }
}

// --------------------------------------
// Setup
// --------------------------------------

void setup()
{
  Serial.begin(115200);

  randomSeed(
    analogRead(0)
  );

  connectWiFi();

  client.setServer(
    mqtt_server,
    mqtt_port
  );
}

// --------------------------------------
// Main Loop
// --------------------------------------

void loop()
{
  if (!client.connected())
  {
    connectMQTT();
  }

  client.loop();

  if (
    millis() - lastPublishTime
    >= PUBLISH_INTERVAL
  )
  {
    lastPublishTime =
      millis();

    // ------------------------------
    // Simulated Sensor Data
    // ------------------------------

    float temperature =
      random(250, 351) / 10.0;

    float humidity =
      random(400, 901) / 10.0;

    int light =
      random(0, 1024);

    // ------------------------------
    // Build JSON
    // ------------------------------

    String json =
      "{"
      "\"temperature\":" +
      String(temperature, 1) +
      ","
      "\"humidity\":" +
      String(humidity, 1) +
      ","
      "\"light\":" +
      String(light) +
      "}";

    // ------------------------------
    // Publish MQTT Message
    // ------------------------------

    client.publish(
      mqtt_topic,
      json.c_str()
    );

    Serial.println(
      json
    );
  }
}
```

> The browser dashboard must subscribe to the **same MQTT topic** used by the ESP32.

---

## 11. Expected Serial Monitor Output

Example:

```text
Wi-Fi connected
IP Address: 192.168.1.105
Connecting to MQTT...connected

{"temperature":27.8,"humidity":68.5,"light":620}
{"temperature":28.2,"humidity":70.1,"light":645}
{"temperature":28.6,"humidity":71.4,"light":690}
```

Each line is one complete JSON object.

---

# Part C — GitHub Pages JSON Dashboard

## 12. Dashboard Requirement

The dashboard should display:

```text
+----------------------------+
|      IoT Dashboard         |
|                            |
| Temperature                |
|      28.5 °C               |
|                            |
| Humidity                   |
|      72.0 %                |
|                            |
| Light                      |
|      640                   |
|                            |
| MQTT Connected             |
+----------------------------+
```

---

## 13. Parsing JSON in JavaScript

When an MQTT message arrives, it is initially a text string:

```text
{"temperature":28.5,"humidity":72.0,"light":640}
```

Convert the string into a JavaScript object using:

```javascript
const data =
  JSON.parse(
    message.toString()
  );
```

Then access individual values:

```javascript
data.temperature
data.humidity
data.light
```

---

## 14. Complete Dashboard Program

Create an `index.html` file:

```html
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>
    Lab 3 IoT Dashboard
  </title>

  <script
    src="https://unpkg.com/mqtt/dist/mqtt.min.js">
  </script>

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 30px;
      background: #f4f6f8;
    }

    .dashboard {
      width: 420px;
      max-width: 90%;
      margin: auto;
      padding: 30px;
      background: white;
      border: 1px solid #ccc;
      border-radius: 12px;
    }

    .value {
      font-size: 36px;
      margin-bottom: 20px;
    }

    #status {
      font-weight: bold;
    }
  </style>
</head>

<body>

  <div class="dashboard">

    <h1>
      IoT Sensor Dashboard
    </h1>

    <h2>
      Temperature
    </h2>

    <div
      id="temperature"
      class="value">
      -- °C
    </div>

    <h2>
      Humidity
    </h2>

    <div
      id="humidity"
      class="value">
      -- %
    </div>

    <h2>
      Light
    </h2>

    <div
      id="light"
      class="value">
      --
    </div>

    <p id="status">
      Connecting...
    </p>

  </div>

  <script>

    // --------------------------------------
    // MQTT Configuration
    // --------------------------------------

    const broker =
      "wss://broker.hivemq.com:8884/mqtt";

    const topic =
      "iot/lab/student01/sensors";

    // --------------------------------------
    // Connect MQTT
    // --------------------------------------

    const client =
      mqtt.connect(
        broker,
        {
          clean: true,
          connectTimeout: 4000,
          reconnectPeriod: 2000
        }
      );

    // --------------------------------------
    // MQTT Connected
    // --------------------------------------

    client.on(
      "connect",
      function()
      {
        document.getElementById(
          "status"
        ).innerText =
          "MQTT Connected";

        client.subscribe(
          topic
        );
      }
    );

    // --------------------------------------
    // Receive MQTT Message
    // --------------------------------------

    client.on(
      "message",
      function(topic, message)
      {
        try
        {
          const data =
            JSON.parse(
              message.toString()
            );

          // Basic validation
          if (
            typeof data.temperature
              === "number"
          )
          {
            document.getElementById(
              "temperature"
            ).innerText =
              data.temperature +
              " °C";
          }

          if (
            typeof data.humidity
              === "number"
          )
          {
            document.getElementById(
              "humidity"
            ).innerText =
              data.humidity +
              " %";
          }

          if (
            typeof data.light
              === "number"
          )
          {
            document.getElementById(
              "light"
            ).innerText =
              data.light;
          }
        }

        catch(error)
        {
          console.log(
            "JSON Error:",
            error
          );
        }
      }
    );

    // --------------------------------------
    // MQTT Error
    // --------------------------------------

    client.on(
      "error",
      function(error)
      {
        document.getElementById(
          "status"
        ).innerText =
          "MQTT Error";

        console.log(error);
      }
    );

    // --------------------------------------
    // MQTT Offline
    // --------------------------------------

    client.on(
      "offline",
      function()
      {
        document.getElementById(
          "status"
        ).innerText =
          "MQTT Disconnected";
      }
    );

  </script>

</body>

</html>
```

---

# Part D — System Integration

## 15. Complete Data Flow

```text
Temperature
Humidity
Light
   |
   v
ESP32
   |
   | Build JSON
   v
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
   |
   | MQTT Publish
   v
MQTT Broker
   |
   | MQTT over WSS
   v
GitHub Pages
   |
   | JSON.parse()
   v
Dashboard
```

---

## 16. Experiment 1 — JSON Transmission

1. Run the ESP32.
2. Open the Serial Monitor.
3. Record five JSON messages.
4. Verify that all three sensor variables are included.

| Message | JSON Payload |
|---:|---|
| 1 | |
| 2 | |
| 3 | |
| 4 | |
| 5 | |

---

## 17. Experiment 2 — Dashboard Parsing

Open the GitHub Pages dashboard.

Compare the ESP32 JSON:

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

with the dashboard:

```text
Temperature: 28.5 °C
Humidity:    72.0 %
Light:       640
```

Verify that each JSON field is displayed correctly.

---

## 18. Experiment 3 — Add Device Information

Extend the JSON message to include a device identifier:

```json
{
  "device": "ESP32-01",
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Modify the ESP32 JSON construction:

```cpp
String json =
  "{"
  "\"device\":\"ESP32-01\","
  "\"temperature\":" + String(temperature, 1) + ","
  "\"humidity\":"    + String(humidity, 1) + ","
  "\"light\":"       + String(light) +
  "}";
```

Modify the dashboard to display:

```text
Device: ESP32-01
```

---

## 19. Experiment 4 — Add Message Counter

Add a sequence number:

```json
{
  "device": "ESP32-01",
  "sequence": 125,
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Declare:

```cpp
unsigned long sequence = 0;
```

Before creating each JSON message:

```cpp
sequence++;
```

The dashboard can display:

```text
Message #: 125
```

---

## 20. Experiment 5 — Timestamp Concept

A sensor record often includes time information:

```json
{
  "device": "ESP32-01",
  "sequence": 125,
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640,
  "timestamp": 12345678
}
```

For this laboratory, the timestamp may initially represent milliseconds since the ESP32 started:

```cpp
millis()
```

A later laboratory can introduce real date and time using NTP.

---

# Part E — JSON Design

## 21. Flat JSON Structure

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

A flat structure is easy to read and suitable for simple IoT devices.

---

## 22. Nested JSON Structure

A more structured format is:

```json
{
  "device": "ESP32-01",
  "sensors": {
    "temperature": 28.5,
    "humidity": 72.0,
    "light": 640
  }
}
```

JavaScript accesses the values using:

```javascript
data.sensors.temperature
data.sensors.humidity
data.sensors.light
```

Nested structures can be useful as an IoT application becomes larger.

---

## 23. Metadata

IoT messages can contain both sensor values and metadata:

```json
{
  "device": "ESP32-01",
  "location": "Lab-A",
  "sequence": 125,
  "sensors": {
    "temperature": 28.5,
    "humidity": 72.0,
    "light": 640
  }
}
```

Here:

```text
device
location
sequence
```

are metadata, while the measurements are stored under:

```text
sensors
```

---

# Part F — Multiple Devices

## 24. Topic-Based Device Separation

Two ESP32 devices may publish to:

```text
iot/lab/device01/sensors
iot/lab/device02/sensors
```

A dashboard may subscribe to:

```text
iot/lab/+/sensors
```

The MQTT `+` character is a **single-level wildcard**.

This allows one dashboard to receive sensor data from multiple devices.

---

## 25. Device Data Example

Device 1:

```json
{
  "device": "ESP32-01",
  "temperature": 28.5,
  "humidity": 72.0
}
```

Device 2:

```json
{
  "device": "ESP32-02",
  "temperature": 29.1,
  "humidity": 68.0
}
```

The dashboard could display:

```text
ESP32-01
Temperature: 28.5 °C
Humidity:    72.0 %

ESP32-02
Temperature: 29.1 °C
Humidity:    68.0 %
```

---

# Part G — Comparison of Message Designs

## 26. Separate Topics

Example:

```text
iot/lab/temperature
iot/lab/humidity
iot/lab/light
```

Advantages:

- simple payloads;
- easy individual subscriptions.

Disadvantages:

- more MQTT topics;
- related measurements may arrive at different times;
- harder to treat several values as one sensor record.

---

## 27. Single JSON Topic

Topic:

```text
iot/lab/sensors
```

Payload:

```json
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Advantages:

- related measurements stay together;
- easy to extend;
- convenient for databases and APIs;
- metadata can be included naturally.

Disadvantages:

- larger payload;
- requires JSON parsing;
- malformed JSON can cause errors.

---

# Part H — Error Handling

## 28. Invalid JSON

Suppose the received message is:

```text
temperature=28.5
```

This is not valid JSON.

Calling:

```javascript
JSON.parse(message)
```

will generate an error.

Therefore, the dashboard should use:

```javascript
try
{
  const data =
    JSON.parse(
      message.toString()
    );
}
catch(error)
{
  console.log(
    "Invalid JSON"
  );
}
```

This prevents a malformed message from stopping the dashboard script.

---

# Part I — Security Considerations

## 29. Do Not Include Secrets in JSON

An MQTT sensor message should not contain:

```text
Wi-Fi password
MQTT administrator password
Private API key
Private certificate
```

Only transmit information required by the application.

> Public MQTT brokers should be used only for non-sensitive laboratory data.

---

## 30. Validate Received Data

The dashboard should not automatically trust every MQTT message.

For example:

```javascript
if (
  typeof data.temperature
    === "number"
)
{
  // Update display
}
```

Validation becomes more important when an IoT system controls actuators or automated processes.

---

# Part J — Review Questions

## 31. Questions

1. What is JSON?
2. Why is JSON useful in IoT applications?
3. What is the difference between a JSON key and value?
4. Why can multiple sensor variables be placed in one MQTT message?
5. What does `JSON.parse()` do?
6. What happens if the received payload is invalid JSON?
7. What is metadata?
8. What is the advantage of adding a device ID?
9. What is the purpose of a sequence number?
10. What is the difference between flat and nested JSON?
11. What are the advantages of separate MQTT topics?
12. What are the advantages of a single JSON topic?
13. How can multiple ESP32 devices be distinguished?
14. What does the MQTT `+` wildcard mean?
15. Why should IoT applications validate received data?

---

# Part K — Student Assignment

## 32. Required Implementation

Develop an IoT system with the architecture:

```text
Sensors
   |
   v
ESP32
   |
   v
JSON
   |
   v
MQTT Broker
   |
   v
GitHub Pages
```

The JSON payload must contain at least:

```json
{
  "device": "ESP32-XX",
  "temperature": 0,
  "humidity": 0,
  "light": 0
}
```

The dashboard must display:

- device name;
- temperature;
- humidity;
- light value;
- MQTT connection status.

---

## 33. Extension Task

Extend the payload to:

```json
{
  "device": "ESP32-01",
  "sequence": 100,
  "uptime": 125000,
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

Display:

```text
Device:      ESP32-01
Message #:   100
Uptime:      125000 ms
Temperature: 28.5 °C
Humidity:    72.0 %
Light:       640
```

---

## 34. Advanced Extension

Use two ESP32 devices:

```text
ESP32-01
   |
   +----> iot/lab/device01/sensors
   |
   v
+------------------+
|   MQTT Broker    |
+------------------+
   ^
   |
   +----> iot/lab/device02/sensors
   |
ESP32-02
```

The dashboard should show values from both devices.

---

# Part L — Laboratory Report

## 35. Required Report

The report should contain:

### 1. Objective

Explain the purpose of structured data in IoT systems.

### 2. System Architecture

Draw:

```text
Sensors -> ESP32 -> JSON -> MQTT -> GitHub Pages
```

### 3. JSON Design

Show the JSON payload used in the experiment.

### 4. MQTT Configuration

Report:

```text
MQTT Broker:
MQTT Topic:
WebSocket Endpoint:
Publish Interval:
```

Do not include passwords or other secrets.

### 5. ESP32 Program

Explain how the JSON message is generated.

### 6. Dashboard

Explain how JavaScript parses and displays the JSON data.

### 7. Results

Provide screenshots showing:

- MQTT connection;
- temperature;
- humidity;
- light;
- device ID.

### 8. Discussion

Compare:

```text
Multiple MQTT Topics
vs.
Single JSON Message
```

Discuss the advantages and disadvantages of each approach.

### 9. Conclusion

Summarize the role of JSON in IoT communication.

---

## 36. Expected Result

At the end of the laboratory, the complete system should resemble:

```text
+---------------------+
|       ESP32         |
|                     |
| Temp = 28.5 °C      |
| Humidity = 72.0 %   |
| Light = 640         |
+----------+----------+
           |
           | JSON
           v
{
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
           |
           | MQTT
           v
+---------------------+
|    MQTT Broker      |
+----------+----------+
           |
           | WSS
           v
+---------------------+
|    GitHub Pages     |
|                     |
| Temperature 28.5 °C |
| Humidity 72.0 %     |
| Light 640           |
+---------------------+
```

---

## 37. Key Concept

The progression across the first three laboratories is:

```text
Lab 1
Single Sensor Monitoring

ESP32 --------> Dashboard


Lab 2
Monitoring + Control

ESP32 <-------> Dashboard


Lab 3
Structured Multi-Sensor Data

Multiple Sensors
      |
      v
    ESP32
      |
      v
     JSON
      |
      v
 MQTT Broker
      |
      v
  Dashboard
```

Lab 3 introduces the concept of a **structured IoT data model**, which provides a foundation for more advanced systems involving databases, cloud APIs, Node-RED, digital twins, analytics, and machine learning.

---

## 38. Summary

In this laboratory, students extend the ESP32 MQTT system to support multiple sensor variables using JSON.

Instead of transmitting isolated values, the ESP32 creates a structured data object containing measurements and optional metadata. The JSON payload is published through MQTT, received by a GitHub Pages dashboard, parsed with JavaScript, and converted into human-readable information.

The complete data path is:

```text
Sensors
   |
   v
ESP32
   |
   v
JSON
   |
   v
MQTT
   |
   v
Broker
   |
   v
WSS
   |
   v
GitHub Pages Dashboard
```

This laboratory provides the foundation for the next stage: **real-time visualization and historical data analysis**, where incoming MQTT sensor values can be displayed as dynamic charts rather than only numerical values.
