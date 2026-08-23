# 📘 Theory: Wi-Fi Modes, Embedded Networking, and REST APIs  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

The **ESP32** is a microcontroller platform that integrates processing, memory, peripheral interfaces, and wireless communication capabilities. One of its most important features for Internet of Things (IoT) development is integrated **Wi-Fi connectivity**.

Wi-Fi allows an ESP32-based embedded system to communicate with:

- computers,
- smartphones,
- local servers,
- cloud platforms,
- REST API servers,
- other ESP32 devices,
- IoT gateways,
- databases through application servers.

A typical connected embedded system can be represented as:

```text
Sensor
   │
   ▼
 ESP32
   │
   ▼
 Wi-Fi
   │
   ▼
Network
   │
   ▼
REST API / Server / Cloud
```

Therefore,

$$
\boxed{
\text{Connected Embedded System}
=
\text{MCU}
+
\text{Networking}
+
\text{Communication Protocol}
+
\text{Application Service}
}
$$

This chapter introduces three fundamental concepts:

1. **ESP32 Wi-Fi operating modes**
2. **Embedded networking**
3. **REST API communication**

---

## 📡 2. ESP32 Wi-Fi

Wi-Fi is based on the IEEE 802.11 family of wireless networking standards.

ESP32 variants differ in exactly which Wi-Fi standards and bands they support, but Wi-Fi-enabled ESP32 devices are commonly used to connect embedded applications to an IP network.

Conceptually:

```text
┌─────────────────────────┐
│          ESP32          │
│                         │
│  ┌───────────────────┐  │
│  │ Application       │  │
│  └─────────┬─────────┘  │
│            │            │
│  ┌─────────▼─────────┐  │
│  │ TCP/IP Stack      │  │
│  └─────────┬─────────┘  │
│            │            │
│  ┌─────────▼─────────┐  │
│  │ Wi-Fi Interface   │  │
│  └───────────────────┘  │
└────────────┬────────────┘
             │
             ▼
        Wi-Fi Network
```

This integration makes the ESP32 suitable for network-connected embedded systems.

---

## ⚙️ 3. ESP32 Wi-Fi Operating Modes

Three important operating configurations are:

1. **Station mode — STA**
2. **Access Point mode — AP**
3. **AP + STA mode**

They can be summarized as:

```text
ESP32 Wi-Fi
│
├── Station (STA)
├── Access Point (AP)
└── AP + Station
```

---

## 📶 4. Station Mode

In **Station mode**, the ESP32 behaves like a normal Wi-Fi client.

It connects to an existing wireless access point or router.

```text
ESP32
  │
  │ Wi-Fi
  ▼
Router / AP
  │
  ▼
Local Network
  │
  ▼
Internet
```

The ESP32 therefore becomes a device on an existing IP network.

Typical applications include:

- IoT sensor nodes,
- smart-home devices,
- cloud-connected devices,
- environmental monitoring,
- remote control,
- REST API clients,
- MQTT clients.

---

## 🔑 5. Connecting the ESP32 to Wi-Fi

Using the Arduino framework, a basic station-mode connection can be written as:

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
}
```

The process is:

```text
ESP32 Start
    │
    ▼
Set STA Mode
    │
    ▼
WiFi.begin()
    │
    ▼
Authenticate
    │
    ▼
Obtain Network Configuration
    │
    ▼
Connected
```

---

## 🌐 6. IP Address

After joining a network, the ESP32 needs an **IP address** for IP communication.

For example:

```text
192.168.1.50
```

An IPv4 address contains:

$$
32~\text{bits}
$$

and is conventionally represented as four 8-bit decimal values:

$$
192.168.1.50
$$

Conceptually:

```text
192       168       1        50
 │         │        │         │
8 bits    8 bits   8 bits    8 bits
```

Thus:

$$
4\times8=32~bits.
$$

---

## 🏠 7. Private IP Addresses

Local networks commonly use private IPv4 address ranges.

Examples include:

```text
10.x.x.x
172.16.x.x – 172.31.x.x
192.168.x.x
```

For example:

```text
ESP32      192.168.1.50
Laptop     192.168.1.20
Router     192.168.1.1
```

These devices can communicate within the local network when routing and firewall rules permit it.

---

## 🔢 8. DHCP

The ESP32 commonly receives its network configuration automatically using **DHCP**.

DHCP stands for:

**Dynamic Host Configuration Protocol**

A simplified process is:

```text
ESP32
  │
  │ Request network configuration
  ▼
DHCP Server
  │
  │ Assign configuration
  ▼
ESP32
```

The assigned information may include:

- IP address,
- subnet mask,
- default gateway,
- DNS server.

Therefore:

$$
\boxed{
\text{DHCP}
\rightarrow
\text{Automatic Network Configuration}
}
$$

---

## 🚪 9. Default Gateway

The default gateway allows a device to communicate with destinations outside its local subnet.

For example:

```text
ESP32
192.168.1.50
     │
     ▼
Router
192.168.1.1
     │
     ▼
Internet
```

Without appropriate routing through a gateway, the ESP32 normally cannot reach external IP networks.

---

## 🔎 10. DNS

**DNS — Domain Name System** converts domain names into IP addresses.

For example:

```text
api.example.com
       │
       ▼
      DNS
       │
       ▼
203.0.113.x
```

Conceptually:

$$
\boxed{
\text{Domain Name}
\xrightarrow{\text{DNS}}
\text{IP Address}
}
$$

This allows applications to use hostnames rather than manually specifying server IP addresses.

---

## 📡 11. Access Point Mode

In **Access Point mode**, the ESP32 creates its own Wi-Fi network.

Other devices can connect directly to it.

```text
         ESP32
       Access Point
       /     |     \
      /      |      \
 Laptop   Phone   Tablet
