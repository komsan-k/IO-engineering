# LAB 8 — FreeRTOS Software Timers and Heap Usage Monitoring

## 1. Objective

The objective of this lab is to:

1. Understand the role of **software timers** in FreeRTOS.
2. Create periodic activities using FreeRTOS timers.
3. Compare software timers with `vTaskDelay()` and `vTaskDelayUntil()`.
4. Implement **auto-reload** and **one-shot** timers.
5. Use timer callback functions correctly.
6. Create multiple timers with different periods.
7. Monitor ESP32 heap usage during runtime.
8. Observe how tasks, queues, and timers consume memory.
9. Measure free heap under different workloads.
10. Investigate dynamic memory allocation and deallocation.
11. Detect possible memory leaks and fragmentation.
12. Monitor task stack usage using the stack high-water mark.
13. Analyze the trade-off among timer frequency, workload, execution time, and memory usage.
14. Develop a complete FreeRTOS application combining timers and memory monitoring.

---

## 2. Background

In embedded systems, many activities must occur periodically.

Examples include:

```text
Read Sensor Every 100 ms

Update Display Every 500 ms

Send Status Every 1 s

Check Connection Every 5 s

Record Memory Usage Every 10 s
```

One solution is to create a task for each periodic activity.

For example:

```text
Task
 ↓
Do Work
 ↓
vTaskDelay()
 ↓
Do Work Again
```

However, for short periodic operations, FreeRTOS also provides **software timers**.

A software timer can execute a callback function when a specified period expires.

The basic architecture is:

```text
Timer Started
     ↓
Timer Period Expires
     ↓
Timer Callback Executes
     ↓
Timer Reloads
     ↓
Period Expires Again
```

for an auto-reload timer.

---

## 3. What Is a FreeRTOS Software Timer?

A FreeRTOS software timer is a kernel-managed timing mechanism.

Unlike a hardware timer, it does not require a separate hardware timer peripheral for each application timer.

A timer contains:

```text
Timer Name
    +
Timer Period
    +
Auto-Reload Setting
    +
Timer ID
    +
Callback Function
```

The timer service manages the expiration of software timers and executes their callback functions.

---

## 4. Software Timer Architecture

Conceptually:

```text
Application
    ↓
xTimerCreate()
    ↓
Software Timer
    ↓
Timer Service
    ↓
Timer Expires
    ↓
Callback Function
```

For example:

```text
500-ms Timer
    ↓
Callback
    ↓
Toggle LED
```

---

## 5. Auto-Reload vs. One-Shot Timer

### Auto-Reload Timer

An auto-reload timer restarts automatically after expiration.

```text
Start
  ↓
500 ms
  ↓
Callback
  ↓
500 ms
  ↓
Callback
  ↓
500 ms
  ↓
Callback
```

Useful for:

```text
Periodic Sensor Sampling
Periodic Status Update
Periodic LED Blink
Periodic Memory Monitoring
```

### One-Shot Timer

A one-shot timer expires only once.

```text
Start
  ↓
2 s
  ↓
Callback
  ↓
Stop
```

Useful for:

```text
Timeout
Delayed Action
Alarm
Watchdog-Style Application Event
```

---

## 6. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- LED
- 220–330 $\Omega$ resistor
- Optional potentiometer or analog sensor
- Breadboard
- Jumper wires

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS
- Serial Monitor

---

## 7. Important FreeRTOS Timer Functions

Create a timer:

```cpp
xTimerCreate()
```

Start a timer:

```cpp
xTimerStart()
```

Stop a timer:

```cpp
xTimerStop()
```

Reset a timer:

```cpp
xTimerReset()
```

Change the timer period:

```cpp
xTimerChangePeriod()
```

Delete a timer:

```cpp
xTimerDelete()
```

---

## 8. Timer Handle

A timer is represented by:

```cpp
TimerHandle_t timerHandle;
```

For example:

```cpp
TimerHandle_t ledTimer;
```

The handle is used to start, stop, reset, modify, or delete the timer.

---

## 9. Creating a Software Timer

Example:

```cpp
ledTimer =
    xTimerCreate(
      "LED Timer",
      pdMS_TO_TICKS(500),
      pdTRUE,
      NULL,
      ledTimerCallback
    );
```

The parameters represent:

```text
"LED Timer"
      ↓
Timer Name

pdMS_TO_TICKS(500)
      ↓
Period = 500 ms

pdTRUE
      ↓
Auto-Reload Enabled

NULL
      ↓
Timer ID

ledTimerCallback
      ↓
Callback Function
```

---

## 10. Timer Callback Function

A timer callback has the form:

```cpp
void ledTimerCallback(
    TimerHandle_t xTimer
) {

  // Timer action
}
```

For example:

```cpp
void ledTimerCallback(
    TimerHandle_t xTimer
) {

  digitalWrite(
    LED_PIN,
    !digitalRead(LED_PIN)
  );
}
```

---

## 11. Important Callback Design Rule

Timer callbacks should be:

```text
Short
+
Non-Blocking
+
Predictable
```

Avoid placing the following inside a timer callback:

```text
Long Delay
Long Computation
Blocking Network Operation
Large File Operation
Long Sensor Transaction
```

