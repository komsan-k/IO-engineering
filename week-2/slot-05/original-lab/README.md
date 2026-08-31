# LAB 5 — Scheduling using pthread and FreeRTOS

## 1. Objective
The objective of this lab is to:
1. Understand task scheduling principles on embedded systems.
2. Implement multitasking using **POSIX threads (pthread)** on platforms that support them.
3. Implement multitasking using **FreeRTOS tasks** on the ESP32.
4. Compare cooperative vs. preemptive scheduling.
5. Demonstrate real-time constraints in task execution.

---

## 2. Background

Modern embedded systems often need to handle multiple tasks concurrently, such as sensor sampling, communication, and user interface updates.  
Two common approaches to multitasking are:

- **pthread (POSIX Threads)**: A portable API for multithreading on systems with an operating system (Linux, embedded Linux boards like Raspberry Pi).
- **FreeRTOS**: A real-time operating system kernel designed for microcontrollers like the ESP32, offering lightweight task management and scheduling.

**Key Concepts**:
- **Task/Thread**: Independent sequence of execution.
- **Scheduler**: Decides which task runs at any given time.
- **Priority**: Determines importance of tasks.
- **Preemption**: Ability of a higher-priority task to interrupt a lower-priority one.
- **Tick Rate**: The scheduler's time quantum, often in milliseconds.

---

## 3. Hardware & Software Requirements

### Hardware
- ESP32 development board
- USB cable
- Serial Monitor (Arduino IDE / PlatformIO)
- Optional: LED and resistor for visual indicators

### Software
- Arduino IDE or PlatformIO (ESP32 core installed)
- For pthread: Linux host or ESP-IDF on ESP32 (pthread API supported)
- FreeRTOS: Included in ESP32 Arduino core or ESP-IDF

---

## 4. Task Description

### 4.1 pthread Scheduling Example on ESP32

This section demonstrates how to use **POSIX threads (pthreads)** on ESP32 for multitasking.  
You will see examples of blinking LEDs, printing messages, and sharing variables between threads.

---

## Example 1 — Blinking LEDs with Threads
```cpp
#include <pthread.h>

#define LED1 2
#define LED2 12

void *ThreadFunc1(void *threadid) {
   while (1) {
      digitalWrite(LED1, HIGH); delay(1000);
      digitalWrite(LED1, LOW);  delay(1000);
   }
   return NULL;
}

void *ThreadFunc2(void *threadid) {
   while (1) {
      digitalWrite(LED2, HIGH); delay(500);
      digitalWrite(LED2, LOW);  delay(500);
   }
   return NULL;
}

void setup() {
   pinMode(LED1, OUTPUT);
   pinMode(LED2, OUTPUT);

   pthread_t thLED1, thLED2;
   pthread_create(&thLED1, NULL, ThreadFunc1, NULL);
   pthread_create(&thLED2, NULL, ThreadFunc2, NULL);
}

void loop() {}
```

This code creates two threads:  
- One toggles LED1 every 1 second.  
- Another toggles LED2 every 0.5 seconds.  

---

## Example 2 — Serial Printing from Threads
```cpp
#include <pthread.h>

void *ThreadFunc1(void *threadid) {
   while (1) {
      Serial.println("Thread 1: Running...");
      delay(1000);
   }
   return NULL;
}

void *ThreadFunc2(void *threadid) {
   while (1) {
      Serial.println("Thread 2: Running...");
      delay(1500);
   }
   return NULL;
}

void setup() {
   Serial.begin(115200);
   delay(1000);

   pthread_t th1, th2;
   pthread_create(&th1, NULL, ThreadFunc1, NULL);
   pthread_create(&th2, NULL, ThreadFunc2, NULL);
}

void loop() {}
```

This demonstrates two threads running concurrently, printing different messages at different intervals.

---

