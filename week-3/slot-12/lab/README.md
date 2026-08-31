# LAB 12 — Low-Power IoT Operation Using Deep Sleep and Wake-Up

## 1. Objective

The objective of this lab is to:

1. Understand the importance of **low-power operation** in embedded and IoT systems.
2. Publish sensor data before entering low-power mode.
3. Configure the ESP32 to enter **deep sleep**.
4. Wake the ESP32 using a **timer**.
5. Wake the ESP32 using a **GPIO interrupt source**.
6. Identify the source that caused the ESP32 to wake.
7. Combine sensing, wireless communication, and sleep into a low-power IoT cycle.
8. Compare continuous operation with duty-cycled operation.
9. Estimate duty cycle and average power consumption.
10. Design a simple battery-oriented IoT sensing node.

---

## 2. Background

IoT devices are often deployed in locations where continuous power is unavailable. Many systems therefore operate from:

- Batteries
- Rechargeable cells
- Solar energy
- Energy-harvesting systems

Continuous operation of the processor and wireless interface can consume significant energy.

A common low-power strategy is:

```text
Wake Up
   ↓
Read Sensor
   ↓
Connect to Network
   ↓
Publish Data
   ↓
Disconnect
   ↓
Deep Sleep
   ↓
Wake Up
```

Instead of remaining active continuously, the device spends most of its time in a low-power sleep state.

This approach is known as **duty-cycled operation**.

### Key Concepts

- **Active Mode:** CPU and peripherals are operating normally.
- **Light Sleep:** Some system components are suspended while more state is retained.
- **Deep Sleep:** Most ESP32 processing blocks are powered down to minimize energy consumption.
- **Wake-Up Source:** Event that causes the ESP32 to leave deep sleep.
- **Timer Wake-Up:** Device wakes after a programmed time interval.
- **GPIO Wake-Up:** Device wakes when an external pin changes to a configured state.
- **Duty Cycle:** Fraction of time that the device remains active.
- **RTC Memory:** Memory region that can retain selected information during deep sleep.
- **Telemetry Cycle:** Sequence of sensing, communication, and sleeping.

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Wi-Fi access point
- Push button
- Resistor if required
- Analog sensor or potentiometer
- Optional temperature/humidity sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- MQTT broker and MQTT client application

### Libraries

For Wi-Fi and MQTT communication:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
```

For JSON telemetry:

```cpp
#include <ArduinoJson.h>
```

Deep-sleep functions are provided by the ESP32 platform.

---

## 4. Low-Power IoT Architecture

A low-power IoT node can be represented as:

```text
            ┌───────────────┐
            │   Wake Event  │
            └───────┬───────┘
                    ↓
             ┌─────────────┐
             │    ESP32    │
             │ Active Mode │
             └──────┬──────┘
                    ↓
               Read Sensor
                    ↓
               Create JSON
                    ↓
               Wi-Fi / MQTT
                    ↓
                Publish
                    ↓
               Deep Sleep
                    │
                    └──────────────┐
                                   │
                            Timer / GPIO
                                   │
                                   └────► Wake