```

The ESP32 behaves as a small wireless access point.

This is useful when:

- no external router is available,
- direct configuration is required,
- a local control interface is needed,
- devices need to connect directly to the ESP32.

---

## 💻 12. Creating an ESP32 Access Point

Example:

```cpp
#include <WiFi.h>

const char* ssid = "ESP32-AP";
const char* password = "12345678";

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);

    WiFi.softAP(ssid, password);

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void loop()
{
}
```

The architecture becomes:

```text
Smartphone
    │
    │ Wi-Fi
    ▼
 ESP32 AP
    │
    ▼
Local Embedded Application
```

---

## 📊 13. STA vs. AP Mode

| Feature | Station Mode | Access Point Mode |
|---|---|---|
| ESP32 connects to router | Yes | No requirement |
| ESP32 creates Wi-Fi network | No | Yes |
| Internet access | Possible through network | Not automatically |
| Direct local connection | Possible | Excellent |
| IoT cloud application | Common | Less common alone |
| Device configuration portal | Possible | Very suitable |

---

## 🔀 14. AP + STA Mode

The ESP32 can also operate with both station and access-point interfaces.

Conceptually:

```text
Smartphone
    │
    ▼
ESP32 AP
    │
    │ ESP32 STA
    ▼
Wi-Fi Router
    │
    ▼
Internet
```

This configuration can be useful for:

- provisioning,
- configuration portals,
- local maintenance,
- gateway-like applications.

---

## 🧠 15. Example Provisioning Architecture

Suppose a new IoT device does not yet know the user's Wi-Fi credentials.

The ESP32 can initially create:

```text
ESP32-Setup
```

The user connects with a smartphone:

```text
Phone
  │
  ▼
ESP32 AP
  │
  ▼
Configuration Page
```

The user provides:

```text
Home SSID
Password
```

The ESP32 can then connect to the home network as a station.

```text
ESP32
  │
  ▼
Home Router
  │
  ▼
Internet
```

---

## 🌐 16. Embedded Networking

**Embedded networking** refers to networking functions integrated into resource-constrained embedded systems.

An embedded networking stack can be viewed as:

```text
Application
     │
     ▼
HTTP / MQTT / CoAP
     │
     ▼
TCP / UDP
     │
     ▼
IP
     │
     ▼
Wi-Fi
```

Each layer performs a specific communication function.

---

## 🧱 17. Simplified Network Stack

A simplified model is:

| Layer | Examples | Purpose |
|---|---|---|
| Application | HTTP, MQTT, CoAP | Application communication |
| Transport | TCP, UDP | End-to-end data transport |
| Network | IP | Addressing and routing |
| Link | Wi-Fi | Local network communication |
| Physical | Radio | Wireless transmission |

This can be expressed as:

$$
\boxed{
\text{Application}
\rightarrow
\text{Transport}
\rightarrow
\text{IP}
\rightarrow
\text{Wi-Fi}
}
$$

---

## 📦 18. TCP

**TCP — Transmission Control Protocol** provides connection-oriented reliable transport.

Important properties include:

- ordered delivery,
- retransmission,
- error detection,
- flow control,
- congestion-control mechanisms.

Conceptually:

```text
ESP32
  │
  │ Establish Connection
  ▼
Server
  │
  │ Exchange Data
  ▼
ESP32
```

TCP is commonly used by:

- HTTP,
- HTTPS,
- MQTT,
- many web services.

---

## 🚀 19. UDP

**UDP — User Datagram Protocol** provides a simpler connectionless transport service.

Conceptually:

```text
ESP32
  │
  ├──── Datagram ────► Device
  │
  ├──── Datagram ────► Device
  │
  └──── Datagram ────► Device
```

UDP has lower protocol overhead than TCP but does not itself guarantee:

- delivery,
- ordering,
- retransmission.

---

## 📊 20. TCP vs. UDP

| Characteristic | TCP | UDP |
|---|---|---|
| Connection-oriented | Yes | No |
| Reliable delivery | Yes | Not guaranteed |
| Ordered delivery | Yes | Not guaranteed |
| Protocol overhead | Higher | Lower |
| Retransmission | Yes | No at UDP layer |
| HTTP | Commonly TCP-based | Not classic HTTP/1.1 |
| Real-time telemetry | Possible | Often useful |
| Simple datagrams | Less suitable | Excellent |

---

## 🚪 21. Network Ports

A network port identifies an application or service associated with a host.

Conceptually:

```text
IP Address
    │
    ├── Port 80   → HTTP
    ├── Port 443  → HTTPS
    └── Other Ports → Other Services
```

A communication endpoint is commonly identified using:

$$
\boxed{
\text{IP Address}+\text{Port Number}
}
$$

For example:

```text
192.168.1.100:80
```

---

## 🌍 22. HTTP

**HTTP — Hypertext Transfer Protocol** is an application-layer protocol widely used for communication between clients and servers.

Typical architecture:

```text
ESP32 Client
     │
     │ HTTP Request
     ▼
Web / API Server
     │
     │ HTTP Response
     ▼
ESP32 Client
```

HTTP is especially useful for connecting embedded systems to **REST APIs**.

---

## 🔐 23. HTTP vs. HTTPS

HTTP communication may be transmitted without transport encryption.

HTTPS adds TLS security.

```text
HTTP
Application Data
     │
     ▼
TCP
```

versus:

```text
HTTPS
Application Data
     │
     ▼
TLS
     │
     ▼
TCP
```

Therefore:

$$
\boxed{
\text{HTTPS}
=
\text{HTTP over TLS}
}
$$

HTTPS provides important security properties such as:

- encryption,
- integrity protection,
- server authentication.

---

## 🔗 24. What Is an API?

An **API — Application Programming Interface** defines how software components interact.

For an embedded device:

```text
ESP32
  │
  │ API Request
  ▼
