# 📘 Theory: FreeRTOS Fundamentals, Scheduling, Preemption, and Task Management

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Traditional embedded programs often execute sequentially inside a single main loop. This is suitable for simple systems, but larger ESP32 applications may need to read sensors, control actuators, update displays, process data, communicate through Wi-Fi/MQTT, and monitor user inputs at the same time.

A **Real-Time Operating System (RTOS)** organizes such work into independently scheduled **tasks**.

$$
\boxed{
\text{Application}
+
\text{Tasks}
+
\text{Scheduler}
+
\text{RTOS Services} =
\text{Real-Time Embedded System}
}
$$

---

## 🧠 2. What Is an RTOS?

An RTOS is an operating system designed for predictable timing and scheduling behavior.

Important characteristics include:

- deterministic behavior,
- task scheduling,
- priority management,
- timing control,
- synchronization,
- inter-task communication,
- efficient resource utilization.

$$
\boxed{
\text{Real-Time}
\neq
\text{Maximum Speed}
}
$$

A better interpretation is

$$
\boxed{
\text{Real-Time} =
\text{Correct Result at the Required Time}
}
$$

---

## ⏱️ 3. Real-Time Requirements

If a system must respond within a deadline $T_D$, then

$$
T_R \leq T_D
$$

where:

- $T_R$ = response time,
- $T_D$ = deadline.

Thus,

$$
\boxed{
T_R \leq T_D
\Rightarrow
\text{Deadline Satisfied}
}
$$

---

## 🆚 4. General-Purpose OS vs. RTOS

| Characteristic | General-Purpose OS | RTOS |
|---|---|---|
| Main objective | Throughput / user experience | Predictable timing |
| Scheduling | General purpose | Priority-oriented |
| Determinism | Lower | Higher |
| Deadline awareness | Usually secondary | Important |
| Resource footprint | Often large | Usually small |
| Typical platform | PC / server | MCU / embedded system |
| Examples | Windows, Linux | FreeRTOS, Zephyr, ThreadX |

---

## ⚙️ 5. What Is FreeRTOS?

**FreeRTOS** is a real-time operating-system kernel widely used on microcontrollers.

It provides:

- task creation,
- scheduling,
- priorities,
- delays,
- queues,
- semaphores,
- mutexes,
- software timers,
- event groups,
- task notifications.

```text
Application
     │
     ▼
┌──────────────────────┐
│       FreeRTOS       │
│ Scheduler            │
│ Tasks                │
│ Queues               │
│ Semaphores           │
│ Timers               │
└──────────────────────┘
     │
     ▼
ESP32 Hardware
```

---

## 🔬 6. FreeRTOS on the ESP32

A simplified software stack is:

```text
Application
     │
     ▼
Arduino / ESP-IDF APIs
     │
     ▼
FreeRTOS
     │
     ▼
ESP32 Drivers / HAL
     │
     ▼
ESP32 Hardware
```

---

## 🧩 7. What Is a Task?

A task is an independently scheduled function with its own execution context.

Examples:

```text
Task 1 → Read Temperature
Task 2 → Update Display
Task 3 → Send MQTT Data
Task 4 → Control Motor
```

Conceptually,

$$
\boxed{
\text{Task} =
\text{Code}
+
\text{Stack}
+
\text{State}
+
\text{Priority}
}
$$

---

## 💻 8. Basic FreeRTOS Task

```cpp
void TaskLED(void *pvParameters)
{
    while (1)
    {
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));

        digitalWrite(LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

A FreeRTOS task normally:

- runs inside a function,
- contains a loop,
- blocks or delays when idle,
- is scheduled by the RTOS.

---

## 🏗️ 9. Creating a Task

```cpp
xTaskCreate(
    TaskLED,
    "LED Task",
    2048,
    NULL,
    1,
    NULL
);
```

Typical task attributes are:

| Attribute | Description |
|---|---|
| Task function | Code executed |
| Name | Human-readable identifier |
| Stack | Private runtime memory |
| Parameter | Data passed into task |
| Priority | Scheduling importance |
| Handle | Reference used to manage task |

---

## 🔄 10. Multitasking

Suppose an ESP32 has three tasks:

```text
Task A → Read sensor
Task B → Blink LED
Task C → Send network data
```

A single CPU core may execute them in rapid alternation:

```text
Time ─────────────────────────────►
CPU: | A | A | B | C | A | B | C |
```

This is **multitasking**.

On multicore ESP32 variants, some tasks can execute truly in parallel on separate cores.

---

## 🧠 11. The Scheduler

The **scheduler** decides which task should execute.

```text
                Scheduler
                    │
        ┌───────────┼───────────┐
        ▼           ▼           ▼
      Task A      Task B      Task C
    Priority 1  Priority 3  Priority 2
