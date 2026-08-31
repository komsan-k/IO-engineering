# LAB 6 — FreeRTOS Mutex for Race-Condition Prevention

## 1. Objective

The objective of this lab is to:

1. Understand shared resources in concurrent embedded systems.
2. Create multiple FreeRTOS tasks that access the same variable.
3. Demonstrate a **race condition** caused by unsynchronized access.
4. Identify a **critical section** in a multitasking program.
5. Understand why operations such as `counter++` are not always atomic.
6. Create and use a FreeRTOS **mutex**.
7. Protect a shared resource using `xSemaphoreTake()` and `xSemaphoreGive()`.
8. Compare system behavior before and after mutex protection.
9. Observe task blocking while waiting for a mutex.
10. Understand mutual exclusion and ownership.
11. Measure the effect of synchronization on correctness and timing.
12. Apply mutex protection to realistic shared resources such as Serial output and sensor data.

---

## 2. Background

In a multitasking embedded system, several tasks may access the same resource.

Examples include:

```text
Shared Variable
Shared Sensor Data
Serial Port
I²C Bus
SPI Bus
Display
File
Actuator
```

Consider two tasks:

```text
Task A
   ↓
counter++

Task B
   ↓
counter++
```

Both tasks access the same variable:

```text
counter
```

If they execute concurrently without synchronization, the final result may be incorrect.

This problem is called a **race condition**.

---

## 3. Race Condition

A race condition occurs when the program result depends on the timing or interleaving of multiple tasks accessing shared data.

Conceptually:

```text
Task A ─────┐
            ├──→ Shared Resource
Task B ─────┘
```

Without protection:

```text
Task A
  ↓
Read counter

Task B
  ↓
Read counter

Task A
  ↓
Modify

Task B
  ↓
Modify

Task A
  ↓
Write

Task B
  ↓
Write
```

One update can overwrite another.

---

## 4. Why `counter++` Can Cause a Race Condition

The expression:

```cpp
counter++;
```

appears to be one operation, but conceptually it may consist of:

```text
Read counter
     ↓
Add 1
     ↓
Write counter
```

Mathematically:

$$
x_{\text{new}} = x_{\text{old}} + 1.
$$

Suppose:

```text
counter = 10
```

Two tasks execute almost simultaneously.

### Task A

```text
Read 10
Add 1
Result = 11
```

### Task B

```text
Read 10
Add 1
Result = 11
```

Both write:

```text
11
```

The expected result is:

```text
12
```

but the actual result may be:

```text
11
```

This is called a **lost update**.

---

## 5. Critical Section

A **critical section** is a section of code that accesses a shared resource and should not be executed by multiple tasks simultaneously.

For example:

```cpp
counter++;
```

may be treated as a critical section.

Conceptually:

```text
Enter Critical Section
        ↓
Access Shared Resource
        ↓
Leave Critical Section
```

Only one task should enter the protected section at a time.

---

## 6. Mutual Exclusion

A mutex provides:

```text
MUTual EXclusion
```

The basic rule is:

```text
One Shared Resource
       ↓
One Mutex
       ↓
Only One Task at a Time
```

If Task A owns the mutex:

```text
Task A
  ↓
LOCKED
  ↓
Shared Resource
```

Task B must wait:

```text
Task B
  ↓
Waiting
```

When Task A releases the mutex:

```text
Task A
  ↓
Unlock
  ↓
Task B Can Continue
```

---

## 7. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Optional LED
- Optional sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS
- Serial Monitor

---

## 8. FreeRTOS Mutex Functions

FreeRTOS mutexes are provided through the semaphore API.

Include:

```cpp
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
```

Create a mutex handle:

```cpp
SemaphoreHandle_t mutex;
```

Create the mutex:

```cpp
mutex = xSemaphoreCreateMutex();
```

Acquire it:

```cpp
xSemaphoreTake(
    mutex,
    portMAX_DELAY
);
```

Release it:

```cpp
xSemaphoreGive(
    mutex
);
```

---

## 9. Mutex Operation

The protected sequence is:

```text
Take Mutex
    ↓
Critical Section
    ↓
Give Mutex
```

In code:

```cpp
xSemaphoreTake(
    mutex,
    portMAX_DELAY
);

counter++;

xSemaphoreGive(
    mutex
);
```

---