Server
  │
  │ API Response
  ▼
ESP32
```

An API may allow the ESP32 to:

- upload sensor data,
- request configuration,
- retrieve weather data,
- update device state,
- control remote services,
- query databases indirectly.

---

## 🌐 25. What Is a REST API?

**REST** stands for:

**Representational State Transfer**

REST is an architectural style commonly used to design web APIs around **resources** and standard HTTP semantics.

For example:

```text
/api/sensors
/api/temperature
/api/devices
/api/led
```

These paths identify resources or collections of resources.

---

## 🧩 26. REST Architecture

A simplified REST system is:

```text
┌─────────────┐
│    ESP32    │
│   Client    │
└──────┬──────┘
       │
       │ HTTP Request
       ▼
┌─────────────┐
│ REST Server │
└──────┬──────┘
       │
       │ Process
       ▼
┌─────────────┐
│ Application │
│ / Database  │
└─────────────┘
```

The response follows the reverse path.

---

## 📤 27. HTTP Request

A simplified HTTP request may look like:

```http
GET /api/temperature HTTP/1.1
Host: example.com
```

The request contains information such as:

- HTTP method,
- resource path,
- headers,
- optional message body.

---

## 📥 28. HTTP Response

A server might respond:

```http
HTTP/1.1 200 OK
Content-Type: application/json

{
  "temperature": 28.5
}
```

The response normally includes:

```text
Status
Headers
Body
```

---

## 🔧 29. Common HTTP Methods

Important HTTP methods used in REST APIs include:

| Method | Typical Purpose |
|---|---|
| `GET` | Retrieve a resource |
| `POST` | Submit/create data |
| `PUT` | Replace/update a resource |
| `PATCH` | Partially update a resource |
| `DELETE` | Remove a resource |

A simplified CRUD mapping is:

```text
CREATE → POST
READ   → GET
UPDATE → PUT / PATCH
DELETE → DELETE
```

---

## 📥 30. GET Request

A `GET` request retrieves information.

Example:

```text
GET /api/temperature
```

Architecture:

```text
ESP32
  │
  │ GET temperature
  ▼
Server
  │
  │ 28.5 °C
  ▼
ESP32
```

GET should normally be used to retrieve resource representations rather than cause application-state changes.

---

## 📤 31. POST Request

A `POST` request commonly submits data to a server.

For example:

```text
POST /api/sensor
```

with:

```json
{
  "temperature": 28.5,
  "humidity": 70
}
```

Architecture:

```text
Sensor
  │
  ▼
ESP32
  │
  │ POST JSON
  ▼
REST Server
  │
  ▼
Database
```

---

## 🔄 32. PUT Request

`PUT` can be used to create or replace the representation of a resource at a known URI.

Example:

```text
PUT /api/led/1
```

Body:

```json
{
  "state": "ON"
}
```

Conceptually:

```text
ESP32
  │
  │ PUT
  ▼
Server
  │
  ▼
Update Resource
```

---

## ✏️ 33. PATCH Request

`PATCH` is commonly used for a partial modification.

For example:

```json
{
  "brightness": 75
}
```

Instead of replacing the complete LED resource, only the specified field may be modified.

---

## 🗑️ 34. DELETE Request

A `DELETE` request asks the server to remove a resource.

Example:

```text
DELETE /api/device/10
```

Conceptually:

```text
Client
  │
  │ DELETE
  ▼
Server
  │
  ▼
Remove Resource
```

---

## 📦 35. JSON

REST APIs commonly exchange data using **JSON — JavaScript Object Notation**.

Example:

```json
{
  "device": "ESP32-01",
  "temperature": 28.5,
  "humidity": 70,
  "led": true
}
```

JSON represents information using structures such as:

- objects,
- arrays,
- strings,
- numbers,
- Boolean values,
- null values.

---

## 🧠 36. ESP32 Sensor JSON Example

Suppose the ESP32 measures:

$$
T=29.2^\circ C
$$

and:

$$
H=68\%.
$$

It might send:

```json
{
  "temperature": 29.2,
  "humidity": 68
}
```

The server can then:

```text
Receive JSON
     │
     ▼
Parse Data
     │
     ▼
Store Database
     │
     ▼
Dashboard
```

---

## 🧪 37. ESP32 HTTP GET Example

Using the Arduino framework:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Connected");
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        http.begin("http://example.com/api/data");

        int httpCode = http.GET();

        if (httpCode > 0)
        {
            String payload = http.getString();

            Serial.println(httpCode);
            Serial.println(payload);
        }

        http.end();
    }

    delay(5000);
}
```

The process is:

```text
ESP32
  │
  ▼
Connect Wi-Fi
  │
  ▼
Create HTTP Client
  │
  ▼
GET Resource
  │
  ▼
Receive Response
  │
  ▼
Process Payload
```

---

## 📤 38. ESP32 HTTP POST Example

A sensor value can be sent using POST.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>

