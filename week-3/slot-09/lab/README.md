# LAB 9 — Wi-Fi Networking & HTTP Clients

## 1. Objective

The objective of this lab is to:

1. Understand the basic principles of **Wi-Fi networking** on the ESP32.
2. Connect an ESP32 to a wireless access point.
3. Obtain and display network information such as **IP address, RSSI, and gateway**.
4. Synchronize system time using an **NTP server**.
5. Implement an **HTTP GET client**.
6. Implement an **HTTP POST client**.
7. Exchange sensor data with a web server using **HTTP and JSON**.
8. Integrate Wi-Fi communication with FreeRTOS tasks.
9. Investigate connection status, response codes, latency, and communication reliability.

---

## 2. Background

Connected embedded systems commonly communicate with cloud platforms, web servers, dashboards, and REST APIs through Wi-Fi.

A typical communication architecture is:

```text
Sensor
   ↓
ESP32
   ↓
Wi-Fi Access Point
   ↓
Internet / Local Network
   ↓
Web Server / REST API
```

The ESP32 includes an integrated Wi-Fi interface and supports standard TCP/IP networking.

In this lab, the ESP32 operates as a **Wi-Fi station** and connects to an existing wireless network.

### Key Concepts

- **SSID:** Name of a Wi-Fi network.
- **Password:** Security credential used to access the network.
- **Station Mode (STA):** ESP32 connects to an existing Wi-Fi access point.
- **Access Point Mode (AP):** ESP32 creates its own Wi-Fi network.
- **IP Address:** Logical address assigned to a network device.
- **Gateway:** Device that connects the local network to another network.
- **DNS:** Converts domain names into IP addresses.
- **RSSI:** Received Signal Strength Indicator.
- **HTTP:** Application-layer protocol used for web communication.
- **HTTP GET:** Requests data from a server.
- **HTTP POST:** Sends data to a server.
- **REST API:** Web interface based on HTTP operations.
- **JSON:** Lightweight data format commonly used by web APIs.
- **NTP:** Network Time Protocol used to synchronize clocks.

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Wi-Fi access point or smartphone hotspot
- Optional analog sensor
- Optional temperature/environmental sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- Internet connection for external HTTP experiments

### Libraries

The following libraries are included with the ESP32 Arduino environment:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
```

For JSON-based applications, the optional **ArduinoJson** library may also be used.

---

## 4. Wi-Fi Communication Architecture

The basic ESP32 networking process is:

```text
ESP32 Startup
     ↓
WiFi.begin()
     ↓
Connect to Access Point
     ↓
Obtain IP Address
     ↓
TCP/IP Communication
     ↓
HTTP Request
     ↓
Server Response
```

In station mode:

```text
             Wi-Fi
ESP32 ─────────────────► Router
                           │
                           ▼
                       Internet
                           │
                           ▼
                       Web Server
```

---

## 5. Experiment 1 — Connecting the ESP32 to Wi-Fi

The first experiment connects the ESP32 to a wireless network.

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

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
Connecting to Wi-Fi.....
Wi-Fi connected
IP Address: 192.168.1.105
```

The actual IP address depends on the local network.

---

## 6. Experiment 2 — Displaying Network Information

Once connected, the ESP32 can display additional network information.

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());

  Serial.print("DNS Server: ");
  Serial.println(WiFi.dnsIP());

  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void loop() {}
```

A typical output is:

```text
IP Address: 192.168.1.105
Gateway: 192.168.1.1
Subnet Mask: 255.255.255.0
DNS Server: 192.168.1.1
RSSI: -48 dBm
```

---

## 7. Wi-Fi Signal Strength

The ESP32 reports signal strength using:

```cpp
WiFi.RSSI();
```

RSSI is measured in **dBm**.

A simplified interpretation is:

| RSSI | Approximate Quality |
|---:|---|
| -30 dBm | Excellent |
| -50 dBm | Very good |
| -60 dBm | Good |
| -70 dBm | Weak |
| -80 dBm | Poor |
| Below -90 dBm | Very poor / unstable |

The received signal quality generally decreases as the ESP32 moves farther from the access point.

---

## 8. Experiment 3 — Monitoring Wi-Fi Connection Status

This experiment continuously checks the connection.

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    Serial.print("Connected | IP: ");
    Serial.print(WiFi.localIP());

    Serial.print(" | RSSI: ");
    Serial.print(WiFi.RSSI());

    Serial.println(" dBm");

  } else {

    Serial.println("Wi-Fi disconnected");
  }

  delay(2000);
}
```