A timer callback should normally trigger or perform a short action and return quickly.

---

## 12. Experiment 1 — Basic Auto-Reload LED Timer

Define:

```cpp
#define LED_PIN 2
```

Create a timer callback:

```cpp
void ledTimerCallback(
    TimerHandle_t xTimer
) {

  digitalWrite(
    LED_PIN,
    !digitalRead(LED_PIN)
  );

  Serial.println(
    "LED timer callback"
  );
}
```

Create and start the timer:

```cpp
TimerHandle_t ledTimer;

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  ledTimer =
      xTimerCreate(
        "LED Timer",
        pdMS_TO_TICKS(500),
        pdTRUE,
        NULL,
        ledTimerCallback
      );

  if (
    ledTimer != NULL
  ) {

    xTimerStart(
      ledTimer,
      0
    );
  }
}
```

---

## 13. Complete Auto-Reload Timer Program

```cpp
#include <Arduino.h>

#define LED_PIN 2

TimerHandle_t ledTimer;

void ledTimerCallback(
    TimerHandle_t xTimer
) {

  digitalWrite(
    LED_PIN,
    !digitalRead(LED_PIN)
  );

  Serial.print(
    "LED timer at "
  );

  Serial.println(
    millis()
  );
}

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  ledTimer =
      xTimerCreate(
        "LED Timer",
        pdMS_TO_TICKS(500),
        pdTRUE,
        NULL,
        ledTimerCallback
      );

  if (
    ledTimer == NULL
  ) {

    Serial.println(
      "Timer creation failed"
    );

    while (1) {
    }
  }

  xTimerStart(
    ledTimer,
    0
  );
}

void loop() {
}
```

---

## 14. Expected Result

Possible output:

```text
LED timer at 500
LED timer at 1000
LED timer at 1500
LED timer at 2000
```

The expected timer period is:

$$
T=500~\text{ms}.
$$

Therefore, the callback frequency is approximately:

$$
f=
\frac{1}{0.5}
=
2~\text{Hz}.
$$

---

## 15. Experiment 2 — One-Shot Timer

Create:

```cpp
TimerHandle_t oneShotTimer;
```

Callback:

```cpp
void oneShotCallback(
    TimerHandle_t xTimer
) {

  Serial.println(
    "One-shot timer expired"
  );
}
```

Create:

```cpp
oneShotTimer =
    xTimerCreate(
      "One Shot",
      pdMS_TO_TICKS(3000),
      pdFALSE,
      NULL,
      oneShotCallback
    );
```

The important parameter is:

```text
pdFALSE
```

which means:

```text
Do Not Auto-Reload
```

---

## 16. Expected One-Shot Behavior

```text
Program Starts
      ↓
3 Seconds
      ↓
Callback
      ↓
No Further Callback
```

The timer executes only once unless it is started again.

---

## 17. Experiment 3 — Multiple Software Timers

Create three timers:

```text
Timer A → 250 ms
Timer B → 500 ms
Timer C → 1000 ms
```

Define:

```cpp
TimerHandle_t timerA;
TimerHandle_t timerB;
TimerHandle_t timerC;
```

Callbacks:

```cpp
void callbackA(
    TimerHandle_t xTimer
) {

  Serial.println(
    "Timer A"
  );
}

void callbackB(
    TimerHandle_t xTimer
) {

  Serial.println(
    "Timer B"
  );
}

void callbackC(
    TimerHandle_t xTimer
) {

  Serial.println(
    "Timer C"
  );
}
```

---

## 18. Multiple Timer Creation

```cpp
timerA =
    xTimerCreate(
      "A",
      pdMS_TO_TICKS(250),
      pdTRUE,
      NULL,
      callbackA
    );

timerB =
    xTimerCreate(
      "B",
      pdMS_TO_TICKS(500),
      pdTRUE,
      NULL,
      callbackB
    );

timerC =
    xTimerCreate(
      "C",
      pdMS_TO_TICKS(1000),
      pdTRUE,
      NULL,
      callbackC
    );
```

Start:

```cpp
xTimerStart(
  timerA,
  0
);

xTimerStart(
  timerB,
  0
);

xTimerStart(
  timerC,
  0
);
```

---

## 19. Expected Timer Timeline

Conceptually:

```text
Time → 0    250   500   750   1000   1250   1500

A          A     A     A      A      A      A
B                B            B             B
C                             C
```

This demonstrates that several periodic activities can be scheduled independently.

---

## 20. Experiment 4 — Changing Timer Period at Runtime

Initially:

```text
Timer Period = 1000 ms
```

After several seconds, change to:

```text
Timer Period = 200 ms
```

Use:

```cpp
xTimerChangePeriod(
    ledTimer,
    pdMS_TO_TICKS(200),
    0
);
```

Observe the change in callback frequency.

---

## 21. Timer Period Relationship

If:

$$
T=1000~\text{ms},
$$

then:

$$
f=1~\text{Hz}.
$$

After changing to:

$$
T=200~\text{ms},
$$

the frequency becomes:

$$
f=
\frac{1}{0.2}
=
5~\text{Hz}.
$$

---

