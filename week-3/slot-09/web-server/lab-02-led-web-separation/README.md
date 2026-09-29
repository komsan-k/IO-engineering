# Lab — ESP32 Web Server for LED Control Using a Webpage File

## Objective

Create a simple **ESP32 web server** for controlling an LED, while storing the webpage in a separate `index.html` file.

This separates the ESP32 control program from the webpage layout and JavaScript.

---

## Project Structure

```text
ESP32_LED_WebServer/
│
├── ESP32_LED_WebServer.ino
│
└── data/
    └── index.html
```

The webpage is stored in the ESP32 filesystem using **LittleFS**.

---

## System Concept

```text
Web Browser
    |
    | GET /
    v
ESP32 Web Server
    |
    | Load index.html
    v
Web Page
    |
    +---- LED ON
    |
    +---- LED OFF
    |
    v
HTTP Requests
    |
    +---- GET /led/on
    |
    +---- GET /led/off
    |
    v
ESP32 GPIO 2
    |
    v
LED ON / OFF
```

---

## Required Libraries

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
```

---

## ESP32 Program

Save the Arduino code as:

```text
ESP32_LED_WebServer.ino
```

The ESP32 serves `/index.html` from LittleFS and responds to `/led/on` and `/led/off`.

---

## Webpage File

Create:

```text
data/index.html
```

The page contains two buttons and JavaScript using `fetch()`:

```javascript
fetch("/led/on");
fetch("/led/off");
```

This allows the browser to control the LED without rebuilding the HTML inside the Arduino code.

---

## How It Works

When the browser opens:

```text
http://ESP32-IP/
```

the ESP32 returns:

```text
/index.html
```

Pressing **LED ON** sends:

```text
GET /led/on
```

Pressing **LED OFF** sends:

```text
GET /led/off
```

The ESP32 returns:

```text
ON
```

or:

```text
OFF
```

and the webpage updates the status.

---

## Upload Procedure

1. Create a `data` folder inside the Arduino project.
2. Copy the webpage file into it and rename it to `index.html`.
3. Upload the LittleFS filesystem image.
4. Upload the Arduino sketch.
5. Open Serial Monitor at `115200`.
6. Record the ESP32 IP address.
7. Open the IP address in a browser.

---

## Expected Result

```text
+--------------------------+
|    ESP32 LED Control     |
|                          |
| LED Status: ON / OFF     |
|                          |
| [ LED ON ] [ LED OFF ]   |
+--------------------------+
```

---

## Key Concept

The uploaded lab originally builds the webpage as a `String` inside the ESP32 sketch. This revised version moves that webpage into a dedicated `index.html` file while keeping the same LED control routes:

```text
/led/on
/led/off
```

This makes the webpage easier to edit and is closer to the structure used in larger web-based IoT applications.
