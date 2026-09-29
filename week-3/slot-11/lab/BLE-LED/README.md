# Lab — Simple ESP32-to-ESP32 BLE Control for 3 LEDs

## Objective

Use two ESP32 boards to control **three LEDs** over **Bluetooth Low Energy (BLE)**.

- **ESP32 #1:** BLE Client / Controller
- **ESP32 #2:** BLE Server / LED Controller

ESP32 #1 sends text commands such as:

```text
LED1_ON
LED1_OFF
LED2_ON
LED2_OFF
LED3_ON
LED3_OFF
```

ESP32 #2 receives the command and controls the corresponding LED.

---

## System Architecture

```text
+----------------------+
|      ESP32 #1        |
|      BLE Client      |
|                      |
| Send LED Commands    |
+----------+-----------+
           |
           | BLE Write
           v
+----------------------+
|      ESP32 #2        |
|      BLE Server      |
|                      |
| Receive Commands     |
+----------+-----------+
           |
     +-----+-----+
     |     |     |
     v     v     v
   LED1  LED2  LED3
```

---

## LED GPIOs

| LED | ESP32 #2 GPIO |
|---|---:|
| LED 1 | GPIO 2 |
| LED 2 | GPIO 12 |
| LED 3 | GPIO 14 |

---

## BLE UUIDs

Both ESP32 boards must use the same UUIDs.

```cpp
#define SERVICE_UUID   "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID   "abcdefab-1234-1234-1234-abcdefabcdef"
```

---

# Part A — ESP32 #2 BLE Server

Upload this code to **ESP32 #2**.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID   "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID   "abcdefab-1234-1234-1234-abcdefabcdef"

const int LED1 = 2;
const int LED2 = 12;
const int LED3 = 14;

class LEDCallbacks :
  public BLECharacteristicCallbacks
{
  void onWrite(
    BLECharacteristic* characteristic
  )
  {
    String command =
      characteristic
        ->getValue()
        .c_str();

    Serial.print("Received: ");
    Serial.println(command);

    if (command == "LED1_ON")
      digitalWrite(LED1, HIGH);

    else if (command == "LED1_OFF")
      digitalWrite(LED1, LOW);

    else if (command == "LED2_ON")
      digitalWrite(LED2, HIGH);

    else if (command == "LED2_OFF")
      digitalWrite(LED2, LOW);

    else if (command == "LED3_ON")
      digitalWrite(LED3, HIGH);

    else if (command == "LED3_OFF")
      digitalWrite(LED3, LOW);

    else if (command == "ALL_ON")
    {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
    }

    else if (command == "ALL_OFF")
    {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    }
  }
};

void setup()
{
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  BLEDevice::init(
    "ESP32-LED-Server"
  );

  BLEServer* server =
    BLEDevice::createServer();

  BLEService* service =
    server->createService(
      SERVICE_UUID
    );

  BLECharacteristic* characteristic =
    service->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE
    );

  characteristic->setCallbacks(
    new LEDCallbacks()
  );

  service->start();

  BLEAdvertising* advertising =
    BLEDevice::getAdvertising();

  advertising->addServiceUUID(
    SERVICE_UUID
  );

  BLEDevice::startAdvertising();

  Serial.println(
    "BLE LED Server started"
  );

  Serial.println(
    "Waiting for Client..."
  );
}

void loop()
{
  delay(1000);
}
```

### Expected Serial Monitor

```text
BLE LED Server started
Waiting for Client...

