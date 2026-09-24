# Practical Series: pthread Programming on ESP32 with Arduino

## Overview

This 5-lab series introduces **POSIX Threads (`pthread`) on the ESP32 using the Arduino framework**, with an **LDR (Light Dependent Resistor)** as the common sensor theme.

Unlike the Linux version, the ESP32 reads a **real LDR sensor through its ADC** and controls a **physical LED**. The examples intentionally use the ESP32 Arduino core's `pthread` support rather than `xTaskCreate()` so that students can study POSIX-style thread programming in an embedded environment.

The learning progression is:

```text
Lab 1: Basic pthread on ESP32
        ↓
Lab 2: Multiple pthreads
        ↓
Lab 3: Shared Data and Race Condition
        ↓
Lab 4: pthread Mutex Synchronization
        ↓
Lab 5: Producer–Consumer with pthread
```

---

## Hardware

- ESP32 development board
- LDR sensor
- 10 kΩ resistor
- LED
- 220–330 Ω resistor
- Breadboard
- Jumper wires

## Pin Configuration

```cpp
#define LDR_PIN 36
#define LED_PIN 2
```

### LDR Connection

A simple voltage divider can be used:

```text
3.3 V
  │
 LDR
  │
  ├──────── GPIO 34
  │
10 kΩ
  │
 GND
```

> Depending on the orientation of the LDR and resistor, the ADC value may increase or decrease as the light level increases. Adjust the threshold logic if necessary.

---

# Lab 1 — Basic pthread: LDR Sensor Thread

## Objective

Understand how to create, execute, and terminate a `pthread` on an ESP32 using Arduino.

## Key Functions

```cpp
pthread_create()
pthread_join()
```

## Arduino ESP32 Program

```cpp
#include <Arduino.h>
#include <pthread.h>

#define LDR_PIN 36

void *ldrThread(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        int ldrRaw = analogRead(LDR_PIN);

        Serial.print("LDR RAW = ");
        Serial.println(ldrRaw);

        delay(1000);
    }

    Serial.println("LDR thread completed.");

    return NULL;
}

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);

    pthread_t thread;

    int result = pthread_create(
        &thread,
        NULL,
        ldrThread,
        NULL
    );

    if (result != 0)
    {
        Serial.print("pthread_create failed: ");
        Serial.println(result);
        return;
    }

    pthread_join(thread, NULL);

    Serial.println("Main: thread joined.");
}

void loop()
{
}
```

## Expected Serial Output

```text
LDR RAW = 3250
LDR RAW = 2780
LDR RAW = 1420
LDR RAW = 850
...
LDR thread completed.
Main: thread joined.
```

## Assignment

Modify the thread so that it reports both the raw ADC value and normalized value:

$$
LDR_{\mathrm{norm}}
= \frac{LDR_{\mathrm{raw}}}{4095}
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

# Lab 2 — Multiple pthreads: LDR + LED

## Objective

Understand concurrent execution using multiple `pthread` threads on the ESP32.

Create two threads:

```text
Thread 1 → Read LDR
Thread 2 → Control LED
```

## Architecture

```text
        LDR
         │
         ▼
 ┌──────────────┐
 │ Sensor Thread│
 └──────┬───────┘
        │
     ldrValue
        │
        ▼
 ┌──────────────┐
 │  LED Thread  │
 └──────┬───────┘
        │
        ▼
       LED
```

## Arduino ESP32 Program

```cpp
#include <Arduino.h>
#include <pthread.h>

#define LDR_PIN 34
#define LED_PIN 2

volatile int ldrValue = 0;

pthread_t sensorThread;
pthread_t ledThread;

void *readLDR(void *arg)
{
    while (true)
    {
        ldrValue = analogRead(LDR_PIN);

        Serial.print("[Sensor] LDR = ");
        Serial.println(ldrValue);

        delay(500);
    }

    return NULL;
}

void *controlLED(void *arg)
{
    while (true)
    {
        int value = ldrValue;

        if (value < 1500)
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("[LED] ON");
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("[LED] OFF");
        }

        delay(500);
    }

    return NULL;
}

void setup()
{
    Serial.begin(115200);

    pinMode(LDR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    pthread_create(
        &sensorThread,
        NULL,
        readLDR,
        NULL
    );

    pthread_create(
        &ledThread,
        NULL,
        controlLED,
        NULL
    );
}

void loop()
{
    delay(1000);
}
```

## Control Logic

$$
LED =
\begin{cases}
ON, & LDR < 1500 \\
OFF, & LDR \ge 1500
\end{cases}
$$

## Assignment

Modify the program to classify the environment into:

```text
LDR < 1000          → DARK
1000 ≤ LDR < 3000   → NORMAL
LDR ≥ 3000          → BRIGHT
```

Display:

```text
LDR = 823   State = DARK
LDR = 1562  State = NORMAL
LDR = 3468  State = BRIGHT
```

---

# Lab 3 — Shared Data and Race Conditions

## Objective

Understand the problem of **unsynchronized shared data** when multiple pthreads execute concurrently.

Instead of sharing one integer, use a structure:

```cpp
typedef struct
{
    int raw;
    float normalized;
    unsigned long sampleNumber;
} LDRData;
```

## Shared-Data Architecture

```text
                 Shared LDRData

                ┌──────────────┐
Sensor Thread → │ raw          │ ← Monitor Thread
                │ normalized   │
                │ sampleNumber │
                └──────────────┘
```

## Arduino ESP32 Program

```cpp
#include <Arduino.h>
#include <pthread.h>

#define LDR_PIN 34

typedef struct
{
    int raw;
    float normalized;
    unsigned long sampleNumber;
} LDRData;

LDRData data = {0, 0.0, 0};

pthread_t sensorThread;
pthread_t monitorThread;

void *sensorFunction(void *arg)
{
    while (true)
    {
        // Intentionally no mutex in this lab.
        data.raw = analogRead(LDR_PIN);

        delay(1);

        data.normalized = data.raw / 4095.0;

        delay(1);

        data.sampleNumber++;

        delay(10);
    }

    return NULL;
}

void *monitorFunction(void *arg)
{
    while (true)
    {
        Serial.print("Sample ");
        Serial.print(data.sampleNumber);

        Serial.print(" | RAW = ");
        Serial.print(data.raw);

        Serial.print(" | Normalized = ");
        Serial.println(data.normalized, 3);

        delay(5);
    }

    return NULL;
}

void setup()
{
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);

    pthread_create(
        &sensorThread,
        NULL,
        sensorFunction,
        NULL
    );

    pthread_create(
        &monitorThread,
        NULL,
        monitorFunction,
        NULL
    );
}

void loop()
{
    delay(1000);
}
```

## Race Condition

The monitor thread may access the structure while the sensor thread is still updating it:

```text
Sensor Thread                  Monitor Thread

Write raw
    ↓
delay
    ↓
Write normalized  ←---------- Read raw
    ↓
delay
    ↓
Write sampleNumber ----------- Read normalized
```

The displayed values can therefore belong to different sensor samples.

## Assignment

1. Reduce the delays in both threads.
2. Observe the Serial Monitor.
3. Identify the shared variables.
4. Identify the critical section.
5. Explain the term **race condition**.
6. Explain why `volatile` alone does not provide mutual exclusion.

---

# Lab 4 — Shared LDR Data with pthread Mutex

## Objective

Use a **pthread mutex** to protect shared sensor data on the ESP32.

## Mutex Declaration

```cpp
pthread_mutex_t ldrMutex;
```

## Initialization

```cpp
pthread_mutex_init(&ldrMutex, NULL);
```

## Critical Section

```cpp
pthread_mutex_lock(&ldrMutex);

// Access shared data

pthread_mutex_unlock(&ldrMutex);
```

## Arduino ESP32 Program

```cpp
#include <Arduino.h>
#include <pthread.h>
#include <string.h>

#define LDR_PIN 34

typedef struct
{
    int raw;
    float normalized;
    unsigned long sampleNumber;
    char state[10];
} LDRData;

LDRData data = {0, 0.0, 0, "UNKNOWN"};

pthread_mutex_t ldrMutex;

pthread_t sensorThread;
pthread_t monitorThread;

void *sensorFunction(void *arg)
{
    while (true)
    {
        int raw = analogRead(LDR_PIN);
        float normalized = raw / 4095.0;

        char state[10];

        if (raw < 1000)
            strcpy(state, "DARK");
        else if (raw < 3000)
            strcpy(state, "NORMAL");
        else
            strcpy(state, "BRIGHT");

        pthread_mutex_lock(&ldrMutex);

        data.raw = raw;
        data.normalized = normalized;
        data.sampleNumber++;
        strcpy(data.state, state);

        pthread_mutex_unlock(&ldrMutex);

        delay(500);
    }

    return NULL;
}

void *monitorFunction(void *arg)
{
    while (true)
    {
        LDRData localData;

        pthread_mutex_lock(&ldrMutex);

        localData = data;

        pthread_mutex_unlock(&ldrMutex);

        Serial.print("Sample ");
        Serial.print(localData.sampleNumber);

        Serial.print(" | RAW = ");
        Serial.print(localData.raw);

        Serial.print(" | Norm = ");
        Serial.print(localData.normalized, 3);

        Serial.print(" | State = ");
        Serial.println(localData.state);

        delay(500);
    }

    return NULL;
}

void setup()
{
    Serial.begin(115200);

    pinMode(LDR_PIN, INPUT);

    pthread_mutex_init(&ldrMutex, NULL);

    pthread_create(
        &sensorThread,
        NULL,
        sensorFunction,
        NULL
    );

    pthread_create(
        &monitorThread,
        NULL,
        monitorFunction,
        NULL
    );
}

void loop()
{
    delay(1000);
}
```

