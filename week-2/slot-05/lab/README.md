# LAB 5 — FreeRTOS Preemptive Scheduling with Multiple Tasks and Priorities

## 1. Objective

The objective of this lab is to:

1. Understand task creation in FreeRTOS.
2. Create multiple FreeRTOS tasks on the ESP32.
3. Assign different priorities to tasks.
4. Observe how the FreeRTOS scheduler selects the highest-priority ready task.
5. Understand **preemptive scheduling**.
6. Compare task behavior under equal and different priorities.
7. Observe the effects of `vTaskDelay()` on task execution.
8. Investigate CPU-intensive tasks and task starvation.
9. Measure and analyze task execution using Serial Monitor output.
10. Understand the relationship among task state, priority, delay, and scheduling.
11. Use `vTaskPrioritySet()` to modify task priorities at runtime.
12. Develop a basic real-time multitasking application.

---

## 2. Background

An embedded system often needs to perform several activities at the same time.

For example:

```text
Read Sensor
    +
Control LED
    +
Process Data
    +
Send Messages
```

Without an RTOS, these operations are often placed inside one sequential `loop()`.

With FreeRTOS, the application can be divided into independent **tasks**:

```text
Task 1 → Sensor Acquisition

Task 2 → LED Control

Task 3 → Data Processing

Task 4 → Communication
```

The FreeRTOS scheduler determines which task should execute at a particular time.

---

## 3. FreeRTOS Task Model

A FreeRTOS task can generally be in one of several states:

```text
        ┌─────────┐
        │  Ready  │
        └────┬────┘
             ↓
        ┌─────────┐
        │ Running │
        └────┬────┘
             ↓
        ┌─────────┐
        │ Blocked │
        └────┬────┘
             ↓
           Ready
```

Important states include:

| State | Description |
|---|---|
| Running | Task currently using the CPU |
| Ready | Task can run but is waiting for CPU time |
| Blocked | Task is waiting for time or an event |
| Suspended | Task has been explicitly suspended |

Only one task can execute at a time on a single CPU core.

---

## 4. Task Priorities

Each FreeRTOS task has a priority.

Conceptually:

```text
Higher Number
     ↓
Higher Priority
```

For example:

| Task | Priority |
|---|---:|
| Task A | 1 |
| Task B | 2 |
| Task C | 3 |

If all three tasks are ready:

```text
Task C
   ↓
Runs First
```

because it has the highest priority.

---

## 5. Preemptive Scheduling

FreeRTOS commonly uses **preemptive priority-based scheduling**.

This means that when a higher-priority task becomes ready, it can interrupt a lower-priority task.

Example:

```text
Time ─────────────────────────────→

Task Low:
████████████████
        ↑
        │ Higher-priority task becomes ready
        │
Task High:
        ████████
```

The higher-priority task **preempts** the lower-priority task.

Conceptually:

```text
Low-Priority Task Running
          ↓
High-Priority Task Becomes Ready
          ↓
Scheduler
          ↓
Low-Priority Task Preempted
          ↓
High-Priority Task Runs
```

---

## 6. Scheduling Rule

For ready tasks, the scheduler generally selects:

$$
T_{\text{run}}
=
\arg\max_{T_i\in\mathcal{R}}
P_i,
$$

where:

- $\mathcal{R}$ = set of ready tasks
- $P_i$ = priority of task $i$
- $T_{\text{run}}$ = task selected to execute

Therefore:

```text
Highest-Priority Ready Task
             ↓
          Running
```

---

## 7. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Two or three LEDs
- 220–330 $\Omega$ resistors
- Breadboard
- Jumper wires

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS included with ESP32 framework
- Serial Monitor

---

## 8. FreeRTOS Task Creation

A task can be created using:

```cpp
xTaskCreate(
    taskFunction,
    "Task Name",
    stackSize,
    parameter,
    priority,
    &taskHandle
);
```

On ESP32, a task may also be assigned to a specific processor core using:

```cpp
xTaskCreatePinnedToCore(
    taskFunction,
    "Task Name",
    stackSize,
    parameter,
    priority,
    &taskHandle,
    coreID
);
```