void sendData(float temperature)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        http.begin("http://example.com/api/sensor");

        http.addHeader(
            "Content-Type",
            "application/json"
        );

        String json =
            "{\"temperature\":" +
            String(temperature) +
            "}";

        int code = http.POST(json);

        Serial.print("HTTP code: ");
        Serial.println(code);

        http.end();
    }
}
```

---

## 📊 39. HTTP Status Codes

The server returns an HTTP status code indicating the result of a request.

Common categories are:

| Range | Category |
|---|---|
| `1xx` | Informational |
| `2xx` | Successful |
| `3xx` | Redirection |
| `4xx` | Client error |
| `5xx` | Server error |

---

## ✅ 40. Important HTTP Status Codes

Common examples include:

| Code | Meaning |
|---:|---|
| `200` | OK |
| `201` | Created |
| `204` | No Content |
| `400` | Bad Request |
| `401` | Unauthorized |
| `403` | Forbidden |
| `404` | Not Found |
| `500` | Internal Server Error |
| `503` | Service Unavailable |

An ESP32 application should inspect the status code rather than assuming every request succeeds.

---

## 🔐 41. REST API Authentication

A REST API may require authentication.

Common approaches include:

```text
API Key
Bearer Token
Username / Password
OAuth-derived Access Token
Client Certificate
```

For a simple API-key scheme, a request might contain:

```http
Authorization: Bearer TOKEN
```

Credentials should not be hard-coded carelessly in production firmware.

---

## 🛡️ 42. Why HTTPS Matters

Suppose an ESP32 sends:

```json
{
  "device": "ESP32-01",
  "command": "OPEN"
}
```

With unencrypted communication, data may potentially be observed or modified by an attacker with appropriate network access.

TLS-protected HTTPS helps provide:

$$
\boxed{
\text{Confidentiality}
+
\text{Integrity}
+
\text{Authentication}
}
$$

For Internet-connected IoT applications, HTTPS should generally be preferred over plain HTTP when REST APIs are used.

---

## 🔑 43. TLS Certificate Verification

HTTPS security depends not only on encryption but also on verifying the server identity.

Conceptually:

```text
ESP32
   │
   ▼
Connect Server
   │
   ▼
Receive Certificate
   │
   ▼
Verify Trust / Identity
   │
   ├── Valid → Continue
   │
   └── Invalid → Reject
```

Skipping certificate verification weakens protection against man-in-the-middle attacks.

---

## 📡 44. ESP32 as REST Client

A common architecture is:

```text
Temperature Sensor
       │
       ▼
     ESP32
       │
       │ POST
       ▼
   REST API
       │
       ▼
    Database
       │
       ▼
   Dashboard
```

In this configuration, the ESP32 initiates requests.

Therefore:

$$
\boxed{
\text{ESP32}
=
\text{REST Client}
}
$$

---

## 🖥️ 45. ESP32 as HTTP Server

The ESP32 can also provide an HTTP server.

For example:

```text
Laptop
   │
   │ HTTP GET
   ▼
 ESP32
   │
   ▼
Web Page / Data
```

The ESP32 can serve:

- sensor readings,
- device status,
- configuration pages,
- actuator controls.

---

## 💡 46. Simple ESP32 Web Server

Example:

```cpp
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void handleRoot()
{
    server.send(
        200,
        "text/plain",
        "Hello from ESP32"
    );
}

void setup()
{
    WiFi.begin("SSID", "PASSWORD");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    server.on("/", handleRoot);

    server.begin();
}

void loop()
{
    server.handleClient();
}
```

A client can request:

```text
http://ESP32-IP/
```

and receive:

```text
Hello from ESP32
```

---

## 🌡️ 47. ESP32 Sensor API

The ESP32 can expose a resource such as:

```text
/api/temperature
```

A request:

```http
GET /api/temperature
```

could produce:

```json
{
  "temperature": 29.5
}
```

The architecture is:

```text
Sensor
  │
  ▼
ESP32 REST Endpoint
  ▲
  │ GET
  │
Phone / Browser / Application
```

---

## 💡 48. REST-Controlled LED

A REST-like embedded interface could expose:

```text
/api/led
```

For example:

```text
PUT /api/led
```

with:

```json
{
  "state": true
}
```

The processing path becomes:

```text
Application
     │
     │ HTTP Request
     ▼
ESP32
     │
     ▼
Parse JSON
     │
     ▼
GPIO
     │
     ▼
LED ON
```

---

## 🧠 49. Embedded Networking State Machine

Wi-Fi networking should not always be implemented using long blocking loops.

A more robust embedded application may use states such as:

```text
INIT
 │
 ▼
CONNECTING
 │
 ├──── Success ────► CONNECTED
 │
 └──── Timeout ────► RETRY
                       │
                       ▼
                   CONNECTING
```

This approach allows other tasks to continue while networking is being managed.

---

## 🔄 50. Wi-Fi Reconnection

Wireless connections may fail because of:

- weak signal,
- router restart,
- interference,
- authentication failure,
- network reconfiguration.

Therefore, embedded software should include reconnection logic.

```text
Connected
    │
    ▼
Connection Lost?
    │
   Yes
    │
    ▼
Reconnect
    │
    ├── Success → Connected
    │
    └── Failure → Retry
```

---

## ⏱️ 51. Network Timeout

Network operations should use appropriate timeout handling.

Conceptually:

```text
Send Request
    │
    ▼
Wait Response
    │
    ├── Response → Process
    │
    └── Timeout  → Error Handling
```

Without timeout handling, an embedded application may spend excessive time waiting for unavailable services.

---

## 🔁 52. Retry Strategy

A simple retry system may use:

```text
Request
   │
   ▼
Success?
 │    │
Yes   No
 │    │
 ▼    ▼
Done Delay
       │
       ▼
      Retry