## Architecture

```text
Sensor Thread
     │
     ▼
pthread_mutex_lock()
     │
     ▼
┌──────────────┐
│ Shared LDR   │
│    Data      │
└──────────────┘
     ▲
     │
pthread_mutex_lock()
     ▲
     │
Monitor Thread

      Mutex
```

## Assignment

Compare:

```text
Lab 3 → Without mutex
Lab 4 → With mutex
```

Explain:

1. What is the critical section?
2. Why is the mutex required?
3. What happens when one thread owns the mutex?
4. Why should the critical section be kept short?

---

# Lab 5 — Producer–Consumer LDR Monitoring System

## Objective

Build a **producer-consumer system using pthreads, a mutex, condition variables, and a circular buffer** on the ESP32.

## Architecture

```text
                    ┌───────────────┐
LDR Sensor ────────►│ Sensor Thread │
                    │   Producer    │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │Circular Buffer│
                    │ [ ][ ][ ][ ]  │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │Control Thread │
                    │   Consumer    │
                    └───────┬───────┘
                            │
                            ▼
                           LED
```

## Sensor Data Structure

```cpp
typedef struct
{
    unsigned long sample;
    int raw;
    float normalized;
} LDRData;
```

## Circular Buffer

```cpp
#define BUFFER_SIZE 10

LDRData buffer[BUFFER_SIZE];

int head = 0;
int tail = 0;
int count = 0;
```

## Synchronization Objects

```cpp
pthread_mutex_t bufferMutex;
pthread_cond_t notEmpty;
pthread_cond_t notFull;
```

## Producer Algorithm

```text
Read LDR
   ↓
Normalize
   ↓
Create LDRData
   ↓
LOCK
   ↓
Buffer full?
   │
   └── Yes → pthread_cond_wait()
   ↓
Insert measurement
   ↓
pthread_cond_signal(notEmpty)
   ↓
UNLOCK
```

## Consumer Algorithm

```text
LOCK
  ↓
Buffer empty?
  │
  └── Yes → pthread_cond_wait()
  ↓
Remove measurement
  ↓
pthread_cond_signal(notFull)
  ↓
UNLOCK
  ↓
Analyze illumination
  ↓
Control LED
```

## Arduino ESP32 Program

```cpp
#include <Arduino.h>
#include <pthread.h>

#define LDR_PIN 34
#define LED_PIN 2
#define BUFFER_SIZE 10

typedef struct
{
    unsigned long sample;
    int raw;
    float normalized;
} LDRData;

LDRData buffer[BUFFER_SIZE];

int head = 0;
int tail = 0;
int count = 0;

pthread_mutex_t bufferMutex;
pthread_cond_t notEmpty;
pthread_cond_t notFull;

pthread_t producerThread;
pthread_t consumerThread;

void *sensorProducer(void *arg)
{
    unsigned long sampleNumber = 1;

    while (true)
    {
        LDRData measurement;

        measurement.sample = sampleNumber++;
        measurement.raw = analogRead(LDR_PIN);
        measurement.normalized =
            measurement.raw / 4095.0;

        pthread_mutex_lock(&bufferMutex);

        while (count == BUFFER_SIZE)
        {
            pthread_cond_wait(
                &notFull,
                &bufferMutex
            );
        }

        buffer[head] = measurement;

        head =
            (head + 1) % BUFFER_SIZE;

        count++;

        pthread_cond_signal(&notEmpty);

        pthread_mutex_unlock(&bufferMutex);

        delay(500);
    }

    return NULL;
}

void *ledConsumer(void *arg)
{
    while (true)
    {
        LDRData measurement;

        pthread_mutex_lock(&bufferMutex);

        while (count == 0)
        {
            pthread_cond_wait(
                &notEmpty,
                &bufferMutex
            );
        }

        measurement = buffer[tail];

        tail =
            (tail + 1) % BUFFER_SIZE;

        count--;

        pthread_cond_signal(&notFull);

        pthread_mutex_unlock(&bufferMutex);

        const char *state;
        const char *ledState;

        if (measurement.raw < 1000)
        {
            state = "DARK";
            ledState = "ON";

            digitalWrite(
                LED_PIN,
                HIGH
            );
        }
        else if (measurement.raw < 3000)
        {
            state = "NORMAL";
            ledState = "ON";

            digitalWrite(
                LED_PIN,
                HIGH
            );
        }
        else
        {
            state = "BRIGHT";
            ledState = "OFF";

            digitalWrite(
                LED_PIN,
                LOW
            );
        }

        Serial.printf(
            "[%03lu] LDR=%d  Norm=%.2f  State=%-6s  LED=%s\n",
            measurement.sample,
            measurement.raw,
            measurement.normalized,
            state,
            ledState
        );
    }

    return NULL;
}

void setup()
{
    Serial.begin(115200);

    pinMode(LDR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    pthread_mutex_init(
        &bufferMutex,
        NULL
    );

    pthread_cond_init(
        &notEmpty,
        NULL
    );

    pthread_cond_init(
        &notFull,
        NULL
    );

    pthread_create(
        &producerThread,
        NULL,
        sensorProducer,
        NULL
    );

    pthread_create(
        &consumerThread,
        NULL,
        ledConsumer,
        NULL
    );
}

void loop()
{
    delay(1000);
}
```

