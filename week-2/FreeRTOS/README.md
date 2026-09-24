# Practical Series: ESP32 FreeRTOS Programming with an LDR Sensor

## Overview

This 5-lab series introduces **concurrent embedded programming on the ESP32 using Arduino and FreeRTOS**, with an **LDR (Light Dependent Resistor)** as the common sensor theme. The series progresses from a basic FreeRTOS task to multiple tasks, shared-data race conditions, mutex synchronization, and a producer-consumer sensor-control system.

All example programs are designed for the **Arduino IDE with an ESP32 board**.

### Suggested Hardware

- ESP32 development board
- LDR sensor
- 10 kΩ resistor for the LDR voltage divider
- LED
- 220–330 Ω LED resistor
- Breadboard and jumper wires

### Pin Configuration

```cpp
const int LDR_PIN = 36;   // ADC input
const int LED_PIN = 2;    // LED output
```

> **Note:** GPIO 34 is input-only and is suitable for the LDR ADC input. If your ESP32 board uses different available pins, modify the definitions accordingly.

### Learning Path

```text
Lab 1: Basic FreeRTOS Task
        ↓
Lab 2: Multiple Tasks
        ↓
Lab 3: Shared Data and Race Condition
        ↓
Lab 4: Mutex Synchronization
        ↓
Lab 5: Producer–Consumer with FreeRTOS Queue
```

### Target Architecture

```text
              LDR Sensor
                  │
                  ▼
          ┌───────────────┐
          │  Sensor Task  │
          │   Producer    │
          └───────┬───────┘
                  │
            LDR measurement
                  │
                  ▼
          Shared Data / Queue
                  │
                  ▼
          ┌───────────────┐
          │ Control Task  │
          │   Consumer    │
          └───────┬───────┘
                  │
               Decision
                  │
                  ▼
                 LED
```

---

# Lab 1 — Basic FreeRTOS Task: LDR Sensor

## Objective

Understand how to create and execute a **FreeRTOS task** on the ESP32 using the Arduino framework.

Unlike the original PC-based pthread example, the ESP32 can read the LDR directly through its ADC.

## Example Program

```cpp
#include <Arduino.h>

const int LDR_PIN = 36;

void ldrTask(void *parameter)
{
    for (int i = 0; i < 10; i++)
    {
        int ldrRaw = analogRead(LDR_PIN);

        Serial.print("LDR RAW = ");
        Serial.println(ldrRaw);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    Serial.println("LDR task completed.");

    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);

    xTaskCreate(
        ldrTask,          // Task function
        "LDR Task",       // Task name
        2048,             // Stack size
        NULL,             // Parameter
        1,                // Priority
        NULL              // Task handle
    );
}

void loop()
{
    // FreeRTOS task performs the work.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Upload and Run

1. Connect the ESP32 to the computer.
2. Open the program in the Arduino IDE.
3. Select the correct ESP32 board and serial port.
4. Upload the program.
5. Open **Serial Monitor** at **115200 baud**.

Example output:

```text
LDR RAW = 3250
LDR RAW = 2780
LDR RAW = 1420
LDR RAW = 850
...
LDR task completed.
```

## Assignment

Modify the program so that the LDR task reports both the raw ADC value and normalized value:

$$
LDR_{\mathrm{norm}} =
\frac{LDR_{\mathrm{raw}}}{4095}
$$

Example:

```cpp
float normalized = ldrRaw / 4095.0;

Serial.print("RAW = ");
Serial.print(ldrRaw);
Serial.print(" | Normalized = ");
Serial.println(normalized, 3);
```

---

# Lab 2 — Multiple FreeRTOS Tasks: LDR + LED

## Objective

Understand concurrent execution using multiple FreeRTOS tasks on the ESP32.

Create two tasks:

```text
Task 1 → Read LDR
Task 2 → Control LED
```

## Example Program

```cpp
#include <Arduino.h>

const int LDR_PIN = 34;
const int LED_PIN = 2;

volatile int ldrValue = 0;

