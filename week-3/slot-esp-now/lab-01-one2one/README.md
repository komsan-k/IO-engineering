# Lab 1 — Simple ESP-NOW One-to-One Communication

## Objective

In this lab, two ESP32 boards communicate directly using **ESP-NOW**.

- **ESP32 #1** acts as the **Sender**
- **ESP32 #2** acts as the **Receiver**

The sender transmits a counter value every 2 seconds.

---

## System Architecture

```text
+----------------------+
|      ESP32 #1        |
|       Sender         |
|                      |
| Counter: 1, 2, 3...  |
+----------+-----------+
           |
           | ESP-NOW
           v
+----------------------+
|      ESP32 #2        |
|      Receiver        |
|                      |
| Receive Counter      |
+----------+-----------+
           |
           v
+----------------------+
|    Serial Monitor    |
|                      |
| Received: 1          |
| Received: 2          |
| Received: 3          |
+----------------------+
```

---

## Important Concept

ESP-NOW uses the ESP32 Wi-Fi radio for direct device-to-device communication.

The receiver is identified by its **Wi-Fi MAC address**.

First, read the MAC address of ESP32 #2:

```cpp
Serial.println(
  WiFi.macAddress()
);
```

Then copy that MAC address into the sender program.

Example MAC address:

```text
24:6F:28:12:34:56
```

Convert it to:

```cpp
uint8_t receiverMac[] =
{
  0x24, 0x6F, 0x28,
  0x12, 0x34, 0x56
};
```

---

# Part A — ESP32 #1 ESP-NOW Sender

Upload this program to **ESP32 #1**.

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Receiver MAC Address
// --------------------------------

// Replace with ESP32 #2 MAC address
uint8_t receiverMac[] =
{
  0x24, 0x6F, 0x28,
  0x12, 0x34, 0x56
};

// --------------------------------
// Data Structure
// --------------------------------

typedef struct
{
  int counter;
}
DataPacket;

DataPacket data;


// ========================================
// Send Callback
// ========================================

void onDataSent(
  const uint8_t *macAddr,
  esp_now_send_status_t status
)
{
  Serial.print(
    "Send Status: "
  );

  if (
    status == ESP_NOW_SEND_SUCCESS
  )
  {
    Serial.println(
      "Success"
    );
  }
  else
  {
    Serial.println(
      "Failed"
    );
  }
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  // ESP-NOW uses Wi-Fi radio
  WiFi.mode(
    WIFI_STA
  );

  Serial.print(
    "Sender MAC: "
  );

  Serial.println(
    WiFi.macAddress()
  );


  // --------------------------------
  // Initialize ESP-NOW
  // --------------------------------

  if (
    esp_now_init()
    != ESP_OK
  )
  {
    Serial.println(
      "ESP-NOW initialization failed"
    );

    return;
  }


  // --------------------------------
  // Register Send Callback
  // --------------------------------

  esp_now_register_send_cb(
    onDataSent
  );


  // --------------------------------
  // Add Receiver as Peer
  // --------------------------------

  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    receiverMac,
    6
  );

  peerInfo.channel = 0;
  peerInfo.encrypt = false;


  if (
    esp_now_add_peer(
      &peerInfo
    )
    != ESP_OK
  )
  {
    Serial.println(
      "Failed to add peer"
    );

    return;
  }


  Serial.println(
    "ESP-NOW Sender ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
  data.counter++;

  esp_err_t result =
    esp_now_send(
      receiverMac,
      (uint8_t *)&data,
      sizeof(data)
    );


  Serial.print(
    "Sending Counter: "
  );

  Serial.println(
    data.counter
  );


  if (
    result != ESP_OK
  )
  {
    Serial.println(
      "Error sending data"
    );
  }


  delay(2000);
}
```

---

## Expected Serial Monitor — Sender

```text
Sender MAC: 24:6F:28:AA:BB:CC
ESP-NOW Sender ready

Sending Counter: 1
Send Status: Success

Sending Counter: 2
Send Status: Success
```

---

# Part B — ESP32 #2 ESP-NOW Receiver

Upload this program to **ESP32 #2**.

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Data Structure
// --------------------------------

typedef struct
{
  int counter;
}
DataPacket;

DataPacket receivedData;


// ========================================
// Receive Callback
// ========================================

void onDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *incomingData,
  int len
)
{
  memcpy(
    &receivedData,
    incomingData,
    sizeof(receivedData)
  );


  Serial.print(
    "Received Counter: "
  );

  Serial.println(
    receivedData.counter
  );
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(115200);

  WiFi.mode(
    WIFI_STA
  );


  Serial.print(
    "Receiver MAC: "
  );

  Serial.println(
    WiFi.macAddress()
  );


  // --------------------------------
  // Initialize ESP-NOW
  // --------------------------------

  if (
    esp_now_init()
    != ESP_OK
  )
  {
    Serial.println(
      "ESP-NOW initialization failed"
    );

    return;
  }


  // --------------------------------
  // Register Receive Callback
  // --------------------------------

  esp_now_register_recv_cb(
    onDataRecv
  );


  Serial.println(
    "ESP-NOW Receiver ready"
  );
}


// ========================================
// Main Loop
// ========================================

void loop()
{
}
```

---

## Expected Serial Monitor — Receiver

```text
Receiver MAC: 24:6F:28:12:34:56
ESP-NOW Receiver ready

Received Counter: 1
Received Counter: 2
Received Counter: 3
```

---

## Communication Flow

```text
ESP32 #1
Sender
   |
   | ESP-NOW
   | Counter
   v
ESP32 #2
Receiver
   |
   v
Serial Monitor
```

---

## Important Commands

### Set Wi-Fi Station Mode

```cpp
WiFi.mode(
  WIFI_STA
);
```

### Initialize ESP-NOW

```cpp
esp_now_init();
```

### Add Receiver as Peer

```cpp
esp_now_add_peer(
  &peerInfo
);
```

### Send Data

```cpp
esp_now_send(
  receiverMac,
  (uint8_t *)&data,
  sizeof(data)
);
```

### Register Receive Callback

```cpp
esp_now_register_recv_cb(
  onDataRecv
);
```

---

## How to Run

1. Upload the receiver program to ESP32 #2.
2. Open the Serial Monitor.
3. Record the Wi-Fi MAC address of ESP32 #2.
4. Replace `receiverMac[]` in the sender program with that MAC address.
5. Upload the sender program to ESP32 #1.
6. Open both Serial Monitors.
7. Observe the counter values being transmitted.

---

## Checkpoint Questions

1. What is ESP-NOW?
2. Which ESP32 acts as the sender?
3. Which ESP32 acts as the receiver?
4. Why is the receiver MAC address required?
5. What Wi-Fi mode is used for ESP-NOW?
6. What function initializes ESP-NOW?
7. What function sends data?
8. What function receives data?
9. What does the send callback report?
10. How often is the counter sent in this example?

---

## Simple Assignment

Modify the sender so it transmits a simulated ADC value instead of a counter.

Example:

```cpp
int sensorValue =
  random(0, 4096);
```

Then modify the structure:

```cpp
typedef struct
{
  int sensorValue;
}
DataPacket;
```

The receiver should display:

```text
Received Sensor Value: 1532
Received Sensor Value: 2890
Received Sensor Value: 745
```
