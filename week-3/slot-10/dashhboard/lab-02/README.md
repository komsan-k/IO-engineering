# Lab 2: Real-Time IoT Monitoring and Device Control Using ESP32, MQTT, and GitHub.io

## 1. Introduction

In Lab 1, an ESP32 was configured as an MQTT publisher to transmit
sensor data to a web dashboard hosted on GitHub Pages. The system
demonstrated one-way IoT communication:

``` text
ESP32 → MQTT Broker → GitHub.io
```

However, many practical Internet of Things (IoT) systems require
**bidirectional communication**. Users must be able to monitor physical
devices while also sending commands back to them.

In this laboratory, the previous system is extended so that the
GitHub.io dashboard can control an LED connected to the ESP32 while
simultaneously receiving temperature and device-status information.

The resulting communication model is:

``` text
                 MQTT Broker
                /           \
               /             \
              ▼               ▼
          ESP32  ◄────────► GitHub.io
           │                 Dashboard
           │
      ┌────┴────┐
      │         │
Temperature    LED
 Sensor       Actuator
```

Both the ESP32 and web dashboard therefore operate as **MQTT publishers
and subscribers**.

------------------------------------------------------------------------

# 2. Objectives

After completing this laboratory, students should be able to:

1.  Explain bidirectional MQTT communication.
2.  Configure an ESP32 as both an MQTT publisher and subscriber.
3.  Subscribe an ESP32 to an MQTT command topic.
4.  Receive MQTT messages using an MQTT callback function.
5.  Control a physical LED from an MQTT message.
6.  Develop ON/OFF controls using HTML and JavaScript.
7.  Publish MQTT commands from a GitHub.io dashboard.
8.  Display real-time sensor and actuator states.
9.  Explain the difference between monitoring and control messages.
10. Implement a simple closed interaction between the cyber and physical
    components of an IoT system.

------------------------------------------------------------------------

# 3. System Architecture

The system contains four major functions:

``` text
                    Internet
                       │
               ┌───────▼───────┐
               │  MQTT Broker  │
               │               │
               │ MQTT + WSS    │
               └───┬───────┬───┘
                   │       │
              MQTT │       │ WSS
                   │       │
          ┌────────▼─┐   ┌─▼──────────────┐
          │  ESP32   │   │   GitHub.io   │
          │          │   │   Dashboard   │
          │ Sensor   │   │               │
          │ LED      │   │ Temperature   │
          │          │   │ LED ON / OFF  │
          └────┬─────┘   └────────────────┘
               │
        ┌──────┴──────┐
        ▼             ▼
 Temperature         LED
   Sensor          Actuator
```

The ESP32 publishes sensor information and receives actuator commands.

The dashboard receives sensor information and publishes actuator
commands.

------------------------------------------------------------------------

# 4. Publish/Subscribe Model

Lab 1 used only:

``` text
ESP32
  │
  │ Publish
  ▼
Temperature Topic
  │
  ▼
Dashboard
```

Lab 2 introduces the opposite communication direction:

``` text
Dashboard
  │
  │ Publish
  ▼
LED Command Topic
  │
  ▼
ESP32
```

Combining the two creates:

``` text
ESP32 ───── Sensor Data ─────► Dashboard

ESP32 ◄──── Control Data ───── Dashboard
```

This is the fundamental communication model used in many remote IoT
monitoring and control systems.

------------------------------------------------------------------------

# 5. Hardware Requirements

Required hardware:

-   ESP32 development board
-   USB cable
-   LED
-   Current-limiting resistor, e.g. 220--330 Ω
-   Breadboard
-   Jumper wires
-   Computer
-   Wi-Fi connection

A temperature sensor is optional. Simulated temperature values may
continue to be used.

Some ESP32 development boards also contain a built-in LED. If available,
it may be used instead of an external LED.

------------------------------------------------------------------------

# 6. Software Requirements

Students require:

-   Arduino IDE
-   ESP32 Arduino board package
-   PubSubClient
-   GitHub account
-   GitHub Pages
-   Web browser
-   MQTT broker with WebSocket/WSS support
-   MQTT.js