### Observation

Try:

- Moving the ESP32 away from the access point.
- Temporarily disabling the Wi-Fi hotspot.
- Re-enabling the network.

Observe how the connection status changes.

---

## 9. Experiment 4 — Automatic Wi-Fi Reconnection

A connected embedded device should recover from temporary network failures.

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void connectWiFi() {

  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("Connected!");
}

void setup() {

  Serial.begin(115200);

  connectWiFi();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("Connection lost");

    WiFi.disconnect();

    connectWiFi();
  }

  delay(2000);
}
```

The recovery sequence is:

```text
Connected
    ↓
Connection Lost
    ↓
Detect Failure
    ↓
Reconnect
    ↓
Resume Communication
```

---

## 10. Experiment 5 — Network Time Synchronization Using NTP

IoT systems frequently require correct timestamps.

The ESP32 can obtain time from an NTP server.

```cpp
#include <WiFi.h>
#include <time.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer = "pool.ntp.org";

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    ntpServer
  );

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    Serial.println("Failed to obtain time");

    return;
  }

  Serial.println(
    &timeinfo,
    "%Y-%m-%d %H:%M:%S"
  );
}

void loop() {}
```

### Expected Output

```text
2026-08-31 08:30:15
```

The exact value depends on the current network time.

---

## 11. HTTP Communication

HTTP uses a request-response model:

```text
ESP32 Client
     ↓
HTTP Request
     ↓
Web Server
     ↓
HTTP Response
     ↓
ESP32 Client
```

Common HTTP methods include:

```text
GET     → Request data
POST    → Send new data
PUT     → Update data
DELETE  → Delete data
```

This lab focuses primarily on **GET** and **POST**.

---

## 12. Experiment 6 — HTTP GET Request

An HTTP GET request can retrieve information from a web server.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL =
    "http://example.com";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  HTTPClient http;

  http.begin(serverURL);

  int httpCode = http.GET();

  Serial.print(
    "HTTP Response Code: "
  );

  Serial.println(httpCode);

  if (httpCode > 0) {

    String payload =
      http.getString();

    Serial.println(
      "Response:"
    );

    Serial.println(payload);
  }

  http.end();
}

void loop() {}
```

The sequence is:

```text
ESP32
  ↓
HTTP GET
  ↓
Server
  ↓
HTTP Response
  ↓
Payload
```

---

## 13. HTTP Status Codes

A web server returns a numerical status code.

Common examples include:

| Code | Meaning |
|---:|---|
| 200 | OK |
| 201 | Created |
| 204 | Success with no response body |
| 301 | Redirect |
| 400 | Bad Request |
| 401 | Unauthorized |
| 403 | Forbidden |
| 404 | Not Found |
| 500 | Internal Server Error |
| 503 | Service Unavailable |

A successful GET request commonly returns:

```text
HTTP 200 OK
```

---

## 14. Experiment 7 — Periodic HTTP GET

Move the GET operation into `loop()`:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL =
    "http://example.com";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
  }

  Serial.println(
    "Wi-Fi connected"
  );
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(serverURL);

    int httpCode =
      http.GET();

    Serial.printf(
      "HTTP Code: %d\n",
      httpCode
    );

    if (httpCode > 0) {

      String payload =
        http.getString();

      Serial.println(payload);
    }

    http.end();
  }

  delay(5000);
}
```

This sends a request approximately every 5 seconds.

---

## 15. Experiment 8 — HTTP POST Request

HTTP POST is commonly used to send sensor or application data to a server.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL =
    "http://example.com/api/data";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
  }

  Serial.println(
    "Wi-Fi connected"
  );

  HTTPClient http;

  http.begin(serverURL);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  String jsonData =
    "{\"temperature\":25.5,\"humidity\":60.2}";

  int httpCode =
    http.POST(jsonData);

  Serial.print(
    "HTTP Response Code: "
  );

  Serial.println(httpCode);

  if (httpCode > 0) {

    String response =
      http.getString();

    Serial.println(response);
  }

  http.end();
}

void loop() {}
```

