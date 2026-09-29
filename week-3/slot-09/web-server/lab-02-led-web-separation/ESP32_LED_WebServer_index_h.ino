#include <WiFi.h>
#include <WebServer.h>
#include "index.h"

// --------------------------------
// Wi-Fi Configuration
// --------------------------------

const char* ssid =
  "YOUR_WIFI_NAME";

const char* password =
  "YOUR_WIFI_PASSWORD";

// --------------------------------
// LED Configuration
// --------------------------------

const int LED_PIN = 2;

// --------------------------------
// Web Server
// --------------------------------

WebServer server(80);

// ========================================
// Root Page
// ========================================

void handleRoot()
{
  server.send_P(
    200,
    "text/html",
    INDEX_HTML
  );
}

// ========================================
// LED ON
// ========================================

void handleLEDOn()
{
  digitalWrite(
    LED_PIN,
    HIGH
  );

  Serial.println(
    "LED ON"
  );

  server.send(
    200,
    "text/plain",
    "ON"
  );
}

// ========================================
// LED OFF
// ========================================

void handleLEDOff()
{
  digitalWrite(
    LED_PIN,
    LOW
  );

  Serial.println(
    "LED OFF"
  );

  server.send(
    200,
    "text/plain",
    "OFF"
  );
}

// ========================================
// Setup
// ========================================

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
  // Web Server Routes
  // --------------------------------

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

  // --------------------------------
  // Start HTTP Server
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
}
