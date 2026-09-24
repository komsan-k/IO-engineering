# ESP32 FreeRTOS --- Two LED Blinking Tasks

## Objective

Create two independent **FreeRTOS tasks** on an ESP32 to blink two LEDs
at different rates. Both tasks use the same task function, while the
**LED pin** and **blinking interval** are passed as parameters during
task creation.

## Hardware Configuration

  Task          GPIO   Blink Interval
  ----------- ------ ----------------
  LED1 Task        2           500 ms
  LED2 Task       12          1000 ms

## Complete Arduino Code

``` cpp
#include <Arduino.h>

struct LEDParams {
  int pin;
  uint32_t interval;
};

LEDParams led1 = {2, 500};
LEDParams led2 = {12, 1000};

void blinkTask(void *parameter) {
  LEDParams *led = (LEDParams *)parameter;
  pinMode(led->pin, OUTPUT);

  while (1) {
    digitalWrite(led->pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(led->interval));

    digitalWrite(led->pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(led->interval));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 FreeRTOS - Two LED Blinking Tasks");

  xTaskCreate(
    blinkTask,
    "LED1 Task",
    2048,
    &led1,
    1,
    NULL
  );

  xTaskCreate(
    blinkTask,
    "LED2 Task",
    2048,
    &led2,
    1,
    NULL
  );
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Short Description

The program creates two FreeRTOS tasks using the same `blinkTask()`
function.

The task parameters are:

``` cpp
LEDParams led1 = {2, 500};
LEDParams led2 = {12, 1000};
```

They are passed through the fourth argument of `xTaskCreate()`:

``` cpp
xTaskCreate(blinkTask, "LED1 Task", 2048, &led1, 1, NULL);
xTaskCreate(blinkTask, "LED2 Task", 2048, &led2, 1, NULL);
```

Inside the task, the parameter is recovered using:

``` cpp
LEDParams *led = (LEDParams *)parameter;
```

`vTaskDelay()` suspends only the current task, allowing both LEDs to
blink independently.

## Expected Operation

``` text
ESP32 FreeRTOS
      |
      +-------------------+
      |                   |
      v                   v
  LED1 Task           LED2 Task
  GPIO 2              GPIO 12
  500 ms              1000 ms
      |                   |
      v                   v
    LED 1               LED 2
```

This example demonstrates **FreeRTOS task creation, parameter passing,
concurrent execution, and task delays** on the ESP32.
