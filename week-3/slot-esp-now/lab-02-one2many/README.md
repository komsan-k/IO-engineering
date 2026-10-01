# Lab — ESP-NOW One-to-Many Communication

## Objective

In this lab, one ESP32 sends data to multiple ESP32 receivers using **ESP-NOW**.

The system uses:

- **ESP32 #1** as the **Sender**
- **ESP32 #2** as **Receiver 1**
- **ESP32 #3** as **Receiver 2**
- **ESP32 #4** as **Receiver 3**

The sender transmits the same counter value to all three receivers.

---

## System Architecture

```text
ESP32 #1
Sender
   |
   +----> ESP32 #2 Receiver
   |
   +----> ESP32 #3 Receiver
   |
   +----> ESP32 #4 Receiver
```

---

## Important Concept

ESP-NOW uses the ESP32 Wi-Fi radio for direct device-to-device communication.

Each receiver is identified by its **Wi-Fi MAC address**.

Before uploading the sender code:

1. upload the receiver code to each ESP32 receiver;
2. open the Serial Monitor;
3. record the Wi-Fi MAC address of each receiver;
4. copy the MAC addresses into the sender program.

Example MAC address:

```text
24:6F:28:11:22:33
```

Convert it to:

```cpp
uint8_t receiver1[] =
{
  0x24, 0x6F, 0x28,
  0x11, 0x22, 0x33
};
```

---

# Part A — ESP32 Sender Code

Upload this code to **ESP32 #1**.

```cpp
#include <WiFi.h>
#include <esp_now.h>

// --------------------------------
// Receiver MAC Addresses
// --------------------------------

uint8_t receiver1[] =
{
  0x24, 0x6F, 0x28,
  0x11, 0x22, 0x33
};

uint8_t receiver2[] =
{
  0x24, 0x6F, 0x28,
  0x44, 0x55, 0x66
};

uint8_t receiver3[] =
{
  0x24, 0x6F, 0x28,
  0x77, 0x88, 0x99
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
// Add Peer
// ========================================

void addPeer(
  uint8_t *mac
)
{
  esp_now_peer_info_t peerInfo = {};

  memcpy(
    peerInfo.peer_addr,
    mac,
    6
  );

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (
    esp_now_add_peer(
      &peerInfo
    )
    == ESP_OK
  )
  {
    Serial.println(
      "Peer added"
    );
  }
  else
  {
    Serial.println(
      "Failed to add peer"
    );
  }
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
  // Add Receivers
  // --------------------------------

  addPeer(
    receiver1
  );

  addPeer(
    receiver2
  );

  addPeer(
    receiver3
  );


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


  Serial.print(
    "Sending Counter: "
  );

  Serial.println(
    data.counter
  );


  // Send to Receiver 1
  esp_now_send(
    receiver1,
    (uint8_t *)&data,
    sizeof(data)
  );


  // Send to Receiver 2
  esp_now_send(
    receiver2,
    (uint8_t *)&data,
    sizeof(data)
  );


  // Send to Receiver 3
  esp_now_send(
    receiver3,
    (uint8_t *)&data,
    sizeof(data)
  );


  delay(2000);
}
```

---

## Expected Serial Monitor — Sender

```text
Sender MAC: 24:6F:28:AA:BB:CC
Peer added
Peer added
Peer added
ESP-NOW Sender ready

Sending Counter: 1
Send Status: Success
Send Status: Success
Send Status: Success

Sending Counter: 2
Send Status: Success
Send Status: Success
Send Status: Success
```

---

# Part B — ESP32 Receiver Code

Use the same receiver code on:

- ESP32 #2
- ESP32 #3
- ESP32 #4

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

Each receiver should show:

```text
Receiver MAC: 24:6F:28:11:22:33
ESP-NOW Receiver ready

Received Counter: 1
Received Counter: 2
Received Counter: 3
```

---

## Communication Flow

```text
ESP32 Sender
      |
      | Counter = 1
      |
      +------> Receiver 1
      |
      +------> Receiver 2
      |
      +------> Receiver 3
```

All receivers receive the same data.

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

### Add a Receiver as a Peer

```cpp
esp_now_add_peer(
  &peerInfo
);
```

### Send Data

```cpp
esp_now_send(
  receiver1,
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

1. Upload the receiver code to ESP32 #2.
2. Record its Wi-Fi MAC address.
3. Upload the receiver code to ESP32 #3.
4. Record its Wi-Fi MAC address.
5. Upload the receiver code to ESP32 #4.
6. Record its Wi-Fi MAC address.
7. Replace `receiver1`, `receiver2`, and `receiver3` MAC addresses in the sender code.
8. Upload the sender code to ESP32 #1.
9. Open the Serial Monitors.
10. Observe the same counter value arriving at all receivers.

---

## Experiment 1 — Send a Simulated Sensor Value

Replace:

```cpp
data.counter++;
```

with a simulated ADC value.

For example:

```cpp
data.counter =
  random(0, 4096);
```

Now all receivers receive the same simulated sensor value.

---

## Experiment 2 — Different Action at Each Receiver

The same transmitted value can be used differently at each receiver.

Example:

```text
Receiver 1 -> LED 1
Receiver 2 -> LED 2
Receiver 3 -> LED 3
```

This allows one sender to coordinate multiple ESP32 nodes.

---

## Checkpoint Questions

1. What is ESP-NOW?
2. What does one-to-many communication mean?
3. How many senders are used in this lab?
4. How many receivers are used?
5. Why does the sender need each receiver's MAC address?
6. What function initializes ESP-NOW?
7. What function adds a peer?
8. What function sends data?
9. Can all receivers use the same receiver program?
10. How could this system be used in an IoT application?

---

## Simple Assignment

Modify the system so that one ESP32 sender controls three remote LEDs:

```text
ESP32 Sender
   |
   +----> Receiver 1 -> LED 1
   |
   +----> Receiver 2 -> LED 2
   |
   +----> Receiver 3 -> LED 3
```

The sender should periodically transmit a control value, and each receiver should use the received value to control its own LED.