------------------------------------------------------------------------

# 7. MQTT Topic Design

Three MQTT topics are used.

  Topic                   Direction           Function
  ----------------------- ------------------- ------------------
  `iot/lab/temperature`   ESP32 → Dashboard   Temperature
  `iot/lab/led/set`       Dashboard → ESP32   LED command
  `iot/lab/led/status`    ESP32 → Dashboard   Actual LED state

The distinction between `/set` and `/status` is important.

The `/set` topic represents the **requested state**:

``` text
iot/lab/led/set

ON
```

The `/status` topic represents the **actual state reported by the
device**:

``` text
iot/lab/led/status

ON
```

This allows the dashboard to distinguish between:

> **"I requested the LED to turn ON."**

and:

> **"The ESP32 reports that the LED is ON."**

For shared laboratory infrastructure, students should use unique topic
prefixes, such as:

``` text
iot/lab/student01/temperature
iot/lab/student01/led/set
iot/lab/student01/led/status
```

------------------------------------------------------------------------

# Part A --- Hardware

## 8. LED Connection

An external LED can be connected as:

``` text
ESP32 GPIO
    │
    │
  Resistor
  220–330 Ω
    │
    ▼
   LED
    │
    ▼
   GND
```

For example:

``` text
GPIO 2 ── Resistor ── LED ── GND
```

Verify the GPIO assignment for the ESP32 board used in the laboratory
before wiring.

------------------------------------------------------------------------

# Part B --- ESP32 MQTT Subscriber

## 9. ESP32 Program Structure

The ESP32 now performs two tasks:

### Publishing

``` text
Read Temperature
      ↓
Publish Temperature
      ↓
MQTT Broker
```

### Subscribing

``` text
MQTT Broker
      ↓
Receive LED Command
      ↓
MQTT Callback
      ↓
ON or OFF?
      ↓
Control GPIO
      ↓
Publish LED Status
```

Therefore:

``` text
                ESP32
                  │
        ┌─────────┴─────────┐
        │                   │
     Publish             Subscribe
        │                   │
        ▼                   ▼
 Temperature Data        LED Command
```

------------------------------------------------------------------------

# 10. MQTT Callback Function

A callback function is executed whenever a new MQTT message arrives on a
subscribed topic.

Conceptually:

``` text
Message Arrives
      ↓
Callback()
      ↓
Check Topic
      ↓
Read Payload
      ↓
Determine Command
      ↓
Control Hardware
```

For example:

``` cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String message;

    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    if (message == "ON")
    {
        digitalWrite(LED_PIN, HIGH);
    }

    if (message == "OFF")
    {
        digitalWrite(LED_PIN, LOW);
    }
}
```

------------------------------------------------------------------------

# 11. ESP32 Program

The following program extends the Lab 1 implementation.