Received: LED1_ON
Received: LED1_OFF
Received: LED2_ON
Received: LED3_ON
```

---

# Part B — ESP32 #1 BLE Client

Upload this code to **ESP32 #1**.

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>

#define SERVICE_UUID   "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID   "abcdefab-1234-1234-1234-abcdefabcdef"

BLEAdvertisedDevice* targetDevice = nullptr;

BLERemoteCharacteristic*
  remoteCharacteristic = nullptr;

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
        BLEUUID(
          SERVICE_UUID
        )
      )
    )
    {
      Serial.println(
        "BLE LED Server found"
      );

      BLEDevice::getScan()->stop();

      targetDevice =
        new BLEAdvertisedDevice(
          device
        );

      deviceFound = true;
    }
  }
};

void connectToServer()
{
  client =
    BLEDevice::createClient();

  Serial.println(
    "Connecting..."
  );

  if (
    !client->connect(
      targetDevice
    )
  )
  {
    Serial.println(
      "Connection failed"
    );

    return;
  }

  BLERemoteService* service =
    client->getService(
      SERVICE_UUID
    );

  if (service == nullptr)
  {
    Serial.println(
      "Service not found"
    );

    return;
  }

  remoteCharacteristic =
    service->getCharacteristic(
      CHARACTERISTIC_UUID
    );

  if (
    remoteCharacteristic
      == nullptr
  )
  {
    Serial.println(
      "Characteristic not found"
    );

    return;
  }

  connected = true;

  Serial.println(
    "Connected"
  );
}

void sendCommand(
  const char* command
)
{
  if (
    connected
    &&
    remoteCharacteristic
      != nullptr
  )
  {
    remoteCharacteristic->writeValue(
      (uint8_t*)command,
      strlen(command),
      false
    );

    Serial.print(
      "Sent: "
    );

    Serial.println(
      command
    );
  }
}

void setup()
{
  Serial.begin(115200);

  BLEDevice::init(
    "ESP32-LED-Client"
  );

  BLEScan* scan =
    BLEDevice::getScan();

  scan->setAdvertisedDeviceCallbacks(
    new ScanCallbacks()
  );

  scan->setActiveScan(
    true
  );

  Serial.println(
    "Scanning for BLE Server..."
  );

  scan->start(
    10,
    false
  );
}

void loop()
{
  if (
    deviceFound
    &&
    !connected
  )
  {
    deviceFound = false;

    connectToServer();
  }

  if (connected)
  {
    sendCommand("LED1_ON");
    delay(2000);

    sendCommand("LED1_OFF");
    delay(1000);

    sendCommand("LED2_ON");
    delay(2000);

    sendCommand("LED2_OFF");
    delay(1000);

    sendCommand("LED3_ON");
    delay(2000);

    sendCommand("LED3_OFF");
    delay(2000);
  }
}
```

### Expected Serial Monitor

```text
Scanning for BLE Server...
BLE LED Server found
Connecting...
Connected

Sent: LED1_ON
Sent: LED1_OFF
Sent: LED2_ON
Sent: LED2_OFF
Sent: LED3_ON
Sent: LED3_OFF
```

---

## Communication Flow

```text
ESP32 #1
BLE Client
   |
   | Scan
   v
Find ESP32 #2
   |
   | Connect
   v
BLE Characteristic
   |
   | Write Command
   v
ESP32 #2
BLE Server
   |
   +---- LED1_ON  -> LED 1 ON
   +---- LED1_OFF -> LED 1 OFF
   +---- LED2_ON  -> LED 2 ON
   +---- LED2_OFF -> LED 2 OFF
   +---- LED3_ON  -> LED 3 ON
   +---- LED3_OFF -> LED 3 OFF
```

---

# Experiment 1 — Sequential LED Control

1. Upload the server program to ESP32 #2.
2. Open Serial Monitor.
3. Upload the client program to ESP32 #1.
4. Open Serial Monitor.
5. Observe the LEDs.

Expected sequence:

```text
LED 1 ON
LED 1 OFF
LED 2 ON
LED 2 OFF
LED 3 ON
LED 3 OFF
```

---

# Experiment 2 — All LEDs ON/OFF

The server also accepts:

```text
ALL_ON
ALL_OFF
```

The client can send:

```cpp
sendCommand(
  "ALL_ON"
);

delay(3000);

sendCommand(
  "ALL_OFF"
);
```

---

# Experiment 3 — Control LEDs Using Push Buttons

Connect three buttons to ESP32 #1.

| Button | ESP32 #1 GPIO | Function |
|---|---:|---|
| Button 1 | GPIO 18 | Toggle LED 1 |
| Button 2 | GPIO 19 | Toggle LED 2 |
| Button 3 | GPIO 21 | Toggle LED 3 |

Target system:

```text
Button 1
Button 2
Button 3
   |
   v
ESP32 #1
BLE Client
   |
   | BLE Commands
   v
ESP32 #2
BLE Server
   |
   +---- LED 1
   +---- LED 2
   +---- LED 3
```

---

## Checkpoint Questions

1. Which ESP32 is the BLE Server?
2. Which ESP32 is the BLE Client?
3. What is the purpose of the Service UUID?
4. What is the purpose of the Characteristic UUID?
5. What BLE property is used for sending commands?
6. What command turns LED 1 on?
7. What command turns LED 3 off?
8. Which callback receives commands on the server?
9. Which function sends commands from the client?
10. How can push buttons replace the automatic command sequence?

---

## Simple Assignment

Modify ESP32 #1 so that three push buttons control the three LEDs on ESP32 #2.

```text
+----------------------+
|      ESP32 #1        |
|      BLE Client      |
|                      |
| Button 1             |
| Button 2             |
| Button 3             |
+----------+-----------+
           |
           | BLE
           v
+----------------------+
|      ESP32 #2        |
|      BLE Server      |
|                      |
| LED 1                |
| LED 2                |
| LED 3                |
+----------------------+
```
