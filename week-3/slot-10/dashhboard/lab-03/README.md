# Lab 3: Structured IoT Data Using JSON, Multiple Sensors, MQTT, and GitHub.io

## 1. Introduction

In Lab 1, the ESP32 published a single temperature value to an MQTT
broker and displayed it on a GitHub.io dashboard. In Lab 2, the system
was extended to support bidirectional communication, allowing the
dashboard to monitor sensor data and remotely control an LED.

In this laboratory, the system is extended again to support **multiple
sensor variables using structured JSON messages**.

Instead of publishing several independent values as simple text
messages, the ESP32 will combine multiple measurements into a single
JSON payload.

For example:

`json id="a7k1m2" {   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

This approach is widely used in practical IoT systems because JSON
provides a flexible and human-readable method for representing
structured data.

The system architecture becomes:

`text id="b6pl2r" Multiple Sensors       ↓     ESP32       ↓  JSON Payload       ↓      MQTT       ↓  MQTT Broker       ↓ MQTT over WSS       ↓   GitHub.io       ↓ Structured Dashboard`

------------------------------------------------------------------------

# 2. Objectives

After completing this laboratory, students should be able to:

1.  Explain why structured data is useful in IoT systems.
2.  Describe the basic structure of JSON.
3.  Generate JSON messages on an ESP32.
4.  Publish multiple sensor variables in one MQTT message.
5.  Parse JSON data using JavaScript.
6.  Display multiple sensor values on a GitHub.io dashboard.
7.  Compare single-value MQTT messages with structured JSON messages.
8.  Design MQTT topics for structured IoT communication.
9.  Add device metadata to an MQTT payload.
10. Extend an IoT dashboard to display multiple environmental
    measurements.

------------------------------------------------------------------------

# 3. System Architecture

The system consists of:

`text id="4zy2lk" ┌─────────────────────┐ │      Sensors        │ │                     │ │ Temperature         │ │ Humidity            │ │ Light               │ └──────────┬──────────┘            │            ▼ ┌─────────────────────┐ │       ESP32         │ │                     │ │ Read Sensors        │ │ Build JSON          │ │ MQTT Publish        │ └──────────┬──────────┘            │            │ MQTT            ▼ ┌─────────────────────┐ │    MQTT Broker      │ └──────────┬──────────┘            │            │ WSS            ▼ ┌─────────────────────┐ │     GitHub.io       │ │     Dashboard       │ │                     │ │ Temperature         │ │ Humidity            │ │ Light               │ └─────────────────────┘`

------------------------------------------------------------------------

# 4. Why Use JSON?

In previous laboratories, each MQTT message contained only one simple
value.

For example:

\`\`\`text id="avw2ek" Topic: iot/lab/temperature

Payload: 28.5


    If three measurements are required, one possible approach is:

    ```text id="p4zmh5"
    iot/lab/temperature → 28.5

    iot/lab/humidity → 72

    iot/lab/light → 640

This works, but the system must manage several topics.

Another approach is to publish all values together:

`json id="pt59xy" {   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

Using JSON makes it easier to group related data.

------------------------------------------------------------------------

# 5. JSON Structure

JSON stands for:

**JavaScript Object Notation**

A simple JSON object contains key-value pairs.

Example:

`json id="rzur2v" {   "temperature": 28.5,   "humidity": 72 }`

The keys are:

`text id="47cps6" temperature humidity`

The values are:

`text id="ohr7cf" 28.5 72`

JSON can also contain text:

`json id="4jocbc" {   "device": "ESP32-01",   "status": "ONLINE" }`

and Boolean values:

`json id="0at2uq" {   "led": true }`

------------------------------------------------------------------------

# 6. MQTT Topic Design

In this laboratory, use one main sensor topic:

`text id="9cnpvl" iot/lab/sensors`

Example payload:

`json id="lvx0qf" {   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

The communication becomes:

`text id="k5ytg5" ESP32   │   │ Publish JSON   ▼ iot/lab/sensors   │   ▼ MQTT Broker   │   ▼ GitHub.io`

For a shared laboratory broker, students should use a unique topic:

`text id="na0r0d" iot/lab/student01/sensors`

------------------------------------------------------------------------

# Part A --- Simulated Multiple Sensors

## 7. Simulated Sensor Values

Physical sensors are optional in this laboratory.

The ESP32 may generate simulated values:

\`\`\`cpp id="b71zwf" float temperature = random(250, 350) / 10.0;

float humidity = random(400, 900) / 10.0;

int light = random(0, 1024);


    Example output:

    ```text id="llf9c5"
    Temperature: 28.4 °C
    Humidity: 71.2 %
    Light: 640

------------------------------------------------------------------------

# 8. Sensor Data Model

The three measurements are:

  Variable        Example Unit
  ------------- --------- -----------
  Temperature        28.5 °C
  Humidity             72 \%
  Light               640 ADC level

These values will be encoded into one JSON message.

------------------------------------------------------------------------

# Part B --- ESP32 JSON Publisher

## 9. JSON Message Construction

A simple JSON message can be created manually.

Example:

`cpp id="4vulq4" String json =     "{"     "\"temperature\":" +     String(temperature, 1) +     ","     "\"humidity\":" +     String(humidity, 1) +     ","     "\"light\":" +     String(light) +     "}";`

The resulting payload may look like:

`json id="g0sh4x" {   "temperature": 28.5,   "humidity": 72.3,   "light": 640 }`

------------------------------------------------------------------------

# 10. ESP32 Program

The following program extends the previous laboratory.

\`\`\`cpp id="w8f2y0" #include \<WiFi.h\> #include \<PubSubClient.h\>

const char\* ssid = "YOUR_WIFI";

const char\* password = "YOUR_PASSWORD";

const char\* mqtt_server = "YOUR_MQTT_BROKER";

WiFiClient espClient; PubSubClient client(espClient);

// -------------------------------------- // Wi-Fi Connection //
--------------------------------------

void connectWiFi() { WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");

    while (
        WiFi.status() != WL_CONNECTED
    )
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");

}

// -------------------------------------- // MQTT Connection //
--------------------------------------

void connectMQTT() { while (!client.connected()) { Serial.print(
"Connecting to MQTT..." );

        if (
            client.connect(
                "ESP32_Lab3"
            )
        )
        {
            Serial.println(
                "connected"
            );
        }

        else
        {
            Serial.println(
                "failed"
            );

            delay(2000);
        }
    }

}

// -------------------------------------- // Setup //
--------------------------------------

void setup() { Serial.begin(115200);

    connectWiFi();

    client.setServer(
        mqtt_server,
        1883
    );

}

// -------------------------------------- // Main Loop //
--------------------------------------

void loop() { if (!client.connected()) { connectMQTT(); }

    client.loop();

    float temperature =
        random(250, 350) / 10.0;

    float humidity =
        random(400, 900) / 10.0;

    int light =
        random(0, 1024);

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

    client.publish(
        "iot/lab/sensors",
        json.c_str()
    );

    Serial.println(json);

    delay(2000);

}


    ---

    # 11. Expected Serial Monitor Output

    Example:

    ```text id="1f4pd2"
    WiFi connected
    MQTT connected

    {"temperature":27.8,"humidity":68.5,"light":620}

    {"temperature":28.2,"humidity":70.1,"light":645}

    {"temperature":28.6,"humidity":71.4,"light":690}

Each line is one complete JSON object.

------------------------------------------------------------------------

# Part C --- GitHub.io JSON Dashboard

## 12. Dashboard Requirement

The dashboard should display:

`text id="4iys7n" ┌────────────────────────────┐ │      IoT Dashboard         │ │                            │ │ Temperature                │ │      28.5 °C               │ │                            │ │ Humidity                   │ │       72 %                 │ │                            │ │ Light                      │ │       640                  │ │                            │ │ MQTT Connected             │ └────────────────────────────┘`

------------------------------------------------------------------------

# 13. Parsing JSON in JavaScript

When the MQTT message arrives, it is initially a text string.

Example:

`text id="bgygjm" {"temperature":28.5,"humidity":72,"light":640}`

JavaScript can convert this text into an object using:

`javascript id="18ozsv" const data =     JSON.parse(         message.toString()     );`

Then individual values can be accessed:

`javascript id="ho49ai" data.temperature data.humidity data.light`

------------------------------------------------------------------------

# 14. Dashboard Program

Create or modify `index.html`:

\`\`\`html id="g4b9v2" \<!DOCTYPE html\>

```{=html}
<html>
```
```{=html}
<head>
```
```{=html}
<title>
```
Lab 3 IoT Dashboard
```{=html}
</title>
```
```{=html}
<script src=
"https://unpkg.com/mqtt/dist/mqtt.min.js">
</script>
```
```{=html}
<style>

body {
    font-family: Arial;
    text-align: center;
    margin-top: 30px;
}

.dashboard {
    width: 420px;
    margin: auto;
    padding: 30px;
    border: 1px solid #ccc;
    border-radius: 12px;
}

.value {
    font-size: 36px;
    margin-bottom: 20px;
}

</style>
```
```{=html}
</head>
```
```{=html}
<body>
```
::: dashboard
```{=html}
<h1>
```
IoT Sensor Dashboard
```{=html}
</h1>
```
```{=html}
<h2>
```
Temperature
```{=html}
</h2>
```
::: {#temperature .value}
    -- °C
:::

```{=html}
<h2>
```
Humidity
```{=html}
</h2>
```
::: {#humidity .value}
    -- %
:::

```{=html}
<h2>
```
Light
```{=html}
</h2>
```
::: {#light .value}
    --
:::

```{=html}
<p id="status">
```
Connecting...
```{=html}
</p>
```
:::

```{=html}
<script>

const broker =
    "wss://YOUR_BROKER_WEBSOCKET_ADDRESS";

const client =
    mqtt.connect(broker);


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
            "iot/lab/sensors"
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

            document.getElementById(
                "temperature"
            ).innerText =
                data.temperature +
                " °C";

            document.getElementById(
                "humidity"
            ).innerText =
                data.humidity +
                " %";

            document.getElementById(
                "light"
            ).innerText =
                data.light;
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

</script>
```
```{=html}
</body>
```
```{=html}
</html>
```

    ---

    # Part D — System Integration

    ## 15. Complete Data Flow

    The complete process is:

    ```text id="jlf4av"
    Temperature
    Humidity
    Light
       │
       ▼
     ESP32
       │
       │
       │ Create JSON
       ▼
    {
      temperature,
      humidity,
      light
    }
       │
       │ MQTT
       ▼
    Broker
       │
       │ WSS
       ▼
    GitHub.io
       │
       │ JSON.parse()
       ▼
    Dashboard

------------------------------------------------------------------------

# 16. Experiment 1 --- JSON Transmission

Run the ESP32.

Observe the Serial Monitor.

Record five JSON messages.

    Sample JSON Message
  -------- --------------
         1 
         2 
         3 
         4 
         5 

Verify that all three sensor variables are included.

------------------------------------------------------------------------

# 17. Experiment 2 --- Dashboard Parsing

Open the GitHub.io dashboard.

Compare:

\`\`\`text id="uq6z0q" ESP32 JSON

{"temperature":28.5,"humidity":72.0,"light":640}


    with:

    ```text id="gnsd9b"
    Dashboard

    Temperature: 28.5 °C

    Humidity: 72.0 %

    Light: 640

Verify that each JSON field is displayed correctly.

------------------------------------------------------------------------

# 18. Experiment 3 --- Add Device Information

Extend the JSON message to include a device identifier.

Example:

`json id="ubdqzx" {   "device": "ESP32-01",   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

Modify the ESP32 code:

`cpp id="z1hzhq" String json =     "{"     "\"device\":\"ESP32-01\","     "\"temperature\":" +     String(temperature, 1) +     ","     "\"humidity\":" +     String(humidity, 1) +     ","     "\"light\":" +     String(light) +     "}";`

Modify the dashboard to display:

`text id="vcc6b0" Device: ESP32-01`

------------------------------------------------------------------------

# 19. Experiment 4 --- Add Message Counter

Add a sequence number:

`json id="wqengn" {   "device": "ESP32-01",   "sequence": 125,   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

The sequence number increases each time the ESP32 publishes.

For example:

`cpp id="d3nv4o" unsigned long sequence = 0;`

Then:

`cpp id="534vdz" sequence++;`

Include it in the JSON message.

The dashboard can display:

`text id="q66qs0" Message #: 125`

------------------------------------------------------------------------

# 20. Experiment 5 --- Timestamp Concept

A sensor record often includes time information.

Example:

`json id="8gu2zf" {   "device": "ESP32-01",   "sequence": 125,   "temperature": 28.5,   "humidity": 72,   "light": 640,   "timestamp": 12345678 }`

For this laboratory, the timestamp may initially represent milliseconds
since the ESP32 started:

`cpp id="vi2w7m" millis()`

Later laboratories can introduce real clock time using NTP.

------------------------------------------------------------------------

# Part E --- JSON Design

## 21. Flat JSON Structure

A simple flat message is:

`json id="l9im7c" {   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

This is easy to read and suitable for simple devices.

------------------------------------------------------------------------

# 22. Nested JSON Structure

A more structured format is:

`json id="n3wa47" {   "device": "ESP32-01",   "sensors": {     "temperature": 28.5,     "humidity": 72,     "light": 640   } }`

JavaScript accesses values using:

`javascript id="ab8a0n" data.sensors.temperature data.sensors.humidity data.sensors.light`

Nested structures can be useful for larger IoT systems.

------------------------------------------------------------------------

# 23. Metadata

IoT messages can contain both sensor values and metadata.

Example:

`json id="8fq8u0" {   "device": "ESP32-01",   "location": "Lab-A",   "sequence": 125,   "sensors": {     "temperature": 28.5,     "humidity": 72,     "light": 640   } }`

Here:

`text id="m8tg6n" device location sequence`

are metadata.

The actual sensor measurements are under:

`text id="efak21" sensors`

------------------------------------------------------------------------

# Part F --- Multiple Devices

## 24. Topic-Based Device Separation

Two ESP32 devices may use:

\`\`\`text id="qd8lnb" iot/lab/device01/sensors

iot/lab/device02/sensors


    The dashboard may subscribe to:

    ```text id="nv510j"
    iot/lab/+/sensors

The `+` is an MQTT single-level wildcard.

This allows the dashboard to receive data from multiple devices.

------------------------------------------------------------------------

# 25. Device Data Example

Device 1:

`json id="pj5cc4" {   "device": "ESP32-01",   "temperature": 28.5,   "humidity": 72 }`

Device 2:

`json id="nsmvkz" {   "device": "ESP32-02",   "temperature": 29.1,   "humidity": 68 }`

The dashboard could display:

\`\`\`text id="fk305y" ESP32-01 Temperature: 28.5 °C Humidity: 72 %

ESP32-02 Temperature: 29.1 °C Humidity: 68 %


    ---

    # Part G — Comparison of Message Designs

    ## 26. Separate Topics

    Example:

    ```text id="zvz4uc"
    iot/lab/temperature
    iot/lab/humidity
    iot/lab/light

Advantages:

-   simple messages;
-   easy individual subscriptions.

Disadvantages:

-   more MQTT topics;
-   measurements may arrive at different times;
-   harder to treat several values as one sensor record.

------------------------------------------------------------------------

# 27. JSON Topic

Example:

`text id="ducxhx" iot/lab/sensors`

Payload:

`json id="bii9ki" {   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

Advantages:

-   related measurements stay together;
-   easy to extend;
-   convenient for databases and APIs;
-   includes metadata naturally.

Disadvantages:

-   larger payload;
-   requires parsing;
-   malformed JSON can cause errors.

------------------------------------------------------------------------

# Part H --- Error Handling

## 28. Invalid JSON

Suppose the received message is:

`text id="5m98f2" temperature=28.5`

This is not valid JSON.

Calling:

`javascript id="qslwh1" JSON.parse(message)`

will generate an error.

Therefore, the dashboard should use:

`javascript id="2ns7u6" try {     const data =         JSON.parse(             message.toString()         ); } catch(error) {     console.log(         "Invalid JSON"     ); }`

This prevents a malformed message from stopping the entire dashboard.

------------------------------------------------------------------------

# Part I --- Security Considerations

## 29. Do Not Include Secrets in JSON

An MQTT sensor message should not contain:

`text id="1hh91g" Wi-Fi password MQTT administrator password Private API key Private certificate`

A public MQTT payload may be visible to authorized subscribers.

Only transmit information required by the application.

------------------------------------------------------------------------

# 30. Validate Received Data

The dashboard should not automatically trust every MQTT message.

For example:

`javascript id="0rw6ek" if (     typeof data.temperature     === "number" ) {     // Update display }`

Validation becomes increasingly important when IoT systems interact with
actuators or automated decision systems.

------------------------------------------------------------------------

# Part J --- Review Questions

## 31. Questions

1.  What is JSON?

2.  Why is JSON useful in IoT applications?

3.  What is the difference between a JSON key and value?

4.  Why can multiple sensor variables be placed in one MQTT message?

5.  What does `JSON.parse()` do?

6.  What happens if the received payload is invalid JSON?

7.  What is metadata?

8.  What is the advantage of adding a device ID?

9.  What is the purpose of a sequence number?

10. What is the difference between flat and nested JSON?

11. What are the advantages of separate MQTT topics?

12. What are the advantages of a single JSON topic?

13. How can multiple ESP32 devices be distinguished?

14. What does the MQTT `+` wildcard mean?

15. Why should IoT applications validate received data?

------------------------------------------------------------------------

# Part K --- Student Assignment

## 32. Required Implementation

Develop an IoT system with the architecture:

`text id="qnuf3b" Sensors    ↓ ESP32    ↓ JSON    ↓ MQTT Broker    ↓ GitHub.io`

The JSON payload must contain at least:

`json id="dqa5dr" {   "device": "ESP32-XX",   "temperature": 0,   "humidity": 0,   "light": 0 }`

The dashboard must display:

-   device name;
-   temperature;
-   humidity;
-   light value;
-   MQTT connection status.

------------------------------------------------------------------------

# 33. Extension Task

Add:

`json id="nvxo1q" {   "device": "ESP32-01",   "sequence": 100,   "uptime": 125000,   "temperature": 28.5,   "humidity": 72,   "light": 640 }`

Display:

\`\`\`text id="uh89og" Device: ESP32-01

Message #: 100

Uptime: 125000 ms

Temperature: 28.5 °C

Humidity: 72 %

Light: 640


    ---

    # 34. Advanced Extension

    Use two ESP32 devices.

    For example:

    ```text id="u54s2f"
    ESP32-01
          │
          ├──► iot/lab/device01/sensors
          │
          ▼
     MQTT Broker
          ▲
          │
          ├──► iot/lab/device02/sensors
          │
    ESP32-02

The dashboard should show the values from both devices.

------------------------------------------------------------------------

# Part L --- Laboratory Report

## 35. Required Report

The report should contain:

### 1. Objective

Explain the purpose of structured data in IoT systems.

### 2. System Architecture

Draw:

`text id="4ku7z7" Sensors → ESP32 → JSON → MQTT → GitHub.io`

### 3. JSON Design

Show the JSON payload used in the experiment.

### 4. MQTT Configuration

Report:

`text id="g0ba8a" MQTT Broker: MQTT Topic: WebSocket Endpoint: Publish Interval:`

Do not include passwords.

### 5. ESP32 Program

Explain how the JSON message is generated.

### 6. Dashboard

Explain how JavaScript parses and displays the JSON data.

### 7. Results

Provide screenshots showing:

-   MQTT connection;
-   temperature;
-   humidity;
-   light;
-   device ID.

### 8. Discussion

Compare:

`text id="f55w17" Multiple Topics vs. Single JSON Message`

Discuss the advantages and disadvantages of each approach.

### 9. Conclusion

Summarize the role of JSON in IoT communication.

------------------------------------------------------------------------

# 36. Expected Result

At the end of the laboratory, the complete system should resemble:

`text id="nw82cl" ┌─────────────────────┐ │       ESP32         │ │                     │ │ Temp = 28.5 °C      │ │ Humidity = 72 %     │ │ Light = 640         │ └──────────┬──────────┘            │            │ JSON            ▼ {  "temperature":28.5,  "humidity":72,  "light":640 }            │            │ MQTT            ▼ ┌─────────────────────┐ │    MQTT Broker      │ └──────────┬──────────┘            │            │ WSS            ▼ ┌─────────────────────┐ │     GitHub.io       │ │                     │ │ Temperature 28.5 °C │ │ Humidity 72 %       │ │ Light 640           │ └─────────────────────┘`

------------------------------------------------------------------------

# 37. Key Concept

The progression across the first three laboratories is:

\`\`\`text id="efoncr" Lab 1 Single Sensor Monitoring

ESP32 ─────► Dashboard

Lab 2 Monitoring + Control

ESP32 ◄────► Dashboard

Lab 3 Structured Multi-Sensor Data

Multiple Sensors ↓ ESP32 ↓ JSON ↓ MQTT Broker ↓ Dashboard \`\`\`

Lab 3 introduces the concept of a **structured IoT data model**, which
is necessary for more advanced systems involving databases, cloud APIs,
Node-RED, digital twins, analytics, and machine learning.

------------------------------------------------------------------------

# 38. Summary

In this laboratory, students extended the ESP32 MQTT system to support
multiple sensor variables using JSON.

Instead of transmitting isolated values, the ESP32 creates a structured
data object containing measurements and optional metadata. The JSON
payload is published through MQTT, received by a GitHub.io dashboard,
parsed with JavaScript, and converted into human-readable information.

The complete data path is:

**Sensors → ESP32 → JSON → MQTT → Broker → WSS → GitHub.io Dashboard**

This laboratory provides the foundation for the next stage: **real-time
visualization and historical data analysis**, where incoming MQTT sensor
values can be displayed as dynamic charts rather than only numerical
values.
