# ESP32 HTTP POST with JSON Data

## Objective

In this lab, students will use an **ESP32** to send simulated sensor
data to a web server using an **HTTP POST request**.

Students will learn how to:

-   Connect the ESP32 to Wi-Fi.
-   Create JSON-formatted sensor data.
-   Send data using HTTP POST.
-   Set the `Content-Type` HTTP header.
-   Check HTTP response codes.
-   Display the server response on the Serial Monitor.

------------------------------------------------------------------------

## 1. System Architecture

``` text
+----------------------+
|        ESP32         |
|                      |
| Temperature = 25.5 C |
| Humidity    = 60.2 % |
+----------+-----------+
           |
           | HTTPS POST
           |
           | Content-Type:
           | application/json
           |
           v
+----------------------+
|      httpbin.org     |
|                      |
|      /post API       |
+----------+-----------+
           |
           | JSON Response
           v
+----------------------+
|        ESP32         |
|                      |
|    Serial Monitor    |
+----------------------+
```

The important concept is:

``` text
ESP32
   |
   | Send JSON data
   |
   | HTTP POST
   v
REST API
```

------------------------------------------------------------------------

## 2. Test Data

For the first experiment, we will simulate two sensor values:

``` text
Temperature = 25.5 C
Humidity    = 60.2 %
```

The ESP32 will convert these values into JSON:

``` json
{
  "temperature": 25.5,
  "humidity": 60.2
}
```

------------------------------------------------------------------------

## 3. Test Server

Use the httpbin POST endpoint:

``` text
https://httpbin.org/post
```

The service receives the POST request and returns information about the
request. This makes it convenient for learning and testing HTTP.

------------------------------------------------------------------------

## 4. Complete ESP32 Program

``` cpp
#include <WiFi.h>
#include <HTTPClient.h>

// --------------------------------
// Wi-Fi Configuration
// --------------------------------

const char* ssid =
  "YOUR_WIFI_NAME";

const char* password =
  "YOUR_WIFI_PASSWORD";


// --------------------------------
// HTTP Server
// --------------------------------

const char* serverURL =
  "https://httpbin.org/post";


void setup()
{
  Serial.begin(115200);

  delay(1000);


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
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );


  // --------------------------------
  // Create HTTP Client
  // --------------------------------

  HTTPClient http;

  http.begin(serverURL);


  // --------------------------------
  // HTTP Header
  // --------------------------------

  http.addHeader(
    "Content-Type",
    "application/json"
  );


  // --------------------------------
  // Create JSON Data
  // --------------------------------

  String jsonData =
    "{\"temperature\":25.5,"
    "\"humidity\":60.2}";


  Serial.println();

  Serial.println(
    "Sending JSON:"
  );

  Serial.println(
    jsonData
  );


  // --------------------------------
  // HTTP POST
  // --------------------------------

  int httpCode =
    http.POST(jsonData);


  // --------------------------------
  // Display HTTP Response
  // --------------------------------

  Serial.print(
    "HTTP Response Code: "
  );

  Serial.println(
    httpCode
  );


  if (httpCode > 0)
  {
    String response =
      http.getString();

    Serial.println();

    Serial.println(
      "Server Response:"
    );

    Serial.println(
      response
    );
  }

  else
  {
    Serial.println(
      "HTTP POST failed"
    );
  }


  // --------------------------------
  // Close HTTP Connection
  // --------------------------------

  http.end();
}


void loop()
{
}
```

------------------------------------------------------------------------

## 5. Program Flow

``` text
START
  |
  v
Connect Wi-Fi
  |
  v
Wi-Fi Connected
  |
  v
Create HTTP Client
  |
  v
Set Content-Type
application/json
  |
  v
Create JSON
  |
  v
HTTP POST
  |
  v
httpbin.org/post
  |
  v
Receive HTTP Response
  |
  v
Display Response
  |
  v
END
```

------------------------------------------------------------------------

## 6. Important Commands

### Create the HTTP Client

``` cpp
HTTPClient http;
```

