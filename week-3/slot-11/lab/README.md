# LAB 11 — Bluetooth Low Energy (BLE) GATT Server & Sensor Notifications

## 1. Objective

The objective of this lab is to:

1. Understand the basic architecture of **Bluetooth Low Energy (BLE)**.
2. Configure an ESP32 as a **BLE peripheral**.
3. Create a **GATT server** on the ESP32.
4. Define a custom **BLE service** and **characteristic**.
5. Expose sensor data through a readable GATT characteristic.
6. Enable **notifications** so that sensor data is pushed automatically to a connected client.
7. Connect to the ESP32 using a mobile BLE application.
8. Monitor live sensor notifications from a smartphone.
9. Compare BLE communication with Wi-Fi/MQTT communication.
10. Investigate BLE connection status, update interval, and communication reliability.

---

## 2. Background

Bluetooth Low Energy is designed for low-power, short-range wireless communication. Unlike Wi-Fi, BLE does not normally connect a device directly to the Internet. Instead, devices communicate locally using a **central-peripheral architecture**.

A typical BLE system is:

```text
ESP32
Peripheral
   ↓
BLE Advertising
   ↓
Smartphone
Central
```

Once the smartphone connects, communication is organized using the **Generic Attribute Profile (GATT)**.

```text
BLE Peripheral
     ↓
GATT Server
     ↓
Service
     ↓
Characteristic
     ↓
Sensor Data
```

The ESP32 acts as the **GATT server**, while the mobile application acts as the **GATT client**.

### Key Concepts

- **BLE:** Bluetooth Low Energy.
- **Peripheral:** Device that advertises and accepts connections.
- **Central:** Device that scans for and connects to peripherals.
- **Advertising:** Process used by a peripheral to announce its presence.
- **GATT:** Generic Attribute Profile.
- **Service:** Logical collection of related characteristics.
- **Characteristic:** Data value exposed by a GATT server.
- **UUID:** Universally Unique Identifier for services and characteristics.
- **Read:** Client manually requests the current value.
- **Write:** Client sends data to a characteristic.
- **Notify:** Server sends updated data automatically to a subscribed client.
- **Indicate:** Similar to notify, but requires acknowledgement.

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Smartphone with Bluetooth enabled
- Optional analog sensor or potentiometer
- Optional temperature/humidity sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- Mobile BLE application

Suitable mobile applications include:

- nRF Connect
- LightBlue
- BLE Scanner

---

## 4. BLE Communication Architecture

The complete architecture is:

```text
Sensor
   ↓
ESP32
BLE Peripheral
   ↓
GATT Server
   ↓
Service
   ↓
Characteristic
   ↓
BLE Notification
   ↓
Smartphone App
BLE Central
```

The main sequence is:

```text
ESP32 Starts
    ↓
Create BLE Server
    ↓
Create Service
    ↓
Create Characteristic
    ↓
Start Advertising
    ↓
Phone Scans
    ↓
Phone Connects
    ↓
Phone Subscribes
    ↓
Sensor Notifications
```

---

## 5. GATT Services and Characteristics

A GATT server contains one or more services.

For this lab, define:

```text
Custom Sensor Service
      ↓
Sensor Characteristic
```

Example UUIDs:

```cpp
#define SERVICE_UUID \
"12345678-1234-1234-1234-1234567890ab"

#define SENSOR_CHAR_UUID \
"abcdefab-1234-5678-1234-abcdefabcdef"
```

The service and characteristic are represented as:

```text
Service UUID
12345678-1234-1234-1234-1234567890ab

        ↓

Sensor Characteristic UUID
abcdefab-1234-5678-1234-abcdefabcdef
```

---

## 6. Characteristic Properties

A BLE characteristic may support several properties.

| Property | Purpose |
|---|---|
| `READ` | Client reads current value |
| `WRITE` | Client writes new value |
| `NOTIFY` | Server pushes updates |
| `INDICATE` | Server pushes acknowledged updates |

For this lab, the sensor characteristic will use:

```text
READ + NOTIFY
```

This means the smartphone can:

1. Manually read the current sensor value.
2. Subscribe to receive automatic updates.

---

## 7. Experiment 1 — BLE Advertising

The first experiment creates a simple BLE device.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>

void setup() {

  Serial.begin(115200);

  BLEDevice::init(
    "ESP32-BLE"
  );

  BLEServer *server =
      BLEDevice::createServer();

  BLEAdvertising *advertising =
      BLEDevice::getAdvertising();

  advertising->start();

  Serial.println(
    "BLE advertising started"
  );
}

