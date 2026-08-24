
# 📘 Theory: Software Timers, RTOS Memory Management, and Dynamic vs. Static Allocation

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Real-time embedded systems often need to perform operations at specific time intervals while efficiently managing limited memory resources. In an ESP32 system running **FreeRTOS**, two important mechanisms help achieve these goals:

1. **Software timers** — execute callback functions after a specified period.
2. **RTOS memory management** — allocates memory for tasks, queues, semaphores, timers, and other kernel objects.

Memory can generally be allocated using two approaches:

- **Dynamic allocation**
- **Static allocation**

These concepts are closely related to system reliability, timing predictability, RAM utilization, and real-time performance.

The overall relationship can be summarized as:

$$
\boxed{
\text{FreeRTOS Application} =
\text{Tasks}
+
\text{Timers}
+
\text{RTOS Objects}
+
\text{Memory Management}
}
$$

For ESP32 applications, understanding these mechanisms is important when developing reliable systems such as:

- IoT devices,
- sensor nodes,
- industrial controllers,
- smart-home devices,
- robotic systems,
- data acquisition systems,
- edge-computing devices.

---

# ⏱️ 2. What Is a Software Timer?

A **software timer** is an RTOS service that allows an application to execute a function after a specified time interval.

Unlike a hardware timer, a software timer does not directly depend on a dedicated hardware counter for each timer instance.

Conceptually:

```text
Start Timer
    │
    ▼
Wait for Period
    │
    ▼
Timer Expires
    │
    ▼
Callback Function
```

Therefore,

$$
\boxed{
\text{Software Timer} =
\text{Time-Based RTOS Event}
}
$$

---

# ⚙️ 3. Why Use Software Timers?

Suppose an ESP32 application needs to:

- blink an LED every 500 ms,
- sample system status every second,
- trigger a timeout after 5 seconds,
- periodically update a display,
- check Wi-Fi connectivity every 10 seconds.

One approach is to create a separate task for each operation.

For example:

```text
LED Task
Sensor Task
Display Task
Wi-Fi Task
```

However, creating many simple periodic tasks consumes additional RAM because every task requires its own stack and task-control structure.

Software timers provide a lightweight alternative for suitable short operations.

```text
FreeRTOS Timer Service
        │
        ├── LED Timer
        ├── Sensor Timer
        ├── Display Timer
        └── Wi-Fi Timer
```

---

# 🔄 4. Software Timer Architecture

A simplified FreeRTOS timer architecture is:

```text
Application Task
      │
      ▼
Timer Command
      │
      ▼
┌───────────────────┐
│ Timer Command     │
│ Queue             │
└───────────────────┘
      │
      ▼
┌───────────────────┐
│ Timer Service     │
│ Task              │
└───────────────────┘
      │
      ▼
Timer Callback
```

The application normally does not directly execute the timer callback.

Instead, the FreeRTOS **timer service task**, sometimes called the **timer daemon task**, manages software timer events.

---

# 🧠 5. Software Timer Components

A software timer generally contains:

- timer handle,
- timer name,
- timer period,
- auto-reload setting,
- timer ID,
- callback function.

Conceptually:

```text
Software Timer
├── Name
├── Period
├── Reload Mode
├── Timer ID
└── Callback Function
```

---

# 🕒 6. Timer Period

The timer period determines how long the RTOS waits before the timer expires.

For example:

$$
T=1000~ms
$$

means that the timer expires approximately once every:

$$
1~s.
$$

In FreeRTOS, milliseconds are normally converted into operating-system ticks:

```cpp
pdMS_TO_TICKS(1000)
```

Therefore:

```cpp
pdMS_TO_TICKS(1000)
```

represents approximately one second according to the configured RTOS tick rate.

---

# 🔢 7. RTOS Tick

FreeRTOS scheduling and software timer timing are based on the **RTOS tick**.

If the tick frequency is:

$$
f_{tick}
$$

then the nominal tick period is:

$$
T_{tick}=\frac{1}{f_{tick}}.
$$

For example, if:

$$
f_{tick}=1000~Hz,
$$

then:

$$
T_{tick}=1~ms.
$$

A requested delay of:

$$
500~ms
$$

corresponds nominally to:

$$
N_{tick}=500
$$

ticks for a 1-kHz tick rate.

The actual tick configuration depends on the ESP32/FreeRTOS environment.

---

# 🔁 8. Two Types of Software Timers

FreeRTOS software timers can generally operate as:

1. **One-shot timers**
2. **Auto-reload timers**

---

# 1️⃣ 9. One-Shot Timer

A one-shot timer expires only once after being started.

```text
Start
  │
  ▼
Wait T
  │
  ▼
Callback
  │
  ▼
Stop
```

Mathematically:

$$
t_{event}=t_{start}+T.
$$

Example application:

```text
Button Press
     │
     ▼
Start 5-s Timer
     │
     ▼
5 Seconds
     │
     ▼
Turn Off LED
```

One-shot timers are useful for:

- communication timeout,
- delayed shutdown,
- temporary alarms,
- button timeout,
- connection timeout.

---

# 🔄 10. Auto-Reload Timer

An auto-reload timer automatically restarts after expiration.

```text
Start
  │
  ▼
Wait T
  │
  ▼
Callback
  │
  ▼
Wait T
  │
  ▼
Callback
  │
  ▼
 ...
```

Ideally, callbacks occur periodically:

$$
t_n=t_0+nT.
$$

This is useful for:

- periodic LED updates,
- sensor status checks,
- watchdog-like application checks,
- periodic logging,
- telemetry updates.

---

# 📊 11. One-Shot vs. Auto-Reload Timer

| Feature | One-Shot | Auto-Reload |
|---|---|---|
| Number of expirations | One | Repeated |
| Automatic restart | No | Yes |
| Timeout detection | Excellent | Possible |
| Periodic operation | No | Yes |
| LED blinking | Possible but inconvenient | Suitable |
| Connection timeout | Suitable | Usually unnecessary |
| Periodic monitoring | No | Suitable |

---

# 🛠️ 12. Creating a Software Timer