``` cpp
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid =
    "YOUR_WIFI";

const char* password =
    "YOUR_PASSWORD";

const char* mqtt_server =
    "YOUR_MQTT_BROKER";

#define LED_PIN 2

WiFiClient espClient;

PubSubClient client(espClient);


// ------------------------------------------------
// Wi-Fi Connection
// ------------------------------------------------

void connectWiFi()
{
    WiFi.begin(ssid, password);

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


// ------------------------------------------------
// MQTT Callback
// ------------------------------------------------

void callback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String message;

    for (
        unsigned int i = 0;
        i < length;
        i++
    )
    {
        message +=
            (char)payload[i];
    }

    Serial.print("Received: ");
    Serial.println(message);

    if (
        String(topic) ==
        "iot/lab/led/set"
    )
    {
        if (message == "ON")
        {
            digitalWrite(
                LED_PIN,
                HIGH
            );

            client.publish(
                "iot/lab/led/status",
                "ON"
            );
        }

        else if (message == "OFF")
        {
            digitalWrite(
                LED_PIN,
                LOW
            );

            client.publish(
                "iot/lab/led/status",
                "OFF"
            );
        }
    }
}


// ------------------------------------------------
// MQTT Connection
// ------------------------------------------------

void connectMQTT()
{
    while (!client.connected())
    {
        Serial.print(
            "Connecting to MQTT..."
        );

        if (
            client.connect(
                "ESP32_Lab2"
            )
        )
        {
            Serial.println(
                "connected"
            );

            client.subscribe(
                "iot/lab/led/set"
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


// ------------------------------------------------
// Setup
// ------------------------------------------------

void setup()
{
    Serial.begin(115200);

    pinMode(
        LED_PIN,
        OUTPUT
    );

    digitalWrite(
        LED_PIN,
        LOW
    );

    connectWiFi();

    client.setServer(
        mqtt_server,
        1883
    );

    client.setCallback(
        callback
    );
}


// ------------------------------------------------
// Main Loop
// ------------------------------------------------

void loop()
{
    if (!client.connected())
    {
        connectMQTT();
    }

    client.loop();

    float temperature =
        random(250, 350) / 10.0;

    char value[10];

    dtostrf(
        temperature,
        4,
        1,
        value
    );

    client.publish(
        "iot/lab/temperature",
        value
    );

    Serial.print(
        "Temperature: "
    );

    Serial.println(value);

    delay(2000);
}
```

------------------------------------------------------------------------

# Part C --- GitHub.io Control Dashboard

## 12. Dashboard Requirements

The new dashboard should display:

``` text
┌────────────────────────────┐
│       IoT Dashboard        │
│                            │
│       Temperature          │
│                            │
│         28.5 °C            │
│                            │
│       LED Control          │
│                            │
│      [ ON ]  [ OFF ]       │
│                            │
│      LED Status: ON        │
│                            │
│      MQTT Connected        │
└────────────────────────────┘
```

The dashboard therefore performs both:

``` text
SUBSCRIBE
Temperature
LED Status
```

and:

``` text
PUBLISH
LED Command
```

------------------------------------------------------------------------

# 13. Dashboard Program

Modify the `index.html` created in Lab 1.

``` html
<!DOCTYPE html>

<html>

<head>

<title>IoT Control Dashboard</title>

<script src=
"https://unpkg.com/mqtt/dist/mqtt.min.js">
</script>

<style>

body {
    font-family: Arial;
    text-align: center;
    margin-top: 40px;
}

.dashboard {
    width: 400px;
    margin: auto;
    padding: 30px;
    border: 1px solid #ccc;
    border-radius: 12px;
}

.temperature {
    font-size: 48px;
    margin: 20px;
}

button {
    font-size: 20px;
    padding: 12px 25px;
    margin: 10px;
}

.status {
    margin-top: 20px;
}

</style>

</head>


<body>

<div class="dashboard">

<h1>IoT Dashboard</h1>


<h2>Temperature</h2>

<div
    id="temperature"
    class="temperature">

    -- °C

</div>


<h2>LED Control</h2>

<button onclick="ledOn()">
ON
</button>

<button onclick="ledOff()">
OFF
</button>


<h3>
LED Status:
<span id="ledStatus">
Unknown
</span>
</h3>


<p
    id="mqttStatus"
    class="status">

Connecting...

</p>

</div>


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
            "mqttStatus"
        ).innerText =
            "MQTT Connected";

        client.subscribe(
            "iot/lab/temperature"
        );

        client.subscribe(
            "iot/lab/led/status"
        );
    }
);


// --------------------------------------
// Receive MQTT Messages
// --------------------------------------

client.on(
    "message",
    function(topic, message)
    {
        const value =
            message.toString();

        if (
            topic ==
            "iot/lab/temperature"
        )
        {
            document.getElementById(
                "temperature"
            ).innerText =
                value + " °C";
        }

        if (
            topic ==
            "iot/lab/led/status"
        )
        {
            document.getElementById(
                "ledStatus"
            ).innerText =
                value;
        }
    }
);


// --------------------------------------
// LED ON
// --------------------------------------

function ledOn()
{
    client.publish(
        "iot/lab/led/set",
        "ON"
    );
}


// --------------------------------------
// LED OFF
// --------------------------------------

function ledOff()
{
    client.publish(
        "iot/lab/led/set",
        "OFF"
    );
}


// --------------------------------------
// MQTT Error
// --------------------------------------

client.on(
    "error",
    function(error)
    {
        document.getElementById(
            "mqttStatus"
        ).innerText =
            "MQTT Error";

        console.log(error);
    }
);

</script>

</body>

</html>
```