## Example 3 — Sharing Variables Between Threads
```cpp
#include <pthread.h>

pthread_mutex_t lock;
volatile int counter = 0;

void *IncrementThread(void *threadid) {
   while (1) {
      pthread_mutex_lock(&lock);
      counter++;
      Serial.printf("Increment Thread: counter = %d\n", counter);
      pthread_mutex_unlock(&lock);
      delay(1000);
   }
   return NULL;
}

void *DecrementThread(void *threadid) {
   while (1) {
      pthread_mutex_lock(&lock);
      counter--;
      Serial.printf("Decrement Thread: counter = %d\n", counter);
      pthread_mutex_unlock(&lock);
      delay(1500);
   }
   return NULL;
}

void setup() {
   Serial.begin(115200);
   delay(1000);

   pthread_mutex_init(&lock, NULL);

   pthread_t thInc, thDec;
   pthread_create(&thInc, NULL, IncrementThread, NULL);
   pthread_create(&thDec, NULL, DecrementThread, NULL);
}

void loop() {}
```

This example shows how to **share variables safely between threads** using a `mutex`.

---

## Notes
- `pthread_create()` starts a new thread.  
- Use `delay()` or `usleep()` to simulate periodic execution.  
- Use `pthread_mutex_lock()` and `pthread_mutex_unlock()` when **sharing variables** between threads.  
- On ESP32, pthread is available when using **ESP-IDF** or **Arduino** with `CONFIG_PTHREAD_TASK_PRIO_DEFAULT`.  

---

### 4.2 FreeRTOS Scheduling Example (ESP32 Arduino)

#### Code
```cpp
#include <Arduino.h>

TaskHandle_t Task1, Task2;

void Task1code(void* pvParameters) {
  for (;;) {
    Serial.println("Task 1 running...");
    vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms delay
  }
}

void Task2code(void* pvParameters) {
  for (;;) {
    Serial.println("Task 2 running...");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1s delay
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    Task1code, "Task1", 10000, NULL, 1, &Task1, 0);

  xTaskCreatePinnedToCore(
    Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
}

void loop() {
  // Empty - tasks run independently
}
```

#### Notes
- `xTaskCreatePinnedToCore()` creates a task on a specific core (ESP32 has 2 cores).
- `vTaskDelay()` yields control to the scheduler.
- Task priority can be adjusted to influence execution order.

---

## 5. Experiments

1. **Vary Task Priorities**  
   Change priority values in `xTaskCreatePinnedToCore()` and observe execution frequency.
2. **Add LED Blink Tasks**  
   Assign different blink rates to LEDs and verify scheduling consistency.
3. **Simulate Heavy Load**  
   Add computational loops to test preemption in FreeRTOS.
4. **Mix pthread and FreeRTOS (ESP-IDF)**  
   Run both models together for comparison.

---

## 6. Comparison Table

| Feature        | pthread                  | FreeRTOS                      |
|----------------|--------------------------|--------------------------------|
| Portability    | High (POSIX systems)     | Microcontroller-focused        |
| Overhead       | Higher                   | Lower                          |
| Real-time      | No strict real-time      | Designed for real-time tasks   |
| Memory use     | Depends on OS            | Small footprint                |
| API complexity | Moderate                 | Simple for embedded use        |

---

## 7. Exercises
1. Implement three FreeRTOS tasks:  
   - Task 1: Blink LED every 200 ms  
   - Task 2: Read sensor every 1 s  
   - Task 3: Send data via Serial every 3 s
2. Modify priorities to give the sensor task highest priority.
3. Implement mutex locking for shared resource access.
4. Port the FreeRTOS code to ESP-IDF and run without Arduino framework.

---

## 8. Conclusion
In this lab, we implemented scheduling using both **pthread** and **FreeRTOS**. pthread provided a familiar multithreading interface for POSIX systems, while FreeRTOS offered a lightweight, deterministic scheduler tailored for embedded microcontrollers. By experimenting with task priorities, delays, and workloads, we observed how the scheduler manages execution order and timing. FreeRTOS's real-time nature makes it ideal for embedded IoT systems requiring predictable behavior.