## 22. Experiment 5 — Timer vs. `vTaskDelay()`

Create one task using:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(500)
);
```

Create one timer using:

```text
Period = 500 ms
```

Observe both.

### Task Method

```text
Task
 ↓
Execute
 ↓
Delay
 ↓
Execute
```

### Timer Method

```text
Timer Service
    ↓
Period Expires
    ↓
Callback
```

---

## 23. Timer vs. Task Comparison

| Feature | Software Timer | FreeRTOS Task |
|---|---|---|
| Separate task stack | No application task stack per timer | Yes |
| Suitable for short periodic action | Yes | Yes |
| Suitable for long computation | No | Yes |
| Can block safely | Callback should not block | Task can block |
| Memory overhead | Usually lower | Higher |
| Independent complex logic | Limited | Good |

A useful design guideline is:

```text
Short Periodic Action
        ↓
Software Timer

Long / Blocking Activity
        ↓
Dedicated Task
```

---

## 24. Heap Memory in ESP32

The ESP32 uses heap memory for dynamic allocations.

Heap may be consumed by:

```text
FreeRTOS Tasks
Queues
Timers
Dynamic Objects
Network Libraries
Buffers
Application Data
```

Available heap can change while the program runs.

---

## 25. Check Free Heap

In Arduino ESP32, use:

```cpp
ESP.getFreeHeap()
```

Example:

```cpp
Serial.print(
  "Free Heap = "
);

Serial.println(
  ESP.getFreeHeap()
);
```

The result is typically given in:

```text
Bytes
```

---

## 26. Experiment 6 — Monitor Free Heap Periodically

Create a timer:

```cpp
TimerHandle_t heapTimer;
```

Callback:

```cpp
void heapTimerCallback(
    TimerHandle_t xTimer
) {

  Serial.print(
    "Free Heap = "
  );

  Serial.print(
    ESP.getFreeHeap()
  );

  Serial.println(
    " bytes"
  );
}
```

Create:

```cpp
heapTimer =
    xTimerCreate(
      "Heap Timer",
      pdMS_TO_TICKS(2000),
      pdTRUE,
      NULL,
      heapTimerCallback
    );
```

---

## 27. Heap Monitoring Architecture

```text
Application
     ↓
Tasks + Queues + Timers
     ↓
Heap Consumption
     ↓
Heap Monitor Timer
     ↓
ESP.getFreeHeap()
     ↓
Serial Monitor
```

This allows memory usage to be observed continuously.

---

## 28. Experiment 7 — Measure Baseline Heap

Immediately after boot, record:

```cpp
uint32_t baselineHeap =
    ESP.getFreeHeap();
```

Display:

```cpp
Serial.print(
  "Baseline Free Heap = "
);

Serial.println(
  baselineHeap
);
```

Record:

| Stage | Free Heap |
|---|---:|
| Startup |  |
| After Timer Creation |  |
| After Task Creation |  |
| After Queue Creation |  |

This shows how FreeRTOS objects consume memory.

---

## 29. Experiment 8 — Heap Usage After Creating Tasks

Record heap:

```cpp
Serial.println(
  ESP.getFreeHeap()
);
```

before creating tasks.

Then create:

```text
Task 1
Task 2
Task 3
```

Measure again.

Define heap consumption as:

$$
M_{\text{used}}
=
H_{\text{before}}
-
H_{\text{after}},
$$

where:

- $H_{\text{before}}$ = free heap before object creation
- $H_{\text{after}}$ = free heap after object creation

---

## 30. Task Memory Study

Create tasks with different stack sizes.

For example:

```text
Task A → 2048
Task B → 4096
Task C → 8192
```

Record:

| Task Configuration | Free Heap |
|---|---:|
| No Task |  |
| 2048 Stack |  |
| 4096 Stack |  |
| 8192 Stack |  |

Observe how stack allocation affects heap availability.

---

## 31. Experiment 9 — Heap Usage After Queue Creation

Create:

```cpp
QueueHandle_t queue;
```

Test queue lengths:

```text
5
10
20
50
```

For each case:

```cpp
queue =
    xQueueCreate(
      queueLength,
      sizeof(int)
    );
```

Measure free heap.

Record:

| Queue Length | Item Size | Free Heap | Approx. Heap Used |
|---:|---:|---:|---:|
| 5 | `sizeof(int)` |  |  |
| 10 | `sizeof(int)` |  |  |
| 20 | `sizeof(int)` |  |  |
| 50 | `sizeof(int)` |  |  |

---

## 32. Queue Memory Relationship

The approximate data-storage requirement grows with:

$$
M_Q
\propto
N_Q
\times
S_{\text{item}},
$$

where:

- $N_Q$ = queue length
- $S_{\text{item}}$ = size of each queue item

There is also kernel-management overhead.

Therefore:

```text
Longer Queue
    ↓
More Heap Usage
```

---

## 33. Experiment 10 — Heap Usage After Timer Creation

Create:

```text
1 Timer
5 Timers
10 Timers
20 Timers
```

Measure heap after each configuration.

Record:

| Number of Timers | Free Heap |
|---:|---:|
| 0 |  |
| 1 |  |
| 5 |  |
| 10 |  |
| 20 |  |

This demonstrates that software timers also consume memory resources.

---

## 34. Experiment 11 — Dynamic Memory Allocation

Allocate memory:

```cpp
int *buffer =
    new int[1000];