For a scheduling experiment, pinning all tasks to the **same core** makes the preemption behavior easier to observe.

---

## 9. Task Function Structure

A FreeRTOS task normally has the form:

```cpp
void taskFunction(void *parameter) {

  while (1) {

    // Task work

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}
```

A task normally contains an infinite loop because FreeRTOS repeatedly schedules it during the lifetime of the application.

---

## 10. Experiment 1 — Create Two Equal-Priority Tasks

Create two tasks with the same priority.

```cpp
#include <Arduino.h>

void TaskA(void *parameter) {

  while (1) {

    Serial.println("Task A");

    vTaskDelay(
      pdMS_TO_TICKS(500)
    );
  }
}

void TaskB(void *parameter) {

  while (1) {

    Serial.println("Task B");

    vTaskDelay(
      pdMS_TO_TICKS(500)
    );
  }
}

void setup() {

  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    TaskA,
    "Task A",
    2048,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    TaskB,
    "Task B",
    2048,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
}
```

Both tasks have:

```text
Priority = 1
```

Observe the Serial Monitor.

Possible output:

```text
Task A
Task B
Task A
Task B
Task A
Task B
```

The exact interleaving can vary.

---

## 11. Equal-Priority Scheduling

If multiple tasks have the same priority and are ready, FreeRTOS can share CPU time between them through time slicing when configured accordingly.

Conceptually:

```text
Priority 1:

Task A → Task B → Task A → Task B → ...
```

This demonstrates that priority alone does not distinguish equal-priority tasks.

---

## 12. Experiment 2 — Assign Different Priorities

Modify the tasks:

```text
Task A → Priority 1
Task B → Priority 2
```

```cpp
xTaskCreatePinnedToCore(
    TaskA,
    "Task A",
    2048,
    NULL,
    1,
    NULL,
    1
);

xTaskCreatePinnedToCore(
    TaskB,
    "Task B",
    2048,
    NULL,
    2,
    NULL,
    1
);
```

Now:

```text
Priority 2 > Priority 1
```

Therefore, whenever Task B is ready, Task B has precedence over Task A.

---

## 13. Priority Scheduling Example

Suppose:

```text
Task A
Priority = 1

Task B
Priority = 2
```

The scheduling process is:

```text
Task A Running
     ↓
Task B Delay Expires
     ↓
Task B Becomes Ready
     ↓
Priority 2 > Priority 1
     ↓
Task A Preempted
     ↓
Task B Runs
```

This is the key behavior to observe in this lab.

---

## 14. Experiment 3 — Add a Third Task

Create:

```text
Task A → Priority 1
Task B → Priority 2
Task C → Priority 3
```

Example:

```cpp
void TaskA(void *parameter) {

  while (1) {

    Serial.println(
      "Task A - Priority 1"
    );

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}

void TaskB(void *parameter) {

  while (1) {

    Serial.println(
      "Task B - Priority 2"
    );

    vTaskDelay(
      pdMS_TO_TICKS(700)
    );
  }
}

void TaskC(void *parameter) {

  while (1) {

    Serial.println(
      "Task C - Priority 3"
    );

    vTaskDelay(
      pdMS_TO_TICKS(400)
    );
  }
}
```

Create:

```cpp
xTaskCreatePinnedToCore(
    TaskA,
    "Task A",
    2048,
    NULL,
    1,
    NULL,
    1
);

xTaskCreatePinnedToCore(
    TaskB,
    "Task B",
    2048,
    NULL,
    2,
    NULL,
    1
);

xTaskCreatePinnedToCore(
    TaskC,
    "Task C",
    2048,
    NULL,
    3,
    NULL,
    1
);
```

---

## 15. Expected Scheduling Behavior

Task priorities are:

$$
P_C>P_B>P_A.
$$

Therefore:

```text
If C is Ready
    ↓
Run C

Else if B is Ready
    ↓
Run B

Else if A is Ready
    ↓
Run A
```

This can be represented as:

$$
C>B>A.
$$

in scheduling preference.

---

## 16. Why Lower-Priority Tasks Still Run

Even though Task C has the highest priority, it calls:

```cpp
vTaskDelay(...)
```

When Task C is delayed:

```text
Task C
  ↓
Blocked
```

FreeRTOS can then schedule Task B.

When Task B is also blocked:

```text
Task A
  ↓
Runs
```

Therefore:

```text
High Priority Does Not Mean
Continuous CPU Ownership
```

when the task regularly blocks or delays.

---

## 17. Experiment 4 — Observe Preemption Directly

Create a CPU-intensive low-priority task.

```cpp
void LowPriorityTask(
    void *parameter
) {

  while (1) {

    Serial.println(
      "LOW task running"
    );

    for (
      volatile long i = 0;
      i < 1000000;
      i++
    ) {
    }
  }
}
```

Create a periodic high-priority task:

```cpp
void HighPriorityTask(
    void *parameter
) {

  while (1) {

    Serial.println(
      "*** HIGH task running ***"
    );

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}
```

Priorities:

```text
LOW  = 1
HIGH = 3
```

---

## 18. Expected Observation

Initially:

```text
LOW task running
LOW task running
LOW task running
```

When the delay of the high-priority task expires:

```text
*** HIGH task running ***
```

The scheduler interrupts the low-priority task.

Timeline:

```text
LOW  ██████████████████
HIGH           ███
LOW               █████████████
HIGH                         ███
```

This demonstrates **preemption**.

---

## 19. Experiment 5 — Observe the Effect of `vTaskDelay()`

Consider:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(1000)
);
```

The task changes from:

```text
Running
   ↓
Blocked
```

for approximately:

```text
1000 ms
```

After the delay:

```text
Blocked
   ↓
Ready
```

If its priority is the highest among ready tasks:

```text
Ready
   ↓
Running
```

---

## 20. Task State Transition

The process can be represented as:

```text
         Scheduler
            ↓
Ready ─────────────→ Running
 ↑                    │
 │                    │ vTaskDelay()
 │                    ↓
 └──────────────── Blocked
      Delay expires
