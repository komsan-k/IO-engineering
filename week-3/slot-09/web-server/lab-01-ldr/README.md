# Lab 2 — Simple ESP32 Web Server for Displaying LDR Data

## Objective

Create a simple **ESP32 web server** that reads an **LDR (Light Dependent Resistor)** using the ESP32 ADC and displays the light level on a web browser.

Students will learn how to:

- connect an ESP32 to Wi-Fi;
- read an analog sensor using `analogRead()`;
- create a simple HTTP web server;
- display sensor data on a web page;
- refresh the sensor value automatically.

---

## System Concept

```text
+-------------+
|     LDR     |
+------+------+
       |
       | Analog Input
       v
+-------------+
|    ESP32    |
| Web Server  |
+------+------+
       |
       | HTTP
       v
+-------------+
| Web Browser |
| Light: 640  |
+-------------+
```

---

## LDR Connection

Example using GPIO 36:

```text
3.3V
 |
 LDR
 |
 +-------> GPIO 36
 |
10 kΩ
 |
GND
```

The LDR and resistor form a voltage divider.

> Depending on how the LDR and resistor are connected, a higher ADC value may represent either a brighter or darker condition.

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

// --------------------------------
// Wi-Fi Configuration
// --------------------------------

const char* ssid =
  "YOUR_WIFI_NAME";

const char* password =
  "YOUR_WIFI_PASSWORD";

// --------------------------------
// LDR Configuration
// --------------------------------

const int LDR_PIN = 36;

// --------------------------------
// Create Web Server
// --------------------------------

WebServer server(80);

// ========================================
// Create Web Page
// ========================================

String createWebPage()
{
  int lightValue =
    analogRead(LDR_PIN);

  String page = "";

  page += "<!DOCTYPE html>";
  page += "<html>";

  page += "<head>";

  page += "<meta name='viewport' ";
  page += "content='width=device-width, initial-scale=1'>";

  page += "<meta http-equiv='refresh' content='2'>";

  page += "<title>";
  page += "ESP32 LDR Monitor";
  page += "</title>";

  page += "</head>";

  page += "<body style='";
  page += "text-align:center;";
  page += "font-family:Arial;'>";

  page += "<h1>";
  page += "ESP32 Web Server";
  page += "</h1>";

  page += "<h2>";
  page += "LDR Light Monitor";
  page += "</h2>";

  page += "<p style='font-size:40px;'>";

  page += "Light Value: ";

  page += String(lightValue);

  page += "</p>";

  page += "<p>";
  page += "Page refreshes every 2 seconds";
  page += "</p>";

  page += "</body>";
  page += "</html>";

  return page;
}

// ========================================
// Root Page
// ========================================

void handleRoot()
{
  server.send(
    200,
    "text/html",
    createWebPage()
  );
}

// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  pinMode(
    LDR_PIN,
    INPUT
  );

  // --------------------------------
  // Connect to Wi-Fi
  // --------------------------------

  Serial.print(
    "Connecting to Wi-Fi"
  );

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

  Serial.println(
    "Wi-Fi connected"
  );

  Serial.print(
    "ESP32 IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  // --------------------------------
  // Web Server Route
  // --------------------------------

  server.on(
    "/",
    handleRoot
  );

  // --------------------------------
  // Start Web Server
  // --------------------------------

  server.begin();

  Serial.println(
    "HTTP server started"
  );
}

// ========================================
// Main Loop
// ========================================

void loop()
{
  server.handleClient();

  int lightValue =
    analogRead(LDR_PIN);

  Serial.print(
    "LDR Value: "
  );

  Serial.println(
    lightValue
  );

  delay(500);
}
```

---

## How to Run

1. Replace the Wi-Fi credentials:

```cpp
const char* ssid =
  "YOUR_WIFI_NAME";

const char* password =
  "YOUR_WIFI_PASSWORD";
```

2. Upload the code to the ESP32.

3. Open the Serial Monitor at:

```text
115200 baud
```

4. Record the ESP32 IP address, for example:

```text
ESP32 IP Address:
192.168.1.105
```

5. Open a browser and enter:

```text
http://192.168.1.105
```

---

## Expected Web Page

```text
+--------------------------+
|    ESP32 Web Server      |
|                          |
|    LDR Light Monitor     |
|                          |
|     Light Value: 640     |
|                          |
| Refresh every 2 seconds  |
+--------------------------+
```

As the light level changes, the LDR reading changes.

Example:

```text
Bright environment
Light Value: 3200

Dim environment
Light Value: 900

Dark environment
Light Value: 200
```

The exact behavior depends on the LDR voltage-divider connection.

---

## Important Commands

Read the analog sensor:

```cpp
int lightValue =
  analogRead(LDR_PIN);
```

Create the web server:

```cpp
WebServer server(80);
```

Define the home page:

```cpp
server.on(
  "/",
  handleRoot
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

## Automatic Refresh

This HTML line refreshes the page every 2 seconds:

```html
<meta http-equiv='refresh' content='2'>
```

The data flow is:

```text
LDR
 |
 v
analogRead()
 |
 v
ESP32
 |
 v
HTML Page
 |
 v
Browser
 |
 | Refresh every 2 s
 +-------------------+
```

---

## Checkpoint Questions

1. What does `analogRead()` do?
2. Why is GPIO 36 suitable for analog input?
3. What is the role of the ESP32 in this experiment?
4. What is the role of the browser?
5. What does `WebServer server(80)` mean?
6. Why does the page refresh every 2 seconds?
7. What happens to the LDR value when the light level changes?

---

## Simple Assignment

Modify the page to classify the light condition.

For example:

```text
Light Value: 650
Status: DARK
```

Use:

```cpp
String status;

if (lightValue < 1000)
{
  status = "DARK";
}
else
{
  status = "BRIGHT";
}
```

Then display both the numeric value and the light status on the web page.