The communication model is:

```text
Sensor Data
    ↓
JSON
    ↓
HTTP POST
    ↓
REST API
    ↓
Server
```

---

## 16. JSON Data Format

JSON represents data using key-value pairs.

Example:

```json
{
  "temperature": 25.5,
  "humidity": 60.2
}
```

A more complete IoT message may contain:

```json
{
  "device": "ESP32-01",
  "temperature": 25.5,
  "humidity": 60.2,
  "timestamp": 1788138000
}
```

JSON is commonly used because it is:

- Human-readable
- Lightweight
- Easy to parse
- Widely supported by REST APIs

---

## 17. Experiment 9 — Sending Sensor Data via HTTP POST

An analog sensor can be read and transmitted to a server.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 34

const char* ssid =
    "YOUR_WIFI_NAME";

const char* password =
    "YOUR_WIFI_PASSWORD";

const char* serverURL =
    "http://example.com/api/data";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
  }

  Serial.println(
    "Wi-Fi connected"
  );
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    int sensorValue =
      analogRead(SENSOR_PIN);

    String jsonData =
      "{\"sensor\":" +
      String(sensorValue) +
      "}";

    HTTPClient http;

    http.begin(serverURL);

    http.addHeader(
      "Content-Type",
      "application/json"
    );

    int responseCode =
      http.POST(jsonData);

    Serial.print(
      "Sensor = "
    );

    Serial.println(sensorValue);

    Serial.print(
      "Response = "
    );

    Serial.println(responseCode);

    http.end();
  }

  delay(5000);
}
```

---

## 18. Experiment 10 — HTTP Client with FreeRTOS Tasks

Wi-Fi communication can be separated from sensor acquisition using FreeRTOS.

The architecture is:

```text
Sensor Task
     ↓
Queue
     ↓
HTTP Task
     ↓
Wi-Fi
     ↓
REST API
```

Example:

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 34

QueueHandle_t sensorQueue;

const char* ssid =
    "YOUR_WIFI_NAME";

const char* password =
    "YOUR_WIFI_PASSWORD";

const char* serverURL =
    "http://example.com/api/data";

void SensorTask(void *pvParameters) {

  int value;

  for (;;) {

    value =
      analogRead(SENSOR_PIN);

    xQueueSend(
      sensorQueue,
      &value,
      portMAX_DELAY
    );

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}

void HTTPTask(void *pvParameters) {

  int receivedValue;

  for (;;) {

    if (xQueueReceive(
          sensorQueue,
          &receivedValue,
          portMAX_DELAY) == pdTRUE) {

      if (WiFi.status() ==
          WL_CONNECTED) {

        HTTPClient http;

        http.begin(serverURL);

        http.addHeader(
          "Content-Type",
          "application/json"
        );

        String json =
          "{\"sensor\":" +
          String(receivedValue) +
          "}";

        int code =
          http.POST(json);

        Serial.printf(
          "HTTP response: %d\n",
          code
        );

        http.end();
      }
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

  sensorQueue =
    xQueueCreate(
      5,
      sizeof(int)
    );

  xTaskCreate(
    SensorTask,
    "Sensor",
    2048,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    HTTPTask,
    "HTTP",
    8192,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

This demonstrates the progression from the previous RTOS labs:

```text
Sensor Acquisition
      ↓
FreeRTOS Queue
      ↓
Network Task
      ↓
HTTP
      ↓
Web Server
```

---

## 19. Measuring HTTP Latency

The time required for an HTTP request can be measured using `millis()`.

```cpp
unsigned long startTime =
    millis();

int httpCode =
    http.GET();