void ldrTask(void *parameter)
{
    while (true)
    {
        ldrValue = analogRead(LDR_PIN);

        Serial.print("LDR: ");
        Serial.println(ldrValue);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ledTask(void *parameter)
{
    while (true)
    {
        if (ldrValue < 1500)
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED: ON");
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED: OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(LDR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    xTaskCreate(
        ldrTask,
        "LDR Task",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        ledTask,
        "LED Task",
        2048,
        NULL,
        1,
        NULL
    );
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Control Logic

$$
LED =
\begin{cases}
ON, & LDR < LDR_{\mathrm{threshold}} \\
OFF, & LDR \geq LDR_{\mathrm{threshold}}
\end{cases}
$$

## Assignment

Add three illumination levels:

```text
LDR < 1000          → DARK
1000 ≤ LDR < 3000   → NORMAL
LDR ≥ 3000          → BRIGHT
```

Display both the sensor value and environmental state.

---

# Lab 3 — Shared Data and Race Conditions

## Objective

Understand problems caused by **unsynchronized shared data** between ESP32 FreeRTOS tasks.

Instead of sharing only one variable, define:

```cpp
typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
} LDRData;
```

Both tasks access the same structure:

```text
                 Shared LDRData
                ┌──────────────┐
Sensor Task  →  │ raw          │  ← Control Task
                │ normalized   │
                │ sampleNumber │
                └──────────────┘
```

## Example Program

```cpp
#include <Arduino.h>

const int LDR_PIN = 34;

typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
} LDRData;

LDRData data = {0, 0.0, 0};

void sensorTask(void *parameter)
{
    while (true)
    {
        // Intentionally not protected by a mutex.
        data.raw = analogRead(LDR_PIN);

        // Small delays make inconsistent reads easier to observe.
        vTaskDelay(pdMS_TO_TICKS(1));

        data.normalized = data.raw / 4095.0;

        vTaskDelay(pdMS_TO_TICKS(1));

        data.sampleNumber++;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void controlTask(void *parameter)
{
    while (true)
    {
        // The task may read while sensorTask is updating data.
        Serial.print("Sample ");
        Serial.print(data.sampleNumber);
        Serial.print(": RAW=");
        Serial.print(data.raw);
        Serial.print(", Normalized=");
        Serial.println(data.normalized, 3);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);

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
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

At this stage, **do not use a mutex**.

The consumer may access the structure while the producer is updating it:

```text
Sensor Task                  Control Task

Write raw
    ↓
Write normalized  ←--------- Read raw
    ↓
Write sampleNumber ---------- Read normalized
```

This can produce inconsistent measurements and demonstrates a **race condition**.

## Assignment

1. Increase the sampling rate.
2. Observe the Serial Monitor output.
3. Explain the term **race condition**.
4. Identify the program's **critical section**.
5. Explain why shared sensor data should be protected.

---

# Lab 4 — LDR Shared Data with FreeRTOS Mutex

## Objective

Protect shared LDR sensor data using a **FreeRTOS mutex**.

A mutex on the ESP32 can be created using:

```cpp
SemaphoreHandle_t ldrMutex;

ldrMutex = xSemaphoreCreateMutex();
```

The shared data should only be accessed after the task successfully takes the mutex.

## Complete Example Program

```cpp
#include <Arduino.h>

const int LDR_PIN = 34;

typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
    String ldrState;
} LDRData;

LDRData data = {0, 0.0, 0, "UNKNOWN"};

SemaphoreHandle_t ldrMutex;

void sensorTask(void *parameter)
{
    while (true)
    {
        int raw = analogRead(LDR_PIN);
        float normalized = raw / 4095.0;

        String state;

        if (raw < 1000)
            state = "DARK";
        else if (raw < 3000)
            state = "NORMAL";
        else
            state = "BRIGHT";

        if (xSemaphoreTake(ldrMutex, portMAX_DELAY) == pdTRUE)
        {
            data.raw = raw;
            data.normalized = normalized;
            data.sampleNumber++;
            data.ldrState = state;

            xSemaphoreGive(ldrMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void controlTask(void *parameter)
{
    while (true)
    {
        int raw;
        float normalized;
        int sample;
        String state;

        if (xSemaphoreTake(ldrMutex, portMAX_DELAY) == pdTRUE)
        {
            raw = data.raw;
            normalized = data.normalized;
            sample = data.sampleNumber;
            state = data.ldrState;

            xSemaphoreGive(ldrMutex);
        }

        Serial.print("Sample ");
        Serial.print(sample);
        Serial.print(": RAW=");
        Serial.print(raw);
        Serial.print(", Normalized=");
        Serial.print(normalized, 3);
        Serial.print(", State=");
        Serial.println(state);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);

    ldrMutex = xSemaphoreCreateMutex();

    if (ldrMutex == NULL)
    {
        Serial.println("Failed to create mutex.");
        while (true) {
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
        3072,
        NULL,
        1,
        NULL
    );
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Architecture

```text
Sensor Task
    │
    ▼
   TAKE
    │
    ▼
┌──────────────┐
│ Shared LDR   │
│    Data      │
└──────────────┘
    ▲
    │
   TAKE
    ▲
    │
Control Task

 FreeRTOS Mutex
```

## Assignment

Implement a complete thread-safe LDR monitoring program.

Use an `ldrState` value representing:

```text
DARK
NORMAL
BRIGHT
```

Compare the behavior and architecture:

- Without mutex
- With mutex

Explain how the mutex protects the **critical section**.

---

# Lab 5 — Producer–Consumer LDR Monitoring System

## Objective

Build a realistic concurrent embedded sensor-control architecture using an **ESP32 FreeRTOS Queue**.

Instead of sharing a single measurement, the Sensor Task creates measurements and sends them to a queue. The Control Task receives measurements from the queue and controls the LED.

```text
                    ┌─────────────┐
LDR Sensor ────────►│ Sensor Task │
                    │  Producer   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │FreeRTOS Queue│
                    │ [ ][ ][ ][ ]│
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Control Task │
                    │  Consumer   │
                    └──────┬──────┘
                           │
                           ▼
                          LED
```

## Sensor Data Structure

```cpp
typedef struct {
    int sample;
    int raw;
    float normalized;
} LDRData;
```

## Create the Queue

```cpp
QueueHandle_t ldrQueue;

ldrQueue = xQueueCreate(10, sizeof(LDRData));
```

The FreeRTOS queue already provides safe synchronization between producer and consumer tasks, so a separate mutex is not required for the queue itself.

## Producer Operation

```text
Read LDR
   ↓
Normalize
   ↓
Create LDRData
   ↓
Send to Queue
```

Example:

```cpp
LDRData measurement;

measurement.sample = sampleNumber++;
measurement.raw = analogRead(LDR_PIN);
measurement.normalized = measurement.raw / 4095.0;

xQueueSend(ldrQueue, &measurement, portMAX_DELAY);
```

## Consumer Operation

```text
Receive from Queue
       ↓
Analyze illumination
       ↓
Control LED
```

Example:

```cpp
LDRData measurement;

if (xQueueReceive(ldrQueue, &measurement, portMAX_DELAY) == pdTRUE)
{
    // Process measurement.
}
```

---

# Final Lab 5 Assignment — ESP32 LDR-Based Smart Lighting System

Develop an **ESP32 LDR-Based Smart Lighting System** consisting of three FreeRTOS tasks:

```text
              ┌──────────────┐
              │ Sensor Task  │
              │   Read LDR   │
              └──────┬───────┘
                     │
                     ▼
                   Queue
                     │
              ┌──────▼───────┐
              │ Control Task │
              │   Decision   │
              └──────┬───────┘
                     │
                     ▼
                    LED

              ┌──────────────┐
              │ Logger Task  │
              │ Display Data │
              └──────────────┘
```

## Required Behavior

```text
LDR < 1000
→ DARK
→ LED ON

1000 ≤ LDR < 3000
→ NORMAL
→ LED DIM

LDR ≥ 3000
→ BRIGHT
→ LED OFF
```

## Complete Arduino ESP32 Example

```cpp
#include <Arduino.h>

const int LDR_PIN = 34;
const int LED_PIN = 2;

// PWM settings
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;

typedef struct {
    int sample;
    int raw;
    float normalized;
} LDRData;

typedef struct {
    LDRData sensor;
    int ledDuty;
    char state[10];
} LogData;

QueueHandle_t sensorQueue;
QueueHandle_t logQueue;

void sensorTask(void *parameter)
{
    int sampleNumber = 1;

    while (true)
    {
        LDRData measurement;

        measurement.sample = sampleNumber++;
        measurement.raw = analogRead(LDR_PIN);
        measurement.normalized = measurement.raw / 4095.0;

        xQueueSend(sensorQueue, &measurement, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void controlTask(void *parameter)
{
    LDRData measurement;

    while (true)
    {
        if (xQueueReceive(sensorQueue, &measurement, portMAX_DELAY) == pdTRUE)
        {
            LogData logData;
            logData.sensor = measurement;

            if (measurement.raw < 1000)
            {
                strcpy(logData.state, "DARK");
                logData.ledDuty = 255;
            }
            else if (measurement.raw < 3000)
            {
                strcpy(logData.state, "NORMAL");
                logData.ledDuty = 128;
            }
            else
            {
                strcpy(logData.state, "BRIGHT");
                logData.ledDuty = 0;
            }

            // Arduino-ESP32 LEDC API
            ledcWrite(LED_PIN, logData.ledDuty);

            xQueueSend(logQueue, &logData, portMAX_DELAY);
        }
    }
}

void loggerTask(void *parameter)
{
    LogData logData;

    while (true)
    {
        if (xQueueReceive(logQueue, &logData, portMAX_DELAY) == pdTRUE)
        {
            Serial.printf(
                "[%03d] LDR=%d  Norm=%.2f  State=%-6s  PWM=%d\n",
                logData.sensor.sample,
                logData.sensor.raw,
                logData.sensor.normalized,
                logData.state,
                logData.ledDuty
            );
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(LDR_PIN, INPUT);

    // Configure LED PWM using the current Arduino-ESP32 LEDC API.
    if (!ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION))
    {
        Serial.println("Failed to configure LED PWM.");
        while (true) {
            delay(1000);
        }
    }

    sensorQueue = xQueueCreate(10, sizeof(LDRData));
    logQueue = xQueueCreate(10, sizeof(LogData));

    if (sensorQueue == NULL || logQueue == NULL)
    {
        Serial.println("Failed to create queue.");
        while (true) {
            delay(1000);
        }
    }

    xTaskCreate(
        sensorTask,
        "Sensor Task",
        2048,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        controlTask,
        "Control Task",
        3072,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        loggerTask,
        "Logger Task",
        3072,
        NULL,
        1,
        NULL
    );
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## Example Serial Output

```text
[001] LDR=823   Norm=0.20  State=DARK    PWM=255
[002] LDR=1562  Norm=0.38  State=NORMAL  PWM=128
[003] LDR=3468  Norm=0.85  State=BRIGHT  PWM=0
```

> Depending on how the LDR voltage divider is wired, the ADC value may increase or decrease with light intensity. If your readings behave in the opposite direction, reverse the threshold logic.

---

# ESP32 FreeRTOS Concepts Used in This Lab Series

| Concept | Arduino ESP32 / FreeRTOS |
|---|---|
| Create a concurrent task | `xTaskCreate()` |
| Delete current task | `vTaskDelete(NULL)` |
| Task delay | `vTaskDelay()` |
| Convert milliseconds to ticks | `pdMS_TO_TICKS()` |
| Shared-data mutex | `xSemaphoreCreateMutex()` |
| Lock mutex | `xSemaphoreTake()` |
| Unlock mutex | `xSemaphoreGive()` |
| Create queue | `xQueueCreate()` |
| Send queue item | `xQueueSend()` |
| Receive queue item | `xQueueReceive()` |
| Read ESP32 ADC | `analogRead()` |
| PWM output | `ledcAttach()` / `ledcWrite()` |
| Debug/monitor | `Serial.print()` / `Serial.printf()` |

## Learning Progression

```text
ESP32 Embedded Programming
            ↓
       FreeRTOS Task
            ↓
      Multiple Tasks
            ↓
      Shared Resources
            ↓
       Race Condition
            ↓
           Mutex
            ↓
    Producer–Consumer
            ↓
     FreeRTOS Queue
            ↓
 LDR Smart Lighting System
```

---

# Suggested Extension — ESP32 FreeRTOS LDR Smart Lighting System

Extend the final lab using:

- LDR sensor
- ESP32 ADC
- LED
- PWM brightness control
- Multiple FreeRTOS tasks
- Mutex/Semaphore
- FreeRTOS Queue
- Serial monitoring

Possible extensions include:

1. Replace threshold-based LED control with continuous ADC-to-PWM mapping.
2. Add a push button to enable or disable automatic lighting.
3. Add a second sensor task.
4. Add Wi-Fi and MQTT for remote monitoring.
5. Publish LDR readings and LED state to an IoT dashboard.