------------------------------------------------------------------------

# Part D --- System Operation

## 14. Monitoring Path

The monitoring path remains similar to Lab 1.

``` text
Temperature
     ↓
   ESP32
     ↓
Publish
     ↓
iot/lab/temperature
     ↓
MQTT Broker
     ↓
GitHub.io
     ↓
28.5 °C
```

------------------------------------------------------------------------

# 15. Control Path

When the user presses the **ON** button:

``` text
User
 ↓
[ON]
 ↓
JavaScript
 ↓
MQTT Publish
 ↓
iot/lab/led/set
 ↓
"ON"
 ↓
MQTT Broker
 ↓
ESP32
 ↓
MQTT Callback
 ↓
GPIO HIGH
 ↓
LED ON
```

The ESP32 then reports:

``` text
ESP32
 ↓
Publish
 ↓
iot/lab/led/status
 ↓
"ON"
 ↓
MQTT Broker
 ↓
GitHub.io
 ↓
LED Status: ON
```

This status message provides **device acknowledgement**.

------------------------------------------------------------------------

# Part E --- Experiments

## 16. Experiment 1 --- Temperature Monitoring

Run the ESP32 and open the GitHub.io dashboard.

Verify that:

``` text
ESP32 Serial Monitor
        ↓
Temperature: 28.5

GitHub.io
        ↓
28.5 °C
```

Record five temperature values.

    Sample   ESP32   Dashboard
  -------- ------- -----------
         1         
         2         
         3         
         4         
         5         

The ESP32 and dashboard values should correspond.

------------------------------------------------------------------------

# 17. Experiment 2 --- Remote LED ON

Press:

``` text
[ ON ]
```

Observe:

1.  MQTT command transmission.
2.  ESP32 Serial Monitor.
3.  Physical LED.
4.  Dashboard status.

Expected result:

``` text
Command: ON

        ↓

LED physically turns ON

        ↓

Status: ON
```

------------------------------------------------------------------------

# 18. Experiment 3 --- Remote LED OFF

Press:

``` text
[ OFF ]
```

Expected operation:

``` text
Dashboard
   ↓
OFF
   ↓
MQTT Broker
   ↓
ESP32
   ↓
GPIO LOW
   ↓
LED OFF
   ↓
Status = OFF
   ↓
Dashboard
```

------------------------------------------------------------------------

# 19. Experiment 4 --- Broker Disconnection

Temporarily disconnect the ESP32 from the network or stop its MQTT
connection.

Observe the dashboard.

Consider:

-   Can the dashboard still send an MQTT command?
-   Does the LED respond?
-   Does the displayed LED state necessarily represent the current
    physical state?
-   How can the dashboard detect that the ESP32 is offline?

This experiment introduces an important concept:

> **MQTT connection status is not necessarily the same as device
> status.**

A dashboard may remain connected to the broker even when the ESP32
itself is offline.

------------------------------------------------------------------------

# 20. Experiment 5 --- Command Latency

Measure approximately how long it takes between:

``` text
Button Click
    ↓
LED State Change
```

Perform five measurements.

    Test   Approximate Latency
  ------ ---------------------
       1 
       2 
       3 
       4 
       5 

Calculate the average:

``` text
Average Latency =
(T1 + T2 + T3 + T4 + T5) / 5
```

Discuss the factors that could affect command latency.

------------------------------------------------------------------------

# Part F --- System Analysis

## 21. Monitoring vs. Control

The completed system contains two data flows.

### Monitoring

``` text
Physical World
      ↓
Temperature
      ↓
ESP32
      ↓
MQTT
      ↓
Dashboard
```

### Control