```

Measure:

```cpp
Serial.println(
  ESP.getFreeHeap()
);
```

Then release:

```cpp
delete[] buffer;
```

Measure again.

---

## 35. Dynamic Allocation Flow

```text
Before Allocation
      ↓
Free Heap = H1
      ↓
new[]
      ↓
Free Heap = H2
      ↓
delete[]
      ↓
Free Heap ≈ H1
```

Ideally:

$$
H_{\text{after delete}}
\approx
H_{\text{before allocation}}.
$$

Small differences may occur because of allocator behavior and other runtime activity.

---

## 36. Experiment 12 — Demonstrate a Memory Leak

For demonstration only, allocate repeatedly without freeing:

```cpp
void leakMemory() {

  int *data =
      new int[100];

  data[0] = 1;
}
```

Call repeatedly:

```text
Call 1
 ↓
Allocate

Call 2
 ↓
Allocate

Call 3
 ↓
Allocate

No delete[]
```

Observe:

```text
Free Heap
   ↓
Decreases
   ↓
Decreases
   ↓
Decreases
```

This demonstrates a **memory leak**.

---

## 37. Memory Leak Definition

A memory leak occurs when dynamically allocated memory is no longer needed but is never returned to the heap.

Conceptually:

```text
Allocate Memory
      ↓
Lose Reference
      ↓
Memory Cannot Be Reused
      ↓
Available Heap Decreases
```

A simple leak indicator is:

$$
H(t+\Delta t)<H(t)
$$

repeatedly over time without a legitimate increase in workload.

---

## 38. Experiment 13 — Fix the Memory Leak

Modify:

```cpp
void safeMemory() {

  int *data =
      new int[100];

  data[0] = 1;

  delete[] data;
}
```

Observe that free heap should remain approximately stable.

Compare:

| Iteration | Leak Version | Safe Version |
|---:|---:|---:|
| 1 |  |  |
| 10 |  |  |
| 50 |  |  |
| 100 |  |  |

---

## 39. Heap Fragmentation

Even when memory is released, repeated allocations of different sizes can fragment the heap.

Conceptually:

```text
Before:

[ Free Free Free Free Free Free ]

After Several Allocations:

[Used][Free][Used][Free][Used][Free]
```

The total free memory may still be large, but a sufficiently large contiguous allocation can fail if free blocks are fragmented.

---

## 40. Largest Free Block

On ESP32 platforms where the API is available, the largest available heap block can be inspected in addition to total free heap.

Conceptually compare:

```text
Total Free Heap
        ↓
How Much Memory Is Free?

Largest Free Block
        ↓
What Is the Largest Contiguous Allocation Possible?
```

This helps distinguish general memory consumption from fragmentation.

---

## 41. Experiment 14 — Vary Workload

Create three workload levels.

### Workload A — Light

```text
1 Timer
1 Task
1 Small Queue
```

### Workload B — Medium

```text
3 Timers
3 Tasks
2 Queues
```

### Workload C — Heavy

```text
8 Timers
6 Tasks
4 Larger Queues
Dynamic Buffers
```

Measure:

- Free heap
- Timer callback frequency
- Queue occupancy
- Task responsiveness

---

## 42. Workload Table

Record:

| Workload | Tasks | Timers | Queues | Free Heap | Observation |
|---|---:|---:|---:|---:|---|
| Light | 1 | 1 | 1 |  |  |
| Medium | 3 | 3 | 2 |  |  |
| Heavy | 6 | 8 | 4 |  |  |

Calculate relative heap consumption:

$$
M_{\text{used}}
=
H_{\text{baseline}}
-
H_{\text{workload}}.
$$

---

## 43. Experiment 15 — Timer Callback Workload

Create three versions of a timer callback.

### Version A — Very Short

```cpp
counter++;
```

### Version B — Moderate

```cpp
for (
  volatile int i = 0;
  i < 10000;
  i++
) {
}
```

### Version C — Heavy

```cpp
for (
  volatile int i = 0;
  i < 1000000;
  i++
) {
}
```

Observe timer timing behavior.

---

## 44. Why Long Timer Callbacks Are a Problem

Timer callbacks are serviced through the FreeRTOS timer service context.

If one callback runs too long:

```text
Timer A Callback
      ↓
Long Execution
      ↓
Timer B Callback Delayed
      ↓
Timer C Callback Delayed
```

Therefore:

$$
T_{\text{callback}}
\ll
T_{\text{timer}}
$$

is a useful practical design goal.

---

## 45. Experiment 16 — Measure Callback Execution Time

Inside the callback:

```cpp
unsigned long startTime =
    micros();

// callback work

unsigned long endTime =
    micros();

unsigned long executionTime =
    endTime - startTime;
