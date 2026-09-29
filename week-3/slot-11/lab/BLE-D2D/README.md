# Lab — Simple ESP32-to-ESP32 Communication Using BLE

## Objective

In this lab, two ESP32 boards communicate directly using **Bluetooth Low Energy (BLE)**.

- **ESP32 #1** acts as a **BLE Server**
- **ESP32 #2** acts as a **BLE Client**

The BLE Server sends a counter value, and the BLE Client reads and displays the value on the Serial Monitor.

---

## System Architecture

```text
+----------------------+
|      ESP32 #1        |
|      BLE Server      |
| Counter: 1, 2, 3...  |
+----------+-----------+
           |
           | BLE
           v
+----------------------+
|      ESP32 #2        |
|      BLE Client      |
| Read Counter Value   |
+----------+-----------+
           |
           v
+----------------------+
|    Serial Monitor    |
| Received: 1          |
| Received: 2          |
| Received: 3          |
+----------------------+
```

---

## BLE Roles

| ESP32 | BLE Role | Function |
|---|---|---|
| ESP32 #1 | Server / Peripheral | Advertises and provides data |
| ESP32 #2 | Client / Central | Scans, connects, and reads data |

---

## Required Library

```cpp
#include <BLEDevice.h>
```

---

# Part A — ESP32 #1 BLE Server

Upload the following program to **ESP32 #1**.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID \
  "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID \
  "abcdefab-1234-1234-1234-abcdefabcdef"

BLECharacteristic* pCharacteristic;
int counter = 0;

void setup()
{
  Serial.begin(115200);

  BLEDevice::init("ESP32-BLE-Server");

  BLEServer* pServer =
    BLEDevice::createServer();

  BLEService* pService =
    pServer->createService(
      SERVICE_UUID
    );

  pCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ
    );

  pCharacteristic->setValue("0");

  pService->start();

  BLEAdvertising* pAdvertising =
    BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(
    SERVICE_UUID
  );

  BLEDevice::startAdvertising();

  Serial.println("BLE Server started");
  Serial.println("Waiting for BLE Client...");
}

void loop()
{
  counter++;

  String value = String(counter);

  pCharacteristic->setValue(
    value.c_str()
  );

  Serial.print("Counter = ");
  Serial.println(value);

  delay(2000);
}
```

---

## Expected Serial Monitor — ESP32 #1

```text
BLE Server started
Waiting for BLE Client...

Counter = 1
Counter = 2
Counter = 3
Counter = 4
```

---

# Part B — ESP32 #2 BLE Client

Upload the following program to **ESP32 #2**.

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>

#define SERVICE_UUID \
  "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID \
  "abcdefab-1234-1234-1234-abcdefabcdef"

BLEAdvertisedDevice* targetDevice = nullptr;
BLERemoteCharacteristic* remoteCharacteristic = nullptr;
BLEClient* client = nullptr;

bool deviceFound = false;
bool connected = false;

class ScanCallbacks :
  public BLEAdvertisedDeviceCallbacks
{
  void onResult(
    BLEAdvertisedDevice device
  )
  {
    if (
      device.haveServiceUUID()
      &&
      device.isAdvertisingService(
        BLEUUID(SERVICE_UUID)
      )
    )
    {
      Serial.println("BLE Server found");

      BLEDevice::getScan()->stop();

      targetDevice =
        new BLEAdvertisedDevice(device);

      deviceFound = true;
    }
  }
};

void connectToServer()
{
  client = BLEDevice::createClient();

  Serial.println("Connecting...");

  if (!client->connect(targetDevice))
  {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connected");

  BLERemoteService* service =
    client->getService(
      SERVICE_UUID
    );

  if (service == nullptr)
  {
    Serial.println("Service not found");
    client->disconnect();
    return;
  }

  remoteCharacteristic =
    service->getCharacteristic(
      CHARACTERISTIC_UUID
    );

  if (remoteCharacteristic == nullptr)
  {
    Serial.println("Characteristic not found");
    client->disconnect();
    return;
  }

  connected = true;
}

void setup()
{
  Serial.begin(115200);

  BLEDevice::init("ESP32-BLE-Client");

  BLEScan* scan =
    BLEDevice::getScan();

  scan->setAdvertisedDeviceCallbacks(
    new ScanCallbacks()
  );

  scan->setActiveScan(true);

  Serial.println("Scanning for BLE Server...");

  scan->start(10, false);
}

void loop()
{
  if (deviceFound && !connected)
  {
    deviceFound = false;
    connectToServer();
  }

  if (
    connected
    &&
    remoteCharacteristic != nullptr
  )
  {
    std::string value =
      remoteCharacteristic->readValue();

    Serial.print("Received: ");
    Serial.println(value.c_str());
  }

  delay(2000);
}
```

---

## Expected Serial Monitor — ESP32 #2

```text
Scanning for BLE Server...

BLE Server found
Connecting...
Connected

Received: 3
Received: 4
Received: 5
Received: 6
```

---

## BLE Communication Flow

```text
ESP32 #1
BLE Server
   |
   | Advertise
   v
ESP32 #2
BLE Client
   |
   | Scan
   v
Find Server
   |
   | Connect
   v
BLE Service
   |
   v
BLE Characteristic
   |
   | Read every 2 seconds
   v
Serial Monitor
```

---

## Important BLE Concepts

### BLE Service

A **service** groups related BLE data.

```text
SERVICE_UUID
```

### BLE Characteristic

A **characteristic** contains the actual data.

```text
CHARACTERISTIC_UUID
```

### BLE Server

```text
Advertises
   |
   v
Provides Service
   |
   v
Provides Characteristic
```

### BLE Client

```text
Scans
   |
   v
Finds Server
   |
   v
Connects
   |
   v
Reads Characteristic
```

---

# Experiment 1 — Counter Communication

1. Upload the BLE Server program to ESP32 #1.
2. Open its Serial Monitor.
3. Upload the BLE Client program to ESP32 #2.
4. Open its Serial Monitor.
5. Observe the counter values received by ESP32 #2.

---

# Experiment 2 — Change the Data

Modify the server:

```cpp
String value = "Hello ESP32";
```

Expected client output:

```text
Received: Hello ESP32
```

---

# Experiment 3 — Send Simulated Sensor Data

Replace the counter with a random temperature:

```cpp
float temperature =
  random(250, 351) / 10.0;

String value =
  String(temperature, 1);
```

Example client output:

```text
Received: 28.7
```

---

## Checkpoint Questions

1. What is the role of the BLE Server?
2. What is the role of the BLE Client?
3. What is BLE advertising?
4. What is a BLE Service?
5. What is a BLE Characteristic?
6. Why must both ESP32 boards use the same Service UUID?
7. Why must both boards use the same Characteristic UUID?
8. What function reads the characteristic on the client?
9. How often does the client read the value in this example?
10. How could this lab be extended to send real sensor data?

---

## Simple Assignment

Replace the counter with an LDR sensor value.

```text
+-------------+
|     LDR     |
+------+------+
       |
       v
+-------------+
|   ESP32 #1  |
| BLE Server  |
+------+------+
       |
       | BLE
       v
+-------------+
|   ESP32 #2  |
| BLE Client  |
+------+------+
       |
       v
+-------------+
|   Serial    |
|   Monitor   |
+-------------+
```

ESP32 #1 should read:

```cpp
analogRead(LDR_PIN);
```

and ESP32 #2 should display the received light value.