## 10. Experiment 1 — Shared Counter Without Protection

Create a shared variable:

```cpp
volatile long counter = 0;
```

Create two tasks:

```cpp
void TaskA(void *parameter) {

  for (int i = 0; i < 100000; i++) {
    counter++;
  }

  Serial.println(
    "Task A finished"
  );

  vTaskDelete(NULL);
}
```

and:

```cpp
void TaskB(void *parameter) {

  for (int i = 0; i < 100000; i++) {
    counter++;
  }

  Serial.println(
    "Task B finished"
  );

  vTaskDelete(NULL);
}
```

The expected final value is:

$$
N_{\text{expected}}
= 100000+100000
= 200000.
$$

---

## 11. Complete Race-Condition Program

```cpp
#include <Arduino.h>

volatile long counter = 0;

void TaskA(void *parameter) {

  for (int i = 0; i < 100000; i++) {
    counter++;
  }

  Serial.println(
    "Task A finished"
  );

  vTaskDelete(NULL);
}

void TaskB(void *parameter) {

  for (int i = 0; i < 100000; i++) {
    counter++;
  }

  Serial.println(
    "Task B finished"
  );

  vTaskDelete(NULL);
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

  delay(3000);

  Serial.print(
    "Final counter = "
  );

  Serial.println(
    counter
  );
}

void loop() {
}
```

Run the experiment several times.

---

## 12. Expected Observation

Expected:

```text
Final counter = 200000
```

But the observed value may be smaller:

```text
Final counter = 183421
```

or:

```text
Final counter = 194672
```

The exact result can vary between runs.

This is evidence of a race condition.

---

## 13. Race-Condition Error

Define:

$$
E
= N_{\text{expected}}
- N_{\text{actual}}.
$$

For example:

$$
E
= 200000
- 194672
=
5328.
$$

The percentage error is:

$$
E_{\%}
= \frac{
N_{\text{expected}}
- N_{\text{actual}}
}{
N_{\text{expected}}
}
\times100\%.
$$

---

## 14. Experiment 2 — Repeat the Race-Condition Test

Run the program at least 10 times.

Record:

| Trial | Expected | Actual | Lost Updates |
|---:|---:|---:|---:|
| 1 | 200000 |  |  |
| 2 | 200000 |  |  |
| 3 | 200000 |  |  |
| 4 | 200000 |  |  |
| 5 | 200000 |  |  |
| 6 | 200000 |  |  |
| 7 | 200000 |  |  |
| 8 | 200000 |  |  |
| 9 | 200000 |  |  |
| 10 | 200000 |  |  |

Observe whether the result is deterministic.

---

## 15. Experiment 3 — Increase the Chance of a Race Condition

A race condition may not always be obvious in a small program.

To make it more visible, explicitly separate the read, modify, and write operations.

```cpp
long temp;

temp = counter;

taskYIELD();

temp = temp + 1;

taskYIELD();

counter = temp;
```

This produces:

```text
Read
 ↓
Yield
 ↓
Modify
 ↓
Yield
 ↓
Write
```

The scheduler has more opportunities to switch between tasks.

---

## 16. Demonstration of Interleaving

Suppose:

```text
counter = 5
```

The execution may occur as:

| Time | Task A | Task B | Counter |
|---|---|---|---:|
| 1 | Read 5 | | 5 |
| 2 | | Read 5 | 5 |
| 3 | Calculate 6 | | 5 |
| 4 | | Calculate 6 | 5 |
| 5 | Write 6 | | 6 |
| 6 | | Write 6 | 6 |

Two increments occurred, but:

```text
Final counter = 6
```

instead of:

```text
7
```

---

## 17. Experiment 4 — Create a FreeRTOS Mutex

Declare:

```cpp
SemaphoreHandle_t counterMutex;
```

In `setup()`:

```cpp
counterMutex =
    xSemaphoreCreateMutex();
```

Verify creation:

```cpp
if (
  counterMutex == NULL
) {

  Serial.println(
    "Mutex creation failed"
  );
}
else {

  Serial.println(
    "Mutex created"
  );
}
```

---

## 18. Experiment 5 — Protect the Shared Counter

Modify Task A:

```cpp
void TaskA(void *parameter) {

  for (int i = 0; i < 100000; i++) {

    xSemaphoreTake(
      counterMutex,
      portMAX_DELAY
    );

    counter++;

    xSemaphoreGive(
      counterMutex
    );
  }

  vTaskDelete(NULL);
}
```