```

Mathematically:

$$
T_{\text{callback}}
=
t_{\text{end}}
-
t_{\text{start}}.
$$

Record:

| Callback Workload | Execution Time |
|---|---:|
| Light |  |
| Medium |  |
| Heavy |  |

---

## 46. Experiment 17 — Monitor Task Stack Usage

FreeRTOS provides stack high-water-mark information.

Example:

```cpp
UBaseType_t stackRemaining =
    uxTaskGetStackHighWaterMark(
      NULL
    );
```

This indicates approximately how much stack space has remained unused at the lowest point reached by the task.

A small high-water mark indicates that the task has approached its stack limit.

---

## 47. Stack Monitoring Example

Inside a task:

```cpp
void MonitorTask(
    void *parameter
) {

  while (1) {

    UBaseType_t highWaterMark =
        uxTaskGetStackHighWaterMark(
          NULL
        );

    Serial.print(
      "Stack high-water mark = "
    );

    Serial.println(
      highWaterMark
    );

    vTaskDelay(
      pdMS_TO_TICKS(2000)
    );
  }
}
```

---

## 48. Heap vs. Stack

These concepts are different.

### Heap

Used for:

```text
Dynamic Allocation
Tasks
Queues
Timers
Buffers
Objects
```

### Task Stack

Used for:

```text
Local Variables
Function Calls
Return Addresses
Task Execution Context
```

Conceptually:

```text
System Memory
   ├── Heap
   └── Task Stacks
```

Monitoring both is useful when analyzing embedded memory usage.

---

## 49. Experiment 18 — Combined Timer and Heap Monitor

Create:

```text
Timer 1
LED Toggle
500 ms

Timer 2
Sensor Trigger
1000 ms

Timer 3
Memory Monitor
2000 ms
```

Architecture:

```text
          ┌→ LED Timer
          │
FreeRTOS ─┼→ Sensor Timer
          │
          └→ Heap Monitor Timer
                  ↓
            Free Heap Report
```

---

## 50. Recommended Sensor Design

Instead of performing a long sensor transaction directly inside the timer callback, the timer can notify or trigger a worker task.

Conceptually:

```text
Sensor Timer
    ↓
Short Callback
    ↓
Signal Worker Task
    ↓
Sensor Task
    ↓
Acquire Sensor
```

This prevents the timer service from being blocked by slow operations.

---

## 51. Experiment 19 — Timer-Triggered Sensor Task

Create a binary semaphore:

```cpp
SemaphoreHandle_t sensorTrigger;
```

Timer callback:

```cpp
void sensorTimerCallback(
    TimerHandle_t xTimer
) {

  xSemaphoreGive(
    sensorTrigger
  );
}
```

Worker task:

```cpp
void SensorTask(
    void *parameter
) {

  while (1) {

    if (
      xSemaphoreTake(
        sensorTrigger,
        portMAX_DELAY
      ) == pdTRUE
    ) {

      int value =
          analogRead(34);

      Serial.print(
        "Sensor = "
      );

      Serial.println(
        value
      );
    }
  }
}
```

This is a better design for operations that may take noticeable execution time.

---

## 52. Experiment 20 — Complete Timer and Memory-Monitoring Application

The architecture is:

```text
Timer A
500 ms
   ↓
LED Callback

Timer B
1000 ms
   ↓
Signal Sensor Task
   ↓
Sensor Acquisition

Timer C
2000 ms
   ↓
Memory Monitor
   ↓
Free Heap Report

Sensor Task
   ↓
Queue
   ↓
Processing Task
```

This combines concepts from the previous labs.

---

## 53. Integrated System Architecture

```text
                    ┌──────────────┐
                    │ LED Timer    │
                    │ 500 ms       │
                    └──────┬───────┘
                           ↓
                         LED

                    ┌──────────────┐
                    │ Sensor Timer │
                    │ 1000 ms      │
                    └──────┬───────┘
                           ↓
                       Semaphore
                           ↓
                      Sensor Task
                           ↓
                         Queue
                           ↓
                    Processing Task

                    ┌──────────────┐
                    │ Heap Timer   │
                    │ 2000 ms      │
                    └──────┬───────┘
                           ↓
                   ESP.getFreeHeap()
                           ↓
                     Serial Monitor
```

---

## 54. Complete Example Program

```cpp
#include <Arduino.h>

#define LED_PIN 2
#define SENSOR_PIN 34

TimerHandle_t ledTimer;
TimerHandle_t sensorTimer;
TimerHandle_t heapTimer;

SemaphoreHandle_t sensorTrigger;
QueueHandle_t sensorQueue;

struct SensorData {

  int raw;

  unsigned long timestamp;
};

void ledTimerCallback(
    TimerHandle_t xTimer
) {

  digitalWrite(
    LED_PIN,
    !digitalRead(LED_PIN)
  );
}

void sensorTimerCallback(
    TimerHandle_t xTimer
) {

  xSemaphoreGive(
    sensorTrigger
  );
}

void heapTimerCallback(
    TimerHandle_t xTimer
) {

  Serial.print(
    "Free Heap = "
  );

  Serial.print(
    ESP.getFreeHeap()
  );

  Serial.println(
    " bytes"
  );
}

