# Lab — Simple ESP32 BLE Control for 3 LEDs

## Objective

Create a simple ESP32 BLE application that controls **three LEDs** by receiving text commands over Bluetooth Low Energy (BLE).

The ESP32 acts as a **BLE Server**.

A BLE client, such as another ESP32 or a BLE mobile app, can send commands to control the LEDs.

---

## System Concept

```text
BLE Client
   |
   | BLE Write Command
   v
+----------------------+
|        ESP32         |
|      BLE Server      |
|                      |
| Receive Command      |
+----------+-----------+
           |
     +-----+-----+
     |     |     |
     v     v     v
   LED1  LED2  LED3
```

---

## LED GPIOs

| LED | GPIO |
|---|---:|
| LED 1 | GPIO 2 |
| LED 2 | GPIO 12 |
| LED 3 | GPIO 13 |

---

## BLE Commands

The ESP32 accepts these commands:

```text
LED1_ON
LED1_OFF
LED2_ON
LED2_OFF
LED3_ON
LED3_OFF
ALL_ON
ALL_OFF
```

---

## Arduino Code

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID   "12345678-1234-1234-1234-1234567890ab"

#define CHARACTERISTIC_UUID   "abcdefab-1234-1234-1234-abcdefabcdef"

const int LED1 = 2;
const int LED2 = 12;
const int LED3 = 13;

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
    {
      digitalWrite(LED1, HIGH);
    }
    else if (command == "LED1_OFF")
    {
      digitalWrite(LED1, LOW);
    }
    else if (command == "LED2_ON")
    {
      digitalWrite(LED2, HIGH);
    }
    else if (command == "LED2_OFF")
    {
      digitalWrite(LED2, LOW);
    }
    else if (command == "LED3_ON")
    {
      digitalWrite(LED3, HIGH);
    }
    else if (command == "LED3_OFF")
    {
      digitalWrite(LED3, LOW);
    }
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
    "ESP32-3LED"
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
    "ESP32 BLE Server started"
  );

  Serial.println(
    "Waiting for BLE commands..."
  );
}

void loop()
{
  delay(1000);
}
```

---

## Expected Serial Monitor

```text
ESP32 BLE Server started
Waiting for BLE commands...

Received: LED1_ON
Received: LED1_OFF
Received: LED2_ON
Received: LED3_ON
Received: ALL_OFF
```

---

## Communication Flow

```text
BLE Client
   |
   | LED1_ON
   v
ESP32 BLE Server
   |
   v
LED 1 ON
```

Another example:

```text
BLE Client
   |
   | ALL_ON
   v
ESP32 BLE Server
   |
   +---- LED 1 ON
   +---- LED 2 ON
   +---- LED 3 ON
```

---

## Important BLE Commands

### Create BLE Device

```cpp
BLEDevice::init(
  "ESP32-3LED"
);
```

### Create BLE Server

```cpp
BLEServer* server =
  BLEDevice::createServer();
```

### Create BLE Service

```cpp
BLEService* service =
  server->createService(
    SERVICE_UUID
  );
```

### Create Writable Characteristic

```cpp
BLECharacteristic* characteristic =
  service->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
```

### Receive Data

```cpp
void onWrite(
  BLECharacteristic* characteristic
)
```

This callback executes whenever the BLE client writes a new command.

---

## Simple Test

Send the following commands one by one:

```text
LED1_ON
LED1_OFF
LED2_ON
LED2_OFF
LED3_ON
LED3_OFF
ALL_ON
ALL_OFF
```

Observe the LED states.

---

## Checkpoint Questions

1. What is the role of the ESP32 in this example?
2. What BLE property is required for receiving control commands?
3. What does `onWrite()` do?
4. What command turns LED 1 on?
5. What command turns all LEDs off?
6. Why must the BLE client use the correct Service UUID?
7. Why must the BLE client use the correct Characteristic UUID?

---

## Simple Assignment

Modify the program to add a new command:

```text
LED_SEQUENCE
```

The expected behavior is:

```text
LED 1 ON
   |
   v
LED 2 ON
   |
   v
LED 3 ON
   |
   v
ALL OFF
```