In FreeRTOS, a dynamically allocated software timer can be created using:

```cpp
xTimerCreate()
```

Example:

```cpp
TimerHandle_t ledTimer;

ledTimer = xTimerCreate(
    "LED Timer",
    pdMS_TO_TICKS(500),
    pdTRUE,
    NULL,
    ledTimerCallback
);
```

The parameters represent:

```text
Timer Name
     │
Timer Period
     │
Auto Reload
     │
Timer ID
     │
Callback Function
```

---

# 📋 13. Timer Creation Parameters

The general form is:

```cpp
xTimerCreate(
    pcTimerName,
    xTimerPeriod,
    uxAutoReload,
    pvTimerID,
    pxCallbackFunction
);
```

| Parameter | Purpose |
|---|---|
| `pcTimerName` | Timer name |
| `xTimerPeriod` | Timer period in ticks |
| `uxAutoReload` | One-shot or periodic operation |
| `pvTimerID` | User-defined timer identifier |
| `pxCallbackFunction` | Callback function |

---

# ▶️ 14. Starting a Timer

Creating a timer does not necessarily mean that it has started.

The timer can be started using:

```cpp
xTimerStart()
```

Example:

```cpp
xTimerStart(
    ledTimer,
    portMAX_DELAY
);
```

Conceptually:

```text
Create Timer
    │
    ▼
Timer Exists
    │
    ▼
xTimerStart()
    │
    ▼
Timer Active
```

---

# ⏹️ 15. Stopping a Timer

A timer can be stopped using:

```cpp
xTimerStop()
```

Example:

```cpp
xTimerStop(
    ledTimer,
    pdMS_TO_TICKS(100)
);
```

After the command is processed, the timer remains defined but is no longer active.

```text
Active Timer
     │
     ▼
xTimerStop()
     │
     ▼
Inactive Timer
```

---

# 🔄 16. Resetting a Timer

A timer can be restarted from its full period using:

```cpp
xTimerReset()
```

Example:

```cpp
xTimerReset(
    timeoutTimer,
    0
);
```

This is particularly useful for timeout detection.

Consider:

```text
Packet Received
      │
      ▼
Reset Timeout Timer
      │
      ▼
Packet Received
      │
      ▼
Reset Timeout Timer
      │
      ▼
No Packet
      │
      ▼
Timer Expires
      │
      ▼
Connection Timeout
```

---

# ⏱️ 17. Changing the Timer Period

The timer period can be changed using:

```cpp
xTimerChangePeriod()
```

Example:

```cpp
xTimerChangePeriod(
    ledTimer,
    pdMS_TO_TICKS(1000),
    0
);
```

This changes the timer period to approximately:

$$
1~s.
$$

This allows applications to dynamically adjust timing behavior.

---

# 🧪 18. Timer Callback Function

A timer callback follows a form similar to:

```cpp
void ledTimerCallback(TimerHandle_t xTimer)
{
    digitalWrite(LED_BUILTIN,
                 !digitalRead(LED_BUILTIN));
}
```

Conceptually:

```text
Timer Expires
     │
     ▼
Callback Function
     │
     ▼
Toggle LED
```

---

# ⚠️ 19. Timer Callback Design

Timer callback functions should normally execute quickly.

Avoid operations that:

- block for long periods,
- contain long loops,
- perform heavy calculations,
- wait indefinitely,
- execute lengthy network operations.

The reason is that timer callbacks execute in the context of the timer service task.

Conceptually:

```text
Timer Service Task
       │
       ├── Callback A
       │
       ├── Callback B
       │
       └── Callback C
```

If Callback A takes too long:

```text
Callback A ───────────────►
                           Callback B delayed
                           Callback C delayed
```

Therefore,

$$
\boxed{
\text{Short Callback}
\rightarrow
\text{Better Timer Responsiveness}
}
$$

---

# 💡 20. Recommended Callback Architecture

For complex work, use the callback to signal another task.

Instead of:

```text
Timer Callback
     │
     ▼
Large Processing
```

use:

```text
Timer Callback
     │
     ▼
Queue / Notification
     │
     ▼
Worker Task
     │
     ▼
Large Processing
```

This separates timing control from application processing.

---

# 🧪 21. ESP32 LED Timer Example

```cpp
#include <Arduino.h>

TimerHandle_t ledTimer;

void ledTimerCallback(TimerHandle_t xTimer)
{
    digitalWrite(LED_BUILTIN,
                 !digitalRead(LED_BUILTIN));
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    ledTimer = xTimerCreate(
        "LEDTimer",
        pdMS_TO_TICKS(500),
        pdTRUE,
        NULL,
        ledTimerCallback
    );

    if (ledTimer != NULL)
    {
        xTimerStart(ledTimer, 0);
    }
}

void loop()
{
}
```

The architecture is:

```text
FreeRTOS
   │
   ▼
Software Timer
   │
500 ms
   │
   ▼
Callback
   │
   ▼
Toggle LED
```

---

# 🆚 22. Software Timer vs. Task Delay

A periodic task could be implemented as:

```cpp
while (1)
{
    doSomething();

    vTaskDelay(
        pdMS_TO_TICKS(1000)
    );
}
```

Alternatively:

```text
Software Timer
      │
      ▼
Callback every 1 s
```

The two approaches serve different purposes.

| Feature | Periodic Task | Software Timer |
|---|---|---|
| Dedicated task stack | Yes | No separate application-task stack |
| Complex processing | Suitable | Usually not suitable in callback |
| Blocking operations | Possible when designed correctly | Should be avoided |
| Simple periodic event | Possible | Very suitable |
| Scheduling control | Greater | Timer-service based |

---

# ⚙️ 23. Software Timer vs. Hardware Timer

A **hardware timer** is implemented using MCU timer peripherals.

A **software timer** is managed by the RTOS.

```text
Hardware Timer
      │
      ▼
Timer Peripheral
      │
      ▼
Interrupt
```

versus:

```text
Software Timer
      │
      ▼
RTOS Tick / Kernel
      │
      ▼
Timer Service Task
      │
      ▼
Callback
```