void loop() {}
```

### Expected Result

Open the BLE application on the smartphone and scan for nearby devices.

You should see:

```text
ESP32-BLE
```

The communication process is:

```text
ESP32
   ↓
Advertising Packets
   ↓
Smartphone Scan
   ↓
Device Discovered
```

---

## 8. Experiment 2 — Creating a GATT Service

Create a custom BLE service.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID \
"12345678-1234-1234-1234-1234567890ab"

BLEServer *pServer;
BLEService *pService;

void setup() {

  Serial.begin(115200);

  BLEDevice::init(
    "ESP32-Sensor"
  );

  pServer =
      BLEDevice::createServer();

  pService =
      pServer->createService(
        SERVICE_UUID
      );

  pService->start();

  BLEAdvertising *advertising =
      BLEDevice::getAdvertising();

  advertising->addServiceUUID(
    SERVICE_UUID
  );

  advertising->start();

  Serial.println(
    "GATT service started"
  );
}

void loop() {}
```

After connecting with the mobile application, the custom service should be visible.

---

## 9. Experiment 3 — Creating a Sensor Characteristic

Create a characteristic inside the service.

```cpp
#define SENSOR_CHAR_UUID \
"abcdefab-1234-5678-1234-abcdefabcdef"
```

Add:

```cpp
BLECharacteristic *sensorCharacteristic;

sensorCharacteristic =
    pService->createCharacteristic(
      SENSOR_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ
    );
```

Assign a value:

```cpp
sensorCharacteristic->setValue(
  "25.5"
);
```

The GATT structure becomes:

```text
ESP32 GATT Server
      ↓
Sensor Service
      ↓
Sensor Characteristic
      ↓
"25.5"
```

---

## 10. Experiment 4 — Reading Sensor Data

A complete readable characteristic example is:

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID \
"12345678-1234-1234-1234-1234567890ab"

#define SENSOR_CHAR_UUID \
"abcdefab-1234-5678-1234-abcdefabcdef"

BLECharacteristic *sensorCharacteristic;

void setup() {

  Serial.begin(115200);

  BLEDevice::init(
    "ESP32-Sensor"
  );

  BLEServer *server =
      BLEDevice::createServer();

  BLEService *service =
      server->createService(
        SERVICE_UUID
      );

  sensorCharacteristic =
      service->createCharacteristic(
        SENSOR_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ
      );

  sensorCharacteristic->setValue(
    "25.5"
  );

  service->start();

  BLEAdvertising *advertising =
      BLEDevice::getAdvertising();

  advertising->addServiceUUID(
    SERVICE_UUID
  );

  advertising->start();

  Serial.println(
    "BLE Sensor Ready"
  );
}

void loop() {}
```

### Mobile App Procedure

1. Open the BLE mobile app.
2. Scan for `ESP32-Sensor`.
3. Connect to the ESP32.
4. Open the custom service.
5. Select the sensor characteristic.
6. Press **Read**.

Expected value:

```text
25.5
```

---

## 11. Experiment 5 — Reading an Analog Sensor

Connect a potentiometer or analog sensor.

```cpp
#define SENSOR_PIN 34
```

Update the characteristic:

```cpp
void loop() {

  int sensorValue =
      analogRead(SENSOR_PIN);

  char buffer[20];

  snprintf(
    buffer,
    sizeof(buffer),
    "%d",
    sensorValue
  );

  sensorCharacteristic->setValue(
    buffer
  );

  Serial.println(buffer);

  delay(1000);
}
```

The data flow is:

```text
Analog Sensor
      ↓
analogRead()
      ↓
ESP32
      ↓
GATT Characteristic
      ↓
Mobile App Read
```

---

## 12. Experiment 6 — Enabling BLE Notifications

Manual reading requires the user to request the value repeatedly.

Notifications allow the ESP32 to send updates automatically.

Create the characteristic using:

```cpp
sensorCharacteristic =
    service->createCharacteristic(
      SENSOR_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_NOTIFY
    );
```

A notification descriptor is usually added:

```cpp
#include <BLE2902.h>

