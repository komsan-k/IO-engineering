# Practical Series: pthread Programming with an LDR Sensor

## Overview

This 5-lab series introduces **POSIX Threads (`pthread`)** using an **LDR (Light Dependent Resistor)** as the common sensor theme. The labs progress from basic thread creation to multiple threads, shared-data race conditions, mutex synchronization, and a producer-consumer sensor-control system.

Because a normal Linux computer does not have a directly accessible LDR/ADC input, the LDR measurements are **simulated in software**. The same concurrent-programming concepts can later be transferred to an ESP32 using FreeRTOS.

## Learning Path

```text
Lab 1: Basic pthread
        ↓
Lab 2: Multiple Threads
        ↓
Lab 3: Shared Data and Race Condition
        ↓
Lab 4: Mutex Synchronization
        ↓
Lab 5: Producer–Consumer System
```

## Requirements

- Linux or Linux-compatible environment
- GCC compiler
- POSIX Threads library (`pthread`)
- Terminal

Compile a pthread program using:

```bash
gcc program.c -o program -pthread
```

---

# Lab 1 — Basic pthread: LDR Sensor Task

## Objective

Understand how to create, execute, wait for, and terminate a POSIX thread.

## Key Functions

```c
pthread_create()
pthread_join()
```

## Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

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

int main(void)
{
    pthread_t ldrThread;

    srand(time(NULL));

    if (pthread_create(&ldrThread, NULL, ldrTask, NULL) != 0)
    {
        perror("pthread_create");
        return 1;
    }

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
LDR task completed.
```

## Assignment

Modify the program so that the LDR thread reports both the raw ADC value and normalized value:

$$
LDR_{\mathrm{norm}} =
\frac{LDR_{\mathrm{raw}}}{4095}
$$

Example:

```c
float normalized = ldrRaw / 4095.0f;

printf("RAW = %d | Normalized = %.3f\n",
       ldrRaw, normalized);
```

---

# Lab 2 — Multiple pthreads: LDR + LED

## Objective

Understand concurrent execution using multiple POSIX threads.

Create two threads:

```text
Thread 1 → Read LDR
Thread 2 → Control LED
```

The LED is simulated using terminal output.

## Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

volatile int ldrValue = 0;

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

int main(void)
{
    pthread_t sensorThread;
    pthread_t ledThread;

    srand(time(NULL));

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

Modify the program to classify three illumination levels:

```text
LDR < 1000          → DARK
1000 ≤ LDR < 3000   → NORMAL
LDR ≥ 3000          → BRIGHT
```

Display both the simulated LDR value and environmental state.

---

# Lab 3 — Shared Data and Race Conditions

## Objective

Understand problems caused by **unsynchronized shared data** between pthreads.

Define a structure containing several related sensor values:

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

## Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
} LDRData;

LDRData data = {0, 0.0f, 0};

void *sensorTask(void *arg)
{
    while (1)
    {
        /*
         * Intentionally not protected.
         * The delays make inconsistent reads easier to observe.
         */
        data.raw = rand() % 4096;

        usleep(1000);

        data.normalized = data.raw / 4095.0f;

        usleep(1000);

        data.sampleNumber++;

        usleep(20000);
    }

    return NULL;
}

void *controlTask(void *arg)
{
    while (1)
    {
        printf("Sample %d: RAW=%d, Normalized=%.3f\n",
               data.sampleNumber,
               data.raw,
               data.normalized);

        usleep(10000);
    }

    return NULL;
}

int main(void)
{
    pthread_t sensorThread;
    pthread_t controlThread;

    srand(time(NULL));

    pthread_create(&sensorThread, NULL, sensorTask, NULL);
    pthread_create(&controlThread, NULL, controlTask, NULL);

    pthread_join(sensorThread, NULL);
    pthread_join(controlThread, NULL);

    return 0;
}
```

At this stage, **do not use a mutex**.

The consumer may read the structure while the producer is updating it:

```text
Sensor Thread                  Control Thread

Write raw
    ↓
Write normalized  ←----------- Read raw
    ↓
Write sampleNumber ------------ Read normalized
```

This can produce an inconsistent set of sensor values and demonstrates a **race condition**.

## Assignment

1. Increase the sampling rate.
2. Observe the program output.
3. Explain the term **race condition**.
4. Identify the program's **critical section**.
5. Explain why shared sensor data should be protected.

---

# Lab 4 — LDR Shared Data with pthread Mutex

## Objective

Protect shared sensor data using `pthread_mutex_t`.

## Mutex Declaration

```c
pthread_mutex_t ldrMutex;
```

Initialize the mutex:

```c
pthread_mutex_init(&ldrMutex, NULL);
```

Lock and unlock it using:

```c
pthread_mutex_lock(&ldrMutex);

/* Critical section */

pthread_mutex_unlock(&ldrMutex);
```

## Complete Example Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int raw;
    float normalized;
    int sampleNumber;
    char ldrState[10];
} LDRData;