## Expected Output

```text
[001] LDR=823   Norm=0.20  State=DARK    LED=ON
[002] LDR=1562  Norm=0.38  State=NORMAL  LED=ON
[003] LDR=3468  Norm=0.85  State=BRIGHT  LED=OFF
```

---

# Final Assignment — ESP32 pthread Smart Lighting System

Develop an **LDR-Based Smart Lighting System** using three pthreads:

```text
              ┌───────────────┐
              │ Sensor Thread │
              │   Producer    │
              └───────┬───────┘
                      │
                      ▼
                Circular Buffer
                      │
              ┌───────▼───────┐
              │Control Thread │
              │   Consumer    │
              └───────┬───────┘
                      │
                      ▼
                     LED

              ┌───────────────┐
              │ Logger Thread │
              │ Serial Output │
              └───────────────┘
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

## Requirements

The final Arduino program should demonstrate:

1. ESP32 ADC acquisition using `analogRead()`.
2. At least **three pthreads**.
3. A producer-consumer architecture.
4. Shared data or a circular buffer.
5. `pthread_mutex_t` synchronization.
6. `pthread_cond_t` synchronization.
7. Correct protection of critical sections.
8. Physical LED control.
9. Serial monitoring.
10. Proper organization of sensor, control, and logging operations.

---

# Important pthread Functions

| Purpose | Function |
|---|---|
| Create a thread | `pthread_create()` |
| Wait for a thread | `pthread_join()` |
| Exit current thread | `pthread_exit()` / `return NULL` |
| Initialize mutex | `pthread_mutex_init()` |
| Lock mutex | `pthread_mutex_lock()` |
| Unlock mutex | `pthread_mutex_unlock()` |
| Destroy mutex | `pthread_mutex_destroy()` |
| Initialize condition variable | `pthread_cond_init()` |
| Wait for condition | `pthread_cond_wait()` |
| Signal condition | `pthread_cond_signal()` |
| Broadcast condition | `pthread_cond_broadcast()` |
| Destroy condition variable | `pthread_cond_destroy()` |

---

# Arduino ESP32 and pthread Relationship

The ESP32 Arduino environment runs on top of **ESP-IDF and FreeRTOS**. The `pthread` interface provides a POSIX-style programming abstraction while the underlying ESP32 runtime schedules execution using FreeRTOS.

Conceptually:

```text
Arduino Application
        │
        ▼
 pthread API
        │
        ▼
ESP-IDF pthread Layer
        │
        ▼
    FreeRTOS
        │
        ▼
     ESP32 CPU
```

Therefore, these labs allow students to write familiar POSIX-style concurrent code while working with real embedded hardware.

---

# Learning Progression

```text
Arduino ESP32
      ↓
 pthread_create()
      ↓
 Multiple Threads
      ↓
 Shared Resources
      ↓
 Race Conditions
      ↓
 pthread_mutex_t
      ↓
 Condition Variables
      ↓
 Producer–Consumer
      ↓
 LDR + LED Embedded System
```

## Suggested Extension

After completing the five labs, extend the system with:

- PWM LED brightness control
- Push-button thread
- Multiple sensor threads
- Wi-Fi
- MQTT
- Data logging
- IoT dashboard
- Thread-priority experiments
- CPU-core affinity experiments