```

However, continuously retrying rapidly can overload:

- the ESP32,
- Wi-Fi network,
- API server.

---

## 📈 53. Exponential Backoff

A better retry strategy may progressively increase the waiting period.

For example:

$$
T_n=\min(T_{\max},T_0 2^n)
$$

where:

- $T_0$ = initial delay,
- $n$ = retry number,
- $T_{\max}$ = maximum retry delay.

Example:

```text
1 s
2 s
4 s
8 s
16 s
...
```

This reduces unnecessary network traffic during outages.

---

## 📊 54. Wi-Fi Signal Strength

Received signal strength can be monitored using RSSI.

Example:

```cpp
Serial.println(WiFi.RSSI());
```

RSSI is commonly represented in:

$$
dBm.
$$

For example:

```text
-40 dBm
-60 dBm
-80 dBm
```

Less-negative values generally indicate a stronger received signal.

Exact performance depends on the radio environment, antenna, device, and interference.

---

## 📶 55. RSSI Interpretation

A rough qualitative interpretation might be:

| RSSI | General Interpretation |
|---:|---|
| around `-30 dBm` | Very strong |
| around `-50 dBm` | Strong |
| around `-67 dBm` | Often usable |
| around `-75 dBm` | Weak |
| around `-85 dBm` | Very weak |

These values should be treated as approximate rather than universal thresholds.

---

## 🧠 56. Embedded Networking Challenges

Unlike desktop computers, embedded systems have limited:

- RAM,
- CPU resources,
- energy,
- storage,
- communication bandwidth.

Therefore:

$$
\boxed{
\text{Embedded Networking}
=
\text{Connectivity}
+
\text{Resource Constraints}
}
$$

Developers must consider:

- memory usage,
- network latency,
- timeout handling,
- power consumption,
- security,
- reliability.

---

## 🔋 57. Wi-Fi and Power Consumption

Wi-Fi communication can consume significant energy compared with low-power MCU operation.

A battery-powered system may therefore use:

```text
Wake
 │
 ▼
Connect Wi-Fi
 │
 ▼
Send Data
 │
 ▼
Disconnect
 │
 ▼
Sleep
```

This approach can reduce average energy consumption for applications that only need occasional communication.

---

## ⏲️ 58. Periodic IoT Communication

Suppose an ESP32 measures temperature every:

$$
T_s=10~s
$$

but uploads data every:

$$
T_u=60~s.
$$

The device can buffer several measurements:

```text
Measurement 1
Measurement 2
Measurement 3
Measurement 4
Measurement 5
Measurement 6
       │
       ▼
    REST POST
```

This may reduce communication overhead.

---

## 📦 59. REST Payload Design

An efficient embedded payload should contain only useful information.

Instead of unnecessarily verbose data, a payload might be:

```json
{
  "id": "esp32-01",
  "t": 29.2,
  "h": 68
}
```

However, highly abbreviated field names reduce readability.

Payload design therefore involves a trade-off:

$$
\boxed{
\text{Readability}
\leftrightarrow
\text{Payload Size}
}
$$

---

## 📊 60. REST vs. MQTT

REST/HTTP and MQTT are both widely used in IoT.

| Characteristic | REST/HTTP | MQTT |
|---|---|---|
| Communication model | Request-response | Publish-subscribe |
| Typical infrastructure | Web/API server | MQTT broker |
| Resource-oriented API | Excellent | Not primary model |
| Browser/web integration | Excellent | Requires MQTT support |
| Continuous telemetry | Possible | Very suitable |
| Command/response | Excellent | Possible |
| Protocol overhead | Often higher | Generally lower |

---

## 🔄 61. Request-Response Model

REST commonly follows:

```text
Client
  │
  │ Request
  ▼
Server
  │
  │ Response
  ▼
Client
```

Therefore:

$$
\boxed{
\text{REST}
\approx
\text{Request}
+
\text{Response}
}
$$

This model is easy to integrate with web applications.

---

## 📢 62. Publish-Subscribe Model

MQTT uses:

```text
Publisher
    │
    ▼
 Broker
 /     \
▼       ▼
Sub A  Sub B
```

The publisher and subscribers do not need to communicate directly.

This is useful for asynchronous IoT messaging.

---

## 🧩 63. REST Resource Design

Good REST API design focuses on resources.

Instead of:

```text
/getTemperature
/setLED
/deleteDevice
```

a resource-oriented API might use:

```text
GET    /temperature
PUT    /led
DELETE /devices/10
```

The HTTP method communicates the intended operation.

---

## 🗂️ 64. Example IoT REST API

An IoT server might provide:

| Method | Endpoint | Function |
|---|---|---|
| GET | `/api/devices` | Retrieve devices |
| GET | `/api/devices/1` | Retrieve device 1 |
| POST | `/api/readings` | Add sensor reading |
| GET | `/api/readings/latest` | Retrieve latest reading |
| PUT | `/api/devices/1` | Update device |
| DELETE | `/api/devices/1` | Remove device |

---

## 🏗️ 65. Complete ESP32 REST Architecture

A practical system may look like:

```text
┌─────────────┐
│ Temperature │
│   Sensor    │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│    ESP32    │
│             │
│ Sensor Task │
│ Wi-Fi       │
│ HTTP Client │
└──────┬──────┘
       │
       │ Wi-Fi
       ▼
┌─────────────┐
│   Router    │
└──────┬──────┘
       │
       │ Internet
       ▼
┌─────────────┐
│  REST API   │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  Database   │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│ Dashboard   │
└─────────────┘
```

---

## ⚙️ 66. ESP32 + FreeRTOS Networking Architecture

Networking can be separated into tasks.

```text
Sensor Task
     │
     ▼
Sensor Queue
     │
     ▼
Network Task
     │
     ▼
Wi-Fi / REST API
```

This architecture avoids placing complex network operations directly inside sensor-processing code.

---

## 📤 67. Queue-Based Network Communication

Suppose the sensor task generates:

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
  │ SensorData
  ▼
Queue
  │
  ▼
Network Task
  │
  ▼
JSON
  │
  ▼
REST POST
```

This creates clean separation between acquisition and networking.

---

## 🔒 68. Thread-Safe Networking Design

Multiple tasks should generally avoid uncontrolled simultaneous access to the same network object.

Instead:

```text
Task A ──┐
         │
Task B ──┼──► Queue ──► Network Task ──► Wi-Fi
         │
Task C ──┘
```

This can simplify synchronization and resource ownership.

---

## 🧠 69. REST API Data Flow

Consider an environmental monitoring system.

### Step 1 — Sensor Measurement

$$
T=28.7^\circ C
$$

### Step 2 — Data Structure