Hardware timers are generally more appropriate when precise hardware-level timing is required.

Software timers are useful for application-level timing.

---

# 📊 24. Hardware vs. Software Timer

| Characteristic | Hardware Timer | Software Timer |
|---|---|---|
| Managed by | MCU peripheral | RTOS |
| Timing precision | Typically higher | Tick/scheduling dependent |
| Hardware resource | Timer peripheral | RTOS resources |
| Callback context | Often ISR-related | Timer service task |
| Application timeout | Possible | Excellent |
| Very precise waveform | Better suited | Not recommended |
| Periodic software event | Possible | Excellent |

---

# 💾 25. RTOS Memory Management

FreeRTOS objects require RAM.

Examples include:

```text
Task
Queue
Semaphore
Mutex
Software Timer
Event Group
```

Therefore:

$$
\boxed{
\text{RTOS Objects}
\rightarrow
\text{Memory Consumption}
}
$$

Memory management becomes important because microcontrollers have limited RAM.

---

# 🧠 26. ESP32 Memory Overview

A simplified ESP32 application memory model can be represented as:

```text
ESP32 Memory
│
├── Program / Flash
│
├── Static / Global Data
│
├── Heap
│
└── Stacks
```

Different ESP32 variants have different memory architectures, so exact sizes and capabilities depend on the selected device.

---

# 🗂️ 27. Simplified Runtime Memory Layout

Conceptually:

```text
High Address
┌────────────────────┐
│       Stack        │
├────────────────────┤
│                    │
│       Heap         │
│                    │
├────────────────────┤
│ BSS / Global Data  │
├────────────────────┤
│ Initialized Data   │
├────────────────────┤
│ Program / Code     │
└────────────────────┘
Low Address
```

The exact ESP32 memory architecture is more complex, but this simplified model helps explain allocation concepts.

---

# 📦 28. What Is the Heap?

The **heap** is a memory region used for runtime allocation.

Conceptually:

```text
Heap
│
├── Free
├── Allocated Object
├── Free
├── Allocated Object
└── Free
```

Dynamic allocation obtains memory from an available heap region during program execution.

---

# 📚 29. What Is a Stack?

Each FreeRTOS task requires stack memory.

The stack stores information such as:

- local variables,
- function call information,
- temporary values,
- saved execution context.

Conceptually:

```text
Task A
   │
   └── Stack A

Task B
   │
   └── Stack B

Task C
   │
   └── Stack C
```

Thus, creating more tasks generally increases RAM consumption.

---

# 🧮 30. Approximate Task Memory

A simplified conceptual model is:

$$
M_{task}
\approx
M_{TCB}
+
M_{stack},
$$

where:

- $M_{TCB}$ = Task Control Block memory,
- $M_{stack}$ = task stack memory.

For $N$ tasks:

$$
M_{tasks}
\approx
\sum_{i=1}^{N}
\left(
M_{TCB,i}+M_{stack,i}
\right).
$$

This illustrates why task count and stack sizing matter.

---

# 🧱 31. RTOS Object Memory

Consider an application containing:

```text
4 Tasks
3 Queues
2 Semaphores
1 Mutex
2 Software Timers
```

Each object consumes some memory.

Therefore:

$$
M_{RTOS} =
M_{tasks}
+
M_{queues}
+
M_{sync}
+
M_{timers}
+\cdots
$$

Memory planning is an important part of embedded-system design.

---

# 🔄 32. Dynamic Memory Allocation

**Dynamic allocation** means that memory is requested while the program is running.

Conceptually:

```text
Program Running
      │
      ▼
Request Memory
      │
      ▼
Heap Manager
      │
      ▼
Memory Allocated
```

Examples include dynamically created FreeRTOS objects such as:

```cpp
xTaskCreate()
```

```cpp
xQueueCreate()
```

```cpp
xTimerCreate()
```

depending on the FreeRTOS configuration and platform implementation.

---

# 📦 33. Dynamic Task Creation

A common ESP32 task creation call is:

```cpp
xTaskCreate(
    TaskFunction,
    "Task",
    2048,
    NULL,
    1,
    NULL
);
```

The RTOS allocates the required task-related memory dynamically.

Conceptually:

```text
xTaskCreate()
      │
      ▼
Heap
      │
      ├── Task Control Block
      └── Task Stack
```

---

# 📥 34. Dynamic Queue Creation

For example:

```cpp
QueueHandle_t queue;

queue = xQueueCreate(
    10,
    sizeof(int)
);
```

Conceptually:

```text
xQueueCreate()
      │
      ▼
Heap
      │
      ▼
Queue Memory
```

The RTOS allocates storage required by the queue and its control structure.

---

# ⏱️ 35. Dynamic Timer Creation

Similarly:

```cpp
TimerHandle_t timer;

timer = xTimerCreate(
    "Timer",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    NULL,
    callback
);
```

The timer object requires memory.

With dynamic creation:

```text
xTimerCreate()
      │
      ▼
Heap
      │
      ▼
Timer Object
```

---

# 🗑️ 36. Releasing Dynamically Allocated Objects

Some dynamically created RTOS objects can be deleted when they are no longer required.

Examples include:

```cpp
vTaskDelete()
```

```cpp
vQueueDelete()
```

```cpp
xTimerDelete()
```

Conceptually:

```text
Allocated Object
      │
      ▼
Delete
      │
      ▼
Memory Returned
      │
      ▼
Heap
```

Correct object lifecycle management helps prevent unnecessary memory consumption.

---

# 🧠 37. Advantages of Dynamic Allocation

Dynamic allocation provides:

- flexible object creation,
- runtime configurability,
- simpler APIs,
- memory use only when needed,
- convenient prototyping.

For example:

```text
System Starts
    │
    ▼
Need Worker?
    │
   Yes
    │
    ▼
Create Task
```

This is useful when system requirements change during runtime.

---

# ⚠️ 38. Disadvantages of Dynamic Allocation

Dynamic allocation may introduce:

- allocation failure,
- heap fragmentation,
- variable allocation time,
- more complex lifetime management,
- reduced predictability.

Therefore:

$$
\boxed{
\text{Dynamic Allocation} =
\text{Flexibility}
+
\text{Runtime Memory Management}
}
$$

but potentially:

$$
\boxed{
\text{Runtime Allocation}
\rightarrow
\text{Reduced Predictability}
}
$$

---

# 🧩 39. What Is Memory Fragmentation?

Consider an initially free heap:

```text
[                    FREE                    ]
```

Objects are allocated:

```text
[A][B][C][D]
```

Then B and D are deleted:

```text
[A][ FREE ][C][ FREE ]
```

Although free memory exists, it may be divided into separate regions.

This is called **external fragmentation**.

A future large allocation may fail if no individual free block is large enough.

---

# 📊 40. Fragmentation Example

Suppose the heap contains:

```text
[100 B Free][Object][200 B Free]
```

Total free memory is:

$$
100+200=300~bytes.
$$

However, a request for one contiguous block of:

$$
250~bytes
$$

may fail if the allocator cannot combine those separated regions.

Thus:

$$
\boxed{
\text{Total Free Memory}
\neq
\text{Largest Contiguous Free Block}
}
$$

---

# 🧱 41. Static Memory Allocation

**Static allocation** means the required memory is supplied by the application rather than obtained dynamically from the heap when the RTOS object is created.

Conceptually:

```text
Compile / Program Design
       │
       ▼
Memory Reserved
       │
       ▼
Program Runs
       │
       ▼
Object Uses Reserved Memory
```

This improves memory predictability.

---

# 🧠 42. Static Task Creation

FreeRTOS can support statically allocated tasks through:

```cpp
xTaskCreateStatic()
```

The application provides:

- task stack,
- task-control-block storage.

Conceptually:

```text
Application
   │
   ├── Stack Buffer
   └── TCB Buffer
          │
          ▼
 xTaskCreateStatic()
          │
          ▼
        Task
```

---

# 💻 43. Static Task Example

A simplified example is:

```cpp
#define STACK_SIZE 2048

StaticTask_t taskBuffer;
StackType_t taskStack[STACK_SIZE];

TaskHandle_t taskHandle;

taskHandle = xTaskCreateStatic(
    TaskFunction,
    "StaticTask",
    STACK_SIZE,
    NULL,
    1,
    taskStack,
    &taskBuffer
);
```

The memory is explicitly provided by the application.

---

# 📦 44. Static Queue Creation

A queue can be statically allocated using:

```cpp
xQueueCreateStatic()
```

The application supplies the queue storage.

Example concept:

```text
Application Buffer
      │
      ▼
xQueueCreateStatic()
      │
      ▼
Static Queue
```

A simplified example:

```cpp
#define QUEUE_LENGTH 10

StaticQueue_t queueControl;

uint8_t queueStorage[
    QUEUE_LENGTH * sizeof(int)
];

QueueHandle_t queue;

queue = xQueueCreateStatic(
    QUEUE_LENGTH,
    sizeof(int),
    queueStorage,
    &queueControl
);
```

---

# ⏱️ 45. Static Software Timer

FreeRTOS can support statically allocated software timers through:

```cpp
xTimerCreateStatic()
```

Example concept:

```text
StaticTimer_t
     │
     ▼
xTimerCreateStatic()
     │
     ▼
Software Timer
```

A simplified example:

```cpp
StaticTimer_t timerBuffer;

TimerHandle_t timer;

timer = xTimerCreateStatic(
    "Timer",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    NULL,
    callback,
    &timerBuffer
);
```

---

# 📊 46. Dynamic vs. Static Allocation

| Characteristic | Dynamic | Static |
|---|---|---|
| Allocation time | Runtime | Storage predetermined/provided |
| Heap required | Yes | Not for the object's supplied storage |
| Flexibility | High | Lower |
| Runtime creation | Easy | Possible with preallocated storage |
| Fragmentation concern | Possible | Reduced |
| Memory predictability | Lower | Higher |
| Allocation failure | Possible if heap unavailable | Storage known in advance |
| Code complexity | Often lower | Often higher |
| Real-time determinism | Potentially lower | Generally better |

---

# 🎯 47. When to Use Dynamic Allocation

Dynamic allocation is useful when:

- system configuration changes,
- object count is not known beforehand,
- prototyping rapidly,
- resources are created only when needed,
- flexibility is more important than strict determinism.

Example:

```text
Client Connects
      │
      ▼
Create Worker
      │
      ▼
Process Client
      │
      ▼
Delete Worker
```

---

# 🛡️ 48. When to Use Static Allocation

Static allocation is useful when:

- memory usage must be predictable,
- system reliability is critical,
- runtime allocation should be minimized,
- long-term operation is required,
- certification or deterministic behavior matters.

Examples include:

- industrial controllers,
- safety-oriented embedded systems,
- long-running sensor nodes,
- deterministic real-time applications.

---

# 🔀 49. Hybrid Allocation

An application does not necessarily need to use only one strategy.

For example:

```text
System
│
├── Critical Control Tasks → Static
├── Critical Queues        → Static
├── Optional Network Task  → Dynamic
└── Temporary Worker       → Dynamic
```

This creates a **hybrid allocation strategy**.

Therefore:

$$
\boxed{
\text{Static for Predictability}
+
\text{Dynamic for Flexibility}
}
$$

can be a practical design approach.

---

# 🧠 50. ESP32 Heap Monitoring

ESP32 applications can inspect available heap memory.

In the Arduino environment, a common function is:

```cpp
ESP.getFreeHeap();
```

Example:

```cpp
Serial.print("Free Heap: ");
Serial.println(ESP.getFreeHeap());
```

The output might conceptually be:

```text
Free Heap: 245312 bytes
```

The actual value depends on:

- ESP32 variant,
- application,
- libraries,
- Wi-Fi/Bluetooth usage,
- RTOS objects,
- buffers.

---

# 📉 51. Monitoring Memory Over Time

Memory monitoring can help identify:

- memory leaks,
- unexpected allocation,
- resource growth,
- fragmentation-related behavior.

For example:

```text
Time        Free Heap
0 s         250 KB
10 s        245 KB
20 s        240 KB
30 s        235 KB
```

A continuously decreasing free heap may indicate a memory-management problem.

---

# 🚨 52. Memory Leak

A **memory leak** occurs when allocated memory is no longer needed but is never released.

Conceptually:

```text
Allocate
   │
   ▼
Use
   │
   ▼
Forget Pointer / Object
   │
   ▼
Memory Cannot Be Reused
```

Repeated leaks produce:

$$
M_{free}(t+\Delta t)<M_{free}(t).
$$

Eventually, the application may run out of memory.

---

# 🔍 53. Stack Monitoring

Task stacks must also be monitored.

A task with insufficient stack space may cause:

- corrupted memory,
- crashes,
- watchdog resets,
- unpredictable behavior.

A task with an excessively large stack wastes RAM.

Therefore:

$$
\boxed{
\text{Stack Size} =
\text{Enough for Safety}
+
\text{Avoid Excessive Waste}
}
$$

---

# 📏 54. Stack High-Water Mark

FreeRTOS provides mechanisms for examining the minimum amount of unused stack observed during task execution.

Conceptually:

```text
Task Stack
┌─────────────┐
│ Unused      │
│ Unused      │
├─────────────┤ ← Maximum observed usage
│ Used        │
│ Used        │
│ Used        │
└─────────────┘
```

This helps developers tune task stack sizes.

---

# ⚠️ 55. Stack Overflow

A stack overflow occurs when:

$$
M_{required}>M_{allocated}.
$$

Conceptually:

```text
Allocated Stack
┌─────────────┐
│             │
│             │
│             │
└─────────────┘
       ▲
       │
Required stack exceeds boundary
```

Stack overflow can corrupt adjacent memory and destabilize the system.

---

# 🧮 56. Memory Budget

Before implementation, a rough memory budget can be prepared.

For example:

| Component | Quantity | Approx. Memory |
|---|---:|---:|
| Sensor Task | 1 | 3 KB |
| Control Task | 1 | 3 KB |
| MQTT Task | 1 | 5 KB |
| Queue | 2 | 1 KB |
| Timers | 3 | Application dependent |
| Buffers | — | 8 KB |

Then:

$$
M_{used}
\approx
\sum_i M_i.
$$

The exact memory cost should be measured on the target ESP32 configuration.

---

# 🧠 57. Software Timer Memory Advantage

Suppose five simple periodic operations are implemented using five tasks.

Conceptually:

```text
Task 1 → Stack
Task 2 → Stack
Task 3 → Stack
Task 4 → Stack
Task 5 → Stack
```

Each task requires stack memory.

For suitable short periodic operations, software timers can share the timer-service infrastructure:

```text
Timer Service Task
     │
     ├── Timer 1
     ├── Timer 2
     ├── Timer 3
     ├── Timer 4
     └── Timer 5
```

This may reduce the need for several dedicated application task stacks.

---

# ⚠️ 58. Software Timers Are Not a Replacement for Every Task

Software timers are appropriate for short timer-triggered operations.

Tasks are preferable for:

- long processing,
- blocking communication,
- complex algorithms,
- stateful continuous operations,
- lengthy sensor transactions,
- network processing.

A useful design principle is:

$$
\boxed{
\text{Short Timed Event}
\rightarrow
\text{Software Timer}
}
$$

$$
\boxed{
\text{Long / Blocking Work}
\rightarrow
\text{Task}
}
$$

---

# 📡 59. ESP32 IoT Example

Consider an IoT environmental monitor:

```text
                 ESP32
                   │
               FreeRTOS
                   │
      ┌────────────┼─────────────┐
      ▼            ▼             ▼
 Sensor Task    MQTT Task    Software Timers
                                  │
                                  ├── LED Timer
                                  ├── Status Timer
                                  └── Timeout Timer
```

The Sensor Task handles sensor acquisition.

The MQTT Task handles potentially blocking network communication.

Software timers handle lightweight periodic or timeout events.

---

# 🌐 60. Wi-Fi Timeout Example

A one-shot timer can monitor a connection attempt.

```text
Start Wi-Fi Connection
        │
        ▼
Start Timeout Timer
        │
        ├──────── Wi-Fi Connected
        │              │
        │              ▼
        │         Stop Timer
        │
        └──────── Timeout
                       │
                       ▼
                 Retry / Report
```

If:

$$
T_{timeout}=10~s,
$$

the system can detect a failed connection without continuously polling elapsed time.

---

# 🔄 61. Periodic Sensor Monitoring Example

Suppose a lightweight system-health check is required every:

$$
T=5~s.
$$

An auto-reload timer can trigger:

```text
Timer
  │
  ▼
Check System Status
  │
  ▼
Return
```

For heavier sensor communication, the timer could instead notify a worker task.

```text
Timer
  │
  ▼
Notify Sensor Task
  │
  ▼
Sensor Acquisition
```

---

# 📤 62. Timer + Queue Architecture

A useful architecture combines software timers and queues.

```text
Software Timer
      │
      ▼
Timer Callback
      │
      ▼
Send Event
      │
      ▼
Queue
      │
      ▼
Worker Task
```

The timer controls **when** work begins.

The queue communicates **what** work should be performed.

---

# 🧩 63. Timer + Task Notification Architecture

For simple signaling:

```text
Software Timer
      │
      ▼
Callback
      │
      ▼
Task Notification
      │
      ▼
Worker Task
```

This can be lighter than passing a full message when no additional data is required.

---

# 🚦 64. Smart Traffic Example

Consider an ESP32 traffic-light controller:

```text
Traffic FSM Task
      ▲
      │
Timer Events
      │
┌─────┴───────────┐
│ Green Timer     │
│ Yellow Timer    │
│ Pedestrian Timer│
└─────────────────┘
```

A timer expiration can signal the FSM to change state.

For example:

```text
GREEN
  │
30-s Timer
  ▼
YELLOW
  │
5-s Timer
  ▼
RED
```

This provides clear separation between:

- timing,
- state transitions,
- output control.

---

# 🧠 65. Memory Strategy for Smart Traffic System

A safety-oriented traffic controller might choose:

```text
Traffic FSM Task   → Static allocation
Event Queue        → Static allocation
Critical Timer     → Static allocation
Diagnostic Task    → Dynamic allocation
```

The critical components have predictable memory.

Optional components retain flexibility.

---

# 🔬 66. Sensor Data Logger Example

Consider:

```text
Sensor Task
     │
     ▼
Sensor Queue
     │
     ▼
Logger Task
```

and:

```text
Status Timer
     │
     ▼
Every 10 Seconds
     │
     ▼
Report Memory Usage
```

The timer callback can request or perform a lightweight diagnostic such as reporting:

```text
Free Heap
Queue Occupancy
System Uptime
```

This helps monitor long-running systems.

---

# 📊 67. Dynamic Allocation Failure

Dynamic creation functions should be checked.

For example:

```cpp
TimerHandle_t timer;

timer = xTimerCreate(
    "Timer",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    NULL,
    callback
);

if (timer == NULL)
{
    Serial.println("Timer allocation failed");
}
```

Similarly, queue and task creation should be checked when their APIs provide failure information.

---

# 🛡️ 68. Robust Memory Design

A robust ESP32 application should consider:

1. available heap,
2. task stack requirements,
3. queue storage,
4. communication buffers,
5. Wi-Fi/Bluetooth memory,
6. timer objects,
7. application data,
8. worst-case runtime behavior.

The design objective is:

$$
\boxed{
M_{required}
<
M_{available}
}
$$

with an appropriate safety margin.

---

# 📈 69. Memory Utilization

A simplified memory-utilization metric is:

$$
U_M=
\frac{M_{used}}
{M_{available}}
\times100\%.
$$

For example, if:

$$
M_{used}=200~KB
$$

and:

$$
M_{available}=320~KB,
$$

then:

$$
U_M =
\frac{200}{320}\times100 =
62.5\%.
$$

This simplified calculation is useful for conceptual resource planning.

---

# ⏱️ 70. Timer Jitter

Software timer callbacks may not execute at exactly the ideal mathematical instant because execution depends on:

- RTOS tick resolution,
- scheduler activity,
- timer service task priority,
- other callbacks,
- interrupt activity.

Define timer timing error as:

$$
J_n=t_{actual,n}-t_{ideal,n}.
$$

where:

- $t_{actual,n}$ = actual callback time,
- $t_{ideal,n}$ = expected callback time.

This timing variation is commonly described as **jitter**.

---

# 📊 71. Timer Timing Model

For an ideal periodic timer:

$$
t_n=t_0+nT.
$$

For a practical RTOS software timer:

$$
t_n=t_0+nT+J_n.
$$

For non-critical application timing, small jitter may be acceptable.

For highly precise timing, a hardware timer or dedicated peripheral may be more appropriate.

---

# 🧠 72. Determinism and Memory Allocation

Real-time systems are concerned with predictable behavior.

Dynamic allocation may have runtime-dependent behavior:

```text
Request Memory
      │
      ▼
Search Available Heap
      │
      ▼
Allocate
```

Static allocation avoids runtime heap allocation for that object:

```text
Preallocated Memory
      │
      ▼
Known Storage
```

Therefore, static allocation is often preferred when high predictability is required.

---

# 🔐 73. Predictability vs. Flexibility

The design trade-off can be summarized as:

$$
\boxed{
\text{Dynamic}
\rightarrow
\text{Flexibility}
}
$$

and:

$$
\boxed{
\text{Static}
\rightarrow
\text{Predictability}
}
$$

A good embedded design selects the appropriate strategy for each subsystem.

---

# 🗃️ 74. Object Lifetime

RTOS objects may have different lifetimes.

### Permanent

Exists throughout the application:

```text
System Start
     │
     ├─────────────────────────────┐
     │                             │
     ▼                             ▼
Permanent Task                System Shutdown
```

### Temporary

Exists only when needed:

```text
Event
  │
  ▼
Create Object
  │
  ▼
Use
  │
  ▼
Delete Object
```

Static allocation is particularly natural for permanent objects.

Dynamic allocation can be convenient for temporary resources.

---

# ⚠️ 75. Repeated Create/Delete Cycles

A design such as:

```text
Create
  │
Use
  │
Delete
  │
Create
  │
Use
  │
Delete
  │
...
```

may increase memory-management complexity.

For long-running embedded systems, it may be better to create frequently used objects once and reuse them.

```text
Create Once
    │
    ▼
Use
    │
    ▼
Reuse
    │
    ▼
Reuse
```

This can improve predictability.

---

# 🧠 76. Memory Pool Concept

For applications requiring predictable runtime memory, a fixed-size memory-pool architecture can also be considered.

```text
Preallocated Pool

┌────┬────┬────┬────┐
│ B1 │ B2 │ B3 │ B4 │
└────┴────┴────┴────┘
```

Objects obtain predetermined blocks rather than arbitrary heap regions.

This approach can reduce fragmentation and improve allocation predictability, although it requires additional design effort.

---

# 🧪 77. Example: Periodic Memory Monitor

A software timer can periodically inspect free heap memory.

```cpp
#include <Arduino.h>

TimerHandle_t memoryTimer;

void memoryCallback(TimerHandle_t xTimer)
{
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
}

void setup()
{
    Serial.begin(115200);

    memoryTimer = xTimerCreate(
        "MemoryMonitor",
        pdMS_TO_TICKS(5000),
        pdTRUE,
        NULL,
        memoryCallback
    );

    if (memoryTimer != NULL)
    {
        xTimerStart(memoryTimer, 0);
    }
}

void loop()
{
}
```

Conceptually:

```text
Every 5 Seconds
      │
      ▼
Software Timer
      │
      ▼
Memory Callback
      │
      ▼
ESP.getFreeHeap()
      │
      ▼
Serial Monitor
```