```

A simplified rule is

$$
\boxed{
\text{Scheduler}
\rightarrow
\text{Highest-Priority Ready Task}
}
$$

---

## 🔢 12. Task Priority

If multiple tasks are Ready, a higher-priority task is generally favored.

| Task | Priority |
|---|---:|
| LED | 1 |
| Sensor | 2 |
| Emergency Control | 4 |

---

## 🚦 13. Task States

Major FreeRTOS task states are:

- **Running**
- **Ready**
- **Blocked**
- **Suspended**

```text
Ready
  │
  ▼
Running
  │
  ├──► Blocked
  │
  └──► Suspended
```

### Running
The task is currently executing.

### Ready
The task can run but is waiting for CPU time.

### Blocked
The task is waiting for a delay, queue, semaphore, notification, or event.

### Suspended
The task is explicitly prevented from being scheduled.

A common lifecycle is

$$
\boxed{
Ready
\rightarrow
Running
\rightarrow
Blocked
\rightarrow
Ready
}
$$

---

## ⚡ 14. Preemptive Scheduling

FreeRTOS commonly uses **preemptive priority-based scheduling**.

If a higher-priority task becomes Ready while a lower-priority task is running, the scheduler may stop the lower-priority task and run the higher-priority one.

$$
\boxed{
P_H>P_L
\land
Task_H=\text{Ready}
\Rightarrow
Task_H\text{ may preempt }Task_L
}
$$

---

## ⏱️ 15. RTOS Tick

FreeRTOS uses a periodic tick for timing and scheduling services.

If

$$
f_{tick}
$$

is the tick frequency, then

$$
T_{tick}=\frac{1}{f_{tick}}.
$$

For

$$
f_{tick}=1000~Hz,
$$

$$
T_{tick}=1~ms.
$$

---

## ⏳ 16. `vTaskDelay()`

```cpp
vTaskDelay(pdMS_TO_TICKS(1000));
```

This blocks the task for the requested delay so other tasks can execute.

```text
Running
  │
  ▼
vTaskDelay()
  │
  ▼
Blocked
  │
Delay expires
  ▼
Ready
```

---

## ⚠️ 17. Busy Waiting

Busy waiting repeatedly checks a condition:

```cpp
while (millis() - start < 1000)
{
}
```

This consumes CPU time unnecessarily.

A better RTOS approach is generally:

```cpp
vTaskDelay(pdMS_TO_TICKS(1000));
```

---

## 🔁 18. Round-Robin Scheduling

When equal-priority tasks are Ready and time slicing is enabled, CPU time may be shared.

```text
Task A → Priority 2
Task B → Priority 2
Task C → Priority 2

CPU | A | B | C | A | B | C |
```

---

## 🧮 19. Scheduling Decision

A simplified scheduler model is

$$
T_{next} =
\arg\max_{T_i\in R} P_i
$$

where:

- $R$ = Ready-task set,
- $P_i$ = priority of task $i$.

---

## 🔄 20. Context Switching

A **context switch** occurs when execution changes from one task to another.

```text
Task A
  │
  ▼
Save Context A
  │
  ▼
Scheduler
  │
  ▼
Restore Context B
  │
  ▼
Task B
```

The task context can include:

- CPU registers,
- program counter,
- stack pointer,
- processor state.

---

## 💾 21. Task Stack

Each task has a private stack containing items such as:

- local variables,
- function-call data,
- return addresses,
- saved CPU context.

Insufficient stack allocation can lead to a **stack overflow** and system instability.

---

## 🪪 22. Task Handles

A task handle is a reference used to manage a task.

```cpp
TaskHandle_t Task1Handle;
```

It can be used with APIs for:

- suspension,
- resumption,
- deletion,
- notifications,
- priority changes.

---

## ⏸️ 23. Suspending and Resuming Tasks

Suspend:

```cpp
vTaskSuspend(TaskHandle);
```

Resume:

```cpp
vTaskResume(TaskHandle);
```

State transition:

```text
Ready/Running
    │
    ▼
