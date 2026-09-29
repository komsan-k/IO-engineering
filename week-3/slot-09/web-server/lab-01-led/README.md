# Lab — Simple ESP32 Web Server for LED Control

## Objective

Create a simple **ESP32 web server** that allows a user to turn an LED **ON** and **OFF** from a web browser.

Students will learn to:

- connect an ESP32 to Wi-Fi;
- run an HTTP web server;
- use a browser as an HTTP client;
- control an LED using web requests.

---

## Required Libraries

```cpp
#include <WiFi.h>
#include <WebServer.h>
```

---

## Complete ESP32 Program

```cpp
#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// LED
const int LED_PIN = 2;

// Web server on port 80
WebServer server(80);

String webPage()
{
  String page = "";

  page += "<html>";
  page += "<body style='text-align:center;font-family:Arial;'>";

  page += "<h1>ESP32 LED Control</h1>";

  page += "<p>";
  page += "<a href='/led/on'>";
  page += "<button style='font-size:30px;padding:20px;'>";
  page += "LED ON";
  page += "</button>";
  page += "</a>";
  page += "</p>";

  page += "<p>";
  page += "<a href='/led/off'>";
  page += "<button style='font-size:30px;padding:20px;'>";
  page += "LED OFF";
  page += "</button>";
  page += "</a>";
  page += "</p>";

  page += "</body>";
  page += "</html>";

  return page;
}

void handleRoot()
{
  server.send(
    200,
    "text/html",
    webPage()
  );
}

void handleLEDOn()
{
  digitalWrite(
    LED_PIN,
    HIGH
  );

  Serial.println("LED ON");

  server.send(
    200,
    "text/html",
    webPage()
  );
}

void handleLEDOff()
{
  digitalWrite(
    LED_PIN,
    LOW
  );

  Serial.println("LED OFF");

  server.send(
    200,
    "text/html",
    webPage()
  );
}

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

  WiFi.begin(
    ssid,
    password
  );

  Serial.print(
    "Connecting to Wi-Fi"
  );

  while (
    WiFi.status() != WL_CONNECTED
  )
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "Wi-Fi connected"
  );

  Serial.print(
    "ESP32 IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/led/on",
    handleLEDOn
  );

  server.on(
    "/led/off",
    handleLEDOff
  );

  server.begin();

  Serial.println(
    "HTTP server started"
  );
}

void loop()
{
  server.handleClient();
}
```

---

## How to Run

1. Replace:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

with your Wi-Fi credentials.

2. Upload the program to the ESP32.

3. Open the Serial Monitor at:

```text
115200 baud
```

4. Record the ESP32 IP address, for example:

```text
192.168.1.105
```

5. Open a web browser and enter:

```text
http://192.168.1.105
```

6. Press:

```text
LED ON
```

or:

```text
LED OFF
```

---

## HTTP Requests

LED ON:

```text
GET /led/on
```

LED OFF:

```text
GET /led/off
```

The browser sends an HTTP GET request to the ESP32, and the ESP32 changes the GPIO output.

```text
Browser
   |
   | HTTP GET
   v
ESP32 Web Server
   |
   v
GPIO 2
   |
   v
LED ON / OFF
```

---

## Important Commands

Create the server:

```cpp
WebServer server(80);
```

Define a route:

```cpp
server.on(
  "/led/on",
  handleLEDOn
);
```

Start the server:

```cpp
server.begin();
```

Handle browser requests:

```cpp
server.handleClient();
```

---

## Expected Result

The browser displays:

```text
+----------------------+
|   ESP32 LED Control  |
|                      |
|      [ LED ON ]      |
|                      |
|      [ LED OFF ]     |
+----------------------+
```

The Serial Monitor shows:

```text
Wi-Fi connected
ESP32 IP Address: 192.168.1.105
HTTP server started

LED ON
LED OFF
```

---

## Checkpoint Questions

1. What is the role of the ESP32?
2. What is the role of the web browser?
3. What does port `80` represent?
4. What does `server.handleClient()` do?
5. What URL is used to turn the LED on?
6. What URL is used to turn the LED off?

---

## Simple Assignment

Modify the program to add a third route:

```text
/led/toggle
```

so one button can toggle the LED between ON and OFF.
