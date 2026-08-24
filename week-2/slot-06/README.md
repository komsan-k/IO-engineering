# 📘 Theory: Race Conditions, Critical Sections, Semaphores, and Mutexes

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Modern ESP32 applications frequently perform multiple activities concurrently. For example, an ESP32 system may contain separate FreeRTOS tasks for:

- sensor acquisition,
- actuator control,
- Wi-Fi communication,
- MQTT publishing,
- display updates,
- data processing,
- user-interface handling.

These tasks may need to access the same variables, memory locations, communication interfaces, or hardware peripherals.

Consider two FreeRTOS tasks:

```text
Task A ───────┐
              ├────► Shared Resource
Task B ───────┘
```

If both tasks access the shared resource at the same time without proper coordination, unexpected behavior can occur.

This leads to four important concepts in concurrent embedded programming:

$$
\boxed{
\text{Race Conditions}
+
\text{Critical Sections}
+
\text{Semaphores}
+
\text{Mutexes}
}
$$

These mechanisms are fundamental for building reliable multitasking systems with **ESP32 and FreeRTOS**.

---

# 🧠 2. Why Synchronization Is Needed

Suppose two tasks share a variable:

```cpp
int counter = 0;
```

Both tasks execute:

```cpp
counter++;
```

At first glance, this appears to be a single operation.

However, the processor may internally perform several steps:

```text
1. Read counter
2. Increment value
3. Write counter
```

Therefore,

$$
\texttt{counter++}
$$

is not necessarily an atomic operation.

Conceptually:

```text
Read
 │
 ▼
Modify
 │
 ▼
Write
```

If multiple tasks execute these steps at overlapping times, the final result may be incorrect.

---

# ⚠️ 3. What Is a Race Condition?

A **race condition** occurs when the result of a program depends on the relative timing or execution order of multiple tasks, threads, interrupts, or processor cores accessing shared data.

A general representation is:

$$
\boxed{
\text{Concurrent Execution}
+
\text{Shared Resource}
+
\text{Unsynchronized Access}
\rightarrow
\text{Race Condition}
}
$$

Race conditions can produce:

- incorrect calculations,
- lost updates,
- corrupted data,
- unpredictable behavior,
- intermittent failures,
- system crashes.

They are particularly difficult to debug because the problem may not occur every time.

---

# 🔢 4. Simple Race-Condition Example

Assume:

```cpp
int counter = 10;
```

Task A and Task B both execute:

```cpp
counter++;
```

The expected result is:

$$
10+1+1=12.
$$

However, consider this sequence:

```text
Task A: Read counter = 10

Task B: Read counter = 10

Task A: Increment → 11

Task B: Increment → 11

Task A: Write 11

Task B: Write 11
```

The final value becomes:

$$
\boxed{counter=11}
$$

instead of

$$
\boxed{counter=12}.
$$

One increment has been lost.

This is called a **lost update**.

---

# 📊 5. Race-Condition Timing Diagram

The problem can be represented as:

```text
Time ───────────────────────────────►

Task A: READ(10) ── ADD(1) ── WRITE(11)

Task B:      READ(10) ── ADD(1) ── WRITE(11)

Final value = 11
```

The correct sequential execution would be:

```text
Task A: READ(10) → ADD → WRITE(11)

Task B:                    READ(11) → ADD → WRITE(12)

Final value = 12
```

---

# 🔬 6. ESP32 Race-Condition Example

Consider two ESP32 FreeRTOS tasks:

```cpp
volatile int counter = 0;

void Task1(void *pvParameters)
{
    while (1)
    {
        counter++;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Task2(void *pvParameters)
{
    while (1)
    {
        counter++;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

Both tasks access:

```cpp
counter
```

without synchronization.

Conceptually:

```text
Task 1 ──────┐
             │
             ▼
          counter
             ▲
             │
