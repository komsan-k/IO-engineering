#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const int LED_PIN = 2;

WebServer server(80);

void handleRoot()
{
  File file = LittleFS.open("/index.html", "r");

  if (!file)
  {
    server.send(500, "text/plain", "index.html not found");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void handleLEDOn()
{
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  server.send(200, "text/plain", "ON");
}

void handleLEDOff()
{
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  server.send(200, "text/plain", "OFF");
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if (!LittleFS.begin(true))
  {
    Serial.println("LittleFS mount failed");
    return;
  }

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/on", handleLEDOn);
  server.on("/led/off", handleLEDOff);

  server.begin();

  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}