unsigned long endTime =
    millis();

unsigned long latency =
    endTime - startTime;

Serial.printf(
  "HTTP Latency = %lu ms\n",
  latency
);
```

The latency can be defined as:

$$
T_{\text{HTTP}}
=
t_{\text{response}}
-
t_{\text{request}}
$$

where:

- $t_{\text{request}}$ is the time when the HTTP request begins.
- $t_{\text{response}}$ is the time when the request completes.

---

## 20. Experiment 11 — Wi-Fi Signal Strength vs. HTTP Latency

Measure both:

```cpp
WiFi.RSSI()
```

and HTTP latency.

Record the values at different distances from the access point.

Suggested table:

| Test | RSSI (dBm) | HTTP Response Code | Latency (ms) |
|---:|---:|---:|---:|
| 1 |  |  |  |
| 2 |  |  |  |
| 3 |  |  |  |
| 4 |  |  |  |

Discuss whether weaker signal conditions affect communication performance.

---

## 21. HTTP GET vs. HTTP POST

| Feature | GET | POST |
|---|---|---|
| Main purpose | Retrieve data | Send data |
| Request body | Usually no | Usually yes |
| Typical IoT use | Read configuration | Upload sensor data |
| Data visibility | Often URL/query | Request body |
| JSON support | Response commonly JSON | Request commonly JSON |
| Example | Get weather data | Upload temperature |

A useful rule is:

```text
GET  → Retrieve
POST → Send
```

---

## 22. HTTP vs. Local Sensor Processing

Without networking:

```text
Sensor
   ↓
ESP32
   ↓
Local Processing
   ↓
Local Output
```

With HTTP networking:

```text
Sensor
   ↓
ESP32
   ↓
Wi-Fi
   ↓
HTTP
   ↓
Cloud / Server
   ↓
Dashboard / Database
```

Wi-Fi and HTTP therefore transform an isolated embedded system into a **connected embedded system**.

---

## 23. Experiments

### Experiment 1 — Wi-Fi Connection

Connect the ESP32 to a Wi-Fi network and record:

- SSID
- Local IP address
- Gateway IP
- RSSI

### Experiment 2 — Signal Strength

Measure RSSI at:

- Near access point
- Medium distance
- Far distance

Compare signal strength.

### Experiment 3 — Connection Recovery

Turn off the Wi-Fi access point temporarily.

Observe:

```text
Connection Lost
      ↓
Detection
      ↓
Reconnection
```

Measure reconnection time.

### Experiment 4 — NTP Time

Retrieve current network time.

Record:

- Date
- Time
- Time synchronization delay

### Experiment 5 — HTTP GET

Send a GET request and record:

- URL
- HTTP response code
- Payload size
- Request latency

### Experiment 6 — HTTP POST

Send a JSON message containing:

```json
{
  "temperature": 25.0,
  "humidity": 60.0
}
```

Record the server response.

### Experiment 7 — Periodic Sensor Upload

Read a sensor every second but upload the data every 5 seconds.

Implement:

```text
Sensor Task
   ↓
Queue
   ↓
HTTP Task
   ↓
Server
```

### Experiment 8 — Network Failure

Disconnect Wi-Fi while the system is transmitting.

Observe:

- HTTP error behavior
- Queue behavior
- Reconnection
- Data recovery strategy

---

## 24. Exercises

### Exercise 1 — Wi-Fi Monitor

Create a program that prints every 2 seconds:

```text
Wi-Fi Status
IP Address
RSSI
System Uptime
```

### Exercise 2 — NTP Clock

Create an ESP32 clock that displays:

```text
YYYY-MM-DD HH:MM:SS
```

every second using network time.

### Exercise 3 — HTTP GET Client

Create a FreeRTOS task that sends an HTTP GET request every 10 seconds.

Display:

```text
HTTP Code
Payload Size
Latency
```

### Exercise 4 — JSON Sensor Upload

Create:

```json
{
  "device": "ESP32",
  "sensor": 2048,
  "rssi": -55
}
```

and send it using HTTP POST.

### Exercise 5 — FreeRTOS IoT Pipeline

Develop:

```text
Sensor Task
      ↓