Task 2 ──────┘
```

The result may become timing-dependent.

---

# ⚠️ 7. `volatile` Does Not Solve Race Conditions

A common misconception is that declaring a variable as:

```cpp
volatile int counter;
```

makes concurrent access safe.

It does not.

`volatile` primarily tells the compiler that the value may change unexpectedly and should not be optimized under assumptions that it remains unchanged.

It does not automatically provide:

- mutual exclusion,
- atomicity,
- locking,
- synchronization.

Therefore,

$$
\boxed{
\texttt{volatile}
\neq
\text{Synchronization}
}
$$

---

# 🧠 8. Shared Resources

A shared resource is anything accessed by more than one execution context.

Examples on ESP32 include:

### Shared Variables

```cpp
int sensorValue;
float temperature;
bool alarmState;
```

### Shared Peripherals

```text
UART
SPI
I²C
ADC
Display
SD Card
```

### Shared Communication Resources

```text
Wi-Fi connection
MQTT client
Serial port
TCP socket
```

### Shared Data Structures

```text
Arrays
Buffers
Structures
Linked lists
```

The general architecture is:

```text
Task A ──────┐
Task B ──────┼────► Shared Resource
Task C ──────┘
```

---

# 🔐 9. What Is a Critical Section?

A **critical section** is a section of code that accesses shared data or resources and must not be executed concurrently by conflicting execution contexts.

For example:

```cpp
counter++;
```

may need to be protected.

Conceptually:

```text
Enter Critical Section
        │
        ▼
Read Shared Data
        │
        ▼
Modify Shared Data
        │
        ▼
Write Shared Data
        │
        ▼
Exit Critical Section
```

The fundamental requirement is:

$$
\boxed{
N_{\text{executing in critical section}}\leq1
}
$$

for a mutually exclusive resource.

---

# 🚪 10. Critical Section Analogy

A critical section can be compared to a room with only one key.

```text
Task A
   │
   ▼
Take Key
   │
   ▼
┌───────────────┐
│ Shared Room   │
│               │
│ Critical      │
│ Section       │
└───────────────┘
   │
   ▼
Return Key
```

While Task A is inside, Task B must wait.

---

# 🧩 11. ESP32 Critical Sections

ESP32 FreeRTOS provides mechanisms for protecting short critical sections.

A common ESP32 pattern is:

```cpp
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

portENTER_CRITICAL(&mux);

counter++;

portEXIT_CRITICAL(&mux);
```

The protected operation is:

```text
portENTER_CRITICAL()
        │
        ▼
    counter++
        │
        ▼
