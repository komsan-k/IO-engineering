# Lab --- ESP32 MQTT Dashboard with GitHub Pages

## Objective

In this lab, students create a simple IoT dashboard in which an ESP32
publishes a counter value through MQTT and a web page hosted on GitHub
Pages displays the value.

After completing this lab, students should be able to:

-   Connect an ESP32 to Wi-Fi.
-   Publish data using MQTT.
-   Use an MQTT broker.
-   Subscribe to MQTT data from a browser using WebSocket/WSS.
-   Build a dashboard with HTML, CSS, JavaScript, and MQTT.js.
-   Deploy the dashboard using GitHub Pages.

## System Architecture

``` text
+----------------+
|     ESP32      |
| Counter: 1,2,3 |
+-------+--------+
        |
        | MQTT/TCP :1883
        v
+---------------------+
|     MQTT Broker     |
| broker.hivemq.com   |
+----------+----------+
           |
           | MQTT/WSS :8884
           v
+--------------------------+
|      GitHub Pages        |
| index.html + MQTT.js     |
| Counter: 123             |
+--------------------------+
```

## MQTT Topic

``` text
test/counter_topic
```

The ESP32 publisher and browser subscriber must use the same topic.

> For a classroom with multiple groups, assign each group a unique topic
> such as `coc/iot/group01/counter`.

# Part 1 --- ESP32 MQTT Publisher

## Required Libraries

-   `WiFi.h`
-   `PubSubClient`

Install **PubSubClient** using the Arduino Library Manager if required.

## Complete Arduino Code

Save as `ESP32_MQTT_Counter_GitHub_Pages.ino`.

``` cpp
#include <WiFi.h>
#include <PubSubClient.h>

const char* BROKER_ADDRESS = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char* TOPIC = "test/counter_topic";
const unsigned long PUBLISH_INTERVAL = 3000;

const char* ssid = "coc-iot-lab";
const char* password = "computing";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
unsigned long messageCount = 0;

void setupWiFi();
void reconnectMQTT();
void publishCounter();

void setup() {
  Serial.begin(115200);
  setupWiFi();
  client.setServer(BROKER_ADDRESS, BROKER_PORT);
  Serial.println("ESP32 MQTT Counter Started");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();
  publishCounter();
}

void setupWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");

    String clientId =
      "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" - retrying in 5 seconds");
      delay(5000);
    }
  }
}

void publishCounter() {
  if (millis() - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = millis();
    messageCount++;

    String message = String(messageCount);

    Serial.print("Publishing to ");
    Serial.print(TOPIC);
    Serial.print(": ");
    Serial.println(message);

    client.publish(TOPIC, message.c_str());
  }
}
```

## Expected Serial Output

``` text
Connecting to Wi-Fi: coc-iot-lab
....
Wi-Fi connected
IP address: 192.168.1.25
Connecting to MQTT broker...connected
ESP32 MQTT Counter Started
Publishing to test/counter_topic: 1
Publishing to test/counter_topic: 2
Publishing to test/counter_topic: 3
```

# Part 2 --- GitHub Pages Dashboard

The ESP32 connects to the broker using normal MQTT/TCP on port `1883`.

The browser dashboard uses secure MQTT over WebSocket:

``` text
wss://broker.hivemq.com:8884/mqtt
```

The dashboard loads MQTT.js and subscribes to `test/counter_topic`.

## Complete `index.html`

