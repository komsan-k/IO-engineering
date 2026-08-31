# Original LAB 7 — Inter-Task Communication via FreeRTOS Queues

## 1. Objective

The objective of this lab is to:

1. Understand **inter-task communication** in an RTOS environment.
2. Understand the **producer-consumer model**.
3. Create and manage **FreeRTOS queues** on the ESP32.
4. Send and receive data safely between concurrent tasks.
5. Transfer sensor data between acquisition, processing, and output tasks.
6. Compare **queues, shared variables, semaphores, and mutexes**.
7. Investigate queue capacity, blocking, and task timing.

---

## 2. Background

In the previous lab, **mutexes and semaphores** were used to protect shared resources and synchronize tasks. However, embedded applications also require tasks to **exchange data**.

For example, an IoT system may contain:

```text
Sensor Task
     ↓
Sensor Data
     ↓
Processing Task
     ↓
Processed Data
     ↓
Communication Task
```

Using a global variable for this purpose can create synchronization problems when multiple tasks access the data concurrently.

**FreeRTOS queues** provide a thread-safe mechanism for transferring data between tasks.

### Key Concepts

- **Queue:** A data structure used to transfer data safely between tasks.
- **Producer:** A task that generates and sends data.
- **Consumer:** A task that receives and processes data.
- **FIFO:** First-In, First-Out data ordering.
- **Blocking:** A task can wait efficiently until data or queue space becomes available.
- **Queue Length:** Maximum number of items that can be stored.
- **Item Size:** Size of each element stored in the queue.

The basic communication model is:

```text
Producer Task
     ↓
 xQueueSend()
     ↓
┌─────────────┐
│    Queue    │
│   FIFO Data │
└─────────────┘
     ↓
xQueueReceive()
     ↓
Consumer Task
```

---

## 3. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Serial Monitor
- Optional LED and resistor
- Optional analog sensor or potentiometer
- Optional temperature/environmental sensor

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS included with ESP32 Arduino

---

## 4. FreeRTOS Queue Fundamentals

A queue is represented using:

```cpp
QueueHandle_t
```

For example:

```cpp
QueueHandle_t dataQueue;
```

A queue can be created using:

```cpp
xQueueCreate()
```

The general syntax is:

```cpp
QueueHandle_t queue;

queue = xQueueCreate(
    queueLength,
    itemSize
);
```

For example:

```cpp
dataQueue = xQueueCreate(
    5,
    sizeof(int)
);
```

This creates a queue capable of storing **5 integer values**.

### 4.1 Sending Data

A task can send data using:

```cpp
xQueueSend()
```

Example:

```cpp
int value = 25;

xQueueSend(
    dataQueue,
    &value,
    portMAX_DELAY
);
```

### 4.2 Receiving Data

Another task receives data using:

```cpp
xQueueReceive()
```

Example:

```cpp
int receivedValue;

xQueueReceive(
    dataQueue,
    &receivedValue,
    portMAX_DELAY
);
```

The communication flow is:

```text
value
  ↓
xQueueSend()
  ↓
Queue
  ↓
xQueueReceive()
  ↓
receivedValue
```

---

## 5. Experiment 1 — Basic Producer-Consumer Queue

This experiment creates two tasks:

- **Producer Task:** Generates integer values.
- **Consumer Task:** Receives and displays the values.

```cpp
#include <Arduino.h>

QueueHandle_t dataQueue;

void ProducerTask(void *pvParameters) {

  int value = 0;

  for (;;) {

    value++;

    if (xQueueSend(
          dataQueue,
          &value,
          portMAX_DELAY) == pdPASS) {

      Serial.printf(
        "Producer: Sent %d\n",
        value
      );
    }

    vTaskDelay(
      1000 / portTICK_PERIOD_MS
    );
  }
}

void ConsumerTask(void *pvParameters) {

  int receivedValue;

  for (;;) {

    if (xQueueReceive(
          dataQueue,
          &receivedValue,
          portMAX_DELAY) == pdTRUE) {

      Serial.printf(
        "Consumer: Received %d\n",
        receivedValue
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  dataQueue = xQueueCreate(
    5,
    sizeof(int)
  );

  if (dataQueue == NULL) {
    Serial.println("Queue creation failed!");
    while (1);
  }

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

### Expected Output

```text
Producer: Sent 1
Consumer: Received 1

