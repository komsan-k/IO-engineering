# ESP32 Weather Monitor for Phuket using Open-Meteo

## Objective

Use an **ESP32** to retrieve current weather information for **Phuket,
Thailand** from the Open-Meteo Weather API and display it on the Serial
Monitor.

Students will practice:

-   Wi-Fi connectivity
-   HTTPS GET requests
-   REST APIs
-   JSON weather data
-   JSON parsing with ArduinoJson

## 1. System Architecture

``` text
+-------------+
|    ESP32    |
+------+------+
       |
       | Wi-Fi / HTTPS GET
       v
+------------------+
|    Open-Meteo    |
|   Weather API    |
+--------+---------+
         |
         | JSON Response
         v
+------------------+
|      ESP32       |
|   Parse JSON     |
+--------+---------+
         |
         v
+------------------+
|  Serial Monitor  |
| Temp: 30.2 C     |
| Humidity: 72 %   |
| Wind: 8.5 km/h   |
+------------------+
```

## 2. Phuket Location

Use the approximate coordinates:

``` text
Latitude  = 7.88
Longitude = 98.39
```

API request:

``` text
https://api.open-meteo.com/v1/forecast?latitude=7.88&longitude=98.39&current=temperature_2m,relative_humidity_2m,wind_speed_10m
```

Open-Meteo does not require an API key for this basic experiment.

## 3. Required Libraries

``` cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
```

`WiFi` and `HTTPClient` are included with the ESP32 Arduino Core.

Install **ArduinoJson** using the Arduino IDE Library Manager.

## 4. Complete Arduino Code

``` cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL =
  "https://api.open-meteo.com/v1/forecast"
  "?latitude=7.88"
  "&longitude=98.39"
  "&current=temperature_2m,"
  "relative_humidity_2m,"
  "wind_speed_10m";

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;

  http.begin(serverURL);

  Serial.println();
  Serial.println("Getting Phuket weather...");

  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpCode);

    String payload = http.getString();

    JsonDocument doc;

    DeserializationError error =
      deserializeJson(doc, payload);

    if (error)
    {
      Serial.println("JSON parsing failed!");
      http.end();
      return;
    }

    float temperature =
      doc["current"]["temperature_2m"];

    int humidity =
      doc["current"]["relative_humidity_2m"];

    float windSpeed =
      doc["current"]["wind_speed_10m"];

    Serial.println();
    Serial.println("==========================");
    Serial.println(" Phuket Weather");
    Serial.println("==========================");

    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity    : ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Wind Speed  : ");
    Serial.print(windSpeed);
    Serial.println(" km/h");

    Serial.println("==========================");
  }
  else
  {
    Serial.print("HTTP GET failed: ");
    Serial.println(httpCode);
  }

  http.end();
}

void loop()
{
}
```

## 5. Program Flow

``` text
START
  |
  v
Connect Wi-Fi
  |
  v
HTTPS GET
  |
  v
Open-Meteo API
  |
  v
Receive JSON
  |
  v
deserializeJson()
  |
  +----------------------+
  |          |           |
  v          v           v
Temperature Humidity  Wind Speed
  |          |           |
  +----------+-----------+
             |
             v
       Serial Monitor
```

## 6. JSON Response

Conceptually, the response contains:

``` json
{
  "current": {
    "temperature_2m": 30.2,
    "relative_humidity_2m": 72,
    "wind_speed_10m": 8.5
  }
}
```

Access the values with:

``` cpp
float temperature =
  doc["current"]["temperature_2m"];

int humidity =
  doc["current"]["relative_humidity_2m"];

float windSpeed =
  doc["current"]["wind_speed_10m"];
```

## 7. Run the Experiment

1.  Enter the Wi-Fi SSID and password.
2.  Upload the program to the ESP32.
3.  Open the Serial Monitor.
4.  Set the baud rate to `115200`.
5.  Observe the current Phuket weather.

Example:

``` text
Connecting to Wi-Fi....
Wi-Fi connected
IP Address: 192.168.1.105

Getting Phuket weather...
HTTP Response Code: 200

==========================
 Phuket Weather
==========================
Temperature : 30.2 C
Humidity    : 72 %
Wind Speed  : 8.5 km/h
==========================
```

Actual values depend on the current weather.

## 8. How the Lab Works

``` text
ESP32
  |
  | HTTPS GET
  v
Open-Meteo
  |
  | JSON
  v
ESP32
  |
  v
ArduinoJson
  |
  +--> Temperature
  +--> Humidity
  +--> Wind Speed
  |
  v
Serial Monitor
```

## 9. Checkpoint Questions

1.  What is the purpose of `HTTPClient`?
2.  What does `http.GET()` do?
3.  What does HTTP response code `200` mean?
4.  Why does the API return JSON?
5.  What does `deserializeJson()` do?
6.  How is `temperature_2m` extracted from the JSON object?
7.  Why are latitude and longitude required?

## 10. Extension Exercise

Modify the program to retrieve the Phuket weather every **60 seconds**
using `millis()`.

The program should repeatedly display:

``` text
Phuket Weather
Temperature : ...
Humidity    : ...
Wind Speed  : ...
```