Suspended
    │
    ▼
Ready
```

---

## 🗑️ 24. Deleting a Task

```cpp
vTaskDelete(TaskHandle);
```

A task can delete itself using:

```cpp
vTaskDelete(NULL);
```

---

## ⚠️ 25. Priority Starvation

If a high-priority task remains Ready and never blocks, lower-priority tasks may receive little or no CPU time.

This is **starvation**.

A good task design normally follows:

```text
Wait
 │
 ▼
Receive Event
 │
 ▼
Perform Work
 │
 ▼
Return to Wait
```

Thus,

$$
\boxed{
\text{Efficient Task} =
\text{Event Driven}
+
\text{Blocks When Idle}
}
$$

---

## 🧪 26. ESP32 Two-Task Example

```cpp
void TaskLED(void *pvParameters)
{
    while (1)
    {
        digitalWrite(2, !digitalRead(2));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void TaskSensor(void *pvParameters)
{
    while (1)
    {
        int value = analogRead(34);
        Serial.println(value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

Create the tasks:

```cpp
xTaskCreate(
    TaskLED,
    "LED",
    2048,
    NULL,
    1,
    NULL
);

xTaskCreate(
    TaskSensor,
    "Sensor",
    2048,
    NULL,
    2,
    NULL
);
```

---

## 💤 27. Idle Task

FreeRTOS creates an Idle Task that executes when no higher-priority application task is Ready.

$$
\boxed{
\text{No Ready Application Task}
\Rightarrow
\text{Idle Task Runs}
}
$$

---

## 🧠 28. Common Task-Management APIs

| Function | Purpose |
|---|---|
| `xTaskCreate()` | Create task |
| `vTaskDelay()` | Delay/block task |
| `vTaskDelete()` | Delete task |
| `vTaskSuspend()` | Suspend task |
| `vTaskResume()` | Resume task |
| `vTaskPrioritySet()` | Change priority |
| `uxTaskPriorityGet()` | Read priority |

---

## 🧮 29. Periodic Tasks

For a periodic task with period $T$,

$$
f=\frac{1}{T}.
$$

If

$$
T=100~ms,
$$

then

$$
f=10~Hz.
$$

---

## ⏲️ 30. `vTaskDelayUntil()`

For periodic execution:

```cpp
TickType_t lastWakeTime = xTaskGetTickCount();

while (1)
{
    // Periodic work

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(100)
    );
}
```

`vTaskDelayUntil()` is often better than `vTaskDelay()` when fixed periodic timing is required.

---

## 🔀 31. Task Communication

FreeRTOS provides mechanisms for communication and synchronization:

- queues,
- semaphores,
- mutexes,
- task notifications,
- event groups.

---

## 📦 32. Queue Concept

```text
Producer Task
     │
     ▼
┌───────────────┐
│     Queue     │
│ [D1][D2][D3] │
└───────────────┘
     │
     ▼
Consumer Task
```

$$
\boxed{
\text{Producer}
\rightarrow
\text{Queue}
\rightarrow
\text{Consumer}
}
$$

---

## 🚦 33. Semaphore Concept

Semaphores can be used for:

- event synchronization,
- ISR-to-task signaling,
- resource coordination.

```text
Task A
  │
  ▼
Give Semaphore
  │
  ▼
Task B
Take Semaphore
```

---

## 🔒 34. Mutex Concept

A **mutex** protects a shared resource.

```text
Task A
   │
   ▼
Take Mutex
   │
   ▼
Shared Resource
   │
   ▼
Give Mutex
```

Only the task owning the mutex should access the protected resource.

---

## ⚠️ 35. Race Conditions

A race condition occurs when concurrent tasks access shared data and the result depends on timing.

For example:

```cpp
counter++;
```

may involve:

```text
Read
  │
Increment
  │
Write
```

If multiple tasks perform this concurrently, updates can be lost.

---

## 🔐 36. Critical Sections

A critical section is code that should not be concurrently executed by multiple execution contexts.

```text
Enter Critical Section
        │
        ▼
Access Shared Data
        │
        ▼
Leave Critical Section
```

Critical sections should generally be kept short.

---

## 🚨 37. Priority Inversion

Suppose:

```text
High Priority   → H
Medium Priority → M
Low Priority    → L
```

If L owns a resource required by H, H can become blocked while M continues executing.

This is **priority inversion**.

---

## 🔐 38. Priority Inheritance

FreeRTOS mutexes can use priority inheritance to reduce priority inversion.

```text
Low task owns mutex
       │
High task waits
       │
Low task temporarily inherits higher priority
       │
Low task releases mutex
       │
High task runs
```

---

## ⚡ 39. Interrupts and Tasks

A useful architecture is:

```text
Hardware Event
      │
      ▼
     ISR
      │
      ▼
Notify Task
      │
      ▼
FreeRTOS Task
      │
      ▼
Longer Processing
```

Therefore,

$$
\boxed{
\text{ISR}
\rightarrow
\text{Signal}
\rightarrow
\text{Task}
}
$$

ISRs should generally remain short.

---

## 🖥️ 40. ESP32 Multicore Considerations

Some ESP32 devices provide multiple processor cores.

```text
                 FreeRTOS
                    │
           ┌────────┴────────┐
           ▼                 ▼
         Core 0            Core 1
           │                 │
       Task A/B          Task C/D
```

This enables possible true parallel execution.

---

## 🧠 41. Concurrency vs. Parallelism

### Concurrency
Multiple tasks make progress over overlapping time intervals.

### Parallelism
Multiple tasks execute at the same instant on different processor cores.

$$
\boxed{
\text{Single Core}
\rightarrow
\text{Concurrency}
}
$$

$$
\boxed{
\text{Multicore}
\rightarrow
\text{Concurrency + Possible Parallelism}
}
$$

---

## 🏗️ 42. Example ESP32 RTOS Architecture

```text
                   ESP32
                     │
                 FreeRTOS
                     │
       ┌─────────────┼─────────────┐
       ▼             ▼             ▼
 Sensor Task    Network Task    LED Task
 Priority 2      Priority 3    Priority 1
       │             │             │
       ▼             ▼             ▼
   MPU6050        MQTT/Wi-Fi      GPIO
```

---

## 🔄 43. Data-Flow Architecture

```text
Sensor Task
    │
    ▼
 Sensor Queue
    │
    ▼
Processing Task
    │
    ▼
 Result Queue
    │
    ├────────► Display Task
    │
    └────────► MQTT Task
```

This separates sensing, processing, visualization, and communication.

---

## 🆚 44. Super Loop vs. FreeRTOS

| Feature | Super Loop | FreeRTOS |
|---|---|---|
| Programming model | Sequential | Task-based |
| Scheduling | Manual | RTOS scheduler |
| Priority | Difficult | Built-in |
| Blocking management | Manual | Task states |
| Modularity | Moderate | High |
| Synchronization | Manual | Queues/semaphores/mutexes |
| Timing control | Manual | RTOS services |
| Complexity | Low initially | Higher initially |
| Scalability | Limited | Better for complex systems |

---

## ⚠️ 45. Common FreeRTOS Design Problems

Common problems include:

- inappropriate task priorities,
- too many tasks,
- insufficient stacks,
- busy waiting,
- tasks that never block,
- unsafe shared-variable access,
- mutex misuse,
- deadlocks,
- excessive context switching,
- long ISRs.

A useful principle is

$$
\boxed{
\text{Use Tasks for Logical Concurrency, Not Every Function}
}
$$

---

## 🔒 46. Deadlock

A deadlock can occur when tasks wait indefinitely for resources held by each other.

```text
Task A owns Resource 1
Task A waits for Resource 2

Task B owns Resource 2
Task B waits for Resource 1
```

Neither task can proceed.

---

## 🧠 47. Task Priority Design

Priority should reflect timing criticality.

| Task | Timing Requirement | Suggested Relative Priority |
|---|---|---|
| Emergency stop | Very urgent | High |
| Motor control | Periodic/critical | High |
| Sensor acquisition | Periodic | Medium |
| Network logging | Moderate | Medium/Low |
| LED status | Non-critical | Low |

$$
\boxed{
\text{Priority}
\approx
\text{Timing Criticality}
}
$$

---

## ⏱️ 48. Periodic Real-Time Task Model

A periodic task can be represented as

$$
\tau_i=(C_i,T_i,D_i)
$$

where:

- $C_i$ = execution time,
- $T_i$ = period,
- $D_i$ = deadline.

Example:

$$
\tau_1=(2~ms,10~ms,10~ms).
$$

---

## 📊 49. CPU Utilization

A simple utilization estimate is

$$
U=
\sum_{i=1}^{n}
\frac{C_i}{T_i}.
$$

Example:

$$
C_1=2~ms,\quad T_1=10~ms
$$

$$
C_2=5~ms,\quad T_2=50~ms
$$

Then

$$
U=
\frac{2}{10}
+
\frac{5}{50} =
0.3.
$$

Therefore,

$$
\boxed{
U=30\%
}
$$

---

## 🚦 50. Real-Time Scheduling Perspective

A real-time task should complete before its deadline:

$$
\boxed{
\text{Completion Time}
\leq
\text{Deadline}
}
$$

---

## 🧪 51. Example: Smart Environmental Monitor

Possible task set:

```text
TaskSensor  → Period = 1 s, Priority = 2
TaskDisplay → Period = 500 ms, Priority = 1
TaskMQTT    → Period = 5 s, Priority = 1
TaskAlarm   → Event driven, Priority = 3
```

---

## 🔬 52. Example: Smart Traffic System

```text
                  ESP32
                    │
                FreeRTOS
                    │
       ┌────────────┼────────────┐
       ▼            ▼            ▼
 Traffic FSM    Sensor Task   Comm. Task
 Priority 3     Priority 2    Priority 1
       │
       ▼
 Red/Yellow/Green LEDs
```

---

## 🧠 53. Task Decomposition

A useful design rule is

$$
\boxed{
\text{One Task}
\approx
\text{One Logical Responsibility}
}
$$

Possible responsibilities include:

- sensor acquisition,
- data processing,
- control,
- communication,
- user interface.

---

## 📋 54. Task Design Table

| Task | Function | Period/Event | Priority | Communication |
|---|---|---|---:|---|
| Sensor | Read ADC | 100 ms | 2 | Queue |
| Control | Calculate output | Event | 3 | Queue |
| Display | Show result | 500 ms | 1 | Queue |
| MQTT | Send data | 5 s | 1 | Queue |

---

## 🔄 55. General FreeRTOS Design Flow

```text
Define Requirements
        │
        ▼
Identify Activities
        │
        ▼
Create Tasks
        │
        ▼
Assign Priorities
        │
        ▼
Define Timing
        │
        ▼
Define Communication
        │
        ▼
Implement
        │
        ▼
Test Scheduling
        │
        ▼
Measure Timing
```

---

## 🔍 56. Debugging FreeRTOS Applications

Useful items to inspect include:

- task states,
- priorities,
- stack usage,
- CPU utilization,
- queues,
- semaphores,
- execution periods,
- deadline misses.

Simple serial traces can help:

```text
[1000 ms] Sensor Task
[1100 ms] LED Task
[1500 ms] Network Task
[2000 ms] Sensor Task
```

---

## 💬 57. Discussion Questions

1. What is an RTOS?
2. What does real-time mean?
3. What is FreeRTOS?
4. Why is FreeRTOS useful on ESP32?
5. What is a task?
6. What is the scheduler?
7. What is task priority?
8. What is preemption?
9. What is a context switch?
10. What are Ready, Running, Blocked, and Suspended states?
11. Why should tasks block when idle?
12. What is busy waiting?
13. What is the RTOS tick?
14. What is the difference between `vTaskDelay()` and `vTaskDelayUntil()`?
15. What is starvation?
16. What is priority inversion?
17. What is a mutex?
18. What is a queue?
19. What is a race condition?
20. What is a deadlock?
21. Why should an ISR be short?
22. What is the difference between concurrency and parallelism?

---

## 🧠 58. Exercises

### Exercise 1 — Task Priority

| Task | Priority |
|---|---:|
| LED | 1 |
| Sensor | 2 |
| Alarm | 4 |

If all tasks are Ready, which task executes first?

### Exercise 2 — Tick Period

Calculate the tick period when

$$
f_{tick}=500~Hz.
$$

Use

$$
T_{tick}=\frac{1}{f_{tick}}.
$$

### Exercise 3 — Periodic Task

A task runs every

$$
T=50~ms.
$$

Calculate its frequency.

### Exercise 4 — CPU Utilization

Given

$$
C_1=1~ms,\quad T_1=10~ms
$$

and

$$
C_2=5~ms,\quad T_2=100~ms,
$$

estimate

$$
U=
\frac{C_1}{T_1}
+
\frac{C_2}{T_2}.
$$

### Exercise 5 — Preemption

Task A has priority 1 and is running. Task B has priority 3 and becomes Ready. Explain what happens.

---

## 🔬 59. Practical Analysis Exercise

Consider:

```text
TaskSensor  → Read MPU6050 every 100 ms
TaskLED     → Blink LED every 500 ms
TaskMQTT    → Publish data every 2 s
TaskAlarm   → Respond immediately to alarm
```

Complete:

| Task | Type | Period/Event | Relative Priority | Blocking Method |
|---|---|---|---:|---|
| Sensor | | | | |
| LED | | | | |
| MQTT | | | | |
| Alarm | | | | |

---

## 🚀 60. Advanced Exercise: ESP32 FreeRTOS System

Design an ESP32 application with:

```text
Sensor Task
     │
     ▼
Sensor Queue
     │
     ▼
Processing Task
     │
     ├──────────► Display Task
     │
     └──────────► MQTT Task
```

Requirements:

1. Sensor task every 100 ms.
2. Processing task handles new samples.
3. Display task every 500 ms.
4. MQTT task every 5 s.
5. Tasks block while waiting.
6. Data transfer is synchronized.
7. Priorities are justified.

---

## 🧾 61. Expected Learning Outcomes

Students should be able to:

- explain RTOS and FreeRTOS fundamentals,
- describe ESP32 task scheduling,
- define task states,
- explain priority and preemption,
- understand context switching,
- create and manage tasks,
- use delays correctly,
- distinguish blocking from busy waiting,
- explain queues, semaphores, and mutexes,
- identify race conditions and deadlocks,
- explain priority inversion and inheritance,
- relate ISRs to RTOS tasks,
- explain concurrency and multicore parallelism,
- design basic ESP32 real-time systems.

---

## 📘 62. References

1. FreeRTOS, *FreeRTOS Kernel Documentation*.
2. Espressif Systems, *ESP-IDF Programming Guide — FreeRTOS*.
3. Espressif Systems, *ESP32 Series Datasheet*.
4. Espressif Systems, *ESP32 Technical Reference Manual*.
5. Espressif Systems, *Arduino Core for ESP32 Documentation*.
6. Richard Barry, *Mastering the FreeRTOS Real Time Kernel*.
7. Qing Li and Caroline Yao, *Real-Time Concepts for Embedded Systems*, CMP Books.
8. Jane W. S. Liu, *Real-Time Systems*, Prentice Hall.
9. J. W. Valvano, *Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers*.

---

## 🔑 63. Key Concepts

### FreeRTOS

$$
\boxed{
\text{Tasks}
+
\text{Scheduler}
+
\text{Timing}
+
\text{Synchronization} =
\text{RTOS Application}
}
$$

### Task

$$
\boxed{
\text{Task} =
\text{Code}
+
\text{Stack}
+
\text{State}
+
\text{Priority}
}
$$

### Scheduler

$$
\boxed{
T_{next} =
\arg\max_{T_i\in Ready}P_i
}
$$

### Preemption

$$
\boxed{
P_H>P_L
\Rightarrow
\text{Higher-Priority Ready Task Can Preempt}
}
$$

### Task Lifecycle

$$
\boxed{
Ready
\rightarrow
Running
\rightarrow
Blocked
\rightarrow
Ready
}
$$

### CPU Utilization

$$
\boxed{
U=
\sum_{i=1}^{n}\frac{C_i}{T_i}
}
$$

### ESP32 Real-Time Architecture

$$
\boxed{
\text{Hardware Events}
\rightarrow
\text{FreeRTOS Tasks}
\rightarrow
\text{Scheduling}
\rightarrow
\text{Processing}
\rightarrow
\text{Control}
}
$$

FreeRTOS transforms an ESP32 application from a purely sequential program into a **task-oriented real-time system**. The scheduler coordinates task execution according to **task state and priority**, while preemption allows urgent tasks to respond rapidly.