Modify Task B in the same way:

```cpp
void TaskB(void *parameter) {

  for (int i = 0; i < 100000; i++) {

    xSemaphoreTake(
      counterMutex,
      portMAX_DELAY
    );

    counter++;

    xSemaphoreGive(
      counterMutex
    );
  }

  vTaskDelete(NULL);
}
```

---

## 19. Complete Mutex-Protected Program

```cpp
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

volatile long counter = 0;

SemaphoreHandle_t counterMutex;

void TaskA(void *parameter) {

  for (int i = 0; i < 100000; i++) {

    xSemaphoreTake(
      counterMutex,
      portMAX_DELAY
    );

    counter++;

    xSemaphoreGive(
      counterMutex
    );
  }

  Serial.println(
    "Task A finished"
  );

  vTaskDelete(NULL);
}

void TaskB(void *parameter) {

  for (int i = 0; i < 100000; i++) {

    xSemaphoreTake(
      counterMutex,
      portMAX_DELAY
    );

    counter++;

    xSemaphoreGive(
      counterMutex
    );
  }

  Serial.println(
    "Task B finished"
  );

  vTaskDelete(NULL);
}

void setup() {

  Serial.begin(115200);

  counterMutex =
      xSemaphoreCreateMutex();

  if (
    counterMutex == NULL
  ) {

    Serial.println(
      "Mutex creation failed"
    );

    while (1) {
    }
  }

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

  delay(4000);

  Serial.print(
    "Final counter = "
  );

  Serial.println(
    counter
  );
}

void loop() {
}
```

---

## 20. Expected Result with Mutex

Now the expected result should be:

```text
Final counter = 200000
```

The mutex ensures:

```text
Task A
  ↓
Take Mutex
  ↓
counter++
  ↓
Give Mutex

Task B
  ↓
Take Mutex
  ↓
counter++
  ↓
Give Mutex
```

The tasks can no longer modify the counter simultaneously.

---

## 21. Mutual-Exclusion Timeline

Without mutex:

```text
Task A: READ ─ MODIFY ─ WRITE
Task B:    READ ─ MODIFY ─ WRITE
```

The operations overlap.

With mutex:

```text
Task A: LOCK ─ READ ─ MODIFY ─ WRITE ─ UNLOCK
Task B:                                      LOCK ─ READ ─ MODIFY ─ WRITE ─ UNLOCK
```

The critical sections are serialized.

---

## 22. Experiment 6 — Observe Mutex Blocking

Add messages:

```cpp
Serial.println(
  "Task A waiting for mutex"
);

xSemaphoreTake(
  counterMutex,
  portMAX_DELAY
);

Serial.println(
  "Task A acquired mutex"
);
```

Inside the critical section:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(500)
);
```

Then release:

```cpp
Serial.println(
  "Task A releasing mutex"
);

xSemaphoreGive(
  counterMutex
);
```

Create similar messages in Task B.

---

## 23. Expected Mutex Behavior

Possible output:

```text
Task A waiting for mutex
Task A acquired mutex

Task B waiting for mutex

Task A releasing mutex

Task B acquired mutex
Task B releasing mutex
```

This demonstrates that the waiting task enters a blocked state rather than accessing the protected resource.

---

## 24. Mutex Ownership

A mutex has an ownership concept.

```text
Task A Takes Mutex
       ↓
Task A Owns Mutex
       ↓
Task B Cannot Take It
       ↓
Task A Gives Mutex
       ↓
Ownership Released
```

The task that owns a mutex should release it after leaving the critical section.

---

## 25. Experiment 7 — Protect Serial Output

Serial output itself can be treated as a shared resource when multiple tasks print several related lines.

Create:

```cpp
SemaphoreHandle_t serialMutex;
```

Then:

```cpp
serialMutex =
    xSemaphoreCreateMutex();
```

Protect a group of messages:

```cpp
xSemaphoreTake(
  serialMutex,
  portMAX_DELAY
);

Serial.println(
  "Task A"
);

Serial.println(
  "Sensor value = 125"
);

Serial.println(
  "Status = OK"
);