---

# 📉 78. Detecting a Possible Memory Leak

Suppose the monitor reports:

```text
Free Heap = 220000
Free Heap = 218000
Free Heap = 216000
Free Heap = 214000
Free Heap = 212000
```

If the system workload is otherwise stable and memory is never recovered, this pattern may suggest a leak or unintended accumulation.

The developer should investigate:

- dynamically allocated buffers,
- created tasks,
- queue objects,
- network resources,
- library allocations.

---

# 🔬 79. Complete System Architecture

A robust ESP32 FreeRTOS system might use:

```text
                    ESP32
                      │
                  FreeRTOS
                      │
       ┌──────────────┼───────────────┐
       │              │               │
       ▼              ▼               ▼
 Sensor Task     Control Task      MQTT Task
       │              ▲               ▲
       ▼              │               │
 Sensor Queue ────────┘               │
                                      │
 Result Queue ────────────────────────┘

       Software Timers
       │
       ├── Timeout Timer
       ├── Status Timer
       └── Memory Monitor
```

Memory strategy:

```text
Critical Tasks  → Static
Critical Queues → Static
Critical Timers → Static

Optional Tasks  → Dynamic
Temporary Data  → Controlled Dynamic Allocation
```

---

# 📋 80. Design Checklist

Before implementing an ESP32 FreeRTOS application, ask:

- How many tasks are required?
- What stack size does each task require?
- Which operations can use software timers?
- Which callbacks must remain short?
- How many queues are needed?
- How large should each queue be?
- Which objects are permanent?
- Which objects are temporary?
- Should critical objects use static allocation?
- How much heap remains after initialization?
- Can allocations fail?
- Is memory released correctly?
- Could fragmentation become a problem?
- Are task stacks monitored?
- Is timer jitter acceptable?

---

# 📊 81. Software Timer Design Table

A useful planning table is:

| Timer | Type | Period | Callback Purpose |
|---|---|---:|---|
| LED Timer | Auto-reload | 500 ms | Toggle status LED |
| Sensor Trigger | Auto-reload | 1 s | Notify sensor task |
| Wi-Fi Timeout | One-shot | 10 s | Detect connection failure |
| Memory Monitor | Auto-reload | 5 s | Report free heap |
| Alarm Timeout | One-shot | 30 s | Clear temporary alarm |

---

# 💾 82. Memory Allocation Design Table

| Object | Allocation | Reason |
|---|---|---|
| Control Task | Static | Critical and permanent |
| Sensor Task | Static | Permanent |
| Sensor Queue | Static | Predictable storage |
| Control Queue | Static | Critical communication |
| Status Timer | Static | Permanent |
| Diagnostic Task | Dynamic | Optional |
| Temporary Worker | Dynamic | Created only when needed |

---

# 💬 83. Discussion Questions

1. What is a software timer?
2. How does a software timer differ from a hardware timer?
3. What is the timer service task?
4. What is a timer callback?
5. Why should timer callbacks execute quickly?
6. What is a one-shot timer?
7. What is an auto-reload timer?
8. When should a timer signal a worker task?
9. What is the FreeRTOS tick?
10. What is timer jitter?
11. What is dynamic memory allocation?
12. What is static memory allocation?
13. What is the heap?
14. What is a task stack?
15. Why does every FreeRTOS task require memory?
16. What is heap fragmentation?
17. What is a memory leak?
18. Why can static allocation improve predictability?
19. What are the advantages of dynamic allocation?
20. What is a hybrid memory-allocation strategy?

---

# 🧪 84. Exercise 1 — One-Shot Timer

Create a one-shot timer with:

$$
T=5~s.
$$

The timer should print:

```text
Timeout!
```

when it expires.

Determine the required:

- timer handle,
- callback,
- `xTimerCreate()` configuration,
- `xTimerStart()` call.

---

# 🔄 85. Exercise 2 — Auto-Reload Timer

Create an auto-reload timer with:

$$
T=1000~ms.
$$

The callback should increment:

```cpp
counter++;
```

and display the counter using Serial.

Expected output:

```text
Timer = 1
Timer = 2
Timer = 3
Timer = 4
...
```

---

# 💡 86. Exercise 3 — LED Software Timer

Create:

```text
ESP32
  │
  ▼
Software Timer
  │
500 ms
  │
  ▼
LED
```

Requirements:

- auto-reload timer,
- 500-ms period,
- LED toggles on every callback,
- no `delay()` in `loop()`.

---

# 📡 87. Exercise 4 — Timer-to-Task Communication

Design:

```text
Software Timer
      │
      ▼
Callback
      │
      ▼
Queue
      │
      ▼
Sensor Task
```

The timer should trigger a sensor measurement every:

$$
2~s.
$$

Explain why the actual sensor read may be better performed by the task rather than by a lengthy timer callback.

---

# 💾 88. Exercise 5 — Dynamic Allocation

Create:

- one task using `xTaskCreate()`,
- one queue using `xQueueCreate()`,
- one timer using `xTimerCreate()`.

Measure free heap:

```cpp
ESP.getFreeHeap()
```

before and after object creation.

Complete:

| Condition | Free Heap |
|---|---:|
| Before creation | |
| After task | |
| After queue | |
| After timer | |

---

# 🧱 89. Exercise 6 — Static Allocation

Rewrite the queue using:

```cpp
xQueueCreateStatic()
```

Define:

- queue storage,
- queue control structure,
- queue handle.

Explain which memory is supplied by the application.

---

# 📉 90. Exercise 7 — Memory Monitoring

Create an auto-reload timer that executes every:

$$
5~s
$$

and prints:

```cpp
ESP.getFreeHeap();
```

Run the ESP32 for several minutes and record:

| Time | Free Heap |
|---:|---:|
| 0 s | |
| 30 s | |
| 60 s | |
| 90 s | |
| 120 s | |

Determine whether memory usage remains stable.

---

# 🧮 91. Exercise 8 — Memory Utilization

Suppose:

$$
M_{available}=320~KB
$$

and:

$$
M_{used}=224~KB.
$$