```

The goal is to minimize the time spent in active mode.

---

## 5. ESP32 Deep Sleep

The ESP32 enters deep sleep using:

```cpp
esp_deep_sleep_start();
```

Before calling this function, at least one wake-up source should normally be configured.

For example:

```cpp
esp_sleep_enable_timer_wakeup(
    10 * 1000000ULL
);
```

This configures a timer wake-up after 10 seconds.

The value is specified in microseconds.

---

## 6. Experiment 1 — Basic Deep Sleep

The first experiment places the ESP32 into deep sleep.

```cpp
#include <Arduino.h>

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println(
    "ESP32 entering deep sleep"
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

Without a configured wake-up source, recovery may require a reset depending on board behavior.

This experiment demonstrates the transition:

```text
ESP32 Active
     ↓
esp_deep_sleep_start()
     ↓
Deep Sleep
```

---

## 7. Experiment 2 — Timer Wake-Up

Configure the ESP32 to wake automatically after 10 seconds.

```cpp
#include <Arduino.h>

#define SLEEP_TIME_SEC 10

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println(
    "ESP32 awake"
  );

  esp_sleep_enable_timer_wakeup(
    SLEEP_TIME_SEC *
    1000000ULL
  );

  Serial.println(
    "Sleeping for 10 seconds"
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

The operating cycle becomes:

```text
Boot
 ↓
Active
 ↓
Configure Timer
 ↓
Deep Sleep
 ↓
10 Seconds
 ↓
Wake
 ↓
Boot Again
```

---

## 8. Understanding ESP32 Wake Behavior

After waking from deep sleep, the ESP32 generally starts execution again from `setup()`.

Therefore:

```text
Deep Sleep
    ↓
Wake Event
    ↓
Boot Sequence
    ↓
setup()
```

The program does not simply resume from the line after:

```cpp
esp_deep_sleep_start();
```

This is an important difference between deep sleep and ordinary task delays.

---

## 9. Experiment 3 — Detecting Wake-Up Cause

The ESP32 can identify why it woke.

```cpp
esp_sleep_wakeup_cause_t wakeup_reason;

wakeup_reason =
    esp_sleep_get_wakeup_cause();
```

Example:

```cpp
#include <Arduino.h>

void printWakeupReason() {

  esp_sleep_wakeup_cause_t reason =
      esp_sleep_get_wakeup_cause();

  switch (reason) {

    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println(
        "Wake-up caused by timer"
      );
      break;

    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println(
        "Wake-up caused by external GPIO"
      );
      break;

    default:
      Serial.println(
        "Wake-up not caused by deep sleep"
      );
      break;
  }
}

void setup() {

  Serial.begin(115200);

  delay(1000);

  printWakeupReason();

  esp_sleep_enable_timer_wakeup(
    10 * 1000000ULL
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

---

## 10. Experiment 4 — Counting Wake Cycles

Normal RAM contents are lost during deep sleep, but selected RTC memory can be retained.

Declare:

```cpp
RTC_DATA_ATTR int bootCount = 0;
```

Example:

```cpp
#include <Arduino.h>

RTC_DATA_ATTR int bootCount = 0;

void setup() {

  Serial.begin(115200);

  delay(1000);

  bootCount++;

  Serial.print(
    "Wake count: "
  );

  Serial.println(
    bootCount
  );

  esp_sleep_enable_timer_wakeup(
    10 * 1000000ULL
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

Expected output over multiple wake cycles:

```text
Wake count: 1
Wake count: 2
Wake count: 3
Wake count: 4
```

---

## 11. Experiment 5 — Read Sensor Before Sleeping

Connect an analog sensor to:

```cpp
#define SENSOR_PIN 34
```

Example:

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34
#define SLEEP_TIME_SEC 10

void setup() {

  Serial.begin(115200);

  delay(1000);

  int sensorValue =
      analogRead(SENSOR_PIN);

  Serial.print(
    "Sensor = "
  );

  Serial.println(
    sensorValue
  );

  esp_sleep_enable_timer_wakeup(
    SLEEP_TIME_SEC *
    1000000ULL
  );

  Serial.println(
    "Entering deep sleep"
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

The data flow is:

```text
Wake
 ↓
Read Sensor
 ↓
Print Value
 ↓
Deep Sleep
 ↓
Wake
```

---

## 12. Experiment 6 — Publish Sensor Data Before Deep Sleep

The next experiment combines Wi-Fi, MQTT, sensor acquisition, and deep sleep.

The architecture is:

```text
Wake
 ↓
Read Sensor
 ↓
Connect Wi-Fi
 ↓
Connect MQTT
 ↓
Create JSON
 ↓
Publish
 ↓
Deep Sleep
```

Example:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SENSOR_PIN 34
#define SLEEP_TIME_SEC 30

const char* ssid =
    "YOUR_WIFI_NAME";

const char* password =
    "YOUR_WIFI_PASSWORD";

const char* mqtt_server =
    "YOUR_MQTT_BROKER";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectWiFi() {

  WiFi.begin(
    ssid,
    password
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    "Wi-Fi connected"
  );
}

void connectMQTT() {

  mqttClient.setServer(
    mqtt_server,
    1883
  );

  while (
    !mqttClient.connected()
  ) {

    if (
      mqttClient.connect(
        "ESP32-LowPower"
      )
    ) {

      Serial.println(
        "MQTT connected"
      );

    } else {

      delay(1000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  delay(1000);

  int sensorValue =
      analogRead(SENSOR_PIN);

  connectWiFi();

  connectMQTT();

  JsonDocument doc;

  doc["device"] =
      "ESP32-LP01";

  doc["sensor"] =
      sensorValue;

  doc["rssi"] =
      WiFi.RSSI();

  char buffer[256];

  serializeJson(
    doc,
    buffer
  );

  bool result =
      mqttClient.publish(
        "esp32/lowpower/sensor",
        buffer
      );

  Serial.print(
    "Published: "
  );

  Serial.println(
    buffer
  );

  Serial.print(
    "Publish result: "
  );

  Serial.println(
    result
  );

  delay(500);

  mqttClient.disconnect();

  WiFi.disconnect(true);

  esp_sleep_enable_timer_wakeup(
    SLEEP_TIME_SEC *
    1000000ULL
  );

  Serial.println(
    "Entering deep sleep"
  );

  Serial.flush();

  esp_deep_sleep_start();
}

void loop() {}
```

---

## 13. Example JSON Telemetry

The published message may be:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2048,
  "rssi": -55
}
```

Each wake cycle therefore generates one telemetry packet.

```text
Wake 1 → JSON Message 1
Wake 2 → JSON Message 2
Wake 3 → JSON Message 3
```

---

## 14. Experiment 7 — Add Wake Counter to JSON

Use RTC memory:

```cpp
RTC_DATA_ATTR int bootCount = 0;
```

Then:

```cpp
bootCount++;
```

Add the counter to JSON:

```cpp
doc["wake_count"] =
    bootCount;
```

A message may become:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2072,
  "rssi": -52,
  "wake_count": 5
}
```

This helps identify each measurement cycle.

---

## 15. GPIO Wake-Up

An ESP32 can also wake in response to an external signal.

For example:

```text
Push Button
    ↓
GPIO
    ↓
Wake-Up Event
    ↓
ESP32 Active
```

A GPIO wake-up can be useful for:

- Door sensors
- Motion detectors
- Push buttons
- Alarm inputs
- Reed switches
- External digital sensors

---

## 16. Experiment 8 — Wake Using External GPIO

For an ESP32 that supports `EXT0` wake-up on the selected RTC-capable GPIO, configure:

```cpp
esp_sleep_enable_ext0_wakeup(
    GPIO_NUM_33,
    1
);
```

This requests wake-up when GPIO 33 is HIGH.

Example:

```cpp
#include <Arduino.h>

#define WAKE_PIN GPIO_NUM_33

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println(
    "ESP32 awake"
  );

  pinMode(
    33,
    INPUT_PULLDOWN
  );

  esp_sleep_enable_ext0_wakeup(
    WAKE_PIN,
    1
  );

  Serial.println(
    "Press button to wake ESP32"
  );

  delay(1000);

  esp_deep_sleep_start();
}

void loop() {}
```

Connect a push button so that pressing it drives GPIO 33 HIGH.

---

## 17. External Wake-Up Architecture

The operating model is:

```text
Deep Sleep
    ↓
Push Button
    ↓
GPIO 33 = HIGH
    ↓
Wake-Up
    ↓
setup()
```

Unlike timer wake-up, this is **event-driven**.

---

## 18. Experiment 9 — Timer and GPIO Wake-Up Together

Multiple wake-up sources can be configured.

For example:

```cpp
esp_sleep_enable_timer_wakeup(
    60 * 1000000ULL
);

esp_sleep_enable_ext0_wakeup(
    GPIO_NUM_33,
    1
);
```

The device can then wake because:

```text
          Deep Sleep
              │
       ┌──────┴──────┐
       │             │
       ▼             ▼
  Timer Expires   GPIO Event
       │             │
       └──────┬──────┘
              ▼
             Wake
```

This is useful because the device can perform periodic sensing while still responding immediately to external events.

---

## 19. Experiment 10 — Identify Timer vs. GPIO Wake-Up

Example:

```cpp
#include <Arduino.h>

#define WAKE_PIN GPIO_NUM_33

void printWakeReason() {

  esp_sleep_wakeup_cause_t reason =
      esp_sleep_get_wakeup_cause();

  switch (reason) {

    case ESP_SLEEP_WAKEUP_TIMER:

      Serial.println(
        "Wake source: TIMER"
      );

      break;

    case ESP_SLEEP_WAKEUP_EXT0:

      Serial.println(
        "Wake source: GPIO"
      );

      break;

    default:

      Serial.println(
        "Wake source: RESET/OTHER"
      );

      break;
  }
}

void setup() {

  Serial.begin(115200);

  delay(1000);

  printWakeReason();

  esp_sleep_enable_timer_wakeup(
    30 * 1000000ULL
  );

  esp_sleep_enable_ext0_wakeup(
    WAKE_PIN,
    1
  );

  Serial.println(
    "Entering sleep"
  );

  Serial.flush();

  esp_deep_sleep_start();
}

void loop() {}
```

---

## 20. Experiment 11 — Publish Wake Reason

Wake reason can also be transmitted to the MQTT broker.

Example JSON:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2048,
  "wake_reason": "TIMER",
  "wake_count": 8
}
```

or:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2102,
  "wake_reason": "GPIO",
  "wake_count": 9
}
```

This provides contextual information about the measurement.

---

## 21. Complete Low-Power IoT Cycle

The complete architecture is:

```text
                   ┌────────────────────┐
                   │     Deep Sleep     │
                   └─────────┬──────────┘
                             │
                     Timer / GPIO
                             │
                             ▼
                       ESP32 Wakes
                             │
                             ▼
                       Read Sensor
                             │
                             ▼
                       Connect Wi-Fi
                             │
                             ▼
                       Connect MQTT
                             │
                             ▼
                         JSON Data
                             │
                             ▼
                       Publish Data
                             │
                             ▼
                       Disconnect
                             │
                             ▼
                       Deep Sleep
```

This is a typical **low-power wireless sensor-node architecture**.

---

## 22. Duty Cycle

The duty cycle represents the percentage of time the device remains active.

If:

- Active time = $T_a$
- Sleep time = $T_s$

then:

$$
D
= \frac{T_a}
     {T_a + T_s}
\times 100\%.
$$

For example, suppose:

$$
T_a = 2~\text{s}
$$

and:

$$
T_s = 58~\text{s}.
$$

Then:

$$
D
= \frac{2}
     {2+58}
\times100\%
$$

which gives:

$$
D \approx 3.33\%.
$$

Thus, the device is active only a small fraction of the time.

---

## 23. Average Current Estimation

If the active-mode current is $I_a$ and the sleep current is $I_s$, the approximate average current can be estimated as:

$$
I_{\text{avg}}
= D I_a
+
(1-D)I_s,
$$

where $D$ is expressed as a fraction rather than a percentage.

For example, if:

$$
D=0.05,
$$

then:

$$
I_{\text{avg}}
= 0.05I_a
+
0.95I_s.
$$

Reducing the active duty cycle can therefore significantly reduce average current consumption.

---

## 24. Experiment 12 — Duty-Cycle Comparison

Test the following sleep intervals:

| Test | Active Time | Sleep Time | Duty Cycle |
|---:|---:|---:|---:|
| 1 | 2 s | 8 s |  |
| 2 | 2 s | 28 s |  |
| 3 | 2 s | 58 s |  |
| 4 | 2 s | 298 s |  |

Calculate:

$$
D
= \frac{T_a}{T_a+T_s}
\times100\%.
$$

Discuss the relationship between sleep interval and energy consumption.

---

## 25. Continuous Operation vs. Duty-Cycled Operation

| Feature | Continuous Operation | Duty-Cycled Operation |
|---|---|---|
| CPU | Always active | Periodically active |
| Wireless connection | Usually maintained | Reconnected when needed |
| Power consumption | Higher | Lower |
| Response time | Immediate | Depends on wake strategy |
| Battery life | Shorter | Longer |
| Implementation complexity | Lower | Higher |
| Typical use | Mains-powered system | Battery-powered IoT |

A useful principle is:

```text
Always Awake
    ↓
Higher Responsiveness
    ↓
Higher Energy Usage
```

versus:

```text
Wake → Work → Sleep
        ↓
Lower Average Energy Usage
```

---

## 26. Timer Wake-Up vs. GPIO Wake-Up

| Feature | Timer Wake-Up | GPIO Wake-Up |
|---|---|---|
| Trigger | Time interval | External event |
| Operation | Periodic | Event-driven |
| Typical use | Environmental sensing | Alarm/button/motion |
| Requires external signal | No | Yes |
| Suitable for scheduled telemetry | Excellent | Limited |
| Suitable for immediate event response | Limited | Excellent |

A combined system provides both:

```text
Periodic Monitoring
        +
Event-Driven Response
```

---

## 27. Experiments

### Experiment 1 — Basic Deep Sleep

Enter deep sleep and verify that normal program execution stops.

### Experiment 2 — Timer Wake-Up

Configure the ESP32 to wake every:

```text
10 seconds
```

Observe repeated boot cycles.

### Experiment 3 — Wake Counter

Use:

```cpp
RTC_DATA_ATTR
```

to count wake events.

### Experiment 4 — Sensor Measurement

Read a sensor after every timer wake-up.

### Experiment 5 — MQTT Telemetry

Publish the sensor value before entering deep sleep.

### Experiment 6 — JSON Telemetry

Publish:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2048,
  "wake_count": 5
}
```

### Experiment 7 — GPIO Wake-Up

Use a push button to wake the ESP32.

### Experiment 8 — Multiple Wake Sources

Enable both:

```text
Timer
GPIO
```

and determine which one caused the wake-up.

### Experiment 9 — Publish Wake Reason

Include:

```json
{
  "wake_reason": "TIMER"
}
```

or:

```json
{
  "wake_reason": "GPIO"
}
```

in the MQTT telemetry.

### Experiment 10 — Duty-Cycle Analysis

Change the sleep interval and calculate duty cycle.

---

## 28. Exercises

### Exercise 1 — Periodic Low-Power Sensor

Create:

```text
Wake Every 30 Seconds
        ↓
