# Lab: IoT Dashboard with GitHub.io and MQTT

### Objective

Students will create a simple web-based IoT dashboard hosted using
[GitHub Pages](https://pages.github.com/?utm_source=chatgpt.com). The
dashboard connects to an MQTT broker using WebSockets and displays
sensor data published by an ESP32.

### System Architecture

``` text
┌────────────────┐
│ Temperature    │
│ Sensor / ESP32 │
└───────┬────────┘
        │
        │ MQTT Publish
        ▼
┌────────────────┐
│  MQTT Broker   │
│                │
│ MQTT + WSS     │
└───────┬────────┘
        │
        │ MQTT over WebSocket
        ▼
┌────────────────────────┐
│ GitHub.io              │
│                        │
│ HTML + JavaScript      │
│ MQTT.js                │
│                        │
│ Temperature: 28.5 °C   │
└────────────────────────┘
```

### Part 1 --- MQTT Topic

For the first experiment, use one simple topic:

``` text
iot/lab/temperature
```

The ESP32 can periodically publish a simulated temperature value such
as:

``` text
25.4
26.1
27.8
28.3
```

### Part 2 --- Create the Dashboard

Create a file named `index.html`:

``` html
<!DOCTYPE html>
<html>
<head>
    <title>IoT MQTT Dashboard</title>

    <script src=
    "https://unpkg.com/mqtt/dist/mqtt.min.js">
    </script>
</head>

<body>

<h1>IoT Dashboard</h1>

<h2>Temperature</h2>

<h1 id="temperature">-- °C</h1>

<p id="status">Connecting...</p>

<script>

const broker =
    "wss://YOUR-MQTT-BROKER-WEBSOCKET";

const client = mqtt.connect(broker);

client.on("connect", function () {

    document.getElementById("status").innerText =
        "MQTT Connected";

    client.subscribe("iot/lab/temperature");

});

client.on("message", function(topic, message) {

    const temperature = message.toString();

    document.getElementById("temperature").innerText =
        temperature + " °C";

});

client.on("error", function(error) {

    document.getElementById("status").innerText =
        "Connection Error";

    console.log(error);

});

</script>

</body>
</html>
```

The browser uses
[MQTT.js](https://github.com/mqttjs/MQTT.js?utm_source=chatgpt.com) to
communicate with the broker.

### Part 3 --- Publish from ESP32

The ESP32 performs this basic operation:

``` text
Connect Wi-Fi
     ↓
Connect MQTT Broker
     ↓
Read Temperature
     ↓
Publish
iot/lab/temperature
     ↓
Wait
     ↓
Repeat
```

A simplified Arduino sketch structure is:

``` cpp
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup()
{
    Serial.begin(115200);

    // Connect Wi-Fi

    // Configure MQTT broker

    // Connect MQTT
}

void loop()
{
    float temperature =
        random(250, 350) / 10.0;

    char value[10];

    dtostrf(
        temperature,
        4,
        1,
        value
    );

    mqtt.publish(
        "iot/lab/temperature",
        value
    );

    delay(2000);
}
```

Students can initially use **simulated temperature values**, which
removes the need for a physical sensor during the first experiment.

### Part 4 --- Publish the Dashboard on GitHub.io

Create a GitHub repository, for example:

``` text
iot-mqtt-dashboard
```

Add:

``` text
iot-mqtt-dashboard/
│
├── index.html
└── README.md
```

Commit and push the files to GitHub.

Then open the repository's:

**Settings → Pages**

Select the appropriate branch and publish the site. GitHub will provide
an address similar to:

``` text
username.github.io/iot-mqtt-dashboard/
```

The official [GitHub Pages
documentation](https://docs.github.com/en/pages?utm_source=chatgpt.com)
explains the publishing options.

### Part 5 --- Test the Complete System

The expected communication path is:

``` text
ESP32
  │
  │ publish 28.5
  ▼
iot/lab/temperature
  │
  ▼
MQTT Broker
  │
  │ WSS
  ▼
GitHub.io
  │
  ▼
Temperature
   28.5 °C
```

Change the ESP32's published value and verify that the value displayed
on the webpage changes without refreshing the page.

### Student Tasks

1.  Create a GitHub repository and `index.html`.
2.  Enable GitHub Pages.
3.  Connect the webpage to an MQTT broker supporting secure WebSockets
    (`wss://`).
4.  Subscribe to `iot/lab/temperature`.
5.  Publish simulated temperature data from an ESP32.
6.  Verify real-time updates on the GitHub.io dashboard.
7.  Extend the dashboard with **humidity**, **device status**, and a
    simple historical chart.

### Expected Learning Outcomes

After completing the lab, students should understand the relationship
between **ESP32, MQTT topics, an MQTT broker, MQTT-over-WebSockets,
JavaScript, and GitHub Pages**.

The key concept is:

> **GitHub.io provides the dashboard front end; MQTT provides real-time
> messaging; the MQTT broker connects the IoT device and web
> dashboard.**

For a first lab, I would keep it to **ESP32 → MQTT Broker → GitHub.io**
before adding Node-RED or a database. This makes the role of each IoT
component much easier to observe.