Calculate:

$$
U_M=
\frac{M_{used}}
{M_{available}}
\times100\%.
$$

Discuss whether additional safety margin may be needed.

---

# ⏱️ 92. Exercise 9 — Timer Jitter

A timer should execute every:

$$
1000~ms.
$$

Measured execution times are:

```text
1002 ms
1999 ms
3001 ms
4003 ms
4998 ms
```

Using:

$$
J_n=t_{actual,n}-t_{ideal,n},
$$

calculate the timing error for each event.

---

# 🚦 93. Exercise 10 — Smart Traffic Controller

Design an ESP32 traffic-light system containing:

```text
Traffic FSM Task
Vehicle Sensor Task
Event Queue
Green Timer
Yellow Timer
Pedestrian Timer
```

Determine whether each RTOS object should use:

- static allocation,
- dynamic allocation,

and justify your choices.

A possible architecture is:

```text
Vehicle Sensor
      │
      ▼
Sensor Task
      │
      ▼
Event Queue
      │
      ▼
Traffic FSM
      ▲
      │
Software Timers
```

---

# 🚀 94. Advanced Exercise — IoT System

Design an ESP32 IoT system with:

```text
Sensor Task
Processing Task
Display Task
MQTT Task
```

and software timers for:

```text
Sensor Trigger
MQTT Keepalive Check
Display Timeout
Memory Monitor
```

Requirements:

1. Sensor sampling every 100 ms.
2. Display update every 500 ms.
3. MQTT publishing every 5 s.
4. Memory monitoring every 10 s.
5. Wi-Fi timeout after 15 s.
6. Critical tasks must use predictable memory.
7. Temporary diagnostic operations may use dynamic memory.

Create:

- system architecture,
- timer table,
- task table,
- memory allocation table,
- estimated RAM budget.

---

# 🧾 95. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain FreeRTOS software timers,
- distinguish software and hardware timers,
- distinguish one-shot and auto-reload timers,
- create and control software timers,
- design appropriate timer callbacks,
- explain the timer service task,
- explain RTOS tick-based timing,
- identify software-timer jitter,
- explain ESP32 runtime memory concepts,
- distinguish heap and stack memory,
- explain dynamic allocation,
- explain static allocation,
- create dynamically allocated RTOS objects,
- describe statically allocated tasks, queues, and timers,
- explain memory fragmentation,
- identify memory leaks,
- monitor ESP32 free heap,
- explain task stack requirements,
- compare static and dynamic allocation,
- design hybrid allocation strategies,
- develop memory-aware ESP32 FreeRTOS applications.

---

# 📘 96. References

1. FreeRTOS, *FreeRTOS Kernel Documentation*.
2. Richard Barry, *Mastering the FreeRTOS Real Time Kernel*.
3. Espressif Systems, *ESP-IDF Programming Guide — FreeRTOS*.
4. Espressif Systems, *ESP-IDF Programming Guide — Heap Memory Allocation*.
5. Espressif Systems, *ESP32 Series Datasheet*.
6. Espressif Systems, *ESP32 Technical Reference Manual*.
7. Espressif Systems, *Arduino Core for ESP32 Documentation*.
8. Qing Li and Caroline Yao, *Real-Time Concepts for Embedded Systems*.
9. Jane W. S. Liu, *Real-Time Systems*.
10. J. W. Valvano, *Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers*.

---

# 🔑 97. Key Concepts

### Software Timer

$$
\boxed{
\text{Start}
\rightarrow
\text{Wait }T
\rightarrow
\text{Callback}
}
$$

### One-Shot Timer

$$
\boxed{
\text{Start}
\rightarrow
T
\rightarrow
\text{Callback}
\rightarrow
\text{Stop}
}
$$

### Auto-Reload Timer

$$
\boxed{
T
\rightarrow
\text{Callback}
\rightarrow
T
\rightarrow
\text{Callback}
\rightarrow\cdots
}
$$

### Periodic Timing

$$
\boxed{
t_n=t_0+nT
}
$$

### Practical Timer Timing

$$
\boxed{
t_n=t_0+nT+J_n
}
$$

### Dynamic Allocation

$$
\boxed{
\text{Runtime}
\rightarrow
\text{Heap}
\rightarrow
\text{RTOS Object}
}
$$

### Static Allocation

$$
\boxed{
\text{Preallocated Storage}
\rightarrow
\text{RTOS Object}
}
$$

### Task Memory

$$
\boxed{
M_{task}
\approx
M_{TCB}+M_{stack}
}
$$

### Memory Utilization

$$
\boxed{
U_M=
\frac{M_{used}}
{M_{available}}
\times100\%
}
$$

### Allocation Trade-Off

$$
\boxed{
\text{Dynamic}
\rightarrow
\text{Flexibility}
}
$$

$$
\boxed{
\text{Static}
\rightarrow
\text{Predictability}
}
$$

### Recommended ESP32 Strategy

$$
\boxed{
\text{Critical/Permanent Objects}
\rightarrow
\text{Static Allocation}
}
$$

$$
\boxed{
\text{Optional/Variable Objects}
\rightarrow
\text{Controlled Dynamic Allocation}
}
$$

---

## 🎯 Summary

FreeRTOS software timers provide an efficient mechanism for implementing **periodic events and timeouts** without requiring a dedicated application task for every simple timed activity. One-shot timers are particularly useful for timeout detection, while auto-reload timers support periodic operations.

At the same time, every RTOS component consumes memory. ESP32 developers therefore need to understand the relationship between **task stacks, heap memory, RTOS objects, dynamic allocation, and static allocation**.

The central design principle is:

$$
\boxed{
\text{Reliable ESP32 RTOS System} =
\text{Correct Timing}
+
\text{Controlled Memory Usage}
+
\text{Predictable Resource Management}
}
$$

For systems requiring high predictability, critical and permanent RTOS objects can be statically allocated. Dynamic allocation remains valuable where runtime flexibility is needed. A carefully designed **hybrid approach** often provides an effective balance between flexibility, memory efficiency, and real-time predictability.
