# Lab 1: Real-Time IoT Monitoring Using ESP32, MQTT, and GitHub.io

## 1. Introduction

The Internet of Things (IoT) enables physical devices to collect data,
communicate through networks, and provide information to users through
web or mobile applications. A typical IoT system consists of sensing
devices, communication protocols, cloud or network services, and user
interfaces.

In this laboratory, students develop a simple end-to-end IoT monitoring
system using an **ESP32**, the **MQTT protocol**, an **MQTT broker**,
and a web dashboard hosted on **GitHub Pages (GitHub.io)**.

The ESP32 generates or measures temperature data and publishes it to an
MQTT topic. The MQTT broker receives the message and distributes it to
subscribed clients. A web dashboard running on GitHub.io connects to the
broker using **MQTT over WebSockets** and displays the temperature in
real time.

The laboratory introduces the fundamental data path:

``` text
Physical Data
     ↓
   ESP32
     ↓
    MQTT
     ↓
MQTT Broker
     ↓
MQTT over WebSockets
     ↓
 GitHub.io
     ↓
IoT Dashboard
```

------------------------------------------------------------------------

## 2. Objectives

After completing this laboratory, students should be able to:

1.  Explain the basic architecture of an IoT monitoring system.
2.  Connect an ESP32 to a Wi-Fi network.
3.  Explain the MQTT publish/subscribe communication model.
4.  Publish sensor data from an ESP32 using MQTT.
5.  Configure an MQTT topic for IoT data communication.
6.  Connect a browser-based application to an MQTT broker using
    WebSockets.
7.  Develop a simple IoT dashboard using HTML and JavaScript.
8.  Deploy a web dashboard using GitHub Pages.
9.  Demonstrate real-time communication from an embedded device to a web
    dashboard.

------------------------------------------------------------------------

## 3. System Architecture

The system consists of three major components:

``` text
┌──────────────────┐
│      ESP32       │
│                  │
│ Temperature Data │
│ Wi-Fi            │
│ MQTT Publisher   │
└────────┬─────────┘
         │
         │ MQTT
         │ Publish
         ▼
┌──────────────────┐
│   MQTT Broker    │
│                  │
│ Message Routing  │
│ MQTT             │
│ WebSocket/WSS    │
└────────┬─────────┘
         │
         │ MQTT over
         │ WebSockets
         ▼
┌──────────────────┐
│    GitHub.io     │
│                  │
│ HTML             │
│ JavaScript       │
│ MQTT.js          │
│                  │
│ Temperature      │
│     28.5 °C      │
└──────────────────┘
```

The ESP32 acts as the **data producer**, the MQTT broker acts as the
**message intermediary**, and the GitHub.io dashboard acts as the **data
consumer and visualization interface**.

------------------------------------------------------------------------

## 4. Hardware Requirements

The following hardware is required:

-   ESP32 development board
-   USB cable
-   Computer with Internet connection
-   Wi-Fi network
-   Optional temperature sensor

A physical temperature sensor is not required for the first experiment.
The ESP32 can generate simulated temperature values.

------------------------------------------------------------------------

## 5. Software Requirements

Students require:

-   Arduino IDE
-   ESP32 Arduino board package
-   MQTT client library
-   Git
-   GitHub account
-   Web browser
-   Text/code editor
-   MQTT broker supporting WebSockets
-   MQTT.js JavaScript library

------------------------------------------------------------------------

## 6. MQTT Communication Model

MQTT uses a **publish/subscribe** communication model.

Unlike direct client-server communication, an MQTT publisher does not
need to know which devices receive its messages.

Instead, communication occurs through an MQTT broker.

``` text
Publisher
   │
   │ Publish
   ▼
MQTT Broker
   │
   │ Distribute
   ▼
Subscriber
```

For this laboratory:

``` text
ESP32
Publisher
   │
   │ 28.5
   ▼
iot/lab/temperature
   │
   ▼
MQTT Broker
   │
   ▼
GitHub.io
Subscriber
```

------------------------------------------------------------------------

## 7. MQTT Topic

Use the following topic:

``` text
iot/lab/temperature
```

An MQTT message consists conceptually of:

``` text
Topic:
iot/lab/temperature

Payload:
28.5
```

Therefore, if the ESP32 publishes:

``` text
28.5
```

the dashboard should display:

``` text
Temperature: 28.5 °C
```

For a shared classroom broker, each student should preferably use a
unique topic, for example:

``` text
iot/lab/student01/temperature
iot/lab/student02/temperature
iot/lab/student03/temperature
```

This prevents different students from publishing data to the same topic.

------------------------------------------------------------------------

# Part A --- ESP32 MQTT Publisher

## 8. ESP32 Operation

The ESP32 performs the following sequence:

``` text
START
  │
  ▼
Connect to Wi-Fi
  │
  ▼
Connect to MQTT Broker
  │
  ▼
Generate Temperature
  │
  ▼
Publish MQTT Message
  │
  ▼
Wait 2 Seconds
  │
  └───────────────┐
                  │
                  ▼
          Generate Next Value
```

------------------------------------------------------------------------

## 9. Install the MQTT Library

In Arduino IDE, install an MQTT client library such as **PubSubClient**.

The ESP32 program requires:

``` cpp
#include <WiFi.h>
#include <PubSubClient.h>
```

------------------------------------------------------------------------

## 10. ESP32 Program

The following structure demonstrates the experiment. Students must
replace the Wi-Fi and MQTT configuration with the values provided for
the laboratory environment.

``` cpp
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

const char* mqtt_server = "YOUR_MQTT_BROKER";

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi()
{
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
}

void connectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");

        if (client.connect("ESP32_Lab1"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.println("failed");
            delay(2000);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    connectWiFi();

    client.setServer(mqtt_server, 1883);
}

void loop()
{
    if (!client.connected())
    {
        connectMQTT();
    }

    client.loop();

    float temperature =
        random(250, 350) / 10.0;

    char message[10];

    dtostrf(
        temperature,
        4,
        1,
        message
    );

    client.publish(
        "iot/lab/temperature",
        message
    );

    Serial.print("Temperature: ");
    Serial.println(message);

    delay(2000);
}
```

------------------------------------------------------------------------

## 11. Expected Serial Monitor Output

Open the Arduino Serial Monitor at:

``` text
115200 baud
```

A typical output should appear as:

``` text
Connecting to WiFi....
WiFi connected

Connecting to MQTT...
connected

Temperature: 27.4
Temperature: 29.1
Temperature: 26.8
Temperature: 31.2
```

At this point, the ESP32 is functioning as an MQTT **publisher**.

------------------------------------------------------------------------

# Part B --- GitHub.io Dashboard

## 12. Create the Web Dashboard

Create a file named:

``` text
index.html
```

The webpage requires three components:

``` text
HTML
  ↓
Dashboard structure

CSS
  ↓
Dashboard appearance

JavaScript + MQTT.js
  ↓
MQTT communication
```

------------------------------------------------------------------------

## 13. Dashboard HTML

Use the following basic dashboard:

``` html
<!DOCTYPE html>

<html>

<head>

<title>IoT Lab Dashboard</title>

<script src=
"https://unpkg.com/mqtt/dist/mqtt.min.js">
</script>

<style>

body {
    font-family: Arial;
    text-align: center;
    margin-top: 50px;
}

.dashboard {
    width: 350px;
    margin: auto;
    padding: 30px;
    border: 1px solid #ccc;
    border-radius: 10px;
}

.temperature {
    font-size: 48px;
    margin: 20px;
}

</style>

</head>

<body>

<div class="dashboard">

<h1>IoT Dashboard</h1>

<h2>Temperature</h2>

<div
    class="temperature"
    id="temperature">
    -- °C
</div>

<p id="status">
Connecting to MQTT...
</p>

</div>

<script>

const broker =
    "wss://YOUR_BROKER_WEBSOCKET_ADDRESS";

const client =
    mqtt.connect(broker);

client.on(
    "connect",
    function ()
    {
        document.getElementById(
            "status"
        ).innerText =
            "MQTT Connected";

        client.subscribe(
            "iot/lab/temperature"
        );
    }
);

client.on(
    "message",
    function(topic, message)
    {
        const value =
            message.toString();

        document.getElementById(
            "temperature"
        ).innerText =
            value + " °C";
    }
);

client.on(
    "error",
    function(error)
    {
        document.getElementById(
            "status"
        ).innerText =
            "MQTT Connection Error";

        console.log(error);
    }
);

</script>

</body>

</html>
```

------------------------------------------------------------------------

## 14. Why WebSockets Are Required

The ESP32 normally communicates with the broker using standard MQTT:

``` text
ESP32
   │
   │ MQTT
   ▼
Broker
```

