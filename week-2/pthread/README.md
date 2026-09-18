# Pratical Series: pthread Programming with an LDR Sensor

## Overview

This 5-lab series introduces **POSIX Threads (`pthread`)** using an **LDR (Light Dependent Resistor)** as the common sensor theme. The series progresses from basic thread creation to synchronization and a producer-consumer sensor-control system.

### Learning Path

```text
Lab 1: Basic pthread
        ↓
Lab 2: Multiple Threads
        ↓
Lab 3: Race Condition
        ↓
Lab 4: Mutex Synchronization
        ↓
Lab 5: Producer–Consumer System
        ↓
ESP32 + FreeRTOS
```

### Target Architecture

```text
              LDR Sensor
                  │
                  ▼
          ┌───────────────┐
          │ Sensor Thread │
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
          │ Control Thread│
          │   Consumer    │
          └───────┬───────┘
                  │
             Decision
                  │
                  ▼
                LED
```

---

# Lab 1 — Basic pthread: LDR Sensor Task

## Objective

Understand how to create, execute, and terminate a pthread.

Since a normal Linux PC does not have an ADC connected directly, the LDR ADC reading is initially simulated.

## Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *ldrTask(void *arg)
{
    for (int i = 0; i < 10; i++)
    {
        int ldrRaw = rand() % 4096;

        printf("LDR RAW = %d\n", ldrRaw);

        sleep(1);
    }

    return NULL;
}

int main()
{
    pthread_t ldrThread;

    pthread_create(&ldrThread, NULL, ldrTask, NULL);

    pthread_join(ldrThread, NULL);

    printf("LDR task completed.\n");

    return 0;
}
```

## Compile and Run

```bash
gcc lab1.c -o lab1 -pthread
./lab1
```

Example output:

```text
LDR RAW = 3250
LDR RAW = 2780
LDR RAW = 1420
LDR RAW = 850
...
```

## Assignment

Modify the program so that the LDR thread reports both the raw ADC value and normalized value:

$$
LDR_{\mathrm{norm}} =
\frac{LDR_{\mathrm{raw}}}{4095}
$$

---

# Lab 2 — Multiple Threads: LDR + LED

## Objective

Understand concurrent execution using multiple threads.

Create two threads:

```text
Thread 1 → Read LDR
Thread 2 → Control LED
```

## Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int ldrValue = 0;

void *ldrTask(void *arg)
{
    while (1)
    {
        ldrValue = rand() % 4096;

        printf("LDR: %d\n", ldrValue);

        sleep(1);
    }

    return NULL;
}

void *ledTask(void *arg)
{
    while (1)
    {
        if (ldrValue < 1500)
            printf("LED: ON\n");
        else
            printf("LED: OFF\n");

        sleep(1);
    }

    return NULL;
}

int main()
{
    pthread_t sensorThread;
    pthread_t ledThread;

    pthread_create(&sensorThread, NULL, ldrTask, NULL);
    pthread_create(&ledThread, NULL, ledTask, NULL);

    pthread_join(sensorThread, NULL);
    pthread_join(ledThread, NULL);

    return 0;
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
LDR < 1000        → DARK
1000–2999         → NORMAL
LDR ≥ 3000        → BRIGHT
```

Display both the sensor value and environmental state.

---

# Lab 3 — Shared Data and Race Conditions

## Objective

Understand problems caused by unsynchronized shared data.

Instead of sharing only one variable, define:

```c
typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
} LDRData;
```

Both threads access the same structure:

```text
                 Shared LDRData
                ┌──────────────┐
Sensor Thread → │ raw          │ ← Control Thread
                │ normalized   │
                │ sampleNumber │
                └──────────────┘
```

## Sensor Thread Operation

```c
data.raw = rand() % 4096;
data.normalized = data.raw / 4095.0;
data.sampleNumber++;
```

## Control Thread Operation

```c
printf("Sample %d: RAW=%d, Normalized=%.2f\n",
       data.sampleNumber,
       data.raw,
       data.normalized);
```

At this stage, **do not use a mutex**.

The consumer may read the shared structure while the producer is updating it:

```text
Sensor Thread             Control Thread

Write raw
      ↓
Write normalized  ←------- Read raw
      ↓
Write sampleNumber ------- Read normalized
```

This can result in inconsistent sensor measurements and demonstrates a **race condition**.

## Assignment

1. Increase the sampling rate.
2. Observe the behavior of the program.
3. Explain the term **race condition**.
4. Identify the program's **critical section**.
5. Explain why shared sensor data should be protected.

---

# Lab 4 — LDR Shared Data with Mutex

## Objective

Protect shared sensor data using `pthread_mutex_t`.

Declare the mutex:

```c
pthread_mutex_t ldrMutex;
```

Initialize it:

```c
pthread_mutex_init(&ldrMutex, NULL);
```

## Sensor Thread

```c
pthread_mutex_lock(&ldrMutex);

data.raw = rand() % 4096;
data.normalized = data.raw / 4095.0;
data.sampleNumber++;

pthread_mutex_unlock(&ldrMutex);
```

## Control Thread

```c
pthread_mutex_lock(&ldrMutex);

int raw = data.raw;
float normalized = data.normalized;
int sample = data.sampleNumber;

pthread_mutex_unlock(&ldrMutex);
```

## Architecture

```text
Sensor Thread
     │
     ▼
   LOCK
     │
     ▼
┌──────────────┐
│ Shared LDR   │
│    Data      │
└──────────────┘
     ▲
     │
   LOCK
     ▲
     │
Control Thread

      Mutex
```

## Assignment

Implement a complete thread-safe LDR monitoring program.

Add an `ldrState` field representing:

```text
DARK
NORMAL
BRIGHT
```

Compare the behavior and architecture of the program:

- Without mutex
- With mutex

Explain how the mutex protects the **critical section**.

---

# Lab 5 — Producer–Consumer LDR Monitoring System

## Objective

Build a realistic concurrent embedded sensor-control architecture.

Instead of sharing only one measurement, the sensor thread produces measurements and places them into a buffer.

```text
                    ┌─────────────┐
LDR Sensor ────────►│ Sensor Task │
                    │  Producer   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │ LDR Queue   │
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

```c
typedef struct {
    int sample;
    int raw;
    float normalized;
} LDRData;
```

## Circular Buffer

```c
#define BUFFER_SIZE 10

LDRData buffer[BUFFER_SIZE];

int head = 0;
int tail = 0;
int count = 0;
```

## Producer Operation

```text
Read LDR
   ↓
Normalize
   ↓
Create LDRData
   ↓
LOCK
   ↓
Insert into Queue
   ↓
UNLOCK
```

## Consumer Operation

```text
LOCK
  ↓
Get measurement
  ↓
UNLOCK
  ↓
Analyze illumination
  ↓
Control LED
```

For a proper producer-consumer implementation, use a mutex and condition variables:

```c
pthread_mutex_t mutex;
pthread_cond_t notEmpty;
pthread_cond_t notFull;
```

Important functions include:

```c
pthread_cond_wait();
pthread_cond_signal();
```

Condition variables allow threads to wait efficiently instead of continuously polling the queue.

---

# Final Lab 5 Assignment — LDR-Based Smart Lighting System

Develop an **LDR-Based Smart Lighting System** consisting of three threads:

```text
              ┌──────────────┐
              │ Sensor Thread│
              │ Read LDR     │
              └──────┬───────┘
                     │
                     ▼
                  Queue
                     │
              ┌──────▼───────┐
              │Control Thread│
              │Decision      │
              └──────┬───────┘
                     │
                     ▼
                    LED

              ┌──────────────┐
              │Logger Thread │
              │Display Data  │
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

The logger should produce output similar to:

```text
[001] LDR=823   Norm=0.20  State=DARK    LED=ON
[002] LDR=1562  Norm=0.38  State=NORMAL  LED=DIM
[003] LDR=3468  Norm=0.85  State=BRIGHT  LED=OFF
```

---

# Connection to ESP32 + FreeRTOS

After completing Lab 5, the same architecture can be transferred to an ESP32.

| Linux pthread | ESP32 FreeRTOS |
|---|---|
| `pthread_create()` | `xTaskCreate()` |
| `pthread_mutex_t` | `SemaphoreHandle_t` |
| `pthread_mutex_lock()` | `xSemaphoreTake()` |
| `pthread_mutex_unlock()` | `xSemaphoreGive()` |
| Circular buffer | `xQueueCreate()` |
| Producer | Sensor Task |
| Consumer | Control Task |
| `sleep()` | `vTaskDelay()` |

The learning progression is therefore:

```text
Linux Concurrent Programming
            ↓
         pthread
            ↓
     Multiple Threads
            ↓
      Shared Resources
            ↓
      Race Conditions
            ↓
           Mutex
            ↓
    Producer–Consumer
            ↓
           Queue
            ↓
     ESP32 + FreeRTOS
```

## Suggested Next Lab

**Lab 6 — ESP32 FreeRTOS LDR Smart Lighting System**

Transfer the Lab 5 architecture to physical ESP32 hardware using:

- LDR sensor
- ESP32 ADC
- LED
- PWM brightness control
- FreeRTOS tasks
- Mutex/Semaphore
- FreeRTOS Queue
- Serial monitoring