Read Sensor
        ↓
Print Measurement
        ↓
Deep Sleep
```

### Exercise 2 — MQTT Low-Power Sensor Node

Implement:

```text
Timer Wake
    ↓
Sensor Measurement
    ↓
Wi-Fi
    ↓
MQTT
    ↓
JSON Publish
    ↓
Deep Sleep
```

### Exercise 3 — Event-Driven Wake-Up

Use a push button or digital sensor.

```text
GPIO Event
    ↓
ESP32 Wake
    ↓
Read Sensor
    ↓
Publish Event
    ↓
Deep Sleep
```

### Exercise 4 — Dual Wake-Up Sources

Configure:

```text
Timer = 60 seconds
GPIO = Push Button
```

Publish the wake reason to MQTT.

### Exercise 5 — Low-Power JSON Packet

Create:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2050,
  "wake_count": 12,
  "wake_reason": "TIMER",
  "rssi": -54
}
```

Publish it before returning to deep sleep.

### Exercise 6 — Battery-Oriented IoT Node

Design the following complete system:

```text
          ┌────────────────────┐
          │      Deep Sleep    │
          └─────────┬──────────┘
                    │
             Timer or GPIO
                    │
                    ▼
              Sensor Reading
                    │
                    ▼
              Data Processing
                    │
                    ▼
                JSON Data
                    │
                    ▼
              Wi-Fi + MQTT
                    │
                    ▼
               Publish Data
                    │
                    ▼
             Return to Sleep
```