```cpp
temperature = 28.7;
```

### Step 3 — JSON Encoding

```json
{
  "temperature": 28.7
}
```

### Step 4 — HTTP Request

```text
POST /api/readings
```

### Step 5 — Server Processing

```text
REST API
   │
   ▼
Validate
   │
   ▼
Database
```

### Step 6 — Response

```text
201 Created
```

---

## 📊 70. Networking Performance Metrics

Important metrics include:

### Latency

$$
T_L=t_{response}-t_{request}
$$

### Throughput

$$
R=
\frac{\text{Transferred Data}}
{\text{Time}}
$$

### Packet Loss

$$
P_{loss}
=
\frac{N_{sent}-N_{received}}
{N_{sent}}
\times100\%.
$$

### Availability

$$
A=
\frac{T_{available}}
{T_{total}}
\times100\%.
$$

---

## ⏱️ 71. REST API Latency

REST communication latency can be approximated conceptually as:

$$
T_{REST}
=
T_{network}
+
T_{server}
+
T_{processing}
+
T_{response}.
$$

For HTTPS, connection establishment and TLS processing may add additional overhead, especially when a new connection is created frequently.

---

## 💾 72. Memory Considerations

Networking requires RAM for:

- TCP/IP buffers,
- Wi-Fi driver data,
- HTTP headers,
- JSON payloads,
- TLS buffers,
- application queues.

Conceptually:

```text
ESP32 RAM
│
├── Application
├── FreeRTOS
├── Wi-Fi
├── TCP/IP
├── HTTP
├── TLS
└── JSON Buffers
```

Therefore, networking should be included in the application's overall memory budget.

---

## ⚠️ 73. Avoiding Excessive Dynamic Strings

Code such as:

```cpp
String payload;
```

is convenient, but repeated dynamic string operations may increase memory-management activity.

For long-running or memory-constrained systems, developers should carefully manage:

- string sizes,
- buffers,
- object lifetimes,
- dynamic allocation.

---

## 🛡️ 74. REST API Security Checklist

A production IoT system should consider:

- HTTPS,
- certificate validation,
- authentication,
- authorization,
- secure credential storage,
- input validation,
- timeout handling,
- replay protection where appropriate,
- rate limiting on the server,
- firmware updates,
- logging.

Security should be designed into the system rather than added only after deployment.

---

## 🔑 75. Credential Management

Avoid publishing source code containing:

```cpp
const char* password = "MyRealPassword";
```

For classroom experiments, hard-coded credentials may be convenient, but production systems should use safer provisioning and storage mechanisms.

Conceptually:

```text
Provisioning
    │
    ▼
Secure Credential Storage
    │
    ▼
Wi-Fi / API Authentication
```

---

## 🚨 76. API Error Handling

A robust client should handle:

```text
Wi-Fi disconnected
DNS failure
Connection timeout
TLS failure
HTTP 401
HTTP 404
HTTP 500
Invalid JSON
Empty response
```

Do not assume:

```text
Request → Always Success
```

Instead:

```text
Request
   │
   ▼
Check Result
   │
   ├── Success → Process
   │
   └── Failure → Recover
```

---

## 🔄 77. Robust IoT Communication Loop

A better embedded communication architecture is:

```text
Check Wi-Fi
    │
    ├── Disconnected
    │       │
    │       ▼
    │    Reconnect
    │
    ▼
Prepare Data
    │
    ▼
Send Request
    │
    ▼
Check Status
    │
    ├── Success → Continue
    │
    └── Failure → Retry Policy
```

---

## 🧪 78. Example System — Smart Room

Consider:

```text
Temperature Sensor
Humidity Sensor
Light Sensor
       │
       ▼
     ESP32
       │
       ▼
     Wi-Fi
       │
       ▼
    REST API
       │
       ▼
    Database
       │
       ▼
    Dashboard
```

The ESP32 periodically sends:

```json
{
  "temperature": 27.8,
  "humidity": 65,
  "light": 620
}
```

---

## 🚦 79. Example System — Smart Traffic

A smart traffic node might contain:

```text
Vehicle Sensor
     │
     ▼
ESP32
     │
     ├── Traffic FSM
     ├── Vehicle Count
     ├── Timing Control
     └── Wi-Fi
             │
             ▼
          REST API
             │
             ▼
     Traffic Management Server
```

The ESP32 could send:

```json
{
  "intersection": 3,
  "vehicles": 24,
  "state": "GREEN"
}
```

---

## 🏭 80. Example System — Industrial Monitoring

```text
Machine Sensor
     │
     ▼
   ESP32
     │
     ▼
Industrial Wi-Fi
     │
     ▼
REST Gateway
     │
     ▼
Monitoring Server
```

Possible data:

```json
{
  "machine": "M01",
  "temperature": 63.2,
  "vibration": 0.74
}
```

The server can analyze the information for maintenance and monitoring.

---

## 📱 81. ESP32 Local Control System

The ESP32 can operate as an access point and HTTP server.

```text
Smartphone
    │
    │ Wi-Fi
    ▼
ESP32 AP
    │
    ▼
Web Server
    │
    ├── LED Control
    ├── Sensor Status
    └── Configuration
```

This system can work locally without requiring Internet access.

---

## 🌍 82. ESP32 Cloud-Connected System

Alternatively:

```text
ESP32 STA
    │
    ▼
Wi-Fi Router
    │
    ▼
Internet
    │
    ▼
Cloud REST API
```

This allows remote monitoring and control.

---

## 🔀 83. Hybrid Local + Cloud System

A more advanced architecture can combine both:

```text
Local Phone
    │
    ▼
ESP32 AP + STA
    │
    ├──────── Local Interface
    │
    └──────── Router
                 │
                 ▼
              Cloud API
```

This allows local configuration while maintaining cloud connectivity.

