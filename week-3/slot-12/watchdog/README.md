# Lab — ESP32 Task Watchdog Timer (WDT)

## Objective

In this lab, students will study the **Task Watchdog Timer (TWDT)** on the ESP32.

Students will learn how to:

- configure a watchdog timeout;
- register the current FreeRTOS task with the watchdog;
- periodically reset/feed the watchdog;
- observe what happens when a task becomes stuck;
- understand how a watchdog improves system reliability.

---

## What is a Watchdog?

A **watchdog timer** is a safety mechanism that checks whether the software is still running correctly.

Normal operation:

```text
Program Running
      |
      v
Do Some Work
      |
      v
Reset / Feed Watchdog
      |
      v
Continue Running
```

If the program becomes stuck:

```text
Program Stuck
      |
      v
Watchdog Is Not Reset
      |
      v
Timeout
      |
      v
Watchdog Error / System Recovery
```

---

## ESP32 Watchdog Types

| Watchdog | Purpose |
|---|---|
| Task Watchdog Timer (TWDT) | Detects tasks that do not run or yield correctly |
| Interrupt Watchdog Timer (IWDT) | Detects interrupts being blocked too long |
| RTC Watchdog | Used mainly during startup and low-level system operation |

This lab focuses on the **Task Watchdog Timer**.

---

# Experiment 1 — Normal Watchdog Operation

```cpp
#include <Arduino.h>
#include <esp_task_wdt.h>

const int WDT_TIMEOUT_MS = 5000;

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println(
    "ESP32 Task Watchdog Lab"
  );

  esp_task_wdt_config_t config = {
    .timeout_ms = WDT_TIMEOUT_MS,
    .idle_core_mask = 0,
    .trigger_panic = true
  };

  esp_err_t result =
    esp_task_wdt_init(
      &config
    );

  if (
    result == ESP_ERR_INVALID_STATE
  )
  {
    esp_task_wdt_reconfigure(
      &config
    );
  }

  esp_task_wdt_add(
    NULL
  );

  Serial.println(
    "Watchdog enabled"
  );
}

void loop()
{
  Serial.println(
    "System running"
  );

  esp_task_wdt_reset();

  delay(1000);
}
```

## Expected Serial Monitor

```text
ESP32 Task Watchdog Lab
Watchdog enabled

System running
System running
System running
System running
System running
```

The watchdog timeout is 5 seconds, while the task feeds the watchdog every 1 second.

---

## Important Command

```cpp
esp_task_wdt_reset();
```

This resets the watchdog countdown.

```text
Watchdog Counting
      |
      v
esp_task_wdt_reset()
      |
      v
Countdown Restarts
```

---

# Experiment 2 — Simulate a Stuck Task

Replace `loop()` with:

```cpp
void loop()
{
  Serial.println(
    "Starting task..."
  );

  delay(1000);

  Serial.println(
    "Simulating a stuck task"
  );

  while (true)
  {
    // Intentionally stuck here
  }
}
```

Program behavior:

```text
Task Starts
    |
    v
Infinite Loop
    |
    v
No Watchdog Reset
    |
    | 5 seconds
    v
Watchdog Timeout
    |
    v
Watchdog Error / Recovery
```

The exact watchdog message can vary by ESP32 board and Arduino-ESP32 version.

---

# Experiment 3 — Prevent the Watchdog Timeout

Use:

```cpp
while (true)
{
  Serial.println(
    "Long-running task"
  );

  esp_task_wdt_reset();

  delay(1000);
}
```

The watchdog is periodically fed, so the timeout should not occur.

---

## Why Use a Watchdog?

A watchdog helps detect and recover from:

- infinite loops;
- blocked tasks;
- deadlocks;
- software becoming unresponsive;
- tasks that consume CPU time for too long;
- some communication-related software failures.

Typical IoT concept:

```text
Read Sensor
    |
    v
Process Data
    |
    v
Send Data
    |
    v
Feed Watchdog
    |
    v
Repeat
```

---

# Experiment 4 — Watchdog with a FreeRTOS Task

```cpp
#include <Arduino.h>
#include <esp_task_wdt.h>

const int WDT_TIMEOUT_MS = 5000;

void sensorTask(
  void *parameter
)
{
  esp_task_wdt_add(
    NULL
  );

  while (true)
  {
    int sensorValue =
      random(0, 4096);

    Serial.print(
      "Sensor Value: "
    );

    Serial.println(
      sensorValue
    );

    esp_task_wdt_reset();

    vTaskDelay(
      1000 / portTICK_PERIOD_MS
    );
  }
}

void setup()
{
  Serial.begin(115200);

  delay(1000);

  esp_task_wdt_config_t config = {
    .timeout_ms = WDT_TIMEOUT_MS,
    .idle_core_mask = 0,
    .trigger_panic = true
  };

  esp_err_t result =
    esp_task_wdt_init(
      &config
    );

  if (
    result == ESP_ERR_INVALID_STATE
  )
  {
    esp_task_wdt_reconfigure(
      &config
    );
  }

  xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop()
{
}
```

## Expected Output

```text
Sensor Value: 2350
Sensor Value: 1450
Sensor Value: 3901
Sensor Value: 820
```

---

## Checkpoint Questions

1. What is the purpose of a watchdog timer?
2. What does TWDT stand for?
3. What happens if a registered task does not reset the watchdog?
4. What function resets the Task Watchdog?
5. What is the watchdog timeout in this lab?
6. Why can an infinite loop cause a watchdog timeout?
7. How can `vTaskDelay()` help FreeRTOS tasks cooperate?
8. Why is a watchdog useful in an unattended IoT system?
9. What does `esp_task_wdt_add(NULL)` do?
10. What is the difference between a watchdog timer and a normal application timer?

---

## Simple Assignment

Create a FreeRTOS application with two tasks:

```text
Task 1
Sensor Task
   |
   v
Generate Simulated ADC Value
   |
   v
Feed Watchdog

Task 2
Logger Task
   |
   v
Print System Status
```

Then intentionally modify **Task 1** so that it becomes stuck.

Observe what happens when the watchdog is no longer reset.

---

## Key Takeaway

```text
Watchdog
=
Software Reliability
+
Fault Detection
+
Recovery Mechanism
```

A watchdog is mainly used to detect software failure, not for normal application timing.