```

This is one of the most important FreeRTOS concepts.

---

## 21. Experiment 6 — LED Tasks with Different Priorities

Connect:

```text
LED 1 → GPIO 25
LED 2 → GPIO 26
LED 3 → GPIO 27
```

Define:

```cpp
#define LED1 25
#define LED2 26
#define LED3 27
```

Create three tasks.

### Task 1

```cpp
void LEDTask1(void *parameter) {

  while (1) {

    digitalWrite(
      LED1,
      !digitalRead(LED1)
    );

    Serial.println(
      "LED Task 1"
    );

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}
```

### Task 2

```cpp
void LEDTask2(void *parameter) {

  while (1) {

    digitalWrite(
      LED2,
      !digitalRead(LED2)
    );

    Serial.println(
      "LED Task 2"
    );

    vTaskDelay(
      pdMS_TO_TICKS(600)
    );
  }
}
```

### Task 3

```cpp
void LEDTask3(void *parameter) {

  while (1) {

    digitalWrite(
      LED3,
      !digitalRead(LED3)
    );

    Serial.println(
      "LED Task 3"
    );

    vTaskDelay(
      pdMS_TO_TICKS(300)
    );
  }
}
```

Assign:

```text
LED Task 1 → Priority 1
LED Task 2 → Priority 2
LED Task 3 → Priority 3
```

---

## 22. Complete Three-Task Example

```cpp
#include <Arduino.h>

#define LED1 25
#define LED2 26
#define LED3 27

void LEDTask1(void *parameter) {

  while (1) {

    digitalWrite(
      LED1,
      !digitalRead(LED1)
    );

    Serial.println(
      "Task 1 - Priority 1"
    );

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}

void LEDTask2(void *parameter) {

  while (1) {

    digitalWrite(
      LED2,
      !digitalRead(LED2)
    );

    Serial.println(
      "Task 2 - Priority 2"
    );

    vTaskDelay(
      pdMS_TO_TICKS(600)
    );
  }
}

void LEDTask3(void *parameter) {

  while (1) {

    digitalWrite(
      LED3,
      !digitalRead(LED3)
    );

    Serial.println(
      "Task 3 - Priority 3"
    );

    vTaskDelay(
      pdMS_TO_TICKS(300)
    );
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  xTaskCreatePinnedToCore(
    LEDTask1,
    "LED Task 1",
    2048,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    LEDTask2,
    "LED Task 2",
    2048,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    LEDTask3,
    "LED Task 3",
    2048,
    NULL,
    3,
    NULL,
    1
  );
}

void loop() {
}
```

---

## 23. Experiment 7 — Remove the Delay from the Highest-Priority Task

Modify Task 3:

```cpp
void LEDTask3(void *parameter) {

  while (1) {

    Serial.println(
      "Highest Priority Task"
    );

  }
}
```

Observe the behavior of lower-priority tasks.

Potential result:

```text
Task 3
Task 3
Task 3
Task 3
...
```

Task 1 and Task 2 may receive very little or no CPU time because Task 3 remains permanently ready.

---

## 24. Task Starvation

This problem is called **starvation**.

```text
High-Priority Task
Never Blocks
      ↓
Always Ready
      ↓
Scheduler Always Chooses It
      ↓
Lower-Priority Tasks
Cannot Run
```

Therefore, real-time tasks should usually:

```text
Delay
Block
Wait for Event
Wait for Queue
Wait for Semaphore
```

when they have no useful work to perform.

---

## 25. Experiment 8 — Restore Cooperative CPU Use

Restore:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(10)
);
```

inside the highest-priority task.

Now:

```text
Task 3
  ↓
Runs
  ↓
Blocks
  ↓
Task 2 or Task 1 Can Run
```

Observe the change.

---

## 26. Experiment 9 — Runtime Priority Change

Task handles allow the application to change priorities while running.

Declare:

```cpp
TaskHandle_t taskAHandle;
TaskHandle_t taskBHandle;
```

Create:

```cpp
xTaskCreatePinnedToCore(
    TaskA,
    "Task A",
    2048,
    NULL,
    1,
    &taskAHandle,
    1
);

xTaskCreatePinnedToCore(
    TaskB,
    "Task B",
    2048,
    NULL,
    2,
    &taskBHandle,
    1
);
```

Change Task A:

```cpp
vTaskPrioritySet(
    taskAHandle,
    3
);
```

The priority changes from:

```text
Task A
1 → 3
```

Task A may then preempt Task B when both are ready.

---

## 27. Experiment 10 — Check Task Priority

Use:

```cpp
UBaseType_t priority =
    uxTaskPriorityGet(
      taskAHandle
    );
```

Display:

```cpp
Serial.print(
  "Task A Priority = "
);

Serial.println(
  priority
);
```

This allows the program to inspect task priorities during execution.

---

## 28. Experiment 11 — Measure Task Execution Time

Use:

```cpp
unsigned long start =
    micros();
```

and:

```cpp
unsigned long end =
    micros();
```

Then:

```cpp
unsigned long executionTime =
    end - start;
```

Mathematically:

$$
T_{\text{exec}}
=
t_{\text{end}}
-
t_{\text{start}}.
$$

Record:

| Task | Priority | Execution Time |
|---|---:|---:|
| Task A | 1 |  |
| Task B | 2 |  |
| Task C | 3 |  |

---

## 29. Experiment 12 — Measure Task Period

If a periodic task runs at times:

$$
t_1,t_2,t_3,\ldots
$$

then the measured period is:

$$
T_i
=
t_i-t_{i-1}.
$$

For a nominal:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(1000)
);
```

the observed interval should be approximately:

$$
T\approx1~\text{s}.
$$

However, execution and scheduling overhead may influence the exact measured value.

---

## 30. `vTaskDelay()` vs. `vTaskDelayUntil()`

### `vTaskDelay()`

Delays relative to the current execution time.

```text
Task Work
    ↓
Delay 1000 ms
    ↓
Task Work
```

The task period may include both:

```text
Execution Time
+
Delay Time
```

### `vTaskDelayUntil()`

Supports more regular periodic execution.

Conceptually:

```text
t = 0
t = 1000
t = 2000
t = 3000
```

This is useful for periodic real-time tasks.

---

## 31. Experiment 13 — Periodic Task with `vTaskDelayUntil()`

Example:

```cpp
void PeriodicTask(void *parameter) {

  TickType_t lastWakeTime;

  lastWakeTime =
      xTaskGetTickCount();

  while (1) {

    Serial.println(
      "Periodic Task"
    );

    vTaskDelayUntil(
      &lastWakeTime,
      pdMS_TO_TICKS(1000)
    );
  }
}
```

The intended period is:

$$
T=1000~\text{ms}.
$$

---

## 32. Scheduling Timeline

Consider three tasks:

| Task | Priority | Period |
|---|---:|---:|
| A | 1 | 1000 ms |
| B | 2 | 500 ms |
| C | 3 | 250 ms |

Conceptually:

```text
Time ─────────────────────────────────→

Task C  █   █   █   █   █   █   █
Task B    ██      ██      ██
Task A        █           █
```

The actual timeline depends on task execution time and blocking behavior, but the highest-priority **ready** task receives the CPU.

---

## 33. Priority vs. Period

Priority and period are different properties.

```text
Priority
   ↓
Determines Scheduling Preference

Period
   ↓
Determines How Often Task Becomes Ready
```

For example:

```text
High Priority
does not necessarily mean
High Execution Frequency
```

because a high-priority task may spend most of its time blocked.

---

## 34. Experiment 14 — Simulate a Real Embedded Application

Create three tasks:

```text
Sensor Task
Priority 3

Control Task
Priority 2

Display Task
Priority 1
```

Architecture:

```text
Sensor Task
    ↓
Read Input

Control Task
    ↓
Process / Decide

Display Task
    ↓
Print Status
```

Possible periods:

| Task | Priority | Period |
|---|---:|---:|
| Sensor | 3 | 100 ms |
| Control | 2 | 200 ms |
| Display | 1 | 1000 ms |

This reflects a simple real-time priority scheme:

```text
Time-Critical Sensor
       ↓
Higher Priority

Control
       ↓
Medium Priority

Display
       ↓
Lower Priority
```

---

## 35. Experiment 15 — Introduce CPU Load

Create a low-priority background task:

```cpp
void BackgroundTask(
    void *parameter
) {

  while (1) {

    for (
      volatile long i = 0;
      i < 5000000;
      i++
    ) {
    }
  }
}
```

Assign:

```text
Priority = 1
```

Keep the sensor task at:

```text
Priority = 3
```

Observe whether the sensor task still runs periodically.

This demonstrates how preemptive scheduling protects more important real-time tasks from lower-priority CPU load.

---

## 36. Priority Assignment Principle

A simple real-time priority policy is:

```text
More Time-Critical
       ↓
Higher Priority

Less Time-Critical
       ↓
Lower Priority
```

For example:

| Function | Example Priority |
|---|---:|
| Emergency response | 4 |
| Sensor sampling | 3 |
| Control algorithm | 2 |
| Display/logging | 1 |

Priority should not be chosen merely because a task seems important. It should reflect timing and responsiveness requirements.

---

## 37. Scheduling Problems

Poor priority design can produce several problems.

### Starvation

```text
High-Priority Task
Never Blocks
      ↓
Low-Priority Task
Never Runs
```

### Excessive CPU Usage

```text
Busy Loop
   ↓
CPU Continuously Active
```

### Timing Jitter

```text
Expected Activation
        ≠
Actual Activation
```

### Priority Inversion

A lower-priority task may temporarily block a higher-priority task when both depend on a shared resource.

Priority inversion will be studied further in the synchronization lab.

---

## 38. Scheduling Performance Metrics

Useful scheduling metrics include:

### Execution Time

$$
T_{\text{exec}}
=
t_{\text{finish}}
-
t_{\text{start}}.
$$

### Response Time

$$
T_{\text{response}}
=
t_{\text{start}}
-
t_{\text{ready}}.
$$

### Task Period

$$
T_i
=
t_i-t_{i-1}.
$$

### Jitter

A simple timing error can be represented as:

$$
J_i
=
T_i-T_{\text{desired}}.
$$

These measurements help evaluate real-time behavior.

---

## 39. Experiments Summary

### Experiment 1 — Equal Priorities

Create two tasks with equal priority.

### Experiment 2 — Different Priorities

Assign Task B a higher priority than Task A.

### Experiment 3 — Three Tasks

Create priorities 1, 2, and 3.

### Experiment 4 — Direct Preemption

Observe a high-priority periodic task interrupting a CPU-intensive low-priority task.

### Experiment 5 — Task Delay

Observe the transition between running, blocked, and ready states.

### Experiment 6 — LED Multitasking

Control three LEDs using three tasks.

### Experiment 7 — Starvation

Remove delay from the highest-priority task.

### Experiment 8 — Restore Blocking

Add delay to allow lower-priority tasks to execute.

### Experiment 9 — Dynamic Priority

Change task priority during runtime.

### Experiment 10 — Priority Inspection

Read current task priorities.

### Experiment 11 — Execution Time

Measure task execution time.

### Experiment 12 — Task Period

Measure periodic execution intervals.

### Experiment 13 — `vTaskDelayUntil()`

Create a more precise periodic task.

### Experiment 14 — Real-Time Application

Implement sensor, control, and display tasks.

### Experiment 15 — CPU Load

Study scheduling under background computation.

---

## 40. Exercises

### Exercise 1 — Three-Priority Tasks

Create:

```text
Task A → Priority 1
Task B → Priority 2
Task C → Priority 3
```

Each task should print:

```text
Task Name
Priority
Timestamp
```

Observe the execution order.

### Exercise 2 — LED Scheduler

Implement:

```text
Task 1
 ↓
LED 1
 ↓
Priority 1

Task 2
 ↓
LED 2
 ↓
Priority 2

Task 3
 ↓
LED 3
 ↓
Priority 3
```

Use different blinking intervals.

### Exercise 3 — Demonstrate Preemption

Create:

```text
Low-Priority Task
        ↓
Long CPU Computation

High-Priority Task
        ↓
Periodic Message
```

Demonstrate that the high-priority task preempts the low-priority task.

### Exercise 4 — Demonstrate Starvation

Create a highest-priority task that does not block.

Observe the lower-priority tasks.

Then add:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(10)
);
```

and compare the behavior.

### Exercise 5 — Dynamic Priority

Start with:

```text
Task A = Priority 1
Task B = Priority 2
```

After 10 seconds, change:

```text
Task A = Priority 3
```

Observe the new scheduling behavior.

### Exercise 6 — Periodic Real-Time Tasks

Create:

```text
Sensor Task
100 ms
Priority 3

Control Task
200 ms
Priority 2

Logging Task
1000 ms
Priority 1
```

Use `vTaskDelayUntil()` for periodic execution.

### Exercise 7 — Scheduling Analysis

Record:

| Task | Priority | Desired Period | Measured Period | Execution Time |
|---|---:|---:|---:|---:|
| Sensor | 3 | 100 ms |  |  |
| Control | 2 | 200 ms |  |  |
| Logging | 1 | 1000 ms |  |  |

Discuss whether the measured behavior agrees with priority-based preemptive scheduling.

---

## 41. Checkpoint Questions

1. What is a FreeRTOS task?
2. What does the FreeRTOS scheduler do?
3. What is task priority?
4. Which task runs when several ready tasks have different priorities?
5. What is preemptive scheduling?
6. What does it mean for one task to preempt another?
7. What is the difference between the Ready and Running states?
8. What causes a task to enter the Blocked state?
9. What does `vTaskDelay()` do?
10. Why can lower-priority tasks run when a high-priority task uses `vTaskDelay()`?
11. What can happen if the highest-priority task never blocks?
12. What is task starvation?
13. How can starvation be reduced?
14. What does `xTaskCreate()` do?
15. What does `xTaskCreatePinnedToCore()` do?
16. Why can pinning tasks to the same core help demonstrate scheduling?
17. What is the difference between task priority and task period?
18. What does `vTaskPrioritySet()` do?
19. What does `uxTaskPriorityGet()` return?
20. What is the difference between `vTaskDelay()` and `vTaskDelayUntil()`?
21. Why is `vTaskDelayUntil()` useful for periodic tasks?
22. What is execution time?
23. What is scheduling jitter?
24. Why might a sensor-acquisition task have a higher priority than a display task?
25. Why should task priorities be assigned according to timing requirements?

---

## 42. Expected Results

Students should observe that FreeRTOS schedules tasks according to priority and task state.

With:

```text
Task C → Priority 3
Task B → Priority 2
Task A → Priority 1
```

the scheduler should prefer:

```text
Task C
   ↓
Task B
   ↓
Task A
```

when all three tasks are ready.

Students should also observe:

```text
High-Priority Task
      ↓
Becomes Ready
      ↓
Preempts
      ↓
Low-Priority Task
```

When the high-priority task calls `vTaskDelay()`:

```text
High-Priority Task
      ↓
Blocked
      ↓
Lower-Priority Task
      ↓
Runs
```

Removing all blocking from the highest-priority task should demonstrate starvation of lower-priority tasks.

---

## 43. Submission

Submit:

- FreeRTOS source code.
- Screenshot of Serial Monitor output.
- Two equal-priority task results.
- Two different-priority task results.
- Three-task scheduling results.
- LED multitasking demonstration.
- Preemption experiment.
- Starvation experiment.
- Dynamic-priority experiment.
- `vTaskDelayUntil()` experiment.
- Task execution-time measurements.
- Task-period measurements.
- Brief discussion of observed scheduling behavior.

Suggested result table:

| Experiment | Task | Priority | Delay / Period | Observation |
|---|---|---:|---:|---|
| Equal Priority | A | 1 | 500 ms |  |
| Equal Priority | B | 1 | 500 ms |  |
| Different Priority | A | 1 | 500 ms |  |
| Different Priority | B | 2 | 500 ms |  |
| Three Tasks | A | 1 | 1000 ms |  |
| Three Tasks | B | 2 | 700 ms |  |
| Three Tasks | C | 3 | 400 ms |  |

Suggested timing table:

| Task | Priority | Desired Period | Average Measured Period | Execution Time |
|---|---:|---:|---:|---:|
| Sensor | 3 | 100 ms |  |  |
| Control | 2 | 200 ms |  |  |
| Logging | 1 | 1000 ms |  |  |

---

## 44. Discussion

Students should explain how the observed behavior demonstrates **priority-based preemptive scheduling**.

A task with a higher priority does not necessarily execute continuously. It receives precedence only while it is in the **Ready** state. When it calls `vTaskDelay()`, waits for an event, or otherwise blocks, the scheduler selects another ready task.

The experiments should demonstrate the important relationship:

$$
\boxed{
\text{Scheduling}
=
\text{Priority}
+
\text{Task State}
}
$$

not priority alone.

Students should also discuss why a high-priority task that never blocks can prevent lower-priority tasks from executing. This illustrates the importance of good task design in real-time embedded systems.

---

## 45. Conclusion

In this lab, multiple FreeRTOS tasks are created on the ESP32 and assigned different priorities to investigate **preemptive real-time scheduling**. The experiments demonstrate that the scheduler selects the highest-priority ready task and can preempt a lower-priority task whenever a more important task becomes ready.

The effects of `vTaskDelay()`, task blocking, equal priorities, dynamic priority changes, CPU-intensive tasks, and task starvation are also investigated. Students learn that effective real-time multitasking requires not only appropriate task priorities but also correct use of blocking and periodic scheduling mechanisms.

The FreeRTOS execution model can be summarized as:

```text
Create Tasks
     ↓
Assign Priorities
     ↓
Tasks Become Ready
     ↓
Scheduler Selects
Highest-Priority Ready Task
     ↓
Task Runs
     ↓
Task Blocks / Delays
     ↓
Scheduler Selects Next Task
```

This lab provides the foundation for the next topic:

```text
LAB 5
FreeRTOS Tasks & Preemptive Scheduling
      ↓
LAB 6
Task Synchronization
      ↓
Semaphores
      ↓
Mutexes
      ↓
Race-Condition Prevention
```

The progression moves from **concurrent task execution** to **safe coordination and synchronization among multiple real-time tasks**.