``` text
User
 ↓
Dashboard
 ↓
MQTT
 ↓
ESP32
 ↓
GPIO
 ↓
Physical LED
```

Together:

``` text
Physical System
      ⇅
    ESP32
      ⇅
     MQTT
      ⇅
  Dashboard
      ⇅
     User
```

This creates a basic connection between the **physical world** and the
**cyber world**.

------------------------------------------------------------------------

# 22. Command and State Separation

Consider the topics:

``` text
iot/lab/led/set
```

and:

``` text
iot/lab/led/status
```

They should not be considered equivalent.

`/set` means:

``` text
Requested state
```

while `/status` means:

``` text
Reported device state
```

For example:

``` text
Dashboard
    │
    │ ON request
    ▼
led/set
    │
    ▼
ESP32
    │
    │ LED becomes ON
    ▼
led/status
    │
    ▼
Dashboard
```

This separation becomes increasingly important as IoT systems become
more complex.

------------------------------------------------------------------------

# 23. Device State

The ESP32 now has a simple state:

``` text
        ┌──────────┐
        │ LED OFF  │
        └────┬─────┘
             │
          ON command
             │
             ▼
        ┌──────────┐
        │  LED ON  │
        └────┬─────┘
             │
         OFF command
             │
             └────────────► LED OFF
```

This can be represented as:

``` text
State ∈ {ON, OFF}
```

The MQTT command causes a transition between these states.

------------------------------------------------------------------------

# Part G --- Security Considerations

## 24. Dashboard Security

The dashboard runs inside the user's browser.

Therefore, students must not place:

-   administrator MQTT credentials;
-   private API keys;
-   private certificates;
-   passwords;
-   other secrets

inside public JavaScript or a public GitHub repository.

For laboratory experiments, use accounts and topics with restricted
permissions.

For example, a dashboard account could be permitted to access only:

``` text
iot/lab/student01/#
```

rather than every topic on the broker.

------------------------------------------------------------------------

# 25. Why WSS Is Important

GitHub Pages uses HTTPS.

Therefore, MQTT communication from the browser should normally use:

``` text
wss://
```

rather than:

``` text
ws://
```

The architecture becomes:

``` text
GitHub.io
   │
 HTTPS
   │
 Browser
   │
 WSS
   │
 MQTT Broker
```

WSS provides encrypted WebSocket communication between the browser and
broker.

------------------------------------------------------------------------

# Part H --- Review Questions

## 26. Questions

1.  What is the difference between Lab 1 and Lab 2?

2.  Why does the ESP32 need to subscribe to `iot/lab/led/set`?

3.  What is the purpose of the MQTT callback function?

4.  Why are `/set` and `/status` implemented as different topics?

5.  What happens when the dashboard publishes `ON`?

6.  What happens if the dashboard is connected but the ESP32 is offline?

7.  Why should the ESP32 report the LED status after executing a
    command?

8.  What is the difference between device status and MQTT broker
    connection status?

9.  Why should secure WebSockets be used with GitHub Pages?

10. How could this architecture be extended to control multiple devices?

------------------------------------------------------------------------

# Part I --- Student Assignment

## 27. Required Implementation

Develop a bidirectional IoT system:

``` text
ESP32 ⇄ MQTT Broker ⇄ GitHub.io
```

The dashboard must provide:

-   real-time temperature monitoring;
-   MQTT connection status;
-   LED ON button;
-   LED OFF button;
-   reported LED state.

The ESP32 must:

-   connect to Wi-Fi;
-   connect to the MQTT broker;
-   publish temperature;
-   subscribe to the LED command topic;
-   control a GPIO;
-   publish the resulting LED state.

------------------------------------------------------------------------

# 28. Extension Task

Extend the dashboard to control two outputs:

``` text
┌────────────────────────────┐
│       IoT Dashboard        │
│                            │
│ Temperature                │
│      28.5 °C               │
│                            │
│ LED 1                      │
│ [ON] [OFF]                 │
│ Status: ON                 │
│                            │
│ LED 2                      │
│ [ON] [OFF]                 │
│ Status: OFF                │
│                            │
│ MQTT: Connected            │
└────────────────────────────┘
```