void SensorTask(
    void *parameter
) {

  SensorData data;

  while (1) {

    if (
      xSemaphoreTake(
        sensorTrigger,
        portMAX_DELAY
      ) == pdTRUE
    ) {

      data.raw =
          analogRead(
            SENSOR_PIN
          );

      data.timestamp =
          millis();

      xQueueSend(
        sensorQueue,
        &data,
        pdMS_TO_TICKS(50)
      );
    }
  }
}

void ProcessingTask(
    void *parameter
) {

  SensorData data;

  while (1) {

    if (
      xQueueReceive(
        sensorQueue,
        &data,
        portMAX_DELAY
      ) == pdTRUE
    ) {

      float percentage =
          data.raw
          / 4095.0
          * 100.0;

      Serial.print(
        "Sensor = "
      );

      Serial.print(
        data.raw
      );

      Serial.print(
        " | Level = "
      );

      Serial.print(
        percentage,
        1
      );

      Serial.print(
        "% | Time = "
      );

      Serial.println(
        data.timestamp
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  pinMode(
    SENSOR_PIN,
    INPUT
  );

  Serial.print(
    "Initial Free Heap = "
  );

  Serial.println(
    ESP.getFreeHeap()
  );

  sensorTrigger =
      xSemaphoreCreateBinary();

  sensorQueue =
      xQueueCreate(
        10,
        sizeof(SensorData)
      );

  if (
    sensorTrigger == NULL ||
    sensorQueue == NULL
  ) {

    Serial.println(
      "RTOS object creation failed"
    );

    while (1) {
    }
  }

  xTaskCreatePinnedToCore(
    SensorTask,
    "Sensor Task",
    3072,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    ProcessingTask,
    "Processing Task",
    3072,
    NULL,
    1,
    NULL,
    1
  );

  ledTimer =
      xTimerCreate(
        "LED",
        pdMS_TO_TICKS(500),
        pdTRUE,
        NULL,
        ledTimerCallback
      );

  sensorTimer =
      xTimerCreate(
        "Sensor",
        pdMS_TO_TICKS(1000),
        pdTRUE,
        NULL,
        sensorTimerCallback
      );

  heapTimer =
      xTimerCreate(
        "Heap",
        pdMS_TO_TICKS(2000),
        pdTRUE,
        NULL,
        heapTimerCallback
      );

  if (
    ledTimer == NULL ||
    sensorTimer == NULL ||
    heapTimer == NULL
  ) {

    Serial.println(
      "Timer creation failed"
    );

    while (1) {
    }
  }

  xTimerStart(
    ledTimer,
    0
  );

  xTimerStart(
    sensorTimer,
    0
  );

  xTimerStart(
    heapTimer,
    0
  );

  Serial.print(
    "Free Heap After Setup = "
  );

  Serial.println(
    ESP.getFreeHeap()
  );
}

void loop() {
}
```

---

## 55. Workload Analysis

Define three workload levels.

### Light Workload

```text
1 Timer
1 Task
Queue Length = 5
```

### Medium Workload

```text
3 Timers
3 Tasks
Queue Length = 10
```

### Heavy Workload

```text
8 Timers
6 Tasks
Multiple Queues
Dynamic Buffers
```

Observe:

$$
H_{\text{light}},
\quad
H_{\text{medium}},
\quad
H_{\text{heavy}}.
$$

Normally:

$$
H_{\text{light}}
>
H_{\text{medium}}
>
H_{\text{heavy}},
$$

assuming the heavier configurations allocate more runtime memory.

---

## 56. Memory Utilization

If baseline free heap is:

$$
H_0,
$$

and current free heap is:

$$
H,
$$

then approximate heap consumption is:

$$
M_{\text{used}}
=
H_0-H.
$$

The relative change can be expressed as:

$$
M_{\%}
=
\frac{H_0-H}{H_0}
\times100\%.
$$

---

## 57. Timer Timing Error

Suppose the expected callback interval is:

$$
T_d.
$$

The measured interval is:

$$
T_i=t_i-t_{i-1}.
$$

Timing error is:

$$
E_i
=
T_i-T_d.
$$

A simple absolute jitter metric is:

$$
J_i
=
|T_i-T_d|.
$$

Record:

| Sample | Desired Period | Measured Period | Error |
|---:|---:|---:|---:|
| 1 | 500 ms |  |  |
| 2 | 500 ms |  |  |
| 3 | 500 ms |  |  |
| 4 | 500 ms |  |  |
| 5 | 500 ms |  |  |

---

## 58. Common Timer Errors

### Creating but Not Starting the Timer

```cpp
xTimerCreate(...)
```

does not automatically mean that the timer is running.

Use:

```cpp
xTimerStart(...)
```

### Long Blocking Callback

Avoid:

```cpp
void callback(
    TimerHandle_t timer
) {

  delay(5000);
}
```

This can interfere with other software timer callbacks.

### Performing Heavy Processing in Callback

Prefer:

```text
Timer Callback
     ↓
Signal Task
     ↓
Task Performs Work
```

instead of:

```text
Timer Callback
     ↓
Long Processing
```

### Forgetting to Check Timer Creation

Always check:

```cpp
if (
  timer == NULL
) {

  // Handle error
}
```

---

## 59. Common Memory Errors

### Memory Leak

```text
new
 ↓
No delete
 ↓
Heap Decreases
```

### Oversized Task Stack

```text
Very Large Stack
      ↓
Unused Memory
      ↓
Reduced Free Heap
```

### Undersized Task Stack

```text
Very Small Stack
      ↓
Possible Stack Overflow
```

### Oversized Queue

```text
Large Queue
     ↓
More Buffer Memory
```

### Excessive Dynamic Allocation

Frequent allocation and release can increase fragmentation risk.

---

## 60. Design Guidelines

A robust embedded application should:

1. Keep timer callbacks short.
2. Use tasks for long or blocking operations.
3. Use queues for data transfer.
4. Use semaphores for event signaling.
5. Use mutexes for shared-resource protection.
6. Monitor free heap during development.
7. Monitor task stack high-water marks.
8. Avoid unnecessary dynamic allocation.
9. Delete allocated memory when no longer needed.
10. Select queue and stack sizes according to actual requirements.

This can be summarized as:

$$
\boxed{
\text{Robust RTOS Design}
=
\text{Correct Timing}
+
\text{Safe Communication}
+
\text{Controlled Memory Usage}
}
$$

---

## 61. Experiments Summary

### Experiment 1 — Auto-Reload Timer

Blink an LED periodically.

### Experiment 2 — One-Shot Timer

Execute a delayed action once.

### Experiment 3 — Multiple Timers

Run timers at 250, 500, and 1000 ms.

### Experiment 4 — Dynamic Timer Period

Change timer frequency during runtime.

### Experiment 5 — Timer vs. Task

Compare software timers with `vTaskDelay()`.

### Experiment 6 — Heap Monitor

Report free heap periodically.

### Experiment 7 — Baseline Heap

Measure heap before and after RTOS object creation.

### Experiment 8 — Task Memory

Study task stack-size effects.

### Experiment 9 — Queue Memory

Study queue-length effects.

### Experiment 10 — Timer Memory

Measure memory consumed by multiple timers.

### Experiment 11 — Dynamic Allocation

Allocate and release memory.

### Experiment 12 — Memory Leak

Observe continuously decreasing free heap.

### Experiment 13 — Leak Correction

Use proper memory deallocation.

### Experiment 14 — Workload Variation

Compare light, medium, and heavy RTOS configurations.

### Experiment 15 — Callback Workload

Observe timer behavior under longer callbacks.

### Experiment 16 — Callback Timing

Measure timer callback execution time.

### Experiment 17 — Stack Monitoring

Inspect task stack high-water marks.

### Experiment 18 — Timer and Heap Monitor

Combine several periodic timers.

### Experiment 19 — Timer-Triggered Task

Use a timer to trigger sensor acquisition.

### Experiment 20 — Integrated Application

Combine timers, tasks, queue, semaphore, sensor acquisition, processing, and heap monitoring.

---

## 62. Exercises

### Exercise 1 — Periodic LED Timer

Create a software timer that toggles an LED every:

```text
500 ms
```

Measure the actual period.

### Exercise 2 — Multiple Timers

Create:

```text
Timer A = 200 ms
Timer B = 500 ms
Timer C = 1000 ms
```

Record the callback timestamps.

### Exercise 3 — One-Shot Alarm

Create a one-shot timer that prints:

```text
ALARM
```

five seconds after program start.

### Exercise 4 — Timer Period Change

Start at:

```text
1000 ms
```

and change to:

```text
250 ms
```

after 10 seconds.

### Exercise 5 — Heap Monitor

Report:

```text
Free Heap
```

every two seconds.

Run the system for at least 60 seconds.

Determine whether heap usage remains stable.

### Exercise 6 — Queue Memory

Compare queue lengths:

```text
5
10
20
50
```

Measure the free heap after each queue is created.

### Exercise 7 — Task Stack Study

Create tasks with:

```text
2048
4096
8192
```

stack configurations.

Record free heap and stack high-water mark.

### Exercise 8 — Memory Leak

Create a controlled memory leak.

Record free heap for 20 iterations.

Then correct the code and repeat the experiment.

### Exercise 9 — Workload Comparison

Implement:

```text
Light
Medium
Heavy
```

workloads.

Record:

- Number of tasks
- Number of timers
- Number of queues
- Free heap
- Timer timing error

### Exercise 10 — Timer-Triggered Sensor Pipeline

Construct:

```text
Software Timer
      ↓
Semaphore
      ↓
Sensor Task
      ↓
Queue
      ↓
Processing Task
```

Monitor free heap simultaneously.

---

## 63. Checkpoint Questions

1. What is a FreeRTOS software timer?
2. How is a software timer different from a hardware timer?
3. What is an auto-reload timer?
4. What is a one-shot timer?
5. Which function creates a FreeRTOS timer?
6. Which function starts a timer?
7. What is a timer callback?
8. Why should a timer callback be short?
9. Why should long blocking operations be avoided in callbacks?
10. What does `pdTRUE` mean when creating a timer?
11. What does `pdFALSE` mean?
12. What does `xTimerChangePeriod()` do?
13. When is a software timer preferable to a dedicated task?
14. When is a task preferable to a timer callback?
15. What is heap memory?
16. What types of FreeRTOS objects may use heap memory?
17. What does `ESP.getFreeHeap()` return?
18. Why should free heap be monitored during development?
19. How does task stack size affect free heap?
20. How does queue length affect memory usage?
21. What is dynamic memory allocation?
22. What is a memory leak?
23. Why does a memory leak reduce available heap?
24. What is heap fragmentation?
25. What is the difference between heap and task stack?
26. What does `uxTaskGetStackHighWaterMark()` help determine?
27. Why can a very small task stack be dangerous?
28. Why can a very large task stack be inefficient?
29. Why can a long timer callback delay other timers?
30. Why is a timer-triggered worker task often better for sensor acquisition?

---

## 64. Expected Results

Students should observe that software timers provide an efficient way to execute short periodic activities.

For an auto-reload timer:

```text
Start
 ↓
Period
 ↓
Callback
 ↓
Period
 ↓
Callback
```

For a one-shot timer:

```text
Start
 ↓
Period
 ↓
Callback
 ↓
Stop
```

Heap monitoring should demonstrate that creating additional:

```text
Tasks
Queues
Timers
Buffers
```

reduces available heap.

Under stable operation:

```text
Free Heap
   ↓
Approximately Stable
```

after initialization.

A memory leak should instead produce:

```text
Free Heap
   ↓
Continuous Decrease
```

The lab should also demonstrate that long timer callbacks can reduce timing quality and delay other timer events.

---

## 65. Submission

Submit:

- Auto-reload timer source code.
- One-shot timer source code.
- Multiple-timer experiment.
- Timer-period modification experiment.
- Serial Monitor screenshots.
- Baseline heap measurement.
- Task memory experiment.
- Queue memory experiment.
- Timer memory experiment.
- Dynamic allocation experiment.
- Memory-leak and corrected versions.
- Stack high-water-mark measurements.
- Light/medium/heavy workload comparison.
- Timer callback timing results.
- Integrated timer-sensor-queue application.
- Brief discussion of timing and memory trade-offs.

Suggested timer table:

| Timer | Desired Period | Average Measured Period | Average Error |
|---|---:|---:|---:|
| A | 250 ms |  |  |
| B | 500 ms |  |  |
| C | 1000 ms |  |  |

Suggested memory table:

| Configuration | Tasks | Timers | Queues | Free Heap |
|---|---:|---:|---:|---:|
| Baseline | 0 | 0 | 0 |  |
| Light | 1 | 1 | 1 |  |
| Medium | 3 | 3 | 2 |  |
| Heavy | 6 | 8 | 4 |  |

Suggested stack table:

| Task | Allocated Stack | High-Water Mark | Observation |
|---|---:|---:|---|
| Task A | 2048 |  |  |
| Task B | 4096 |  |  |
| Task C | 8192 |  |  |

---

## 66. Discussion

Students should explain how FreeRTOS software timers can execute short periodic functions without creating a separate application task for every periodic event.

The fundamental timer model is:

```text
Timer
  ↓
Expiration
  ↓
Callback
```

However, timer callbacks should remain short because several software timers share timer-service resources. A callback that performs a long computation or blocking operation can delay other callbacks.

The memory experiments should show that embedded software must operate within finite RAM. Adding tasks, queues, timers, and buffers consumes memory and reduces free heap. Therefore, memory usage should be monitored during both development and stress testing.

The main design relationship can be summarized as:

$$
\boxed{
\text{Real-Time Reliability}
=
\text{Timing Discipline}
+
\text{Memory Discipline}
}
$$

Students should also explain why stable free heap is generally desirable after initialization, while a continuously declining heap value can indicate a memory-management problem.

---

## 67. Conclusion

In this lab, FreeRTOS software timers are used to implement periodic and one-shot activities on the ESP32. Students investigate timer creation, callbacks, auto-reload behavior, period modification, multiple timers, and the effect of callback workload on timing.

The lab also introduces embedded memory monitoring through free-heap measurements and task stack high-water marks. By varying the number of tasks, timers, queues, buffers, and dynamic allocations, students observe how system workload affects available memory.

The overall workflow is:

```text
Create Timers
     ↓
Execute Periodic Events
     ↓
Measure Timing
     ↓
Increase Workload
     ↓
Monitor Heap
     ↓
Monitor Task Stack
     ↓
Identify Memory Problems
     ↓
Optimize RTOS Design
```

The progression across the RTOS labs is:

```text
LAB 5
Tasks & Priorities
      ↓
Preemptive Scheduling
      ↓
LAB 6
Shared Resources
      ↓
Mutex Protection
      ↓
LAB 7
Producer–Consumer
      ↓
FreeRTOS Queues
      ↓
LAB 8
Periodic Events
      ↓
Software Timers
      ↓
Memory Monitoring
```

The RTOS foundation can therefore be summarized as:

$$
\boxed{
\text{RTOS System Design}
=
\text{Scheduling}
+
\text{Synchronization}
+
\text{Communication}
+
\text{Timing}
+
\text{Memory Management}
}
$$

This prepares students for the next stage of embedded-system development, where the RTOS foundation can be integrated with **Wi-Fi networking, HTTP communication, MQTT, BLE, and low-power operation**.