A web browser cannot normally establish the same raw MQTT TCP connection
directly.

Instead, the browser communicates using:

``` text
Browser
   │
   │ MQTT over WebSocket
   ▼
Broker
```

Because GitHub Pages is delivered through HTTPS, a deployed dashboard
should normally use a secure WebSocket connection:

``` text
wss://
```

Therefore:

``` text
ESP32 ── MQTT ────────► Broker
                         │
GitHub.io ◄── WSS ──────┘
```

------------------------------------------------------------------------

# Part C --- GitHub Pages Deployment

## 15. Create a GitHub Repository

Create a new repository named:

``` text
iot-mqtt-dashboard
```

The repository can initially contain:

``` text
iot-mqtt-dashboard/
│
├── index.html
│
└── README.md
```

------------------------------------------------------------------------

## 16. Upload the Dashboard

Add `index.html` to the repository and commit the changes.

A simple commit message is:

``` text
Add Lab 1 MQTT dashboard
```

This also introduces students to basic software version control.

------------------------------------------------------------------------

## 17. Enable GitHub Pages

Open:

``` text
Repository
    ↓
Settings
    ↓
Pages
```

Configure the repository to deploy the website from the appropriate
branch.

After deployment, the dashboard will have an address similar to:

``` text
username.github.io/iot-mqtt-dashboard/
```

Open the website using a web browser.

------------------------------------------------------------------------

# Part D --- System Integration

## 18. Complete Data Flow

Start the ESP32 and open the GitHub.io dashboard.

The complete system should operate as follows:

``` text
       Temperature
            │
            ▼
      ┌───────────┐
      │   ESP32   │
      └─────┬─────┘
            │
            │ MQTT Publish
            │
            ▼
iot/lab/temperature
            │
            ▼
      ┌───────────┐
      │   MQTT    │
      │   Broker  │
      └─────┬─────┘
            │
            │ WSS
            ▼
      ┌───────────┐
      │ GitHub.io │
      │ Dashboard │
      └─────┬─────┘
            │
            ▼
         28.5 °C
```

------------------------------------------------------------------------

## 19. Experiment 1 --- MQTT Connectivity

Start the ESP32.

Observe the Serial Monitor.

Record:

-   Wi-Fi connection status
-   MQTT connection status
-   Published MQTT topic
-   Published temperature values

### Observation

``` text
MQTT Connected: __________________

Topic: ___________________________

First Value: _____________________
```

------------------------------------------------------------------------

## 20. Experiment 2 --- Real-Time Dashboard

Open the GitHub.io dashboard.

Compare the value shown by the ESP32 Serial Monitor with the value
displayed by the dashboard.

### Example

``` text
ESP32 Serial Monitor

Temperature: 28.5


GitHub.io

┌───────────────────────────┐
│      IoT Dashboard        │
│                           │
│      Temperature          │
│                           │
│        28.5 °C            │
│                           │
│      MQTT Connected       │
└───────────────────────────┘
```

Verify that the dashboard changes when a new MQTT message arrives.

------------------------------------------------------------------------

## 21. Experiment 3 --- Update Rate

Change:

``` cpp
delay(2000);
```

to:

``` cpp
delay(5000);
```

Observe the dashboard again.

Compare the behavior of the system with update periods of:

  Experiment     Publish Interval
  ------------ ------------------
  A                      1 second
  B                     2 seconds
  C                     5 seconds
  D                    10 seconds

Discuss how the publishing interval affects dashboard responsiveness and
network traffic.

------------------------------------------------------------------------

## 22. Experiment 4 --- Multiple Sensor Variables

Extend the experiment by adding another MQTT topic:

``` text
iot/lab/humidity
```

The resulting architecture becomes:

``` text
ESP32
 │
 ├── iot/lab/temperature ─────┐
 │                            │
 └── iot/lab/humidity ────────┤
                              ▼
                         MQTT Broker
                              │
                              ▼
                         GitHub.io
                              │
                    ┌─────────┴─────────┐
                    │                   │
               Temperature          Humidity
                  28.5 °C              72 %
```

Modify the dashboard to display both values.

------------------------------------------------------------------------

# Part E --- Analysis

## 23. Role of Each Component

### ESP32

The ESP32 represents the **embedded/physical layer** of the system.

It:

-   collects or generates sensor data;
-   connects to Wi-Fi;
-   creates MQTT messages;
-   publishes information to the network.

