# LAB 6 — Task Synchronization: Semaphores & Mutexes

## 1. Objective

The objective of this lab is to:

1. Understand **race conditions** and shared-resource problems in multitasking systems.
2. Understand the concepts of **critical sections**, **semaphores**, and **mutexes**.
3. Implement mutex-based synchronization using **pthread**.
4. Implement **FreeRTOS mutexes and semaphores** on the ESP32.
5. Compare mutexes, binary semaphores, and counting semaphores.
6. Demonstrate safe access to shared resources in embedded applications.

---

## 2. Background

In a multitasking embedded system, multiple tasks may attempt to access the same resource simultaneously. Examples include:

- Global variables
- Sensors
- Serial communication
- I²C/SPI buses
- Files
- Shared memory
- Actuators

If two tasks modify the same resource without synchronization, a **race condition** may occur.

For example:

```cpp
counter++;
```

Although this appears to be a single operation, conceptually it involves:

```text
Read counter
→ Modify counter
→ Write counter
```

Another task may access `counter` before this sequence is complete, potentially producing an incorrect result.

### Key Concepts

- **Shared Resource:** Data or hardware accessed by multiple tasks.
- **Race Condition:** A situation where program behavior depends on the timing of concurrent operations.
- **Critical Section:** A section of code that accesses a shared resource and should not be executed simultaneously by multiple tasks.
- **Mutex:** A mutual-exclusion mechanism that allows one task at a time to access a protected resource.
- **Semaphore:** A synchronization mechanism used to coordinate tasks or signal events.
- **Binary Semaphore:** Has two logical states: available and unavailable.
- **Counting Semaphore:** Maintains a count representing multiple available resources.

A basic synchronization model is:

**Task → Acquire Lock → Critical Section → Release Lock**

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Serial Monitor
- Optional LEDs and resistors
- Optional sensor for shared-resource experiments

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS included with ESP32
- For pthread experiments: ESP32 pthread support or a Linux/POSIX environment

---

## 4. Task Description

### 4.1 Experiment 1 — Race Condition Without Protection

The first experiment demonstrates a shared variable accessed by two FreeRTOS tasks without synchronization.

```cpp
#include <Arduino.h>

volatile int counter = 0;

void IncrementTask(void *pvParameters) {
  for (;;) {

    int temp = counter;
    temp++;

    vTaskDelay(1);

    counter = temp;

    Serial.printf("Increment: counter = %d\n", counter);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void DecrementTask(void *pvParameters) {
  for (;;) {

    int temp = counter;
    temp--;

    vTaskDelay(1);

    counter = temp;

    Serial.printf("Decrement: counter = %d\n", counter);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {

  Serial.begin(115200);

  xTaskCreate(
    IncrementTask,
    "Increment",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    DecrementTask,
    "Decrement",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

### Observation

Both tasks access the same variable:

```cpp
counter
```

Because access is not protected, task switching can occur during the read-modify-write operation.

This can produce a **race condition**.

---

### 4.2 Experiment 2 — Protecting a Shared Resource with a FreeRTOS Mutex

A mutex can be introduced to ensure that only one task modifies `counter` at a time.

```cpp
#include <Arduino.h>

SemaphoreHandle_t mutex;

volatile int counter = 0;