xSemaphoreGive(
  serialMutex
);
```

This prevents another task from inserting output in the middle of the message group.

---

## 26. Experiment 8 — Serial Output Without Mutex

Create two tasks.

Task A:

```cpp
Serial.println("Task A START");
Serial.println("A processing...");
Serial.println("Task A END");
```

Task B:

```cpp
Serial.println("Task B START");
Serial.println("B processing...");
Serial.println("Task B END");
```

Possible output:

```text
Task A START
Task B START
A processing...
B processing...
Task A END
Task B END
```

This demonstrates uncoordinated access to a shared output resource.

---

## 27. Experiment 9 — Serial Output with Mutex

Protect each complete message group.

```cpp
xSemaphoreTake(
  serialMutex,
  portMAX_DELAY
);

Serial.println("Task A START");
Serial.println("A processing...");
Serial.println("Task A END");

xSemaphoreGive(
  serialMutex
);
```

Expected:

```text
Task A START
A processing...
Task A END

Task B START
B processing...
Task B END
```

or the reverse order, but each group remains intact.

---

## 28. Experiment 10 — Shared Sensor Structure

Consider:

```cpp
struct SensorData {

  float temperature;
  float humidity;
  unsigned long timestamp;
};
```

Create:

```cpp
SensorData sharedData;
```

One task updates:

```text
Sensor Task
    ↓
sharedData
```

Another reads:

```text
Display Task
    ↓
sharedData
```

Without synchronization, the reader could observe values from different update moments.

---

## 29. Protect Shared Sensor Data

Create:

```cpp
SemaphoreHandle_t dataMutex;
```

Writer:

```cpp
xSemaphoreTake(
  dataMutex,
  portMAX_DELAY
);

sharedData.temperature =
    27.5;

sharedData.humidity =
    65.0;

sharedData.timestamp =
    millis();

xSemaphoreGive(
  dataMutex
);
```

Reader:

```cpp
xSemaphoreTake(
  dataMutex,
  portMAX_DELAY
);

SensorData localCopy =
    sharedData;

xSemaphoreGive(
  dataMutex
);
```

Then process `localCopy` outside the critical section.

This keeps the protected section short.

---

## 30. Critical-Section Design

A good critical section should generally be:

```text
Short
+
Necessary
+
Well Defined
```

Avoid:

```text
Take Mutex
    ↓
Long Computation
    ↓
Network Access
    ↓
Long Delay
    ↓
Give Mutex
```

Prefer:

```text
Take Mutex
    ↓
Copy Shared Data
    ↓
Give Mutex
    ↓
Perform Long Processing
```

This reduces waiting time for other tasks.

---

## 31. Experiment 11 — Measure Mutex Waiting Time

Measure before taking the mutex:

```cpp
unsigned long startWait =
    micros();
```

Then:

```cpp
xSemaphoreTake(
  counterMutex,
  portMAX_DELAY
);
```

Measure:

```cpp
unsigned long endWait =
    micros();
```

Calculate:

```cpp
unsigned long waitTime =
    endWait - startWait;
```

Mathematically:

$$
T_{\text{wait}}
= t_{\text{acquire}}
- t_{\text{request}}.
$$

---

## 32. Mutex Waiting-Time Table

Record:

| Trial | Task | Priority | Waiting Time ($\mu$s) |
|---:|---|---:|---:|
| 1 | A | 1 |  |
| 2 | B | 1 |  |
| 3 | A | 1 |  |
| 4 | B | 1 |  |
| 5 | A | 1 |  |

Calculate:

$$
\bar{T}_{\text{wait}}
= \frac{1}{N}
\sum_{i=1}^{N}
T_{\text{wait},i}.
$$

---

## 33. Experiment 12 — Different Task Priorities

Create:

```text
Low-Priority Task
Priority = 1

High-Priority Task
Priority = 3
```

Both use the same mutex.

The low-priority task acquires the mutex first.

Then the high-priority task attempts to acquire it.

Conceptually:

```text
Low Task
   ↓
Owns Mutex

High Task
   ↓
Becomes Ready
   ↓
Needs Mutex
   ↓
Blocked
```

This introduces the concept of **priority inversion**.

---

## 34. Priority Inversion

Priority inversion can occur when:

```text
High-Priority Task
       ↓
Needs Resource

Low-Priority Task
       ↓
Owns Resource
```

The high-priority task must wait for the low-priority task.

Conceptually:

```text
High Priority
     ↓
