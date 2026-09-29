#ifndef INDEX_H
#define INDEX_H

#include <Arduino.h>

// HTML webpage stored in flash memory
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 LED Control</title>

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin-top: 40px;
      background: #f4f6f8;
    }

    .card {
      width: 320px;
      max-width: 90%;
      margin: auto;
      padding: 30px;
      background: white;
      border-radius: 12px;
      box-shadow: 0 2px 8px rgba(0,0,0,0.15);
    }

    button {
      font-size: 24px;
      padding: 15px 30px;
      margin: 10px;
      cursor: pointer;
    }

    #status {
      font-size: 28px;
      font-weight: bold;
    }
  </style>
</head>

<body>
  <div class="card">
    <h1>ESP32 LED Control</h1>

    <p>
      LED Status:
      <span id="status">UNKNOWN</span>
    </p>

    <button onclick="ledOn()">LED ON</button>
    <button onclick="ledOff()">LED OFF</button>
  </div>

  <script>
    async function ledOn()
    {
      const response = await fetch("/led/on");
      const state = await response.text();
      document.getElementById("status").textContent = state;
    }

    async function ledOff()
    {
      const response = await fetch("/led/off");
      const state = await response.text();
      document.getElementById("status").textContent = state;
    }
  </script>
</body>
</html>

)rawliteral";

#endif // INDEX_H
