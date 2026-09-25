# ESP32 FreeRTOS LDR Smart Lighting --- Two States with Logger Task

## Objective

Create a simple ESP32 FreeRTOS smart-lighting system using an LDR and
LED.

The system uses three tasks:

1.  **Sensor Task** --- reads the LDR.
2.  **Control Task** --- determines DARK or BRIGHT and controls the LED.
3.  **Logger Task** --- displays system status on the Serial Monitor.

## Hardware Configuration

  Device     ESP32 GPIO
  -------- ------------
  LDR           GPIO 36
  LED           GPIO 12

``` cpp
#define LIGHT_THRESHOLD ???
```

-   `LDR <  ???` → **DARK** → LED ON
-   `LDR >=  ???` → **BRIGHT** → LED OFF

## Program Architecture

``` text
LDR (GPIO 36)
     |
     v
[ Sensor Task ]
     |
     | LDR value
     v
[ sensorQueue ]
     |
     v
[ Control Task ]
     |
     +---- DARK ----> LED ON
     |
     +---- BRIGHT --> LED OFF
     |
     | LightingData
     v
[ logQueue ]
     |
     v
[ Logger Task ]
     |
     v
Serial Monitor
```

## Complete Arduino Code

``` cpp
#include <Arduino.h>

#define LDR_PIN 36
#define LED_PIN 12
#define LIGHT_THRESHOLD ???

struct LightingData
{
  int ldrValue;
  bool isDark;
  bool ledState;
};

QueueHandle_t sensorQueue;
QueueHandle_t logQueue;

void sensorTask(void *parameter)
{
  int ldrValue;

  while (1)
  {
    ldrValue = analogRead(LDR_PIN);

    xQueueSend(
      sensorQueue,
      &ldrValue,
      portMAX_DELAY
    );

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void controlTask(void *parameter)
{
  int ldrValue;
  LightingData data;

  while (1)
  {
    if (xQueueReceive(
          sensorQueue,
          &ldrValue,
          portMAX_DELAY) == pdPASS)
    {
      data.ldrValue = ldrValue;

      if (ldrValue < LIGHT_THRESHOLD)
      {
        data.isDark = true;
        data.ledState = true;
        digitalWrite(LED_PIN, HIGH);
      }
      else
      {
        data.isDark = false;
        data.ledState = false;
        digitalWrite(LED_PIN, LOW);
      }

      xQueueSend(
        logQueue,
        &data,
        portMAX_DELAY
      );
    }
  }
}

void loggerTask(void *parameter)
{
  LightingData data;

  while (1)
  {
    if (xQueueReceive(
          logQueue,
          &data,
          portMAX_DELAY) == pdPASS)
    {
      Serial.print("LDR = ");
      Serial.print(data.ldrValue);

      if (data.isDark)
      {
        Serial.println(" -> DARK -> LED ON");
      }
      else
      {
        Serial.println(" -> BRIGHT -> LED OFF");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  sensorQueue = xQueueCreate(5, sizeof(int));
  logQueue = xQueueCreate(5, sizeof(LightingData));

  if (sensorQueue == NULL || logQueue == NULL)
  {
    Serial.println("Queue creation failed!");

    while (1)
    {
      delay(1000);
    }
  }

  xTaskCreate(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    controlTask,
    "Control Task",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    loggerTask,
    "Logger Task",
    2048,
    NULL,
    1,
    NULL
  );

  Serial.println("FreeRTOS LDR Smart Lighting Started");
}

void loop()
{
  vTaskDelay(portMAX_DELAY);
}
```

## Queue Communication

  Queue           Producer       Consumer       Data
  --------------- -------------- -------------- ----------------
  `sensorQueue`   Sensor Task    Control Task   LDR value
  `logQueue`      Control Task   Logger Task    `LightingData`

## Example Output

``` text
FreeRTOS LDR Smart Lighting Started
LDR = 850 -> DARK -> LED ON
LDR = 920 -> DARK -> LED ON
LDR = 2450 -> BRIGHT -> LED OFF
LDR = 3100 -> BRIGHT -> LED OFF
```

## Key Learning Points

-   `xTaskCreate()` creates FreeRTOS tasks.
-   `xQueueCreate()` creates communication queues.
-   `xQueueSend()` sends data between tasks.
-   `xQueueReceive()` receives data from a queue.
-   `pdMS_TO_TICKS(1000)` converts 1000 ms to FreeRTOS ticks.
-   `portMAX_DELAY` allows a task to wait until queue data is available.
-   The application separates sensing, control, and logging.

``` text
SENSE -> QUEUE -> CONTROL -> QUEUE -> LOG
```