Producer: Sent 2
Consumer: Received 2

Producer: Sent 3
Consumer: Received 3
```

This demonstrates basic **FIFO inter-task communication**.

---

## 6. Experiment 2 — Producer Faster Than Consumer

Modify the producer delay to:

```cpp
vTaskDelay(
    500 / portTICK_PERIOD_MS
);
```

and add a delay to the consumer:

```cpp
vTaskDelay(
    2000 / portTICK_PERIOD_MS
);
```

The system now behaves approximately as:

```text
Producer
  ↓ 500 ms
[Queue]
[Data ]
[Data ]
[Data ]
[Data ]
[Data ]
  ↓ 2000 ms
Consumer
```

### Observation

Monitor:

- How quickly the queue fills.
- What happens when the queue becomes full.
- Whether the producer blocks while waiting for free space.
- Whether the consumer still receives data in FIFO order.

---

## 7. Experiment 3 — Sensor Data via Queue

A more realistic embedded system separates **sensor acquisition** from **data processing**.

```text
Sensor
   ↓
Sensor Task
   ↓
FreeRTOS Queue
   ↓
Processing Task
   ↓
Serial Monitor
```

Example using an analog input:

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34

QueueHandle_t sensorQueue;

void SensorTask(void *pvParameters) {

  int sensorValue;

  for (;;) {

    sensorValue = analogRead(SENSOR_PIN);

    if (xQueueSend(
          sensorQueue,
          &sensorValue,
          portMAX_DELAY) == pdPASS) {

      Serial.printf(
        "Sensor: %d\n",
        sensorValue
      );
    }

    vTaskDelay(
      1000 / portTICK_PERIOD_MS
    );
  }
}

void ProcessingTask(void *pvParameters) {

  int data;

  for (;;) {

    if (xQueueReceive(
          sensorQueue,
          &data,
          portMAX_DELAY) == pdTRUE) {

      float normalized =
        data / 4095.0;

      Serial.printf(
        "Processed: %.3f\n",
        normalized
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  sensorQueue =
    xQueueCreate(
      10,
      sizeof(int)
    );

  if (sensorQueue == NULL) {
    Serial.println("Queue creation failed!");
    while (1);
  }

  xTaskCreate(
    SensorTask,
    "Sensor",
    2048,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    ProcessingTask,
    "Processing",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

This separates **data acquisition** from **data processing**.

---

## 8. Experiment 4 — Sending Multiple Variables with a Structure

Real embedded applications often need to send several values together.

For example:

```text
Sensor Data = {Temperature, Humidity, Time}
```

A C/C++ `struct` can be passed through a FreeRTOS queue.

```cpp
struct SensorData {
  float temperature;
  float humidity;
  unsigned long timestamp;
};
```

Create the queue using:

```cpp
sensorQueue =
    xQueueCreate(
      5,
      sizeof(SensorData)
    );
```

A complete example is:

```cpp
#include <Arduino.h>

QueueHandle_t sensorQueue;

struct SensorData {
  float temperature;
  float humidity;
  unsigned long timestamp;
};

void SensorTask(void *pvParameters) {

  SensorData data;

  for (;;) {

    data.temperature =
      random(200, 350) / 10.0;

    data.humidity =
      random(400, 800) / 10.0;

    data.timestamp = millis();

    xQueueSend(
      sensorQueue,
      &data,
      portMAX_DELAY
    );

    vTaskDelay(
      1000 / portTICK_PERIOD_MS
    );
  }
}