Queue
      ↓
Processing Task
      ↓
Queue
      ↓
HTTP Communication Task
      ↓
REST Server
```

The processing task should normalize the ADC reading before transmission.

### Exercise 6 — Communication Reliability

Perform 20 HTTP requests.

Record:

- Successful requests
- Failed requests
- Average latency

Calculate the HTTP request success rate:

$$
\text{Success Rate}
=
\frac{N_{\text{successful}}}
     {N_{\text{total}}}
\times 100\%
$$

---

## 25. Checkpoint Questions

1. What is Wi-Fi station mode?
2. What does `WiFi.begin()` do?
3. What is an IP address?
4. What is the purpose of a gateway?
5. What does RSSI represent?
6. Why is RSSI expressed using negative dBm values?
7. What is NTP used for?
8. What is HTTP?
9. What is the difference between HTTP GET and HTTP POST?
10. What is an HTTP status code?
11. What does HTTP status code `200` indicate?
12. Why is JSON commonly used in IoT systems?
13. What is a REST API?
14. Why should `http.end()` be called after an HTTP request?
15. What happens if an HTTP request is attempted while Wi-Fi is disconnected?
16. Why is automatic Wi-Fi reconnection important?
17. How can HTTP latency be measured?
18. Why can network communication be separated into its own FreeRTOS task?
19. What is the advantage of using a queue between sensor and communication tasks?
20. How can weak Wi-Fi signal strength affect an IoT application?

---

## 26. Expected Results

Students should successfully connect an ESP32 to a Wi-Fi network and obtain valid network information.

The basic network workflow should be:

```text
ESP32
   ↓
Wi-Fi Connection
   ↓
IP Configuration
   ↓
HTTP Request
   ↓
Web Server
   ↓
HTTP Response
```

For sensor communication:

```text
Sensor
   ↓
Sensor Task
   ↓
Queue
   ↓
HTTP Task
   ↓
Wi-Fi
   ↓
REST API
```

Students should also observe that network communication introduces variable delay compared with purely local embedded processing.

---

## 27. Submission

Submit the following:

- ESP32 Wi-Fi connection source code.
- Screenshot showing successful Wi-Fi connection.
- Network information including IP address and RSSI.
- NTP synchronization result.
- HTTP GET source code and output.
- HTTP POST source code and output.
- JSON sensor-data example.
- FreeRTOS sensor-to-HTTP pipeline.
- HTTP latency measurements.
- Short discussion of network reliability.

Suggested Wi-Fi results table:

| Test | RSSI (dBm) | Connection Status | IP Address |
|---:|---:|---|---|
| 1 |  |  |  |
| 2 |  |  |  |
| 3 |  |  |  |

Suggested HTTP results table:

| Request | Method | Response Code | Latency (ms) | Success |
|---:|---|---:|---:|---|
| 1 | GET |  |  |  |
| 2 | GET |  |  |  |
| 3 | POST |  |  |  |
| 4 | POST |  |  |  |

---

## 28. Conclusion

In this lab, the ESP32 is extended from a standalone real-time embedded system into a **network-connected embedded platform**. Wi-Fi station mode is used to establish network connectivity, NTP provides synchronized time, and HTTP GET and POST requests enable communication with web servers and REST APIs.

The lab also demonstrates how FreeRTOS queues can connect sensor acquisition and network communication tasks. This separation improves modularity and provides a foundation for more advanced IoT messaging protocols.

The laboratory progression is:

```text
LAB 5
Scheduling & Tasks
      ↓
LAB 6
Semaphores & Mutexes
      ↓
LAB 7
Inter-Task Communication via Queues
      ↓
LAB 8
Software Timers & Memory Management
      ↓
LAB 9
Wi-Fi Networking & HTTP Clients
      ↓
LAB 10
MQTT Telemetry & Publish/Subscribe
```

The next lab focuses on **MQTT telemetry, broker communication, publish/subscribe messaging, JSON payloads, and QoS**.