Waiting

Low Priority
     ↓
Holding Mutex
```

FreeRTOS mutexes support **priority inheritance** to help reduce this problem.

---

## 35. Priority Inheritance

Suppose:

```text
High Task = Priority 3
Low Task  = Priority 1
```

If the high-priority task waits for a mutex owned by the low-priority task, the mutex mechanism may temporarily increase the effective priority of the mutex owner.

Conceptually:

```text
Low Task
Priority 1
   ↓
Owns Mutex

High Task
Priority 3
   ↓
Waits for Mutex

Low Task
Temporarily Inherits
Higher Priority
   ↓
Finishes Critical Section
   ↓
Releases Mutex
```

This helps the mutex owner complete its critical section sooner.

---

## 36. Mutex vs. Binary Semaphore

A mutex and a binary semaphore can appear similar, but their intended purposes are different.

| Feature | Mutex | Binary Semaphore |
|---|---|---|
| Mutual exclusion | Yes | Can be used, but not primary purpose |
| Ownership | Yes | No |
| Priority inheritance | Typically yes | No |
| Resource protection | Preferred | Not preferred |
| Event signaling | No | Yes |

Rule of thumb:

```text
Mutex
   ↓
Protect Shared Resource

Semaphore
   ↓
Signal Event / Synchronize Tasks
```

---

## 37. Mutex vs. Queue

A queue solves a different problem.

```text
Mutex
   ↓
Protect Access

Queue
   ↓
Transfer Data
```

For example:

```text
Shared Variable
     ↓
Mutex
```

whereas:

```text
Sensor Task
     ↓
Queue
     ↓
Processing Task
```

may avoid shared-memory access entirely.

---

## 38. Experiment 13 — Compare Three Designs

Implement three versions.

### Version A — Shared Variable Without Protection

```text
Task A ──→ Shared Counter ←── Task B
```

### Version B — Shared Variable with Mutex

```text
Task A
  ↓
Mutex
  ↓
Shared Counter
  ↑
Mutex
  ↑
Task B
```

### Version C — Queue-Based Communication

```text
Producer Task
      ↓
Queue
      ↓
Consumer Task
```

Compare:

| Property | No Protection | Mutex | Queue |
|---|---|---|---|
| Data correctness | Poor | Good | Good |
| Shared memory | Yes | Yes | Reduced |
| Blocking | No | Yes | Yes |
| Data transfer | No | No | Yes |
| Suitable for shared resource | No | Yes | Sometimes |

---

## 39. Experiment 14 — Compare Execution Time

Measure the total time required for the two tasks to perform all increments.

Without mutex:

$$
T_{\text{no mutex}}.
$$

With mutex:

$$
T_{\text{mutex}}.
$$

Synchronization overhead may produce:

$$
T_{\text{mutex}}
>
T_{\text{no mutex}}.
$$

However, correctness is more important than simply obtaining a smaller execution time.

---

## 40. Correctness vs. Performance

The trade-off can be represented as:

```text
No Synchronization
        ↓
Potentially Faster
        ↓
Incorrect Result

Mutex Synchronization
        ↓
Additional Overhead
        ↓
Correct Shared Access
```

Therefore:

$$
\boxed{
\text{Correctness}
>
\text{Unprotected Speed}
}
$$

for shared-resource access.

---

## 41. Experiment 15 — Safe LED State Resource

Create a shared LED state:

```cpp
int ledState = LOW;
```

Task A modifies it.

Task B reads and writes the physical LED.

Protect:

```cpp
xSemaphoreTake(
  mutex,
  portMAX_DELAY
);

ledState =
    !ledState;

xSemaphoreGive(
  mutex
);
```

This demonstrates mutex use with an application-level shared state.

---

## 42. Common Mutex Errors

### Forgetting to Create the Mutex

```cpp
SemaphoreHandle_t mutex;
```

without:

```cpp
mutex =
    xSemaphoreCreateMutex();
```

### Forgetting to Release the Mutex

```text
Take Mutex
    ↓
Never Give Mutex
    ↓
Other Tasks Wait Forever
```

### Holding the Mutex Too Long

```text
Take
 ↓
Long Delay
 ↓
Long Computation
 ↓