sensorCharacteristic->addDescriptor(
  new BLE2902()
);
```

Send a notification using:

```cpp
sensorCharacteristic->notify();
```

---

## 13. Experiment 7 — Live Sensor Notifications

A complete notification example is:

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SENSOR_PIN 34

#define SERVICE_UUID \
"12345678-1234-1234-1234-1234567890ab"

#define SENSOR_CHAR_UUID \
"abcdefab-1234-5678-1234-abcdefabcdef"

BLECharacteristic *sensorCharacteristic;

void setup() {

  Serial.begin(115200);

  BLEDevice::init(
    "ESP32-Sensor"
  );

  BLEServer *server =
      BLEDevice::createServer();

  BLEService *service =
      server->createService(
        SERVICE_UUID
      );

  sensorCharacteristic =
      service->createCharacteristic(
        SENSOR_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
      );

  sensorCharacteristic->addDescriptor(
    new BLE2902()
  );

  service->start();

  BLEAdvertising *advertising =
      BLEDevice::getAdvertising();

  advertising->addServiceUUID(
    SERVICE_UUID
  );

  advertising->start();

  Serial.println(
    "BLE notification server started"
  );
}

void loop() {

  int sensorValue =
      analogRead(SENSOR_PIN);

  char buffer[20];

  snprintf(
    buffer,
    sizeof(buffer),
    "%d",
    sensorValue
  );

  sensorCharacteristic->setValue(
    buffer
  );

  sensorCharacteristic->notify();

  Serial.print(
    "Notify: "
  );

  Serial.println(buffer);

  delay(1000);
}
```

---

## 14. Monitoring Notifications Using a Mobile App

Using a BLE mobile application:

1. Scan for `ESP32-Sensor`.
2. Connect to the device.
3. Open the custom service.
4. Select the sensor characteristic.
5. Enable **Notify**.
6. Observe the incoming values.

The result should look similar to:

```text
2048
2054
2070
2105
2088
```

The communication now becomes:

```text
Sensor
   ↓
ESP32
   ↓
Characteristic Update
   ↓
notify()
   ↓
Smartphone
   ↓
Live Sensor Display
```

---

## 15. Experiment 8 — Connection Detection

It is useful to know whether a BLE client is currently connected.

Create server callbacks:

```cpp
bool deviceConnected = false;

class MyServerCallbacks :
    public BLEServerCallbacks {

  void onConnect(
      BLEServer *pServer
  ) {

    deviceConnected = true;

    Serial.println(
      "Client connected"
    );
  }

  void onDisconnect(
      BLEServer *pServer
  ) {

    deviceConnected = false;

    Serial.println(
      "Client disconnected"
    );
  }
};
```

Attach the callback:

```cpp
pServer->setCallbacks(
  new MyServerCallbacks()
);
```

Only send notifications when connected:

```cpp
if (deviceConnected) {

  sensorCharacteristic->notify();
}
```

---

## 16. Experiment 9 — Restarting Advertising After Disconnection

When a smartphone disconnects, restart advertising:

```cpp
void onDisconnect(
    BLEServer *pServer
) {

  deviceConnected = false;

  BLEDevice::startAdvertising();

  Serial.println(
    "Advertising restarted"
  );
}
```

The connection cycle becomes:

```text
Advertising
    ↓
Connected
    ↓
Notifications
    ↓
Disconnected
    ↓
Restart Advertising
    ↓
Ready for New Connection
```

---

## 17. Experiment 10 — Temperature and Humidity Characteristics

Instead of one sensor value, a BLE service can contain multiple characteristics.

```text
Environmental Service
        │
        ├── Temperature Characteristic
        │
        └── Humidity Characteristic
```

Example UUID definitions:

```cpp
#define TEMP_CHAR_UUID \
"11111111-1111-1111-1111-111111111111"

#define HUM_CHAR_UUID \
"22222222-2222-2222-2222-222222222222"
```

Create the characteristics:

```cpp
BLECharacteristic *temperatureChar;
BLECharacteristic *humidityChar;

temperatureChar =
    service->createCharacteristic(
      TEMP_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_NOTIFY
    );

humidityChar =
    service->createCharacteristic(
      HUM_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_NOTIFY
    );
```

---

## 18. Structured Sensor Update

Assume:

```cpp
float temperature = 25.5;
float humidity = 60.2;
```

Update the characteristics:

```cpp
char tempBuffer[16];
char humBuffer[16];

snprintf(
  tempBuffer,
  sizeof(tempBuffer),
  "%.2f",
  temperature
);

snprintf(
  humBuffer,
  sizeof(humBuffer),
  "%.2f",
  humidity
);

temperatureChar->setValue(
  tempBuffer
);

humidityChar->setValue(
  humBuffer
);

temperatureChar->notify();
humidityChar->notify();
```

The mobile client receives:

```text
Temperature → 25.50
Humidity    → 60.20
```

---

## 19. Experiment 11 — BLE with FreeRTOS Task

BLE notification can be placed inside a FreeRTOS task.

```cpp
void SensorTask(
    void *pvParameters
) {

  for (;;) {

    int value =
        analogRead(SENSOR_PIN);

    char buffer[20];

    snprintf(
      buffer,
      sizeof(buffer),
      "%d",
      value
    );

    sensorCharacteristic->setValue(
      buffer
    );

    if (deviceConnected) {

      sensorCharacteristic->notify();
    }

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}
```