Possible topics are:

``` text
iot/lab/led1/set
iot/lab/led1/status

iot/lab/led2/set
iot/lab/led2/status
```

------------------------------------------------------------------------

# 29. Advanced Extension --- Device Online/Offline Status

Add another topic:

``` text
iot/lab/device/status
```

Possible messages:

``` text
ONLINE
OFFLINE
```

The dashboard can then distinguish:

``` text
Broker: Connected
Device: Online
```

from:

``` text
Broker: Connected
Device: Offline
```

This provides a more realistic IoT monitoring system.

------------------------------------------------------------------------

# Part J --- Laboratory Report

## 30. Required Report

The laboratory report should contain:

### 1. Objective

Explain the purpose of bidirectional MQTT communication.

### 2. System Architecture

Draw:

``` text
Sensor
   ↓
ESP32 ⇄ MQTT Broker ⇄ GitHub.io
   ↓
 LED
```

### 3. MQTT Topic Design

Create a table containing:

-   topic;
-   publisher;
-   subscriber;
-   payload;
-   purpose.

### 4. ESP32 Implementation

Explain:

-   MQTT publishing;
-   MQTT subscription;
-   callback function;
-   GPIO control.

### 5. Dashboard Implementation

Explain:

-   MQTT.js;
-   WebSocket connection;
-   MQTT subscription;
-   MQTT publishing;
-   ON/OFF buttons.

### 6. Experimental Results

Provide screenshots showing:

-   temperature monitoring;
-   LED ON;
-   LED OFF;
-   MQTT connection;
-   reported LED status.

### 7. Discussion

Discuss:

-   bidirectional communication;
-   MQTT latency;
-   device state;
-   command acknowledgement;
-   broker dependence;
-   security considerations.

### 8. Conclusion

Summarize the completed monitoring and control system.

------------------------------------------------------------------------

# 31. Expected Result

At the end of Lab 2, the complete system should operate as:

``` text
               ┌────────────────┐
               │  MQTT Broker   │
               └───────┬────────┘
                       │
             ┌─────────┴─────────┐
             │                   │
             ▼                   ▼
      ┌─────────────┐     ┌───────────────┐
      │    ESP32    │     │   GitHub.io   │
      │             │     │   Dashboard   │
      │ Temp 28.5°C │────►│   28.5 °C     │
      │             │     │               │
      │ LED: ON     │◄────│ [ON]   [OFF]  │
      │             │────►│ Status: ON    │
      └─────────────┘     └───────────────┘
```

The important progression is:

``` text
Lab 1
Monitoring

ESP32 ─────────────► Dashboard


Lab 2
Monitoring + Control

ESP32 ◄────────────► Dashboard
```

------------------------------------------------------------------------

# 32. Key Concept

Lab 2 transforms the ESP32 from a simple connected sensor into an
**interactive connected object**.

The system now follows the cycle:

``` text
SENSE
  ↓
COMMUNICATE
  ↓
VISUALIZE
  ↓
DECIDE
  ↓
COMMAND
  ↓
ACTUATE
  ↓
REPORT STATE
```

This cycle provides the foundation for more advanced **connected and
intelligent embedded systems**, where decisions may later be performed
automatically by rules, edge computing, or machine-learning algorithms
rather than manually by the user.

------------------------------------------------------------------------

# 33. Summary

In this laboratory, students extended the one-way IoT monitoring system
developed in Lab 1 into a bidirectional monitoring and control system.

The completed architecture is:

**Sensor → ESP32 ⇄ MQTT Broker ⇄ GitHub.io ⇄ User**

The ESP32 operates as both an MQTT publisher and subscriber. Similarly,
the GitHub.io dashboard receives sensor information and publishes
control commands. Separate command and status topics provide a simple
acknowledgement mechanism and improve the representation of the physical
device state.

Lab 2 therefore introduces the transition from a **connected sensing
device** toward an **interactive cyber-physical IoT system**.