Give
```

This increases blocking.

### Taking the Same Non-Recursive Mutex Again

A task should not repeatedly acquire the same normal mutex without releasing it.

### Using a Mutex from an ISR

Normal mutex operations are not intended for interrupt service routines.

Use ISR-safe synchronization mechanisms where appropriate.

---

## 43. Deadlock Concept

A deadlock can occur when tasks wait forever for resources held by each other.

Example:

```text
Task A
  ↓
Owns Mutex 1
  ↓
Waits for Mutex 2

Task B
  ↓
Owns Mutex 2
  ↓
Waits for Mutex 1
```

Result:

```text
Task A waits for Task B
        +
Task B waits for Task A
        ↓
Deadlock
```

This lab uses one mutex to avoid introducing complex deadlock scenarios, but the concept is important for larger systems.

---

## 44. Experiments Summary

### Experiment 1 — Shared Counter

Create two tasks that increment the same counter without protection.

### Experiment 2 — Repeated Race Test

Run the experiment multiple times and record incorrect results.

### Experiment 3 — Forced Interleaving

Use `taskYIELD()` to make the race condition easier to observe.

### Experiment 4 — Create Mutex

Create a FreeRTOS mutex.

### Experiment 5 — Protect Counter

Use the mutex around `counter++`.

### Experiment 6 — Observe Blocking

Display when tasks request, acquire, and release the mutex.

### Experiment 7 — Protect Serial

Use a mutex for grouped Serial output.

### Experiment 8 — Unprotected Serial

Observe message interleaving.

### Experiment 9 — Protected Serial

Prevent interleaved output groups.

### Experiment 10 — Shared Sensor Data

Protect a sensor-data structure.

### Experiment 11 — Waiting Time

Measure mutex acquisition delay.

### Experiment 12 — Priority Interaction

Use different task priorities and observe mutex blocking.

### Experiment 13 — Compare Designs

Compare no protection, mutex, and queue approaches.

### Experiment 14 — Performance

Measure synchronization overhead.

### Experiment 15 — Application Resource

Protect a shared LED control variable.

---

## 45. Exercises

### Exercise 1 — Race Condition

Create two tasks.

```text
Task A
  ↓
100,000 increments

Task B
  ↓
100,000 increments
```

Expected:

$$
200000.
$$

Run 10 trials and record the actual results.

### Exercise 2 — Race Error

Calculate:

$$
E
=
200000
-
N_{\text{actual}}.
$$

and:

$$
E_{\%}
= \frac{E}{200000}
\times100\%.
$$

### Exercise 3 — Mutex Protection

Protect the shared counter using:

```cpp
xSemaphoreTake()
```

and:

```cpp
xSemaphoreGive()
```

Verify that:

```text
Expected = Actual
```

### Exercise 4 — Serial Resource

Create three tasks that print:

```text
Task Name
Priority
Timestamp
Sensor Value
```

First run without a mutex.

Then protect the full message block with a mutex.

Compare the results.

### Exercise 5 — Sensor Structure

Create:

```cpp
struct SensorData {
    float temperature;
    float humidity;
    unsigned long timestamp;
};
```

Use:

```text
Sensor Task
   ↓
Shared Structure
   ↓
Display Task
```

Protect the structure using a mutex.

### Exercise 6 — Waiting-Time Measurement

Measure mutex waiting time for two tasks.

Record:

| Task | Priority | Average Wait |
|---|---:|---:|
| A | 1 |  |
| B | 2 |  |

Discuss the results.

### Exercise 7 — Complete Shared-Resource System

Construct:

```text
Sensor Task
     ↓
Shared Sensor Data
     ↓
Mutex
     ↓
Processing Task

          +

Logging Task
     ↓
Serial Mutex
     ↓
Serial Monitor
```

Demonstrate correct synchronized operation.

---

## 46. Checkpoint Questions

1. What is a shared resource?
2. What is a race condition?
3. Why can two tasks modifying the same variable produce an incorrect result?
4. Why is `counter++` potentially unsafe when accessed concurrently?
5. What is a lost update?
6. What is a critical section?
7. What does mutual exclusion mean?
8. What is a FreeRTOS mutex?
9. Which function creates a mutex?
10. What does `xSemaphoreTake()` do?
11. What does `xSemaphoreGive()` do?
12. What does `portMAX_DELAY` mean in a mutex wait operation?
13. What happens when a task requests an unavailable mutex?
14. What is mutex ownership?
15. Why should the task release a mutex after the critical section?
16. Why should critical sections be kept short?
17. Can a mutex be used to protect Serial output?
18. Why might a sensor-data structure require protection?
19. What is the difference between a mutex and a binary semaphore?
20. What is the difference between a mutex and a queue?
21. What is priority inversion?
22. What is priority inheritance?
23. Why does priority inheritance help real-time systems?
24. What is deadlock?
25. Why is synchronization important in a concurrent embedded system?

---

## 47. Expected Results

Without a mutex:

```text
Task A
   ↓