LDRData data = {0, 0.0f, 0, "UNKNOWN"};

pthread_mutex_t ldrMutex;

void *sensorTask(void *arg)
{
    while (1)
    {
        int raw = rand() % 4096;
        float normalized = raw / 4095.0f;
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
        strcpy(data.ldrState, state);

        pthread_mutex_unlock(&ldrMutex);

        usleep(500000);
    }

    return NULL;
}

void *controlTask(void *arg)
{
    while (1)
    {
        int raw;
        float normalized;
        int sample;
        char state[10];

        pthread_mutex_lock(&ldrMutex);

        raw = data.raw;
        normalized = data.normalized;
        sample = data.sampleNumber;
        strcpy(state, data.ldrState);

        pthread_mutex_unlock(&ldrMutex);

        printf("Sample %d: RAW=%d, Normalized=%.3f, State=%s\n",
               sample, raw, normalized, state);

        usleep(500000);
    }

    return NULL;
}

int main(void)
{
    pthread_t sensorThread;
    pthread_t controlThread;

    srand(time(NULL));

    pthread_mutex_init(&ldrMutex, NULL);

    pthread_create(&sensorThread, NULL, sensorTask, NULL);
    pthread_create(&controlThread, NULL, controlTask, NULL);

    pthread_join(sensorThread, NULL);
    pthread_join(controlThread, NULL);

    pthread_mutex_destroy(&ldrMutex);

    return 0;
}
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

 pthread Mutex
```

## Assignment

Implement a complete thread-safe LDR monitoring program.

Use an `ldrState` field representing:

```text
DARK
NORMAL
BRIGHT
```

Compare:

- Without mutex
- With mutex

Explain how the mutex protects the **critical section**.

---

# Lab 5 — Producer–Consumer LDR Monitoring System

## Objective

Build a realistic concurrent sensor-control architecture using **pthread mutexes and condition variables**.

The Sensor Thread acts as a producer and places measurements into a circular buffer. The Control Thread acts as a consumer.

```text
                    ┌─────────────┐
Simulated LDR ─────►│Sensor Thread│
                    │  Producer   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │ LDR Buffer  │
                    │ [ ][ ][ ][ ]│
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Control Thread│
                    │  Consumer   │
                    └──────┬──────┘
                           │
                           ▼
                    Simulated LED
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

## Synchronization Objects

```c
pthread_mutex_t mutex;
pthread_cond_t notEmpty;
pthread_cond_t notFull;
```

Condition variables allow a thread to sleep efficiently while the queue is empty or full.

## Producer Operation

```text
Generate LDR value
       ↓
Normalize
       ↓
Create LDRData
       ↓
LOCK
       ↓
Wait if Buffer Full
       ↓
Insert into Buffer
       ↓
Signal notEmpty
       ↓
UNLOCK
```

## Consumer Operation

```text
LOCK
  ↓
Wait if Buffer Empty
  ↓
Get Measurement
  ↓
Signal notFull
  ↓
UNLOCK
  ↓
Analyze Illumination
  ↓
Control Simulated LED
```

## Complete pthread Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 10

typedef struct {
    int sample;
    int raw;
    float normalized;
} LDRData;

LDRData buffer[BUFFER_SIZE];

int head = 0;
int tail = 0;
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t notEmpty;
pthread_cond_t notFull;

void *sensorTask(void *arg)
{
    int sampleNumber = 1;

    while (1)
    {
        LDRData measurement;

        measurement.sample = sampleNumber++;
        measurement.raw = rand() % 4096;
        measurement.normalized = measurement.raw / 4095.0f;

        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE)
        {
            pthread_cond_wait(&notFull, &mutex);
        }

        buffer[head] = measurement;
        head = (head + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&notEmpty);

        pthread_mutex_unlock(&mutex);

        usleep(500000);
    }

    return NULL;
}