void ProcessingTask(void *pvParameters) {

  SensorData received;

  for (;;) {

    if (xQueueReceive(
          sensorQueue,
          &received,
          portMAX_DELAY) == pdTRUE) {

      Serial.printf(
        "T = %.1f C, H = %.1f %%, Time = %lu ms\n",
        received.temperature,
        received.humidity,
        received.timestamp
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  sensorQueue =
    xQueueCreate(
      5,
      sizeof(SensorData)
    );

  xTaskCreate(
    SensorTask,
    "Sensor",
    2048,
    NULL,
    2,
    NULL
  );

  xTaskCreate(
    ProcessingTask,
    "Processing",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {}
```

### Expected Output

```text
T = 25.2 C, H = 64.5 %, Time = 1002 ms
T = 28.1 C, H = 55.3 %, Time = 2003 ms
T = 31.0 C, H = 72.1 %, Time = 3004 ms
```

---

## 9. Experiment 5 — Three-Task Data Pipeline

The system can be extended to three tasks:

```text
Sensor Task
     ↓
   Queue 1
     ↓
Processing Task
     ↓
   Queue 2
     ↓
Communication Task
```

The tasks perform:

### Task 1 — Sensor Acquisition

```text
Read Sensor → Queue 1
```

### Task 2 — Data Processing

```text
Queue 1 → Filter/Process → Queue 2
```

### Task 3 — Communication

```text
Queue 2 → Serial / Wi-Fi / MQTT
```

This architecture closely represents a typical **embedded IoT data pipeline**.

---

## 10. Monitoring Queue Status

FreeRTOS provides functions for checking queue status.

The number of messages waiting can be obtained using:

```cpp
uxQueueMessagesWaiting(dataQueue);
```

For example:

```cpp
Serial.printf(
  "Queue items = %d\n",
  uxQueueMessagesWaiting(dataQueue)
);
```

Available queue space can be checked using:

```cpp
uxQueueSpacesAvailable(dataQueue);
```

These functions are useful when investigating queue loading and system performance.

---

## 11. Queue Timing and Blocking

Consider:

```cpp
xQueueSend(
    dataQueue,
    &value,
    portMAX_DELAY
);
```

If the queue is full, the producer waits until space becomes available.

Similarly:

```cpp
xQueueReceive(
    dataQueue,
    &value,
    portMAX_DELAY
);
```

causes the consumer to wait when the queue is empty.

This provides efficient synchronization:

```text
Queue Empty
    ↓
Consumer Blocks
    ↓
Producer Sends Data
    ↓
Consumer Becomes Ready
```

and

```text
Queue Full
    ↓
Producer Blocks
    ↓
Consumer Removes Data
    ↓
Producer Becomes Ready
```

---

## 12. Queue vs. Shared Variable

| Feature | Shared Variable | FreeRTOS Queue |
|---|---|---|
| Data transfer | Yes | **Yes** |
| Thread-safe by default | No | **Yes** |
| FIFO ordering | No | **Yes** |
| Buffer multiple values | Usually no | **Yes** |
| Blocking support | No | **Yes** |
| Producer-consumer | Difficult | **Well suited** |
| Requires mutex | Often | Usually not for queue operations |
| Inter-task communication | Basic | **Recommended** |

For simple data exchange:

```text
Shared Variable
     ↓
Requires Synchronization
     ↓
Mutex / Critical Section
```

With a queue:

```text
Producer
   ↓
Queue
   ↓
Consumer
```

The queue combines **data transfer and synchronization**.

---

## 13. Queue vs. Semaphore vs. Mutex

| Mechanism | Primary Purpose | Transfers Data? | Typical Application |
|---|---|---:|---|
| **Mutex** | Protect shared resource | No | Protect I²C/SPI/Serial |
| **Binary Semaphore** | Signal an event | No | Sensor-ready notification |
| **Counting Semaphore** | Count events/resources | No | Resource pool |
| **Queue** | Transfer data between tasks | **Yes** | Sensor-data pipeline |

A useful rule is:

```text
Mutex       → Protect
Semaphore   → Signal
Queue       → Communicate
```

---

## 14. Experiments

### Experiment 1 — Queue Length

Change:

```cpp
xQueueCreate(5, sizeof(int));
```

to:

```cpp
xQueueCreate(2, sizeof(int));
```

and then:

```cpp
xQueueCreate(10, sizeof(int));
```

Observe how queue capacity affects producer blocking.

### Experiment 2 — Producer Rate

Test producer periods of:

- 200 ms
- 500 ms
- 1000 ms

Keep the consumer rate fixed.

Record the maximum number of items waiting in the queue.

### Experiment 3 — Consumer Rate

Increase the processing time of the consumer.

Determine when:

$$
R_{\text{producer}} > R_{\text{consumer}}
$$

where $R$ represents the data-processing rate.

Observe queue accumulation.

### Experiment 4 — Task Priority

Assign:

```text
Sensor Task      = Priority 3
Processing Task  = Priority 2
Output Task      = Priority 1
```

Then change the priorities and compare system behavior.

### Experiment 5 — Queue Overflow

Replace `portMAX_DELAY` with a finite timeout.

For example:

```cpp
pdMS_TO_TICKS(100)
```

Check the return value:

```cpp
if (xQueueSend(
      dataQueue,
      &value,
      pdMS_TO_TICKS(100)) != pdPASS) {

  Serial.println("Queue full: data not sent");
}
```

Observe when data cannot be inserted into the queue.

---

## 15. Exercises

### Exercise 1 — Basic Producer-Consumer

Create:

- **Producer:** Generate an integer every 500 ms.
- **Consumer:** Receive and print the integer.

Use a queue of length 5.

### Exercise 2 — Sensor Acquisition

Implement:

```text
Analog Sensor
      ↓
Sensor Task
      ↓
Queue
      ↓
Processing Task
      ↓
Serial Monitor
```

Normalize the ADC value before displaying it.

### Exercise 3 — Structured Sensor Data

Create:

```cpp
struct SensorData {
  float temperature;
  float humidity;
  unsigned long timestamp;
};
```

Transfer the structure between two tasks using a queue.

### Exercise 4 — Three-Task IoT Pipeline

Implement:

```text
Sensor Task
     ↓
Queue 1
     ↓
Processing Task
     ↓
Queue 2
     ↓
Communication Task
```

The communication task may output the processed data through the Serial Monitor.

### Exercise 5 — Queue Performance

Measure:

- Producer period
- Consumer period
- Queue length
- Maximum queue occupancy
- Number of failed transmissions

Summarize the relationship between task timing and queue utilization.

---

## 16. Checkpoint Questions

1. What is a FreeRTOS queue?
2. What is the difference between a producer and a consumer?
3. What does FIFO mean?
4. What is the purpose of `xQueueCreate()`?
5. What is the difference between `xQueueSend()` and `xQueueReceive()`?
6. What happens when a consumer attempts to receive from an empty queue?
7. What happens when a producer attempts to send to a full queue?
8. What does `portMAX_DELAY` mean?
9. Why is a queue safer than an unprotected global variable for inter-task data transfer?
10. How does queue length affect system behavior?
11. Why might a structure be useful as a queue item?
12. What happens if the producer operates faster than the consumer?
13. How can task priority affect queue processing?
14. What is the difference between a queue and a semaphore?
15. When should a queue be preferred over a mutex?

---

## 17. Expected Results

Students should observe reliable data transfer between concurrent FreeRTOS tasks.

A basic producer-consumer system should demonstrate:

```text
Producer → Queue → Consumer
```

A more realistic embedded system demonstrates:

```text
Sensor Acquisition
        ↓
      Queue
        ↓
    Processing
        ↓
      Queue
        ↓
  Communication
```

Students should also observe that queue capacity and producer-consumer timing affect **latency, buffering, blocking, and potential data loss when finite timeouts are used**.

---

## 18. Submission

Submit the following:

- FreeRTOS source code.
- Screenshot of the Serial Monitor.
- Producer-consumer queue implementation.
- Sensor-data queue implementation.
- Three-task pipeline implementation.
- Experimental results for different queue sizes.
- Short discussion of queue blocking and task timing.

A suggested results table is:

| Queue Size | Producer Period | Consumer Period | Max Queue Occupancy | Failed Sends |
|---:|---:|---:|---:|---:|
| 2 | 500 ms | 1000 ms |  |  |
| 5 | 500 ms | 1000 ms |  |  |
| 10 | 500 ms | 1000 ms |  |  |

---

## 19. Conclusion

In this lab, **FreeRTOS queues** are used to implement safe inter-task communication on the ESP32. The producer-consumer model demonstrates how one task can generate data while another independently processes it.

Unlike mutexes, which primarily protect shared resources, and semaphores, which primarily provide synchronization or event signaling, queues allow tasks to **transfer actual data while providing thread-safe synchronization**.

The lab establishes the following progression:

```text
LAB 5
Tasks & Scheduling
      ↓
LAB 6
Semaphores & Mutexes
      ↓
LAB 7
Inter-Task Communication
      ↓
LAB 8
Software Timers & Memory Management
```

The next lab extends this RTOS architecture to **FreeRTOS software timers, heap monitoring, and dynamic vs. static memory allocation**.
