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
  "https://httpbin.org/get";


// --------------------------------
// Refresh Interval
// --------------------------------

const unsigned long REFRESH_TIME =
  5000;

unsigned long previousTime = 0;


// ========================================
// Setup
// ========================================

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
  // Initialize Random Generator
  // --------------------------------

  randomSeed(
    analogRead(0)
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  if (
    millis() - previousTime
    >= REFRESH_TIME
  )
  {
    previousTime = millis();


    // --------------------------------
    // Generate Random Sensor Data
    // --------------------------------

    float temperature =
      random(250, 351) / 10.0;

    float humidity =
      random(500, 901) / 10.0;


    // --------------------------------
    // Create GET URL
    // --------------------------------

    String requestURL =
      String(serverURL) +
      "?temperature=" +
      String(temperature, 1) +
      "&humidity=" +
      String(humidity, 1);


    // --------------------------------
    // Display Request
    // --------------------------------

    Serial.println();

    Serial.println(
      "=================================="
    );

    Serial.println(
      "       HTTP GET REQUEST"
    );

    Serial.println(
      "=================================="
    );


    Serial.println();

    Serial.println(
      "HTTP Method:"
    );

    Serial.println(
      "GET"
    );


    Serial.println();

    Serial.println(
      "Request URL:"
    );

    Serial.println(
      requestURL
    );


    Serial.println();

    Serial.println(
      "Sensor Data:"
    );


    Serial.print(
      "Temperature : "
    );

    Serial.print(
      temperature,
      1
    );

    Serial.println(
      " C"
    );


    Serial.print(
      "Humidity    : "
    );

    Serial.print(
      humidity,
      1
    );

    Serial.println(
      " %"
    );


    // --------------------------------
    // Create HTTP Client
    // --------------------------------

    HTTPClient http;

    http.begin(
      requestURL
    );


    // --------------------------------
    // Send HTTP GET
    // --------------------------------

    int httpCode =
      http.GET();


    // --------------------------------
    // Display Response
    // --------------------------------

    Serial.println();

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
      Serial.println();

      Serial.println(
        "HTTP GET failed"
      );
    }


    // --------------------------------
    // Close HTTP Connection
    // --------------------------------

    http.end();


    Serial.println();

    Serial.println(
      "Next request in 5 seconds..."
    );
  }
}