void *controlTask(void *arg)
{
    while (1)
    {
        LDRData measurement;
        const char *state;
        const char *led;

        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            pthread_cond_wait(&notEmpty, &mutex);
        }

        measurement = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&notFull);

        pthread_mutex_unlock(&mutex);

        if (measurement.raw < 1000)
        {
            state = "DARK";
            led = "ON";
        }
        else if (measurement.raw < 3000)
        {
            state = "NORMAL";
            led = "DIM";
        }
        else
        {
            state = "BRIGHT";
            led = "OFF";
        }

        printf("[%03d] LDR=%d  Norm=%.2f  State=%-6s  LED=%s\n",
               measurement.sample,
               measurement.raw,
               measurement.normalized,
               state,
               led);
    }

    return NULL;
}

int main(void)
{
    pthread_t sensorThread;
    pthread_t controlThread;

    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&notEmpty, NULL);
    pthread_cond_init(&notFull, NULL);

    pthread_create(&sensorThread, NULL, sensorTask, NULL);
    pthread_create(&controlThread, NULL, controlTask, NULL);

    pthread_join(sensorThread, NULL);
    pthread_join(controlThread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);

    return 0;
}
```

## Compile and Run

```bash
gcc lab5.c -o lab5 -pthread
./lab5
```

Example output:

```text
[001] LDR=823   Norm=0.20  State=DARK    LED=ON
[002] LDR=1562  Norm=0.38  State=NORMAL  LED=DIM
[003] LDR=3468  Norm=0.85  State=BRIGHT  LED=OFF
```

---

# Final Lab Assignment — pthread LDR Smart Lighting System

Develop an **LDR-Based Smart Lighting System** consisting of three pthreads:

```text
              ┌───────────────┐
              │ Sensor Thread │
              │ Generate LDR  │
              └───────┬───────┘
                      │
                      ▼
              Circular Buffer
                      │
              ┌───────▼───────┐
              │Control Thread │
              │   Decision    │
              └───────┬───────┘
                      │
                      ▼
                 LED State

              ┌───────────────┐
              │ Logger Thread │
              │ Display Data  │
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

The logger should produce output similar to:

```text
[001] LDR=823   Norm=0.20  State=DARK    LED=ON
[002] LDR=1562  Norm=0.38  State=NORMAL  LED=DIM
[003] LDR=3468  Norm=0.85  State=BRIGHT  LED=OFF
```

## Assignment Requirements

The final program should demonstrate:

1. At least **three pthreads**.
2. A producer-consumer architecture.
3. A circular buffer.
4. `pthread_mutex_t` for synchronization.
5. `pthread_cond_t` for producer-consumer signaling.
6. Correct protection of critical sections.
7. Sensor-state classification.
8. LED-state decision logic.
9. Clear terminal logging.
10. Proper thread and synchronization-object cleanup.

---

# pthread Functions Used in This Lab Series

| Purpose | pthread / POSIX Function |
|---|---|
| Create thread | `pthread_create()` |
| Wait for thread | `pthread_join()` |
| Exit current thread | `pthread_exit()` / `return NULL` |
| Create mutex | `pthread_mutex_init()` |
| Lock mutex | `pthread_mutex_lock()` |
| Unlock mutex | `pthread_mutex_unlock()` |
| Destroy mutex | `pthread_mutex_destroy()` |
| Create condition variable | `pthread_cond_init()` |
| Wait on condition | `pthread_cond_wait()` |
| Signal condition | `pthread_cond_signal()` |
| Destroy condition variable | `pthread_cond_destroy()` |
| Delay in seconds | `sleep()` |
| Delay in microseconds | `usleep()` |

## Learning Progression

```text
Linux Concurrent Programming
            ↓
          pthread
            ↓
      Multiple Threads
            ↓
      Shared Resources
            ↓
       Race Condition
            ↓
           Mutex
            ↓
    Producer–Consumer
            ↓
   Condition Variables
            ↓
   Smart Lighting System
```

---

# Connection to ESP32 + FreeRTOS

After completing the pthread labs, the same concurrent-programming concepts can be transferred to the ESP32.

| Linux pthread | ESP32 FreeRTOS |
|---|---|
| `pthread_create()` | `xTaskCreate()` |
| `pthread_join()` | Task lifecycle / synchronization |
| `pthread_mutex_t` | `SemaphoreHandle_t` |
| `pthread_mutex_lock()` | `xSemaphoreTake()` |
| `pthread_mutex_unlock()` | `xSemaphoreGive()` |
| Circular buffer | `xQueueCreate()` |
| `pthread_cond_wait()` | Queue/semaphore blocking |
| Producer thread | Sensor task |
| Consumer thread | Control task |
| `sleep()` / `usleep()` | `vTaskDelay()` |

This progression allows students to first understand the fundamental concepts of **threads, shared resources, race conditions, mutexes, and producer-consumer synchronization** before implementing similar architectures on an ESP32.