### MQTT Broker

The broker provides the **communication layer**.

It:

-   receives MQTT messages;
-   manages topics;
-   manages subscribers;
-   distributes messages.

### GitHub.io

GitHub.io provides the **presentation layer**.

It:

-   hosts the dashboard;
-   executes browser-side JavaScript;
-   subscribes to MQTT topics through WebSockets;
-   presents IoT information to the user.

------------------------------------------------------------------------

## 24. Important Security Consideration

GitHub Pages is a client-side hosting platform. JavaScript downloaded by
the browser can be inspected by users.

Therefore, students should **not place privileged MQTT credentials,
private API keys, passwords, or other secrets directly inside a public
GitHub repository or `index.html` file**.

For this laboratory, use only instructor-provided test infrastructure
and accounts with limited permissions.

A more secure architecture for later laboratories can introduce an
application backend:

``` text
ESP32
  ↓
MQTT Broker
  ↓
Backend / API
  ↓
Web Dashboard
```

------------------------------------------------------------------------

# Part F --- Questions

## 25. Review Questions

1.  What is the role of the MQTT broker?

2.  What is the difference between an MQTT publisher and subscriber?

3.  What is an MQTT topic?

4.  Why does the ESP32 publish data instead of sending it directly to
    the GitHub.io dashboard?

5.  Why does the web dashboard use MQTT over WebSockets?

6.  What is the difference between `ws://` and `wss://`?

7.  What happens if the MQTT broker becomes unavailable?

8.  Why should passwords and privileged MQTT credentials not be stored
    directly in a public GitHub repository?

9.  What happens when the ESP32 publishes faster than the dashboard
    requires?

10. How could the architecture be extended to support 100 IoT devices?

------------------------------------------------------------------------

# Part G --- Student Assignment

## 26. Required Tasks

Develop a working IoT monitoring system that demonstrates:

**ESP32 → MQTT → MQTT Broker → GitHub.io**

The final dashboard must display at least:

-   temperature;
-   MQTT connection status;
-   device name;
-   latest received value.

### Extension

Add:

-   humidity;
-   message timestamp;
-   message counter;
-   online/offline indication.

------------------------------------------------------------------------

## 27. Laboratory Report

Submit a short laboratory report containing:

### 1. System Architecture

Draw and explain:

``` text
ESP32 → MQTT → Broker → WSS → GitHub.io
```

### 2. MQTT Configuration

Report:

``` text
MQTT Broker:
MQTT Port:
WebSocket Endpoint:
Topic:
Publish Interval:
```

Do **not** include passwords or secret credentials in the report.

### 3. ESP32 Program

Include the important sections of the ESP32 implementation.

### 4. Dashboard

Include the HTML/JavaScript implementation and a screenshot of the
deployed GitHub.io dashboard.

### 5. Experimental Results

Show evidence that the ESP32 data successfully reaches the dashboard.

### 6. Discussion

Explain:

-   MQTT publish/subscribe communication;
-   MQTT topics;
-   WebSockets;
-   GitHub Pages;
-   advantages and limitations of the architecture.

### 7. Conclusion

Summarize what was learned from building the complete IoT data path.

------------------------------------------------------------------------

## 28. Expected Result

At the end of the laboratory, students should have a working system
similar to:

``` text
              MQTT Broker
             /           \
            /             \
       MQTT                 WSS
          /                   \
         ▼                     ▼
┌────────────────┐     ┌───────────────────┐
│     ESP32      │     │    GitHub.io     │
│                │     │                   │
│ Temp = 28.5 °C │ ──► │ Temperature      │
│                │     │    28.5 °C        │
│ Wi-Fi: Online  │     │                   │
└────────────────┘     │ MQTT: Connected   │
                       └───────────────────┘
```

The experiment demonstrates an important IoT principle:

**The embedded device generates physical-world information, MQTT
transports the information, the broker distributes it, and the web
application converts the information into a human-readable interface.**

------------------------------------------------------------------------

## 29. Summary

This laboratory introduced a minimal end-to-end IoT architecture based
on ESP32, MQTT, WebSockets, and GitHub Pages.

The complete communication chain is:

**Sensor/Data → ESP32 → MQTT → Broker → MQTT over WebSockets → GitHub.io
→ User**

This architecture provides the foundation for subsequent laboratories
involving **bidirectional device control, JSON messaging, real-time
charts, databases, Node-RED, security, and edge intelligence**.