void IncrementTask(void *pvParameters) {

  for (;;) {

    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {

      counter++;

      Serial.printf(
        "Increment Task: counter = %d\n",
        counter
      );

      xSemaphoreGive(mutex);
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void DecrementTask(void *pvParameters) {

  for (;;) {

    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {

      counter--;

      Serial.printf(
        "Decrement Task: counter = %d\n",
        counter
      );

      xSemaphoreGive(mutex);
    }

    vTaskDelay(700 / portTICK_PERIOD_MS);
  }
}

void setup() {

  Serial.begin(115200);

  mutex = xSemaphoreCreateMutex();

  if (mutex == NULL) {
    Serial.println("Mutex creation failed");
    while (1);
  }

  xTaskCreate(
    IncrementTask,
    "Increment",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    DecrementTask,
    "Decrement",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

The protected operation follows:

```text
xSemaphoreTake()
       ↓
 Access Shared Resource
       ↓
xSemaphoreGive()
```

The mutex prevents the two tasks from modifying the shared variable simultaneously.

---

### 4.3 Experiment 3 — pthread Mutex

The same concept can be demonstrated using a POSIX mutex.

```cpp
#include <pthread.h>

pthread_mutex_t lock;

volatile int counter = 0;

void *IncrementThread(void *arg) {

  while (1) {

    pthread_mutex_lock(&lock);

    counter++;

    Serial.printf(
      "Increment Thread: %d\n",
      counter
    );

    pthread_mutex_unlock(&lock);

    delay(500);
  }

  return NULL;
}

void *DecrementThread(void *arg) {

  while (1) {

    pthread_mutex_lock(&lock);

    counter--;

    Serial.printf(
      "Decrement Thread: %d\n",
      counter
    );

    pthread_mutex_unlock(&lock);

    delay(700);
  }

  return NULL;
}

void setup() {

  Serial.begin(115200);

  pthread_mutex_init(&lock, NULL);

  pthread_t th1;
  pthread_t th2;

  pthread_create(
    &th1,
    NULL,
    IncrementThread,
    NULL
  );

  pthread_create(
    &th2,
    NULL,
    DecrementThread,
    NULL
  );
}

void loop() {}
```

The pthread synchronization sequence is:

```text
pthread_mutex_lock()
        ↓
   Critical Section
        ↓
pthread_mutex_unlock()
```

---

### 4.4 Experiment 4 — Binary Semaphore for Task Signaling

A **binary semaphore** can be used to signal an event from one task to another.

In this example, one task produces an event and another waits for it.

```cpp
#include <Arduino.h>

SemaphoreHandle_t binarySemaphore;

void ProducerTask(void *pvParameters) {

  for (;;) {

    Serial.println("Producer: Event generated");

    xSemaphoreGive(binarySemaphore);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void ConsumerTask(void *pvParameters) {

  for (;;) {

    if (xSemaphoreTake(
          binarySemaphore,
          portMAX_DELAY) == pdTRUE) {

      Serial.println(
        "Consumer: Event received"
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  binarySemaphore =
    xSemaphoreCreateBinary();

  xTaskCreate(
    ProducerTask,
    "Producer",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    ConsumerTask,
    "Consumer",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

The synchronization process is:

**Producer → Give Semaphore → Consumer Unblocked**

Unlike the mutex example, the primary purpose here is **task synchronization/event signaling**, rather than ownership-based protection of a shared resource.

---

### 4.5 Experiment 5 — Counting Semaphore

A counting semaphore can represent multiple available resources or multiple pending events.

```cpp
#include <Arduino.h>

SemaphoreHandle_t countingSemaphore;

void ProducerTask(void *pvParameters) {

  for (;;) {

    xSemaphoreGive(countingSemaphore);

    Serial.println("Event added");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void ConsumerTask(void *pvParameters) {

  for (;;) {

    if (xSemaphoreTake(
          countingSemaphore,
          portMAX_DELAY) == pdTRUE) {

      Serial.println("Event processed");

      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}

void setup() {

  Serial.begin(115200);

  countingSemaphore =
    xSemaphoreCreateCounting(
      5,    // maximum count
      0     // initial count
    );

  xTaskCreate(
    ProducerTask,
    "Producer",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    ConsumerTask,
    "Consumer",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

Here, the semaphore count can be interpreted as the number of **pending events**, up to the configured maximum.

---

## 5. Mutex vs. Semaphore

| Feature | Mutex | Binary Semaphore | Counting Semaphore |
|---|---|---|---|
| Main purpose | Resource protection | Event signaling | Resource/event counting |
| Values | Locked/unlocked | 0 or 1 | 0 to N |
| Ownership | Yes | No | No |
| Shared-resource protection | **Recommended** | Possible, but not preferred | Not typical |
| Task synchronization | Limited | **Yes** | **Yes** |
| Multiple resources/events | No | No | **Yes** |
| Typical example | Protect I²C bus | Signal sensor-ready event | Manage buffer/resource pool |

A useful rule is:

```text
Mutex      → Protect a shared resource
Semaphore  → Synchronize tasks/events
```

---

## 6. Experiments

### Experiment 1 — Observe a Race Condition

1. Run the shared-counter program without a mutex.
2. Record the output.
3. Increase the task execution frequency.
4. Observe whether unexpected counter values occur.

### Experiment 2 — Add Mutex Protection

1. Create a mutex using:

```cpp
xSemaphoreCreateMutex();
```

2. Protect the shared variable with:

```cpp
xSemaphoreTake();
xSemaphoreGive();
```

3. Compare the results with Experiment 1.

### Experiment 3 — Change Task Priorities

Assign different priorities to the two tasks.

For example:

```text
Priority 1 = 1
Priority 2 = 3
```

Observe how priority affects access to the shared resource.

### Experiment 4 — Extend the Critical Section

Add a computational workload inside the protected region.

Observe what happens when one task holds the mutex for a longer period.

### Experiment 5 — Semaphore Signaling

Use a binary semaphore to implement:

```text
Sensor Task
     ↓
Data Ready
     ↓
Semaphore
     ↓
Processing Task
```

Compare this approach with periodic polling.

---

## 7. Exercises

### Exercise 1 — Shared Serial Resource

Create three FreeRTOS tasks:

- **Task 1:** Print sensor information every 500 ms.
- **Task 2:** Print system information every 1 s.
- **Task 3:** Print communication status every 2 s.

Protect the Serial output using a mutex.

### Exercise 2 — Shared Sensor

Create two tasks that access the same sensor:

```text
Task 1 → Read Sensor
Task 2 → Display Sensor Data
```

Use an appropriate synchronization mechanism to prevent unsafe shared-resource access.

### Exercise 3 — Producer and Consumer

Implement:

```text
Producer Task
     ↓
Generate Event
     ↓
Binary Semaphore
     ↓
Consumer Task
     ↓
Process Event
```

Measure the time between the event generation and processing.

### Exercise 4 — Counting Semaphore

Modify the producer-consumer example so that the producer generates events faster than the consumer processes them.

Observe the semaphore count behavior.

### Exercise 5 — Priority Experiment

Create:

- High-priority task
- Medium-priority task
- Low-priority task

Allow the high- and low-priority tasks to use the same mutex-protected resource.

Observe how task priorities influence execution and resource access.

---

## 8. Checkpoint Questions

1. What is a race condition?
2. Why does a shared variable require synchronization?
3. What is a critical section?
4. What is the difference between a mutex and a binary semaphore?
5. Why should a mutex be released as soon as possible?
6. What happens when a task calls `xSemaphoreTake()` but the mutex is unavailable?
7. What does `portMAX_DELAY` mean?
8. When should a counting semaphore be used instead of a binary semaphore?
9. Why is a mutex generally preferred for protecting a shared hardware resource?
10. How can task priority affect synchronization behavior?

---

## 9. Expected Results

Students should observe that an unprotected shared resource may produce timing-dependent behavior. After applying a mutex, access becomes mutually exclusive:

```text
Task A
  ↓
Lock
  ↓
Shared Resource
  ↓
Unlock
  ↓
Task B
```

Binary and counting semaphores demonstrate a different synchronization pattern in which tasks can wait efficiently for events instead of repeatedly polling for changes.

---

## 10. Conclusion

In this lab, **mutexes and semaphores** were applied to synchronize concurrent tasks in pthread and FreeRTOS environments. A mutex provides mutual exclusion for shared resources and prevents race conditions, while semaphores provide mechanisms for coordinating tasks and signaling events.

The experiments demonstrate that correct synchronization is essential for developing reliable multitasking embedded systems.

This lab provides the foundation for the **next topic: inter-task communication using FreeRTOS queues and the producer-consumer model**.