portEXIT_CRITICAL()
```

---

# 💻 12. Critical-Section Example

```cpp
volatile int counter = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void Task1(void *pvParameters)
{
    while (1)
    {
        portENTER_CRITICAL(&mux);

        counter++;

        portEXIT_CRITICAL(&mux);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

Only the minimum required code should normally be placed inside the critical section.

---

# ⚠️ 13. Keep Critical Sections Short

Critical sections can affect system responsiveness.

Avoid:

```cpp
portENTER_CRITICAL(&mux);

delay(5000);
Serial.println("Long operation");
performLargeCalculation();

portEXIT_CRITICAL(&mux);
```

A better design is:

```cpp
portENTER_CRITICAL(&mux);

localValue = sharedValue;

portEXIT_CRITICAL(&mux);

// Long processing outside critical section
processData(localValue);
```

Therefore,

$$
\boxed{
\text{Critical Section Duration}
\rightarrow
\text{As Short As Practical}
}
$$

---

# ⚡ 14. Atomic Operations

An **atomic operation** is an operation that appears to execute as one indivisible action with respect to competing execution contexts.

Conceptually:

```text
Non-Atomic:

READ → MODIFY → WRITE


Atomic:

┌─────────────────┐
│ READ/MODIFY/WRITE│
└─────────────────┘
```

Atomicity is important because other tasks should not observe a partially completed update.

However, whether a particular operation is atomic depends on:

- processor architecture,
- data width,
- compiler,
- memory model,
- operation type.

Therefore, programmers should not assume that a complex C/C++ expression is atomic.

---

# 🚦 15. What Is a Semaphore?

A **semaphore** is a synchronization mechanism used to coordinate tasks or signal the occurrence of events.

Conceptually:

```text
Task / ISR
    │
    ▼
Give Semaphore
    │
    ▼
 Semaphore
    │
    ▼
Waiting Task
    │
    ▼
Take Semaphore
```

A semaphore can be thought of as a signaling mechanism.

---

# 🔢 16. Semaphore Counter

A semaphore can conceptually contain a count:

$$
S\geq0.
$$

A **give** operation may increase the count:

$$
S\leftarrow S+1
$$

while a successful **take** operation decreases it:

$$
S\leftarrow S-1.
$$

If

$$
S=0,
$$

a task attempting to take the semaphore may block.

---

# 🧠 17. Types of Semaphores

FreeRTOS commonly provides:

### Binary Semaphore

Typically represents two conceptual states:

$$
S\in\{0,1\}.
$$

Often used for:

- event notification,
- ISR-to-task synchronization,
- task-to-task signaling.

### Counting Semaphore

Can represent multiple available events or resources:

$$
0\leq S\leq S_{\max}.
$$

Often used for:

- counting events,
- managing multiple identical resources,
- buffering event occurrences.

---

# 🚦 18. Binary Semaphore

A binary semaphore behaves conceptually like an event flag.

```text
0 → unavailable / no event
1 → available / event occurred
```

Example:

```text
Push Button Interrupt
        │
        ▼
       ISR
        │
        ▼
Give Semaphore
        │
        ▼
Sensor Task Wakes
```

This is useful because the ISR can remain short while a normal task performs the longer processing.

---

# 💻 19. Creating a Binary Semaphore

A FreeRTOS binary semaphore can be created using:

```cpp
SemaphoreHandle_t buttonSemaphore;

buttonSemaphore = xSemaphoreCreateBinary();
```

A task can wait for it:

```cpp
xSemaphoreTake(
    buttonSemaphore,
    portMAX_DELAY
);
```

Another task or ISR can signal it using an appropriate semaphore-give operation.

---

# 🔄 20. Semaphore Task Example

```cpp
SemaphoreHandle_t dataReady;

void TaskProducer(void *pvParameters)
{
    while (1)
    {
        // Produce data

        xSemaphoreGive(dataReady);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void TaskConsumer(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(dataReady, portMAX_DELAY) == pdTRUE)
        {
            // Process new data
        }
    }
}
```

Architecture:

```text
Producer
   │
   ▼
Give
   │
   ▼
Semaphore
   │
   ▼
Take
   │
   ▼
Consumer
```

---

# ⚡ 21. ISR-to-Task Synchronization

One important semaphore application is transferring work from an ISR to a task.

```text
Hardware
   │
   ▼
Interrupt
   │
   ▼
  ISR
   │
   ▼
Give Semaphore
   │
   ▼
Task Becomes Ready
   │
   ▼
Process Event
```

This supports the design principle:

$$
\boxed{
\text{ISR} =
\text{Fast Response}
}
$$

and

$$
\boxed{
\text{Task} =
\text{Longer Processing}
}
$$

---

# 🧮 22. Counting Semaphore

Suppose an application has three identical resources.

Then:

$$
S_{\max}=3.
$$

Initially:

$$
S=3.
$$

If one task obtains a resource:

$$
S=2.
$$

If another obtains one:

$$
S=1.
$$

If all resources are occupied:

$$
S=0.
$$

Conceptually:

```text
Available Resources

[R1] [R2] [R3]

Semaphore Count = 3
```

---

# 📊 23. Binary vs. Counting Semaphore

| Characteristic | Binary Semaphore | Counting Semaphore |
|---|---|---|
| Count range | Typically 0–1 | 0–N |
| Event signaling | Excellent | Yes |
| Multiple resources | No | Yes |
| ISR synchronization | Common | Possible |
| Resource counting | Limited | Excellent |

---

# 🔒 24. What Is a Mutex?

A **mutex** means **mutual exclusion**.

Its primary purpose is to protect a shared resource so that only one task owns and accesses the protected resource at a time.

Conceptually:

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

Meanwhile:

```text
Task B
  │
  ▼
Take Mutex
  │
  ▼
WAIT
```

until the mutex becomes available.

---

# 🔑 25. Mutex Ownership

An important mutex concept is **ownership**.

The task that successfully takes a mutex becomes its owner.

Conceptually:

$$
\boxed{
\text{Take Mutex}
\rightarrow
\text{Ownership}
\rightarrow
\text{Use Resource}
\rightarrow
\text{Release Mutex}
}
$$

This ownership behavior distinguishes mutexes from general event-signaling semaphores.

---

# 💻 26. Creating a Mutex

```cpp
SemaphoreHandle_t serialMutex;

serialMutex = xSemaphoreCreateMutex();
```

A task can acquire it using:

```cpp
xSemaphoreTake(
    serialMutex,
    portMAX_DELAY
);
```

and release it using:

```cpp
xSemaphoreGive(serialMutex);
```

---

# 🖨️ 27. ESP32 Serial Mutex Example

Suppose multiple tasks use the same Serial interface.

Without protection:

```text
Task A ─────┐
            ├────► Serial
Task B ─────┘
```

Outputs may become interleaved.

A mutex can protect access:

```cpp
SemaphoreHandle_t serialMutex;

void Task1(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE)
        {
            Serial.println("Message from Task 1");

            xSemaphoreGive(serialMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

# 🧪 28. Two-Task Mutex Example

```cpp
SemaphoreHandle_t mutex;

int sharedCounter = 0;

void Task1(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
        {
            sharedCounter++;

            Serial.print("Task 1: ");
            Serial.println(sharedCounter);

            xSemaphoreGive(mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

Task 2 can use the same mutex before modifying `sharedCounter`.

```text
Task 1 ──────┐
             │
             ▼
           Mutex
             │
             ▼
      sharedCounter
             ▲
             │
Task 2 ──────┘
```

---

# 🆚 29. Semaphore vs. Mutex

Although their APIs may appear similar, semaphores and mutexes serve different conceptual purposes.

| Feature | Semaphore | Mutex |
|---|---|---|
| Main purpose | Signaling / counting | Resource protection |
| Ownership | Generally no | Yes |
| Mutual exclusion | Possible in some patterns | Primary purpose |
| ISR use | Binary/counting semaphore can be useful | Mutexes are task-level synchronization objects |
| Priority inheritance | Not the defining feature | Commonly associated with mutexes |
| Typical example | ISR wakes task | Protect shared UART |

A useful rule is:

$$
\boxed{
\text{Semaphore}
\rightarrow
\text{Signal an Event}
}
$$

$$
\boxed{
\text{Mutex}
\rightarrow
\text{Protect a Resource}
}
$$

---

# 🚨 30. Priority Inversion

Consider three tasks:

| Task | Priority |
|---|---:|
| High | 3 |
| Medium | 2 |
| Low | 1 |

Suppose the Low-priority task owns a mutex.

```text
Low Task
   │
   ▼
Take Mutex
```

Then the High-priority task becomes Ready and requests the same mutex:

```text
High Task
   │
   ▼
Request Mutex
   │
   ▼
Blocked
```

If the Medium-priority task then preempts the Low-priority task, the High-priority task may remain blocked indirectly by the Low-priority task.

This is called **priority inversion**.

---

# 📊 31. Priority-Inversion Timeline

```text
Time ─────────────────────────────────►

Low:    [Own Mutex]───────┐
                          │
High:       [Wait Mutex]  │
                          │
Medium:         [Running] │
                          │
High remains blocked ◄────┘
```

The effective priority relationship becomes temporarily inverted.

---

# 🔐 32. Priority Inheritance

Mutex implementations can use **priority inheritance** to mitigate priority inversion.

If a high-priority task waits for a mutex owned by a low-priority task, the mutex owner may temporarily inherit the higher priority.

```text
Low Task owns Mutex
        │
        ▼
High Task requests Mutex
        │
        ▼
High Task blocks
        │
        ▼
Low Task temporarily receives
higher effective priority
        │
        ▼
Low Task finishes critical work
        │
        ▼
Mutex released
        │
        ▼
High Task runs
```

Therefore,

$$
\boxed{
\text{Priority Inheritance}
\rightarrow
\text{Reduce Priority-Inversion Duration}
}
$$

---

# 🔒 33. Critical Section vs. Mutex

Both can protect shared data, but they are suited to different situations.

| Characteristic | Critical Section | Mutex |
|---|---|---|
| Intended duration | Very short | Can cover longer task-level operations |
| Task blocking | Generally avoided | Waiting task can block |
| Shared peripheral protection | Usually not ideal for long access | Good |
| Shared variable update | Good for short operations | Good |
| Priority inheritance | No | Typically supported by RTOS mutex |
| Typical example | Update counter | Protect I²C/UART |

---

# ⚡ 34. Example: Protecting a Counter

For a very short update:

```cpp
portENTER_CRITICAL(&mux);

counter++;

portEXIT_CRITICAL(&mux);
```

This can be appropriate when the protected operation is extremely short.

For a larger task-level operation:

```cpp
xSemaphoreTake(dataMutex, portMAX_DELAY);

updateDataStructure();
calculateResults();
saveResults();

xSemaphoreGive(dataMutex);
```

a mutex may be more appropriate.

---

# 📡 35. Example: Shared I²C Bus

Suppose two tasks access devices on the same I²C bus.

```text
Sensor Task ──────┐
                  │
                  ▼
               I²C Bus
                  ▲
                  │
Display Task ─────┘
```

Uncoordinated access may cause communication problems.

A mutex can serialize access:

```text
Sensor Task
    │
Take I²C Mutex
    │
Read Sensor
    │
Give Mutex
```

then:

```text
Display Task
    │
Take I²C Mutex
    │
Update Display
    │
Give Mutex
```

---

# 🌐 36. Example: Shared MQTT Client

Consider:

```text
Sensor Task ─────┐
                 │
                 ▼
              MQTT Client
                 ▲
                 │
Alarm Task ──────┘
```

If the MQTT library or shared object is not designed for simultaneous access, the application may need task-level synchronization.

Conceptually:

```text
Take Mutex
    │
    ▼
Publish MQTT Message
    │
    ▼
Give Mutex
```

---

# 🧠 37. Blocking on a Mutex

A task can wait for a mutex using:

```cpp
xSemaphoreTake(mutex, portMAX_DELAY);
```

This means the task can remain blocked until the mutex becomes available.

Alternatively, a timeout can be specified:

```cpp
xSemaphoreTake(
    mutex,
    pdMS_TO_TICKS(100)
);
```

Conceptually:

```text
Request Mutex
     │
     ▼
Available?
 ┌───┴────┐
Yes       No
 │         │
 ▼         ▼
Run      Block
           │
        Timeout /
        Available
```

---

# ⏱️ 38. Timeout Handling

Using a timeout prevents a task from waiting indefinitely.

Example:

```cpp
if (xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) == pdTRUE)
{
    // Access resource

    xSemaphoreGive(mutex);
}
else
{
    // Timeout handling
}
```

This can improve system robustness when resource acquisition is not guaranteed.

---

# 🔒 39. Deadlock

A **deadlock** occurs when tasks wait indefinitely for resources held by one another.

Example:

```text
Task A:
Owns Mutex 1
Waits for Mutex 2

Task B:
Owns Mutex 2
Waits for Mutex 1
```

Graphically:

```text
Task A
  │
  │ waits for
  ▼
Mutex 2
  ▲
  │ owned by
Task B
  │
  │ waits for
  ▼
Mutex 1
  ▲
  │ owned by
Task A
```

Neither task can proceed.

---

# 🛡️ 40. Avoiding Deadlocks

Useful design practices include:

- acquire locks in a consistent order,
- minimize nested locks,
- use timeouts where appropriate,
- keep protected sections short,
- release resources as soon as possible.

For example, define:

$$
Mutex_A < Mutex_B.
$$

All tasks should acquire them in this order:

```text
Mutex A
   │
   ▼
Mutex B
```

rather than allowing arbitrary ordering.

---

# ⚠️ 41. Forgetting to Release a Mutex

A common programming error is:

```cpp
xSemaphoreTake(mutex, portMAX_DELAY);

if (error)
{
    return;
}

xSemaphoreGive(mutex);
```

If `error` occurs, the function returns without releasing the mutex.

Other tasks may then remain blocked.

A safer structure ensures that every successful take has a corresponding release.

Conceptually:

$$
\boxed{
\text{Successful Take}
\rightarrow
\text{Use Resource}
\rightarrow
\text{Give}
}
$$

---

# 🧠 42. Critical Section Design Principle

Only shared-resource operations should normally be protected.

Avoid:

```text
Enter Lock
   │
   ├── Read data
   ├── Large calculation
   ├── Delay
   ├── Network communication
   ├── Serial printing
   └── Exit Lock
```

Prefer:

```text
Enter Lock
   │
   ├── Copy shared data
   │
Exit Lock
   │
   ▼
Perform long calculation
```

Thus:

$$
\boxed{
T_{\text{protected}}
\rightarrow
\text{Minimize}
}
$$

---

# 🔄 43. Producer-Consumer Synchronization

Consider a sensor task and processing task:

```text
Sensor Task
    │
    ▼
Acquire Data
    │
    ▼
Signal
    │
    ▼
Processing Task
```

A binary semaphore can signal that new data is available.

```text
Producer
   │
   ▼
Give Semaphore
   │
   ▼
Consumer Wakes
```

This is different from a mutex because the primary objective is **event synchronization**, not ownership of a resource.

---

# 📦 44. Queue vs. Semaphore

FreeRTOS queues can transfer data, while semaphores primarily provide synchronization or resource counting.

```text
Semaphore:

Event
  │
  ▼
Signal
  │
  ▼
Task


Queue:

Producer
  │
  ▼
Data
  │
  ▼
Queue
  │
  ▼
Consumer
```

A useful distinction is:

$$
\boxed{
\text{Semaphore}
\rightarrow
\text{Synchronization}
}
$$

$$
\boxed{
\text{Queue}
\rightarrow
\text{Data Transfer + Synchronization}
}
$$

---

# 🧪 45. ESP32 Example Architecture

Consider an ESP32 environmental monitoring system:

```text
                     ESP32
                       │
                   FreeRTOS
                       │
        ┌──────────────┼───────────────┐
        ▼              ▼               ▼
   Sensor Task     Display Task     MQTT Task
        │              │               │
        └──────────────┼───────────────┘
                       ▼
                Shared Resources
```

Possible synchronization mechanisms:

| Resource/Event | Mechanism |
|---|---|
| Shared sensor structure | Mutex |
| I²C bus | Mutex |
| UART output | Mutex |
| Button interrupt | Binary semaphore |
| Event count | Counting semaphore |
| Very short shared counter | Critical section |
| Sensor data transfer | Queue |

---

# 🚦 46. Smart Traffic ESP32 Example

Consider:

```text
Vehicle Sensor
      │
      ▼
 Sensor Task
      │
      ▼
Traffic Controller
      │
      ▼
Traffic Light FSM
```

The system may contain:

```text
Traffic Task
Sensor Task
Communication Task
Emergency Task
```

Possible synchronization design:

```text
Vehicle ISR
    │
    ▼
Binary Semaphore
    │
    ▼
Sensor Task
    │
    ▼
Shared Traffic Data
    │
   Mutex
    │
    ▼
Traffic FSM
```

---

# 📊 47. Choosing the Correct Mechanism

| Requirement | Recommended Mechanism |
|---|---|
| Protect very short shared update | Critical section / suitable atomic mechanism |
| Protect shared peripheral | Mutex |
| Protect shared data structure | Mutex |
| Signal task from an event | Binary semaphore or task notification |
| Count repeated events/resources | Counting semaphore |
| Transfer data between tasks | Queue |
| Protect long task-level resource operation | Mutex |

The exact mechanism should depend on the application's timing and concurrency requirements.

---

# 🧠 48. Synchronization Decision Flow

```text
Need Synchronization?
        │
        ▼
Is Data Being Transferred?
    ┌───┴────┐
   Yes       No
    │         │
    ▼         ▼
  Queue    Is It Event Signaling?
             │
        ┌────┴────┐
       Yes        No
        │          │
        ▼          ▼
 Semaphore     Shared Resource?
                   │
              ┌────┴────┐
             Yes        No
              │
              ▼
      Very Short Access?
          ┌───┴────┐
         Yes       No
          │         │
          ▼         ▼
 Critical Section  Mutex
```

---

# ⚠️ 49. Common Synchronization Errors

Common problems include:

1. Accessing shared variables without protection.
2. Assuming `volatile` solves concurrency problems.
3. Holding mutexes for too long.
4. Performing delays while holding a mutex.
5. Forgetting to release a mutex.
6. Using a mutex as an event signal.
7. Using semaphores where a queue would better transfer data.
8. Creating inconsistent lock ordering.
9. Creating nested mutex dependencies.
10. Performing long operations in critical sections.
11. Ignoring timeout conditions.
12. Sharing non-thread-safe peripherals without protection.

---

# 🔬 50. Race Conditions on Multicore ESP32

On a multicore ESP32, concurrency becomes particularly important because tasks may execute simultaneously on different cores.

```text
Core 0                      Core 1

Task A                      Task B
  │                           │
  └──────────┐     ┌──────────┘
             ▼     ▼
          Shared Data
```

This can create true parallel access rather than only time-interleaved execution.

Therefore:

$$
\boxed{
\text{Multicore}
+
\text{Shared Data}
\rightarrow
\text{Greater Need for Correct Synchronization}
}
$$

---

# 📊 51. Concurrency Model

For a shared resource $R$, suppose there are $N$ tasks:

$$
T_1,T_2,\ldots,T_N.
$$

Without mutual exclusion:

$$
N_R\geq1
$$

tasks may attempt access.

For exclusive access, synchronization should enforce:

$$
\boxed{
N_R\leq1
}
$$

during the protected operation.

---

# ⏱️ 52. Synchronization Overhead

Synchronization improves correctness but introduces overhead.

A simplified execution time can be represented as:

$$
T_{\text{total}} =
T_{\text{work}}
+
T_{\text{sync}}
+
T_{\text{wait}}.
$$

where:

- $T_{\text{work}}$ = useful processing,
- $T_{\text{sync}}$ = synchronization overhead,
- $T_{\text{wait}}$ = time waiting for resources.

Therefore, excessive locking can reduce performance.

The goal is not to eliminate synchronization but to use it efficiently.

---

# ⚖️ 53. Correctness vs. Performance

Without synchronization:

```text
High concurrency
      │
      ▼
Possible corruption
```

With excessive synchronization:

```text
Heavy locking
     │
     ▼
Reduced concurrency
     │
     ▼
Lower performance
```

A good embedded design balances:

$$
\boxed{
\text{Correctness}
+
\text{Responsiveness}
+
\text{Efficiency}
}
$$

---

# 🛠️ 54. Recommended ESP32 Design Practices

For reliable ESP32 FreeRTOS applications:

- minimize global shared variables,
- prefer message passing where practical,
- use queues for data transfer,
- use mutexes for shared resources,
- use semaphores for synchronization,
- keep critical sections short,
- avoid blocking while holding locks,
- establish consistent lock ordering,
- assign task priorities carefully,
- consider multicore execution,
- test under high task activity,
- monitor stack and timing behavior.

---

# 🧪 55. Example Integrated System

Consider:

```text
                   ESP32
                     │
                 FreeRTOS
                     │
       ┌─────────────┼─────────────┐
       ▼             ▼             ▼
 Sensor Task    Display Task    MQTT Task
       │             │             │
       ▼             ▼             ▼
      Queue       I²C Mutex     Wi-Fi/MQTT
       │
       ▼
 Processing Task
```

An external button may use:

```text
Button
   │
   ▼
 GPIO ISR
   │
   ▼
Binary Semaphore
   │
   ▼
Control Task
```

This combines several synchronization concepts in one embedded architecture.

---

# 💬 56. Discussion Questions

1. What is a race condition?
2. Why can `counter++` produce a race condition?
3. Why does `volatile` not guarantee thread safety?
4. What is a shared resource?
5. What is a critical section?
6. Why should critical sections remain short?
7. What is an atomic operation?
8. What is a binary semaphore?
9. What is a counting semaphore?
10. What is a mutex?
11. What is mutex ownership?
12. What is the difference between a mutex and a semaphore?
13. When should a mutex be used?
14. When should a binary semaphore be used?
15. What is priority inversion?
16. How does priority inheritance help?
17. What is a deadlock?
18. How can deadlocks be reduced?
19. Why should long delays not normally occur while holding a mutex?
20. How does multicore execution affect race-condition risks?

---

# 🧠 57. Exercise 1 — Identify the Race Condition

Consider:

```cpp
int total = 0;

void TaskA(void *pvParameters)
{
    while (1)
    {
        total++;
    }
}

void TaskB(void *pvParameters)
{
    while (1)
    {
        total++;
    }
}
```

Answer:

1. Which resource is shared?
2. Where can the race condition occur?
3. Why can the result be incorrect?
4. Which synchronization mechanism could protect the update?

---

# 🔐 58. Exercise 2 — Critical Section

Modify the following operation so that it is protected by an ESP32 critical section:

```cpp
sharedCounter++;
```

The intended structure is:

```cpp
portENTER_CRITICAL(&mux);

// Protected operation

portEXIT_CRITICAL(&mux);
```

Explain why only the minimum code should be protected.

---

# 🚦 59. Exercise 3 — Binary Semaphore

Design a system where:

```text
Push Button
    │
    ▼
GPIO Interrupt
    │
    ▼
Binary Semaphore
    │
    ▼
LED Control Task
```

The ISR should signal the semaphore, and the LED task should perform the longer processing.

---

# 🔒 60. Exercise 4 — Mutex

Two tasks need to print messages using `Serial`.

Design:

```text
Task A ───┐
          ▼
        Mutex
          │
          ▼
        Serial
          ▲
          │
Task B ───┘
```

Write pseudocode showing:

1. mutex creation,
2. mutex acquisition,
3. serial access,
4. mutex release.

---

# 🧮 61. Exercise 5 — Counting Semaphore

An ESP32 application has four identical data buffers.

Initially:

$$
S=4.
$$

Answer:

1. What is the semaphore count after one buffer is acquired?
2. What is the count after all four buffers are acquired?
3. What happens when another task requests a buffer when $S=0$?
4. What happens to the count when a buffer is released?

---

# 🚨 62. Exercise 6 — Priority Inversion

Consider:

| Task | Priority |
|---|---:|
| H | 3 |
| M | 2 |
| L | 1 |

Task L owns a mutex needed by Task H.

Explain:

1. how priority inversion can occur,
2. how Task M can make the problem worse,
3. how priority inheritance can help.

---

# 🔒 63. Exercise 7 — Deadlock

Consider:

```text
Task A:
Take Mutex 1
Take Mutex 2

Task B:
Take Mutex 2
Take Mutex 1
```

Explain how deadlock can occur.

Then redesign both tasks to acquire resources in the same order:

```text
Mutex 1
   │
   ▼
Mutex 2
```

---

# 🔬 64. Practical ESP32 Exercise

Design a FreeRTOS application containing:

```text
TaskSensor
TaskDisplay
TaskMQTT
TaskAlarm
```

Resources include:

```text
I²C Bus
Serial Port
Shared Sensor Data
Alarm Event
```

Complete the table:

| Resource/Event | Shared By | Synchronization Mechanism | Reason |
|---|---|---|---|
| I²C Bus | | | |
| Serial Port | | | |
| Sensor Data | | | |
| Alarm Event | | | |

---

# 🚀 65. Advanced Design Exercise

Design an ESP32 smart monitoring system with:

```text
                  ESP32
                    │
                FreeRTOS
                    │
       ┌────────────┼────────────┐
       ▼            ▼            ▼
 Sensor Task   Processing    MQTT Task
       │          Task
       ▼            │
     Queue          ▼
                Shared State
                    │
                   Mutex
                    │
                    ▼
                Display Task
```

Additionally:

```text
Emergency Button
       │
       ▼
      ISR
       │
       ▼
Binary Semaphore
       │
       ▼
Emergency Task
```

Students should identify:

- shared resources,
- critical sections,
- mutex-protected resources,
- semaphore events,
- possible race conditions,
- possible deadlocks,
- appropriate task priorities.

---

# 🧾 66. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain concurrency in ESP32 applications,
- define race conditions,
- identify shared-resource problems,
- explain why `volatile` does not provide synchronization,
- identify critical sections,
- protect short shared operations,
- explain atomicity,
- describe binary and counting semaphores,
- use semaphores for task synchronization,
- explain ISR-to-task signaling,
- define mutexes and mutex ownership,
- protect shared peripherals using mutexes,
- distinguish semaphores from mutexes,
- explain priority inversion,
- explain priority inheritance,
- identify and avoid deadlocks,
- use synchronization timeouts,
- select appropriate FreeRTOS synchronization mechanisms,
- design reliable multicore ESP32 applications.

---

# 📘 67. References

1. FreeRTOS, *FreeRTOS Kernel Documentation*.
2. Richard Barry, *Mastering the FreeRTOS Real Time Kernel*.
3. Espressif Systems, *ESP-IDF Programming Guide — FreeRTOS*.
4. Espressif Systems, *ESP32 Series Datasheet*.
5. Espressif Systems, *ESP32 Technical Reference Manual*.
6. Espressif Systems, *Arduino Core for ESP32 Documentation*.
7. Qing Li and Caroline Yao, *Real-Time Concepts for Embedded Systems*.
8. Jane W. S. Liu, *Real-Time Systems*.
9. J. W. Valvano, *Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers*.

---

# 🔑 68. Key Concepts

### Race Condition

$$
\boxed{
\text{Concurrent Access}
+
\text{Shared Resource}
+
\text{No Synchronization} =
\text{Race Condition}
}
$$

### Critical Section

$$
\boxed{
\text{Enter}
\rightarrow
\text{Protected Shared Access}
\rightarrow
\text{Exit}
}
$$

### Binary Semaphore

$$
\boxed{
\text{Event}
\rightarrow
\text{Give Semaphore}
\rightarrow
\text{Wake Task}
}
$$

### Counting Semaphore

$$
\boxed{
0\leq S\leq S_{\max}
}
$$

### Mutex

$$
\boxed{
\text{Take}
\rightarrow
\text{Own}
\rightarrow
\text{Access Resource}
\rightarrow
\text{Give}
}
$$

### Priority Inheritance

$$
\boxed{
\text{High-Priority Task Waits}
\rightarrow
\text{Mutex Owner Temporarily Inherits Priority}
}
$$

### Deadlock

$$
\boxed{
\text{Task A Waits for B}
+
\text{Task B Waits for A}
\rightarrow
\text{Deadlock}
}
$$

### ESP32 Synchronization

$$
\boxed{
\text{Tasks}
+
\text{Shared Resources}
+
\text{Synchronization} =
\text{Reliable Concurrent System}
}
$$

Race conditions, critical sections, semaphores, and mutexes are fundamental to **ESP32 FreeRTOS programming** because multitasking introduces concurrent access to shared data and peripherals. Critical sections protect very short shared operations, semaphores provide event synchronization and resource counting, while mutexes provide ownership-based mutual exclusion for shared resources. Correct use of these mechanisms prevents data corruption, reduces race conditions, and enables reliable real-time embedded applications.
