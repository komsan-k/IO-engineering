# Lab — ESP32 FreeRTOS with Deep Sleep

## Objective

In this lab, students will combine **FreeRTOS** and **Deep Sleep** on the ESP32.

The ESP32 will:

1. wake up;
2. create a FreeRTOS task;
3. simulate sensor processing;
4. enter Deep Sleep;
5. wake up after a timer;
6. repeat the cycle.

This demonstrates a common low-power IoT pattern:

```text
Wake Up
   |
   v
Run FreeRTOS Task
   |
   v
Process Data
   |
   v
Enter Deep Sleep
   |
   | Timer Wake-Up
   v
Restart
```

---

## Key Concept

FreeRTOS tasks run only while the ESP32 is awake.

When the ESP32 enters Deep Sleep:

- the CPU stops;
- FreeRTOS tasks stop;
- normal RAM contents are not preserved;
- after wake-up, `setup()` runs again;
- FreeRTOS tasks must be created again.

To preserve small values across Deep Sleep cycles, ESP32 RTC memory can be used.

---

## Required Libraries

```cpp
#include <Arduino.h>
#include <esp_sleep.h>
```

---

## Complete Arduino Code

```cpp
#include <Arduino.h>
#include <esp_sleep.h>

RTC_DATA_ATTR int wakeCount = 0;

TaskHandle_t sensorTaskHandle;

void sensorTask(void *parameter)
{
  Serial.println(
    "Sensor Task started"
  );

  int sensorValue =
    random(200, 3500);

  Serial.print(
    "Sensor Value: "
  );

  Serial.println(
    sensorValue
  );

  Serial.println(
    "Processing data..."
  );

  vTaskDelay(
    2000 / portTICK_PERIOD_MS
  );

  Serial.println(
    "Processing completed"
  );

  esp_sleep_enable_timer_wakeup(
    10 * 1000000ULL
  );

  Serial.println(
    "Entering Deep Sleep for 10 seconds..."
  );

  delay(100);

  esp_deep_sleep_start();
}

void setup()
{
  Serial.begin(115200);

  delay(1000);

  wakeCount++;

  Serial.println();
  Serial.println(
    "ESP32 FreeRTOS + Deep Sleep"
  );

  Serial.print(
    "Wake Count: "
  );

  Serial.println(
    wakeCount
  );

  xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    &sensorTaskHandle
  );
}

void loop()
{
}
```

---

## Program Flow

```text
ESP32 Wake-Up
     |
     v
setup()
     |
     v
wakeCount++
     |
     v
Create FreeRTOS Task
     |
     v
Simulate Sensor Reading
     |
     v
Process Data
     |
     v
Deep Sleep 10 seconds
     |
     v
Timer Wake-Up
     |
     v
setup() runs again
```

---

## Expected Serial Monitor

```text
ESP32 FreeRTOS + Deep Sleep
Wake Count: 1
Sensor Task started
Sensor Value: 1425
Processing data...
Processing completed
Entering Deep Sleep for 10 seconds...

ESP32 FreeRTOS + Deep Sleep
Wake Count: 2
Sensor Task started
Sensor Value: 2860
Processing data...
Processing completed
Entering Deep Sleep for 10 seconds...
```

---

## Important Commands

### Create a FreeRTOS Task

```cpp
xTaskCreate(
  sensorTask,
  "Sensor Task",
  2048,
  NULL,
  1,
  &sensorTaskHandle
);
```

### Delay Inside a FreeRTOS Task

```cpp
vTaskDelay(
  2000 / portTICK_PERIOD_MS
);
```

### Set Timer Wake-Up

```cpp
esp_sleep_enable_timer_wakeup(
  10 * 1000000ULL
);
```

### Enter Deep Sleep

```cpp
esp_deep_sleep_start();
```

### Preserve Data Across Deep Sleep

```cpp
RTC_DATA_ATTR int wakeCount = 0;
```

---

## Experiment 1 — Change the Sleep Time

Change:

```cpp
10 * 1000000ULL
```

to:

```cpp
5 * 1000000ULL
```

The ESP32 will wake every 5 seconds.

---

## Experiment 2 — Change the Simulated Sensor Range

Current simulated sensor:

```cpp
int sensorValue =
  random(200, 3500);
```

Change it to:

```cpp
int sensorValue =
  random(0, 4096);
```

This simulates a 12-bit ADC value.

---

## Experiment 3 — Add a Second FreeRTOS Task

Create another task:

```cpp
void communicationTask(
  void *parameter
)
{
  Serial.println(
    "Communication Task started"
  );

  vTaskDelay(
    1000 / portTICK_PERIOD_MS
  );

  Serial.println(
    "Communication completed"
  );

  vTaskDelete(NULL);
}
```

Create it in `setup()`:

```cpp
xTaskCreate(
  communicationTask,
  "Communication Task",
  2048,
  NULL,
  1,
  NULL
);
```

---

## Important Observation

Deep Sleep stops all FreeRTOS tasks.

This architecture is appropriate when the ESP32 performs short bursts of work:

```text
Wake Up
   |
   +---- Read Sensor
   |
   +---- Process Data
   |
   +---- Send Data
   |
   v
Deep Sleep
```

This pattern is useful for battery-powered IoT devices.

---

## Checkpoint Questions

1. What happens to FreeRTOS tasks during Deep Sleep?
2. What function creates a FreeRTOS task?
3. What function enters Deep Sleep?
4. What function configures timer wake-up?
5. Does the ESP32 continue the previous FreeRTOS task after Deep Sleep?
6. Why does `setup()` run again after wake-up?
7. What is the purpose of `RTC_DATA_ATTR`?
8. Why is Deep Sleep useful for battery-powered IoT systems?
9. What is the simulated sensor range in this lab?
10. How could this lab be extended to use a real sensor?

---

## Simple Assignment

Modify the program so the ESP32 performs:

```text
Wake Up
   |
   v
Task 1: Read Simulated Sensor
   |
   v
Task 2: Print "Sending Data"
   |
   v
Deep Sleep for 15 seconds
   |
   v
Wake Up
   |
   v
Repeat
```

The program should also maintain a wake-up counter using:

```cpp
RTC_DATA_ATTR
```