Create the task:

```cpp
xTaskCreate(
  SensorTask,
  "SensorTask",
  4096,
  NULL,
  1,
  NULL
);
```

The architecture becomes:

```text
Sensor
   ↓
Sensor Task
   ↓
BLE Characteristic
   ↓
Notification
   ↓
Mobile Client
```

---

## 20. BLE Notification Interval

Suppose notifications are sent every:

$$
T_n = 1~\text{s}.
$$

The notification rate is:

$$
f_n = \frac{1}{T_n}.
$$

Therefore:

$$
f_n = 1~\text{Hz}.
$$

If the notification interval is reduced to:

$$
T_n = 0.1~\text{s},
$$

then:

$$
f_n = 10~\text{Hz}.
$$

Higher notification rates increase data throughput but may also increase power consumption and wireless activity.

---

## 21. Experiment 12 — Notification Rate

Test the following notification intervals:

```text
2000 ms
1000 ms
500 ms
200 ms
```

Record:

- Number of notifications
- Missing notifications
- Mobile app responsiveness
- ESP32 behavior

Suggested table:

| Interval | Notification Rate | Received Messages | Lost Messages |
|---:|---:|---:|---:|
| 2000 ms | 0.5 Hz |  |  |
| 1000 ms | 1 Hz |  |  |
| 500 ms | 2 Hz |  |  |
| 200 ms | 5 Hz |  |  |

---

## 22. BLE Read vs. Notify

| Feature | Read | Notify |
|---|---|---|
| Initiated by | Client | Server |
| Automatic updates | No | Yes |
| Client request required | Yes | Only subscription initially |
| Suitable for live data | Limited | Excellent |
| Network activity | On demand | Periodic/event-driven |
| Typical use | Configuration/status | Sensor streaming |

A useful rule is:

```text
READ
→ Client requests data

NOTIFY
→ Server pushes data
```

---

## 23. BLE Notify vs. Indicate

| Feature | Notify | Indicate |
|---|---|---|
| Server sends automatically | Yes | Yes |
| Client acknowledgement | No | Yes |
| Overhead | Lower | Higher |
| Reliability | Lower | Higher |
| Typical use | Frequent sensor data | Critical updates |

The difference is:

```text
Notify
Server → Client

Indicate
Server → Client → Acknowledgement
```

---

## 24. BLE vs. MQTT

| Feature | BLE | MQTT |
|---|---|---|
| Communication range | Short | Network-dependent |
| Internet required | No | Usually broker/network required |
| Architecture | Central-peripheral | Publish-subscribe |
| Power consumption | Generally low | Higher with Wi-Fi |
| Direct phone connection | Excellent | Usually indirect |
| Broker required | No | Yes |
| Typical application | Wearable/local sensor | IoT cloud telemetry |
| Remote Internet control | Limited | Excellent |

The communication difference is:

```text
BLE

ESP32
   ↓
Direct Wireless Link
   ↓
Smartphone
```

versus:

```text
MQTT

ESP32
   ↓
Wi-Fi
   ↓
MQTT Broker
   ↓
Application
```

---

## 25. Experiments

### Experiment 1 — BLE Advertising

Advertise the ESP32 as:

```text
ESP32-Sensor
```

Verify that the device appears in the mobile BLE application.

### Experiment 2 — GATT Service

Create a custom service and verify that the UUID appears in the mobile application.

### Experiment 3 — Read Characteristic

Create a readable sensor characteristic.

Read the value manually from the mobile app.

### Experiment 4 — Sensor Data

Connect an analog sensor and expose its ADC value through the characteristic.

### Experiment 5 — Notifications

Enable notifications and observe live sensor updates.

### Experiment 6 — Connection Recovery

Disconnect the smartphone.

Verify that the ESP32 restarts advertising and can reconnect.

### Experiment 7 — Multiple Characteristics

Create:

```text
Temperature
Humidity
```

characteristics and monitor both.

### Experiment 8 — Notification Rate

Change notification intervals and investigate the effect on communication.

---

## 26. Exercises

### Exercise 1 — BLE Sensor Monitor

Implement:

```text
Analog Sensor
     ↓
ESP32
     ↓
BLE GATT
     ↓
Sensor Characteristic
     ↓
Smartphone
```

Send one notification per second.

### Exercise 2 — Environmental Monitor

Create:

```text
Environmental Service
        │
        ├── Temperature
        └── Humidity
```

Both characteristics must support:

```text
READ + NOTIFY
```