``` html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 MQTT Dashboard</title>

  <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>

  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f4f6f8;
      margin: 0;
      text-align: center;
    }

    header {
      background: #1f2937;
      color: white;
      padding: 20px;
    }

    .container {
      max-width: 800px;
      margin: 30px auto;
      padding: 20px;
    }

    .card {
      background: white;
      padding: 25px;
      margin-bottom: 20px;
      border-radius: 12px;
      box-shadow: 0 2px 8px rgba(0,0,0,0.1);
    }

    .counter {
      font-size: 72px;
      font-weight: bold;
      margin: 20px;
    }

    .connected {
      color: green;
      font-weight: bold;
    }

    .disconnected {
      color: red;
      font-weight: bold;
    }

    .topic {
      font-family: monospace;
      background: #eeeeee;
      padding: 8px;
      border-radius: 5px;
    }

    #history {
      text-align: left;
      max-height: 250px;
      overflow-y: auto;
      font-family: monospace;
      background: #f8f8f8;
      padding: 15px;
      border-radius: 8px;
    }
  </style>
</head>

<body>

<header>
  <h1>ESP32 MQTT Dashboard</h1>
  <p>GitHub Pages + MQTT</p>
</header>

<div class="container">

  <div class="card">
    <h2>MQTT Connection</h2>

    <p>
      Status:
      <span id="status" class="disconnected">Connecting...</span>
    </p>

    <p>
      Topic:
      <span id="topicName" class="topic"></span>
    </p>
  </div>

  <div class="card">
    <h2>ESP32 Counter</h2>

    <div id="counter" class="counter">--</div>

    <p>
      Updated:
      <span id="updateTime">Waiting for data...</span>
    </p>
  </div>

  <div class="card">
    <h2>Message History</h2>
    <div id="history">Waiting for MQTT messages...</div>
  </div>

</div>

<script>
  const topic = "test/counter_topic";
  const broker = "wss://broker.hivemq.com:8884/mqtt";

  document.getElementById("topicName").textContent = topic;

  const clientId =
    "github_dashboard_" +
    Math.random().toString(16).substring(2, 10);

  const client = mqtt.connect(broker, {
    clientId: clientId,
    clean: true,
    connectTimeout: 4000,
    reconnectPeriod: 2000
  });

  client.on("connect", function () {
    const status = document.getElementById("status");

    status.textContent = "Connected";
    status.className = "connected";

    client.subscribe(topic, function (err) {
      if (err) {
        console.log("Subscription error:", err);
      } else {
        console.log("Subscribed to:", topic);
      }
    });
  });

  client.on("message", function (receivedTopic, message) {
    const value = message.toString();
    const now = new Date();

    document.getElementById("counter").textContent = value;
    document.getElementById("updateTime").textContent =
      now.toLocaleTimeString();

    const history = document.getElementById("history");

    if (history.textContent.includes("Waiting for MQTT messages")) {
      history.textContent = "";
    }

    const newMessage = document.createElement("div");

    newMessage.textContent =
      now.toLocaleTimeString() +
      " | " +
      receivedTopic +
      " -> " +
      value;

    history.prepend(newMessage);
  });

  client.on("error", function (error) {
    console.log("MQTT Error:", error);

    const status = document.getElementById("status");
    status.textContent = "Connection Error";
    status.className = "disconnected";
  });

  client.on("offline", function () {
    const status = document.getElementById("status");
    status.textContent = "Disconnected";
    status.className = "disconnected";
  });
</script>

</body>
</html>
```

# Part 3 --- Deploy with GitHub Pages

Create a GitHub repository, for example:

``` text
esp32-mqtt-dashboard
```

Place the dashboard file in the repository as:

``` text
esp32-mqtt-dashboard/
|
+-- index.html
```

Then configure:

``` text
Repository
    |
    v
Settings
    |
    v
Pages
    |
    v
Build and deployment
    |
    v
Deploy from a branch
    |
    +--> Branch: main
    |
    +--> Folder: / (root)
    |
    v
Save
```

The deployed address normally follows this form:

``` text
https://username.github.io/esp32-mqtt-dashboard/
```

# Part 4 --- Experiment

1.  Upload the Arduino sketch to the ESP32.
2.  Open Serial Monitor at `115200` baud.
3.  Confirm Wi-Fi connection.
4.  Confirm MQTT connection.
5.  Confirm that a counter value is published every three seconds.
6.  Upload `index.html` to the GitHub repository.
7.  Enable GitHub Pages.
8.  Open the GitHub Pages URL.
9.  Verify that MQTT status shows `Connected`.
10. Verify that the counter updates.
11. Observe the last-update time.
12. Observe the message history.

## Data Flow

``` text
ESP32
  |
counter++
  |
  v
client.publish()
  |
  | MQTT/TCP
  v
MQTT Broker
  |
  | MQTT/WSS
  v
GitHub Pages
  |
  v
MQTT.js subscribe
  |
  v
client.on("message")
  |
  v
Update Dashboard
```

## Checkpoint Questions

1.  What is the role of the MQTT broker?
2.  What does `client.publish()` do?
3.  What is the difference between publish and subscribe?
4.  Why does the ESP32 use port `1883` while the web dashboard uses port
    `8884`?
5.  What happens if the ESP32 and dashboard use different MQTT topics?
6.  Why is `client.loop()` required?
7.  How often does the ESP32 publish when `PUBLISH_INTERVAL = 3000`?
8.  What is the purpose of `client.on("message", ...)`?

# Assignment --- LDR Smart Lighting Dashboard

Extend the experiment to:

``` text
LDR Sensor
    |
    v
ESP32 FreeRTOS
    |
    v
Sensor Task
    |
    v
Control Task
    |
    +--> DARK / BRIGHT
    |
    +--> LED ON / OFF
    |
    v
MQTT
    |
    v
GitHub Pages Dashboard
```

The extended dashboard should display:

-   LDR value
-   Light state: `DARK` or `BRIGHT`
-   LED state: `ON` or `OFF`
-   MQTT connection status
-   Last update time