Record:

- Active time
- Sleep time
- Wake source
- Sensor value
- MQTT success
- Estimated duty cycle

---

## 29. Checkpoint Questions

1. Why is low-power operation important for IoT systems?
2. What is ESP32 deep sleep?
3. What happens to normal program execution during deep sleep?
4. What happens after the ESP32 wakes from deep sleep?
5. How is timer wake-up configured?
6. Why is the timer value commonly expressed in microseconds?
7. What is a GPIO wake-up source?
8. What is the difference between timer and GPIO wake-up?
9. What does `esp_sleep_get_wakeup_cause()` provide?
10. What is `RTC_DATA_ATTR` used for?
11. Why is a wake counter useful?
12. Why should sensor data be published before entering deep sleep?
13. Why should the network connection be closed before sleeping?
14. What is duty-cycled operation?
15. How is duty cycle calculated?
16. Why does a lower duty cycle generally reduce power consumption?
17. Why can frequent Wi-Fi reconnection increase active energy consumption?
18. What is the advantage of combining timer and GPIO wake-up?
19. What information can be included in a low-power JSON telemetry packet?
20. How does a low-power IoT node differ from an always-connected IoT node?

---

## 30. Expected Results

Students should successfully implement periodic deep-sleep operation:

```text
Wake
 ↓
Sensor
 ↓
Publish
 ↓
Sleep
```