---

## 🧮 84. Sampling and Upload Rates

Suppose:

$$
f_s=10~Hz
$$

but network upload occurs at:

$$
f_u=1~Hz.
$$

Then approximately:

$$
N=
\frac{f_s}{f_u}=10
$$

samples can be collected for each upload interval.

The application could:

- send all samples,
- calculate an average,
- send min/max,
- perform edge processing.

---

## 🧠 85. Edge Processing Before REST Upload

Instead of transmitting every sample:

```text
Sensor Samples
     │
     ▼
ESP32 Processing
     │
     ├── Average
     ├── Filtering
     ├── Feature Extraction
     └── Event Detection
              │
              ▼
           REST API
```

This can reduce:

- network traffic,
- server workload,
- power consumption.

Therefore:

$$
\boxed{
\text{Edge Processing}
\rightarrow
\text{Reduced Communication Load}
}
$$

---

## 🤖 86. Embedded AI + REST API

An ESP32-based AI system could perform local inference:

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
Classification
  │
  ▼
REST API
```

Instead of sending raw sensor data, it might send:

```json
{
  "device": "esp32-ai-01",
  "class": "abnormal",
  "confidence": 0.94
}
```

This combines:

$$
\boxed{
\text{Embedded AI}
+
\text{Networking}
=
\text{Connected Intelligence}
}
$$

---

## 📋 87. Recommended ESP32 Network Architecture

For a FreeRTOS application:

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
│  Network Task   │
└────────┬────────┘
         │
         ▼
       Wi-Fi
         │
         ▼
     REST API
```

This provides separation between:

- sensing,
- processing,
- communication.

---

## 📊 88. Wi-Fi Mode Selection Guide

| Application | Recommended Mode |
|---|---|
| Cloud sensor | STA |
| IoT dashboard client | STA |
| Local configuration portal | AP |
| Direct smartphone control | AP |
| Initial provisioning | AP or AP+STA |
| Local + Internet gateway | AP+STA |
| REST API client | Usually STA |
| Standalone web server | AP or STA |

---

## 📊 89. Protocol Selection Guide

| Requirement | Possible Choice |
|---|---|
| Web API integration | HTTP/HTTPS |
| Resource-oriented interface | REST |
| Continuous IoT telemetry | MQTT |
| Lightweight datagrams | UDP |
| Reliable byte stream | TCP |
| Secure REST communication | HTTPS |
| Local web interface | HTTP/HTTPS server |

---

## 🧪 90. Exercise 1 — Wi-Fi Station

Configure the ESP32 as:

```text
ESP32 STA
   │
   ▼
Wi-Fi Router
```

Display:

```text
SSID
IP Address
Gateway
RSSI
```

using the Serial Monitor.

---

## 📡 91. Exercise 2 — Access Point

Configure:

```text
SSID = ESP32-LAB
```

and create:

```text
ESP32 AP
    │
    ├── Laptop
    └── Smartphone
```

Display the ESP32 AP IP address.

---

## 🌐 92. Exercise 3 — HTTP GET

Write an ESP32 program that sends:

```text
GET /api/data
```

to a test server.

Display:

```text
HTTP Status Code
Response Body
```

on the Serial Monitor.

---

## 📤 93. Exercise 4 — REST POST

Create JSON:

```json
{
  "temperature": 28.5,
  "humidity": 70
}
```

and send it to a REST API using:

```text
POST
```

Record:

| Parameter | Result |
|---|---|
| Wi-Fi connected | |
| Server address | |
| HTTP status | |
| Payload size | |
| Response | |

---

## 💡 94. Exercise 5 — ESP32 REST-Like LED Control

Create an endpoint for controlling an LED.

For example:

```text
PUT /api/led
```

Input:

```json
{
  "state": true
}
```

Expected result:

```text
LED → ON
```

---

## 🌡️ 95. Exercise 6 — Sensor API

Create:

```text
GET /api/sensor
```

which returns:

```json
{
  "temperature": 29.1,
  "humidity": 67
}
```

Use either real sensors or simulated values.

---

## 🔄 96. Exercise 7 — Reconnection

Disconnect the Wi-Fi router while the ESP32 is operating.

Observe:

```text
Wi-Fi Connected
      │
      ▼
Router OFF
      │
      ▼
Connection Lost
      │
      ▼
Retry
      │
      ▼
Router ON
      │
      ▼
Reconnect
```

Measure the reconnection time.

---

## 📶 97. Exercise 8 — RSSI Measurement

Move the ESP32 progressively farther from the access point.

Record:

| Position | RSSI (dBm) | HTTP Successful? | Latency |
|---|---:|---|---:|
| A | | | |
| B | | | |
| C | | | |
| D | | | |

Discuss the relationship between signal quality and network performance.

---

## 🔐 98. Exercise 9 — HTTP vs. HTTPS

Compare:

```text
HTTP
```

and:

```text
HTTPS
```

in terms of:

- encryption,
- authentication,
- memory usage,
- connection overhead,
- security.

Explain why HTTPS is normally preferred for Internet-connected APIs.

---

## 🚀 99. Advanced Exercise — ESP32 IoT REST Node

Develop:

```text
Sensor
   │
   ▼
ESP32
   │
   ├── Sensor Task
   ├── Processing Task
   ├── Network Task
   └── Wi-Fi Manager
           │
           ▼
        REST API
           │
           ▼
        Database
```

Requirements:

1. Connect to Wi-Fi in STA mode.
2. Sample sensor data every 1 s.
3. Transfer data through a FreeRTOS queue.
4. Upload data every 10 s.
5. Encode measurements as JSON.
6. Use a REST POST request.
7. Check HTTP status codes.
8. Detect Wi-Fi disconnection.
9. Automatically reconnect.
10. Implement request timeout handling.
11. Record RSSI.
12. Monitor free heap.