### Connect to the Server

``` cpp
http.begin(serverURL);
```

### Specify JSON Content

``` cpp
http.addHeader(
  "Content-Type",
  "application/json"
);
```

This tells the server:

``` text
The body of this request contains JSON data.
```

### Create the JSON Message

``` cpp
String jsonData =
  "{\"temperature\":25.5,"
  "\"humidity\":60.2}";
```

This represents:

``` json
{
  "temperature": 25.5,
  "humidity": 60.2
}
```

### Send the POST Request

``` cpp
int httpCode =
  http.POST(jsonData);
```

The data flow is:

``` text
jsonData
   |
   v
http.POST()
   |
   v
Internet
   |
   v
Web Server
```

------------------------------------------------------------------------

## 7. Expected Serial Monitor

Set the Serial Monitor to:

``` text
115200 baud
```

A successful experiment should look similar to:

``` text
Connecting to Wi-Fi....
Wi-Fi connected
IP Address: 192.168.1.25

Sending JSON:
{"temperature":25.5,"humidity":60.2}

HTTP Response Code: 200

Server Response:
{
  "args": {},
  "data": "{\"temperature\":25.5,\"humidity\":60.2}",
  "json": {
    "humidity": 60.2,
    "temperature": 25.5
  }
}
```

The exact server response may contain additional fields.

The important result is that the response contains the JSON sent by the
ESP32:

``` json
{
  "humidity": 60.2,
  "temperature": 25.5
}
```

This confirms that the POST request was successfully received.

------------------------------------------------------------------------

## 8. HTTP GET vs HTTP POST

This lab can follow directly from the previous weather experiment.

### HTTP GET

``` text
ESP32
   |
   | Request information
   v
Weather API
   |
   | Weather JSON
   v
ESP32
```

### HTTP POST

``` text
ESP32
   |
   | Sensor JSON
   v
Web API
   |
   | Response
   v
ESP32
```

  Method   Purpose                Example
  -------- ---------------------- --------------------
  `GET`    Retrieve information   Get Phuket weather
  `POST`   Send information       Upload sensor data

------------------------------------------------------------------------

## 9. Checkpoint Questions

1.  What is the purpose of an HTTP POST request?
2.  What does `Content-Type: application/json` mean?
3.  What information is stored in `jsonData`?
4.  What does `http.POST(jsonData)` do?
5.  What does HTTP response code `200` indicate?
6.  What is the difference between HTTP GET and POST?
7.  Why do we call `http.end()` after completing the request?

------------------------------------------------------------------------

## 10. Exercise 1 --- Change the Sensor Values

Modify:

``` cpp
String jsonData =
  "{\"temperature\":25.5,"
  "\"humidity\":60.2}";
```

to:

``` cpp
String jsonData =
  "{\"temperature\":30.5,"
  "\"humidity\":75.0}";
```

Observe the server response and confirm that the new values are
returned.

------------------------------------------------------------------------

## 11. Exercise 2 --- Use Variables

Instead of fixed JSON values, define:

``` cpp
float temperature = 28.5;
float humidity = 72.3;
```

Then create the JSON message dynamically:

``` cpp
String jsonData =
  "{\"temperature\":" +
  String(temperature) +
  ",\"humidity\":" +
  String(humidity) +
  "}";
```

The resulting JSON becomes:

``` json
{
  "temperature": 28.5,
  "humidity": 72.3
}
```

This prepares students for replacing the simulated values with **real
sensor measurements**.

------------------------------------------------------------------------

## 12. Recommended Next Lab

A natural extension is:

``` text
Temperature/Humidity Sensor
          |
          v
        ESP32
          |
          | Read Sensor
          v
      Create JSON
          |
          | HTTP POST
          v
       REST API
          |
          v
       Database
          |
          v
      Web Dashboard
```

This provides a clear learning progression:

``` text
HTTP GET
   |
   v
JSON Parsing
   |
   v
HTTP POST
   |
   v
Real Sensor Data
   |
   v
IoT Cloud / Dashboard
```