Timer wake-up should produce:

```text
Deep Sleep
    ↓
Timer Expires
    ↓
ESP32 Wake
```

GPIO wake-up should produce:

```text
Deep Sleep
    ↓
External Event
    ↓
GPIO Trigger
    ↓
ESP32 Wake
```

The complete low-power telemetry process should be:

```text
Timer / GPIO
      ↓
ESP32 Wake
      ↓
Sensor Acquisition
      ↓
JSON Creation
      ↓
MQTT Publish
      ↓
Deep Sleep
```

Students should observe that increasing sleep duration reduces the percentage of time the ESP32 remains active.

---

## 31. Submission

Submit the following:

- ESP32 deep-sleep source code.
- Timer wake-up demonstration.
- GPIO wake-up demonstration.
- Wake-cause output.
- RTC wake-counter output.
- Sensor measurement results.
- MQTT JSON telemetry.
- Combined timer/GPIO wake-up demonstration.
- Duty-cycle calculations.
- Short discussion of power-saving strategies.

Suggested wake-up results table:

| Test | Wake Source | Sensor Value | Wake Count | MQTT Success |
|---:|---|---:|---:|---|
| 1 | Timer |  |  |  |
| 2 | Timer |  |  |  |
| 3 | GPIO |  |  |  |
| 4 | GPIO |  |  |  |

Suggested duty-cycle table:

| Test | Active Time | Sleep Time | Duty Cycle |
|---:|---:|---:|---:|
| 1 |  | 10 s |  |
| 2 |  | 30 s |  |
| 3 |  | 60 s |  |
| 4 |  | 300 s |  |

Suggested telemetry format:

```json
{
  "device": "ESP32-LP01",
  "sensor": 2048,
  "wake_count": 7,
  "wake_reason": "TIMER",
  "rssi": -53
}
```

---

## 32. Conclusion

In this lab, the ESP32 is developed into a **low-power connected sensor node** using deep sleep and configurable wake-up sources. The ESP32 wakes periodically or in response to an external GPIO event, acquires sensor data, connects to the network, publishes a JSON telemetry message through MQTT, and then returns to deep sleep.

This wake-work-sleep architecture significantly reduces the time that the processor and wireless interfaces remain active. Timer-based wake-up is well suited to periodic environmental monitoring, while GPIO wake-up provides event-driven operation for applications such as alarms, buttons, motion detection, and door sensors. Combining both mechanisms provides a flexible architecture for battery-powered IoT systems.