---

## 💬 100. Discussion Questions

1. What is ESP32 Station mode?
2. What is Access Point mode?
3. What is AP+STA mode?
4. What is DHCP?
5. What is DNS?
6. What is a default gateway?
7. What is an IP address?
8. What is the difference between TCP and UDP?
9. What is HTTP?
10. What is HTTPS?
11. What is TLS?
12. What is an API?
13. What is a REST API?
14. What is a REST resource?
15. What is JSON?
16. What is the difference between GET and POST?
17. What is the difference between PUT and PATCH?
18. What does HTTP status code `200` mean?
19. Why should ESP32 applications implement timeouts?
20. Why is reconnection important?
21. What is exponential backoff?
22. What is RSSI?
23. Why can Wi-Fi increase power consumption?
24. How does REST differ from MQTT?
25. Why might a dedicated network task improve an RTOS design?

---

## 🧾 101. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain ESP32 Wi-Fi networking,
- distinguish STA, AP, and AP+STA modes,
- connect an ESP32 to a Wi-Fi network,
- create an ESP32 access point,
- explain IP addressing,
- explain DHCP, DNS, and gateways,
- describe the embedded TCP/IP stack,
- distinguish TCP and UDP,
- explain HTTP and HTTPS,
- explain REST architecture,
- identify common HTTP methods,
- interpret HTTP status codes,
- create JSON payloads,
- implement HTTP GET requests,
- implement HTTP POST requests,
- design simple REST-style embedded interfaces,
- explain authentication and TLS,
- implement network timeout and reconnection strategies,
- monitor Wi-Fi RSSI,
- compare REST and MQTT,
- integrate networking with FreeRTOS tasks and queues,
- design secure and reliable ESP32 IoT architectures.

---

## 🔑 102. Key Concepts

### Wi-Fi Station

$$
\boxed{
\text{ESP32}
\rightarrow
\text{Wi-Fi Router}
\rightarrow
\text{Network}
}
$$

### Wi-Fi Access Point

$$
\boxed{
\text{Client}
\rightarrow
\text{ESP32 AP}
}
$$

### Embedded Network Stack

$$
\boxed{
\text{Application}
\rightarrow
\text{TCP/UDP}
\rightarrow
\text{IP}
\rightarrow
\text{Wi-Fi}
}
$$

### REST Communication

$$
\boxed{
\text{Client Request}
\rightarrow
\text{REST Server}
\rightarrow
\text{Response}
}
$$

### HTTPS

$$
\boxed{
\text{HTTPS}
=
\text{HTTP over TLS}
}
$$

### IoT Data Path

$$
\boxed{
\text{Sensor}
\rightarrow
\text{ESP32}
\rightarrow
\text{Wi-Fi}
\rightarrow
\text{REST API}
\rightarrow
\text{Database}
}
$$

### FreeRTOS Networking

$$
\boxed{
\text{Sensor Task}
\rightarrow
\text{Queue}
\rightarrow
\text{Network Task}
\rightarrow
\text{REST API}
}
$$

### Retry Backoff

$$
\boxed{
T_n=\min(T_{\max},T_0 2^n)
}
$$

---

## 🎯 103. Summary

The ESP32 integrates Wi-Fi connectivity with microcontroller functionality, making it highly suitable for **networked embedded and IoT applications**. Three important Wi-Fi configurations are **Station mode, Access Point mode, and AP+STA mode**. Station mode is commonly used for connecting an ESP32 to an existing network, while Access Point mode enables direct local communication.

Embedded networking builds on a layered architecture involving **Wi-Fi, IP, TCP/UDP, and application protocols**. Understanding IP addresses, DHCP, DNS, gateways, ports, TCP, and UDP is therefore fundamental to building reliable connected devices.

REST APIs provide a convenient mechanism for integrating embedded systems with web servers, cloud services, databases, and dashboards. ESP32 applications can use HTTP methods such as **GET, POST, PUT, PATCH, and DELETE**, while JSON provides a common format for exchanging structured data.

For practical IoT systems, connectivity alone is insufficient. Applications should also implement **timeouts, error handling, Wi-Fi reconnection, retry strategies, authentication, TLS security, and memory-aware networking**.

The overall architecture can be summarized as:

$$
\boxed{
\text{Connected ESP32 System}
=
\text{Embedded Processing}
+
\text{Wi-Fi}
+
\text{TCP/IP}
+
\text{REST API}
+
\text{Security}
}
$$

For an RTOS-based implementation, a particularly useful architecture is:

$$
\boxed{
\text{Sensor}
\rightarrow
\text{Sensor Task}
\rightarrow
\text{Queue}
\rightarrow
\text{Network Task}
\rightarrow
\text{Wi-Fi}
\rightarrow
\text{REST API}
}
$$

This architecture separates **data acquisition, processing, and networking**, providing a strong foundation for more advanced ESP32 applications involving cloud computing, digital twins, MQTT, edge AI, and connected intelligence.

---

## 📘 References

1. Espressif Systems, *ESP-IDF Programming Guide — Wi-Fi Driver*.
2. Espressif Systems, *ESP-IDF Programming Guide — Networking APIs*.
3. Espressif Systems, *ESP32 Series Datasheet*.
4. Espressif Systems, *Arduino ESP32 Documentation*.
5. FreeRTOS, *FreeRTOS Kernel Documentation*.
6. J. F. Kurose and K. W. Ross, *Computer Networking: A Top-Down Approach*.
7. A. S. Tanenbaum and D. J. Wetherall, *Computer Networks*.
8. R. T. Fielding, *Architectural Styles and the Design of Network-Based Software Architectures*.
9. RFC 9110, *HTTP Semantics*.
10. RFC 8259, *The JavaScript Object Notation (JSON) Data Interchange Format*.