Shared Counter
   ↑
Task B
```

may produce:

```text
Expected = 200000
Actual   < 200000
```

because of lost updates.

With mutex protection:

```text
Task A
   ↓
Take Mutex
   ↓
counter++
   ↓
Give Mutex

Task B
   ↓
Take Mutex
   ↓
counter++
   ↓
Give Mutex
```

the expected result becomes:

```text
Expected = 200000
Actual   = 200000
```

Students should also observe that a task waiting for an owned mutex becomes blocked until the resource is released.

---

## 48. Submission

Submit:

- Source code for the unprotected race-condition experiment.
- Source code for the mutex-protected experiment.
- Serial Monitor screenshots.
- Results from at least 10 unprotected trials.
- Race-condition error calculations.
- Results from at least 10 protected trials.
- Serial-output mutex experiment.
- Shared sensor-data experiment.
- Mutex waiting-time measurements.
- Short discussion comparing correctness and synchronization overhead.

Suggested race-condition table:

| Trial | Expected | Without Mutex | Lost Updates | With Mutex |
|---:|---:|---:|---:|---:|
| 1 | 200000 |  |  |  |
| 2 | 200000 |  |  |  |
| 3 | 200000 |  |  |  |
| 4 | 200000 |  |  |  |
| 5 | 200000 |  |  |  |
| 6 | 200000 |  |  |  |
| 7 | 200000 |  |  |  |
| 8 | 200000 |  |  |  |
| 9 | 200000 |  |  |  |
| 10 | 200000 |  |  |  |

Suggested synchronization table:

| Resource | Without Mutex | With Mutex |
|---|---|---|
| Shared counter | Race condition | Correct |
| Serial messages | May interleave | Grouped |
| Sensor structure | Potential inconsistency | Protected |

---

## 49. Discussion

Students should explain why the shared counter may produce an incorrect result even though each task executes the same simple `counter++` statement.

The important concept is that concurrent access can interleave at unsafe points:

```text
Read
 ↓
Modify
 ↓
Write
```

A mutex converts this unsafe sequence into a mutually exclusive critical section:

```text
Lock
 ↓
Read
 ↓
Modify
 ↓
Write
 ↓
Unlock
```

The core relationship can therefore be summarized as:

$$
\boxed{
\text{Correct Shared Access}
= \text{Critical Section}
+
\text{Mutual Exclusion}
}
$$

Students should also discuss the cost of synchronization. Mutex operations introduce some processing and waiting overhead, but they provide deterministic and correct shared-resource access.

---

## 50. Conclusion

In this lab, a race condition is demonstrated by allowing multiple FreeRTOS tasks to modify the same shared variable without synchronization. Because the read-modify-write operations can interleave, updates may be lost and the final result may differ from the expected value.

A FreeRTOS mutex is then introduced to enforce **mutual exclusion**. By requiring each task to acquire the mutex before entering the critical section and release it afterward, only one task can access the shared resource at a time.

The synchronization process can be summarized as:

```text
Multiple Tasks
      ↓
Shared Resource
      ↓
Race Condition
      ↓
Identify Critical Section
      ↓
Create Mutex
      ↓
Take Mutex
      ↓
Access Resource
      ↓
Give Mutex
      ↓
Correct Result
```

This lab advances the RTOS sequence from scheduling to safe task coordination:

```text
LAB 5
FreeRTOS Tasks & Priorities
      ↓
Preemptive Scheduling
      ↓
LAB 6
Shared Resources
      ↓
Race Conditions
      ↓
Mutex Synchronization
      ↓
LAB 7
Inter-Task Communication
      ↓
FreeRTOS Queues
```

The next lab naturally extends synchronization into **safe data transfer between concurrent tasks using FreeRTOS queues**.