### Exercise 3 — Signal Status Characteristic

Create another characteristic that exposes system information such as:

```text
System Uptime
Free Heap
```

### Exercise 4 — FreeRTOS BLE Sensor Task

Implement:

```text
Sensor Task
     ↓
Processing
     ↓
BLE Characteristic
     ↓
Notification
```

Use `vTaskDelay()` for periodic updates.

### Exercise 5 — Notification Reliability

Send 100 notifications.

Record:

- Number sent
- Number received
- Number lost

Calculate notification delivery ratio:

$$
\text{Delivery Ratio}
= \frac{N_{\text{received}}}
     {N_{\text{sent}}}
\times 100\%.
$$

### Exercise 6 — Complete BLE Sensor Node

Develop an ESP32 BLE node that:

1. Advertises as `ESP32-Sensor`.
2. Creates a custom GATT service.
3. Exposes at least two sensor characteristics.
4. Supports `READ`.
5. Supports `NOTIFY`.
6. Detects client connection/disconnection.
7. Restarts advertising after disconnection.
8. Displays live sensor values on a mobile BLE application.

The final architecture should be:

```text
                  Smartphone
                 BLE Central
                     ▲
                     │
                Notifications
                     │
                     ▼
               ┌───────────┐
               │   ESP32   │
               │ GATT      │
               │ Server    │
               └─────┬─────┘
                     │
              ┌──────┴──────┐
              │             │
              ▼             ▼
        Temperature      Humidity
        Characteristic   Characteristic
              ▲             ▲
              └──────┬──────┘
                     │
                   Sensors
```

---

## 27. Checkpoint Questions

1. What is Bluetooth Low Energy?
2. What is the difference between a BLE central and peripheral?
3. What is BLE advertising?
4. What is GATT?
5. What is a GATT server?
6. What is a GATT client?
7. What is a BLE service?
8. What is a BLE characteristic?
9. What is a UUID?
10. What is the difference between `READ` and `NOTIFY`?
11. Why must a client subscribe before receiving notifications?
12. What is the purpose of `BLE2902`?
13. What happens when `notify()` is called?
14. What is the difference between notify and indicate?
15. Why should advertising restart after disconnection?
16. How can multiple sensor values be exposed through GATT?
17. How can BLE be integrated with FreeRTOS?
18. How does notification interval affect data rate?
19. Why is BLE suitable for battery-powered devices?
20. What is the main difference between BLE and MQTT communication?

---

## 28. Expected Results

Students should successfully create an ESP32 BLE peripheral that advertises a custom service.

The initial sequence should be:

```text
ESP32
   ↓
BLE Advertising
   ↓
Smartphone Discovery
   ↓
Connection
```

The GATT structure should be:

```text
GATT Server
    ↓
Service
    ↓
Characteristic
    ↓
Sensor Value
```

After enabling notifications:

```text
Sensor
   ↓
ESP32
   ↓
Characteristic Update
   ↓
BLE Notification
   ↓
Smartphone
```

Students should observe live sensor values without manually requesting each measurement.

---

## 29. Submission

Submit the following:

- ESP32 BLE source code.
- Screenshot showing the ESP32 in the BLE scan.
- Screenshot of the custom GATT service.
- Screenshot of the sensor characteristic.
- Read-characteristic result.
- Live notification output.
- Sensor data measurements.
- Notification-rate experiment.
- Connection/disconnection test.
- Short discussion comparing BLE and MQTT.

Suggested BLE results table:

| Test | Sensor Value | Notification Interval | Received Successfully |
|---:|---:|---:|---|
| 1 |  | 1000 ms |  |
| 2 |  | 1000 ms |  |
| 3 |  | 500 ms |  |
| 4 |  | 500 ms |  |

Suggested notification-performance table:

| Interval | Messages Sent | Messages Received | Delivery Ratio |
|---:|---:|---:|---:|
| 2000 ms |  |  |  |
| 1000 ms |  |  |  |
| 500 ms |  |  |  |
| 200 ms |  |  |  |

---

## 30. Conclusion

In this lab, the ESP32 is configured as a **Bluetooth Low Energy GATT server** that exposes sensor measurements through custom BLE characteristics. A smartphone acts as the BLE central and GATT client, allowing sensor values to be read directly and monitored continuously through notifications.

The lab demonstrates how BLE provides a lightweight and energy-efficient method for local wireless communication. Unlike MQTT, which normally relies on Wi-Fi and a broker, BLE enables direct communication between the ESP32 and a nearby smartphone. This makes BLE particularly suitable for wearables, portable sensors, smart-home devices, and low-power embedded systems.

