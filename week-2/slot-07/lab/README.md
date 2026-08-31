# LAB 7 — Producer–Consumer Sensor Data Processing Using FreeRTOS Queues

## 1. Objective

The objective of this lab is to:

1. Understand inter-task communication in FreeRTOS.
2. Implement a **producer–consumer architecture**.
3. Create a producer task that acquires sensor data.
4. Create a consumer task that processes sensor data.
5. Transfer data safely using a **FreeRTOS queue**.
6. Understand FIFO queue behavior.
7. Use `xQueueCreate()`, `xQueueSend()`, and `xQueueReceive()`.
8. Compare queue-based communication with shared-variable access.
9. Observe blocking behavior when a queue is empty or full.
10. Investigate the effect of producer and consumer execution rates.
11. Monitor queue occupancy and overflow conditions.
12. Transfer structured sensor data through a queue.
13. Develop a multi-stage embedded data-processing pipeline.
14. Apply FreeRTOS queues to realistic IoT and sensor-processing applications.

---

## 2. Background

In a multitasking embedded system, one task may acquire data while another task processes or transmits it.

For example:

```text
Sensor Task
     ↓
Data
     ↓
Processing Task
```

A common solution is the **producer–consumer model**.

```text
Producer
   ↓
Queue
   ↓
Consumer
```

The producer generates data and places it into a queue. The consumer removes data from the queue and processes it.

This approach avoids unsafe direct sharing of variables between tasks.

---

## 3. Producer–Consumer Architecture

In this lab:

```text
Sensor
   ↓
Producer Task
   ↓
FreeRTOS Queue
   ↓
Consumer Task
   ↓
Data Processing
   ↓
Serial Monitor
```

The producer and consumer can operate at different rates.

For example:

```text
Producer
Every 500 ms

Consumer
Every 1000 ms
```

The queue temporarily stores data when the producer generates information faster than the consumer processes it.

---

## 4. What Is a Queue?

A queue is a FreeRTOS kernel object used to transfer data safely between tasks.

It usually follows:

```text
FIFO
```

which means:

```text
First In
First Out
```

Example:

```text
Producer sends:

10
20
30

Queue:

Front → 10 → 20 → 30 → Back
```

The consumer receives:

```text
10
20
30
```

in the same order.

---

## 5. Queue Operation

The basic communication process is:

```text
Producer Task
     ↓
xQueueSend()
     ↓
Queue
     ↓
xQueueReceive()
     ↓
Consumer Task
```

Unlike a shared variable, each queue item is copied into queue storage.

This makes communication more structured and reduces race-condition problems.

---

## 6. Shared Variable vs. Queue

### Shared Variable

```text
Producer ───→ Shared Data ←─── Consumer
```

Potential problem:

```text
Concurrent Access
      ↓
Race Condition
```

A mutex may therefore be required.

### Queue

```text
Producer
   ↓
Queue
   ↓
Consumer
```

The queue itself manages synchronization.

A useful rule is:

```text
Mutex
   ↓
Protect Shared Resource

Semaphore
   ↓
Signal Event

Queue
   ↓
Transfer Data
```

---

## 7. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Potentiometer or analog sensor
- Optional temperature/light sensor
- Breadboard
- Jumper wires

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- FreeRTOS
- Serial Monitor

For the basic experiment, an analog input can be used as a simulated sensor.

---

## 8. Important FreeRTOS Queue Functions

Create a queue:

```cpp
xQueueCreate()
```

Send an item:

```cpp
xQueueSend()
```

Receive an item:

```cpp
xQueueReceive()
```

Inspect number of stored items:

```cpp
uxQueueMessagesWaiting()
```

Inspect available spaces:

```cpp
uxQueueSpacesAvailable()
```

---

## 9. Queue Handle

Create a queue handle:

```cpp
QueueHandle_t sensorQueue;
```

The queue handle is used by the producer and consumer tasks to access the same queue.

---

## 10. Creating a Queue

Suppose the queue stores integers.

```cpp
sensorQueue =
    xQueueCreate(
      5,
      sizeof(int)
    );
```

This creates:

```text
Queue Length = 5 items
Item Size    = sizeof(int)
```

Conceptually:

```text
┌─────┬─────┬─────┬─────┬─────┐
│     │     │     │     │     │
└─────┴─────┴─────┴─────┴─────┘
```

The queue can hold at most five integer values.

---

## 11. Experiment 1 — Basic Producer–Consumer Queue

Create a producer that generates integer values.

```cpp
void ProducerTask(void *parameter) {

  int value = 0;

  while (1) {

    value++;

    xQueueSend(
      sensorQueue,
      &value,
      portMAX_DELAY
    );

    Serial.print(
      "Produced: "
    );

    Serial.println(value);

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}
```

Create a consumer:

```cpp
void ConsumerTask(void *parameter) {

  int receivedValue;

  while (1) {

    if (
      xQueueReceive(
        sensorQueue,
        &receivedValue,
        portMAX_DELAY
      ) == pdPASS
    ) {

      Serial.print(
        "Consumed: "
      );

      Serial.println(
        receivedValue
      );
    }
  }
}
```

---

## 12. Complete Basic Queue Program

```cpp
#include <Arduino.h>

QueueHandle_t sensorQueue;

void ProducerTask(void *parameter) {

  int value = 0;

  while (1) {

    value++;

    xQueueSend(
      sensorQueue,
      &value,
      portMAX_DELAY
    );

    Serial.print(
      "Produced: "
    );

    Serial.println(value);

    vTaskDelay(
      pdMS_TO_TICKS(1000)
    );
  }
}

void ConsumerTask(void *parameter) {

  int receivedValue;

  while (1) {

    if (
      xQueueReceive(
        sensorQueue,
        &receivedValue,
        portMAX_DELAY
      ) == pdPASS
    ) {

      Serial.print(
        "Consumed: "
      );

      Serial.println(
        receivedValue
      );
    }
  }
}

void setup() {

  Serial.begin(115200);

  sensorQueue =
      xQueueCreate(
        5,
        sizeof(int)
      );

  if (
    sensorQueue == NULL
  ) {

    Serial.println(
      "Queue creation failed"
    );

    while (1) {
    }
  }

  xTaskCreatePinnedToCore(
    ProducerTask,
    "Producer",
    2048,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    ConsumerTask,
    "Consumer",
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

---

## 13. Expected Output

Possible Serial Monitor output:

```text
Produced: 1
Consumed: 1
Produced: 2
Consumed: 2
Produced: 3
Consumed: 3
Produced: 4
Consumed: 4
```

This demonstrates FIFO communication.

---

## 14. Experiment 2 — Acquire Real Sensor Data

Use an analog sensor connected to:

```text
GPIO 34
```

Define:

```cpp
#define SENSOR_PIN 34
```

Producer task:

```cpp
void SensorTask(void *parameter) {

  int sensorValue;

  while (1) {

    sensorValue =
        analogRead(
          SENSOR_PIN
        );

    xQueueSend(
      sensorQueue,
      &sensorValue,
      portMAX_DELAY
    );

    vTaskDelay(
      pdMS_TO_TICKS(500)
    );
  }
}
```

Architecture:

```text
Analog Sensor
      ↓
analogRead()
      ↓
Sensor Task
      ↓
Queue
```

---

## 15. Consumer Data Processing

The consumer can process the sensor value.

For example:

```cpp
void ProcessingTask(void *parameter) {

  int sensorValue;

  while (1) {

    if (
      xQueueReceive(
        sensorQueue,
        &sensorValue,
        portMAX_DELAY
      ) == pdPASS
    ) {

      float percentage =
          sensorValue
          / 4095.0
          * 100.0;

      Serial.print(
        "Sensor = "
      );

      Serial.print(
        sensorValue
      );

      Serial.print(
        " | Percentage = "
      );

      Serial.print(
        percentage
      );

      Serial.println("%");
    }
  }
}
```

For a 12-bit ADC:

$$
0\leq x\leq4095.
$$

A normalized percentage can be calculated as:

$$
P=
\frac{x}{4095}
\times100\%.
$$

---

## 16. Experiment 3 — Producer Faster Than Consumer

Modify the producer:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(200)
);
```

Modify the consumer:

```cpp
vTaskDelay(
  pdMS_TO_TICKS(1000)
);
```

Now:

```text
Producer Period = 200 ms

Consumer Period = 1000 ms
```

The producer generates approximately:

$$
f_p= \frac{1}{0.2}
= 5~\text{items/s}.
$$

The consumer processes approximately:

$$
f_c= \frac{1}{1}
= 1~\text{item/s}.
$$

Therefore:

$$
f_p>f_c.
$$

The queue begins to fill.

---

## 17. Queue Filling

Conceptually:

```text
Producer
   ↓
[10]
   ↓
[10][20]
   ↓
[10][20][30]
   ↓
[10][20][30][40]
   ↓
[10][20][30][40][50]
```

When the queue is full:

```text
Producer
   ↓
xQueueSend()
   ↓
Queue Full
   ↓
Wait / Fail
```

depending on the configured timeout.

---

## 18. Queue Send Timeout

Example:

```cpp
if (
  xQueueSend(
    sensorQueue,
    &sensorValue,
    pdMS_TO_TICKS(100)
  ) == pdPASS
) {

  Serial.println(
    "Data sent"
  );
}
else {

  Serial.println(
    "Queue full - data not sent"
  );
}
```

This allows the application to detect congestion.

---

## 19. Experiment 4 — Observe Queue Occupancy

Use:

```cpp
UBaseType_t count =
    uxQueueMessagesWaiting(
      sensorQueue
    );
```

Print:

```cpp
Serial.print(
  "Items in queue = "
);

Serial.println(
  count
);
```

Record:

| Time | Produced | Consumed | Items Waiting |
|---:|---:|---:|---:|
| 1 s |  |  |  |
| 2 s |  |  |  |
| 3 s |  |  |  |
| 4 s |  |  |  |
| 5 s |  |  |  |

---

## 20. Available Queue Space

Use:

```cpp
UBaseType_t freeSpace =
    uxQueueSpacesAvailable(
      sensorQueue
    );
```

For queue length:

$$
N_Q=5,
$$

the relationship is:

$$
N_{\text{used}}
+
N_{\text{free}}
= N_Q.
$$

For example:

```text
Used = 3
Free = 2
Total = 5
```

---

## 21. Experiment 5 — Consumer Faster Than Producer

Set:

```text
Producer Period = 1000 ms
Consumer Processing = 100 ms
```

The consumer frequently reaches:

```cpp
xQueueReceive(
  sensorQueue,
  &data,
  portMAX_DELAY
);
```

when the queue is empty.

The consumer then enters the **Blocked** state.

---

## 22. Queue Empty Behavior

```text
Consumer
   ↓
xQueueReceive()
   ↓
Queue Empty
   ↓
Blocked
   ↓
Producer Sends Data
   ↓
Consumer Becomes Ready
```

This is more efficient than continuously checking the queue using a busy loop.

---

## 23. Experiment 6 — Non-Blocking Receive

Instead of waiting forever:

```cpp
xQueueReceive(
  sensorQueue,
  &data,
  0
);
```

returns immediately.

Example:

```cpp
if (
  xQueueReceive(
    sensorQueue,
    &data,
    0
  ) == pdPASS
) {

  Serial.println(
    "Data received"
  );
}
else {

  Serial.println(
    "Queue empty"
  );
}
```

Compare blocking and non-blocking behavior.

---

## 24. Blocking vs. Polling

### Polling

```text
Check Queue
   ↓
Empty?
   ↓
Check Again
   ↓
Check Again
```

Consumes CPU time.

### Blocking

```text
Wait on Queue
    ↓
Task Blocked
    ↓
CPU Used by Other Tasks
    ↓
Data Arrives
    ↓
Task Ready
```

For RTOS applications, blocking is often preferable.

---

## 25. Experiment 7 — Send Structured Sensor Data

Real applications usually require more than one variable.

Define:

```cpp
struct SensorData {

  int raw;

  float voltage;

  unsigned long timestamp;
};
```

Create the queue using:

```cpp
sensorQueue =
    xQueueCreate(
      10,
      sizeof(SensorData)
    );
```

Now each queue item contains an entire sensor record.

---

## 26. Sensor Data Structure

A producer may create:

```cpp
SensorData data;

data.raw =
    analogRead(
      SENSOR_PIN
    );

data.voltage =
    data.raw
    * 3.3
    / 4095.0;

data.timestamp =
    millis();
```

The voltage is:

$$
V=
\frac{x}{4095}
V_{\text{ref}},
$$

where:

$$
V_{\text{ref}}=3.3~\text{V}
$$

for the simplified lab calculation.

---

## 27. Send the Structure

```cpp
xQueueSend(
    sensorQueue,
    &data,
    portMAX_DELAY
);
```

The complete structure is copied into the queue.

Architecture:

```text
SensorData
 ├── raw
 ├── voltage
 └── timestamp
       ↓
     Queue
```

---

## 28. Receive the Structure

Consumer:

```cpp
SensorData received;

if (
  xQueueReceive(
    sensorQueue,
    &received,
    portMAX_DELAY
  ) == pdPASS
) {

  Serial.print(
    "Raw = "
  );

  Serial.print(
    received.raw
  );

  Serial.print(
    " | Voltage = "
  );

  Serial.print(
    received.voltage
  );

  Serial.print(
    " V | Time = "
  );

  Serial.println(
    received.timestamp
  );
}
```

---

## 29. Experiment 8 — Complete Sensor Producer–Consumer System

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34

struct SensorData {

  int raw;

  float voltage;

  unsigned long timestamp;
};

QueueHandle_t sensorQueue;

void SensorTask(void *parameter) {

  SensorData data;

  while (1) {

    data.raw =
        analogRead(
          SENSOR_PIN
        );

    data.voltage =
        data.raw
        * 3.3
        / 4095.0;

    data.timestamp =
        millis();

    if (
      xQueueSend(
        sensorQueue,
        &data,
        pdMS_TO_TICKS(100)
      ) == pdPASS
    ) {

      Serial.println(
        "Sensor data queued"
      );
    }
    else {

      Serial.println(
        "Queue full"
      );
    }

    vTaskDelay(
      pdMS_TO_TICKS(500)
    );
  }
}

void ProcessingTask(void *parameter) {

  SensorData data;

  while (1) {

    if (
      xQueueReceive(
        sensorQueue,
        &data,
        portMAX_DELAY
      ) == pdPASS
    ) {

      float percentage =
          data.raw
          / 4095.0
          * 100.0;

      Serial.print(
        "Raw = "
      );

      Serial.print(
        data.raw
      );

      Serial.print(
        " | Voltage = "
      );

      Serial.print(
        data.voltage,
        3
      );

      Serial.print(
        " V | Level = "
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
    SENSOR_PIN,
    INPUT
  );

  sensorQueue =
      xQueueCreate(
        10,
        sizeof(SensorData)
      );

  if (
    sensorQueue == NULL
  ) {

    Serial.println(
      "Queue creation failed"
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
}

void loop() {
}
```

---

## 30. Experiment 9 — Add Data Classification

Modify the consumer to classify the sensor level.

For example:

```cpp
uint8_t state;

if (
  data.raw < 1300
) {

  state = 0;
}
else if (
  data.raw < 2700
) {

  state = 1;
}
else {

  state = 2;
}
```

Use:

```text
0 → LOW
1 → MEDIUM
2 → HIGH
```

Processing flow:

```text
Sensor Value
     ↓
Queue
     ↓
Processing Task
     ↓
Threshold Classification
     ↓
LOW / MEDIUM / HIGH
```

---

## 31. Classification Model

The decision can be represented as:

$$
C(x)=
\begin{cases}
\text{LOW}, & x<1300,\\
\text{MEDIUM}, & 1300\leq x<2700,\\
\text{HIGH}, & x\geq2700.
\end{cases}
$$

This demonstrates that the consumer can perform application processing independently of data acquisition.

---

## 32. Experiment 10 — Add Moving Average Processing

The consumer can calculate a moving average.

For $N$ samples:

$$
\bar{x}
= \frac{1}{N}
\sum_{i=1}^{N}x_i.
$$

For example, use:

```text
N = 5
```

The consumer receives:

```text
x1 x2 x3 x4 x5
```

and computes:

```text
Average
```

This simulates basic signal processing.

---

## 33. Producer–Consumer Separation

A key design principle is:

```text
Producer
   ↓
Acquire Data Only

Consumer
   ↓
Process Data Only
```

This provides **separation of concerns**.

Benefits include:

- Cleaner software structure
- Easier debugging
- Better modularity
- Independent task timing
- Easier future extension

---

## 34. Experiment 11 — Task Priorities

Assign:

```text
Sensor Task
Priority = 3

Processing Task
Priority = 2
```

The sensor task may be more timing-sensitive because it should sample at regular intervals.

However, task priority should be selected according to actual timing requirements, not simply because a task is called a sensor task.

---

## 35. Periodic Sensor Acquisition

Use:

```cpp
vTaskDelayUntil()
```

for more regular sensor sampling.

Example:

```cpp
void SensorTask(void *parameter) {

  TickType_t lastWakeTime =
      xTaskGetTickCount();

  const TickType_t period =
      pdMS_TO_TICKS(100);

  while (1) {

    // Acquire and send sensor data

    vTaskDelayUntil(
      &lastWakeTime,
      period
    );
  }
}
```

This gives an intended sampling period:

$$
T_s=100~\text{ms}.
$$

Therefore, the sampling frequency is:

$$
f_s= \frac{1}{T_s}
= 10~\text{Hz}.
$$

---

## 36. Experiment 12 — Measure Queue Latency

Add a timestamp when the producer creates the data:

```cpp
data.timestamp =
    micros();
```

When the consumer receives it:

```cpp
unsigned long receiveTime =
    micros();
```

Calculate:

```cpp
unsigned long latency =
    receiveTime
    - data.timestamp;
```

Mathematically:

$$
T_{\text{queue}}
= t_{\text{receive}}
- t_{\text{send}}.
$$

---

## 37. Queue Latency Table

Record:

| Sample | Producer Time | Consumer Time | Queue Latency |
|---:|---:|---:|---:|
| 1 |  |  |  |
| 2 |  |  |  |
| 3 |  |  |  |
| 4 |  |  |  |
| 5 |  |  |  |
| 6 |  |  |  |
| 7 |  |  |  |
| 8 |  |  |  |
| 9 |  |  |  |
| 10 |  |  |  |

Calculate average latency:

$$
\bar{T}_{Q}
= \frac{1}{N}
\sum_{i=1}^{N}
T_{Q,i}.
$$

---

## 38. Experiment 13 — Queue Length Study

Test:

```text
Queue Length = 1
Queue Length = 5
Queue Length = 10
Queue Length = 20
```

Keep producer and consumer rates constant.

Observe:

- Number of full-queue events
- Average queue occupancy
- Data latency
- Memory usage

Record:

| Queue Length | Full Events | Average Occupancy | Average Latency |
|---:|---:|---:|---:|
| 1 |  |  |  |
| 5 |  |  |  |
| 10 |  |  |  |
| 20 |  |  |  |

---

## 39. Queue Length Trade-Off

A larger queue provides:

```text
More Buffering
```

but requires:

```text
More Memory
```

and may increase:

```text
Data Waiting Time
```

Therefore:

$$
\boxed{
\text{Queue Size}
\leftrightarrow
\text{Memory}
\leftrightarrow
\text{Latency}
\leftrightarrow
\text{Overflow Risk}
}
$$

---

## 40. Experiment 14 — Three-Stage Pipeline

Extend the architecture:

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

This resembles a real connected embedded system.

---

## 41. Pipeline Architecture

```text
Physical Sensor
      ↓
Sensor Acquisition
      ↓
Queue 1
      ↓
Data Processing
      ↓
Queue 2
      ↓
Communication
      ↓
Serial / Wi-Fi / MQTT
```

Each task performs one responsibility.

---

## 42. Processed Data Structure

Define:

```cpp
struct ProcessedData {

  float value;

  uint8_t state;

  unsigned long timestamp;
};
```

Use:

```text
0 → LOW
1 → MEDIUM
2 → HIGH
```

Using a numeric state code avoids unnecessary dynamic memory use and is suitable for embedded applications.

---

## 43. Experiment 15 — Complete Queue Pipeline

Architecture:

```text
Task 1 — Sensor
Priority 3
     ↓
Queue 1
     ↓
Task 2 — Processing
Priority 2
     ↓
Queue 2
     ↓
Task 3 — Logging
Priority 1
```

This demonstrates:

```text
Acquisition
    ↓
Processing
    ↓
Communication
```

using FreeRTOS queues.

---

## 44. Queue Overflow

Queue overflow occurs when:

```text
Producer Rate
     >
Consumer Rate
```

for a sufficiently long time.

Define:

$$
R_p=\text{producer data rate}
$$

and:

$$
R_c=\text{consumer processing rate}.
$$

If:

$$
R_p>R_c,
$$

the queue occupancy tends to increase.

If:

$$
R_p<R_c,
$$

the queue tends to remain empty or lightly occupied.

---

## 45. Approximate Queue Growth Rate

Ignoring scheduler and processing variations:

$$
R_Q
= R_p-R_c.
$$

For example:

$$
R_p=10~\text{samples/s}
$$

and:

$$
R_c=5~\text{samples/s}.
$$

Therefore:

$$
R_Q
= 5~\text{samples/s}.
$$

For a queue containing 20 empty positions, the idealized fill time is approximately:

$$
T_{\text{fill}}
= \frac{20}{5}
= 4~\text{s}.
$$

---

## 46. Queue Send Strategies

When the queue is full, several strategies are possible.

### Wait Forever

```cpp
xQueueSend(
  queue,
  &data,
  portMAX_DELAY
);
```

Producer blocks until space becomes available.

### Wait for a Limited Time

```cpp
xQueueSend(
  queue,
  &data,
  pdMS_TO_TICKS(100)
);
```

### Do Not Wait

```cpp
xQueueSend(
  queue,
  &data,
  0
);
```

The appropriate choice depends on application requirements.

---

## 47. Data-Loss Policy

For some systems:

```text
Every Sample Is Important
```

so blocking may be appropriate.

For other systems:

```text
Only Latest Data Matters
```

dropping stale samples may be preferable.

This leads to application-specific queue design.

---

## 48. Queue vs. Mutex Revisited

The previous lab used:

```text
Shared Data
   ↓
Mutex
```

This lab uses:

```text
Producer
   ↓
Queue
   ↓
Consumer
```

The difference is important.

A mutex answers:

> Who may access this shared resource now?

A queue answers:

> How should data be transferred safely from one task to another?

---

## 49. Queue Advantages

FreeRTOS queues provide:

- Thread-safe data transfer
- FIFO ordering
- Automatic task blocking
- Decoupling between producer and consumer
- Buffering
- Structured inter-task communication
- Reduced need for direct shared memory
- Improved modularity

---

## 50. Common Queue Errors

### Wrong Item Size

Incorrect:

```cpp
xQueueCreate(
  10,
  sizeof(int)
);
```

when sending:

```cpp
SensorData
```

Correct:

```cpp
xQueueCreate(
  10,
  sizeof(SensorData)
);
```

### Passing Incorrect Pointer

Send:

```cpp
xQueueSend(
  queue,
  &data,
  portMAX_DELAY
);
```

not:

```cpp
xQueueSend(
  queue,
  data,
  portMAX_DELAY
);
```

when `data` is a normal structure variable.

### Queue Not Created

Always check:

```cpp
if (
  queue == NULL
) {

  Serial.println(
    "Queue creation failed"
  );
}
```

### Producer Much Faster Than Consumer

This can lead to:

```text
Queue Full
   ↓
Producer Blocking
or
Data Loss
```

### Very Large Queue Items

Queues copy data.

Large structures therefore increase:

```text
Memory Usage
+
Copying Overhead
```

For very large buffers, pointer-based designs may be considered, but memory ownership must then be handled carefully.

---

## 51. Experiments Summary

### Experiment 1 — Basic Queue

Create a producer and consumer using integer values.

### Experiment 2 — Sensor Acquisition

Read analog sensor data and send it through a queue.

### Experiment 3 — Fast Producer

Make the producer faster than the consumer.

### Experiment 4 — Queue Occupancy

Measure the number of stored items.

### Experiment 5 — Fast Consumer

Observe blocking on an empty queue.

### Experiment 6 — Non-Blocking Receive

Compare polling and blocking.

### Experiment 7 — Structured Data

Transfer a `SensorData` structure.

### Experiment 8 — Complete Sensor System

Implement sensor acquisition and processing.

### Experiment 9 — Classification

Classify sensor values as LOW, MEDIUM, or HIGH.

### Experiment 10 — Moving Average

Process multiple received samples.

### Experiment 11 — Task Priority

Assign realistic priorities.

### Experiment 12 — Queue Latency

Measure producer-to-consumer delay.

### Experiment 13 — Queue Length

Study queue size, latency, and overflow.

### Experiment 14 — Three-Stage Pipeline

Use two queues and three tasks.

### Experiment 15 — Complete Data Pipeline

Implement acquisition, processing, and logging.

---

## 52. Exercises

### Exercise 1 — Integer Producer–Consumer

Create:

```text
Producer
   ↓
Integer Queue
   ↓
Consumer
```

The producer generates:

```text
1, 2, 3, 4, ...
```

Verify FIFO order.

### Exercise 2 — Analog Sensor Queue

Use:

```text
GPIO 34
```

Acquire analog values every:

```text
500 ms
```

Send them to a processing task.

### Exercise 3 — Sensor Conversion

Convert ADC value:

$$
0\leq x\leq4095
$$

to:

$$
0\leq P\leq100\%.
$$

using:

$$
P=
\frac{x}{4095}
\times100.
$$

### Exercise 4 — Producer Faster Than Consumer

Configure:

```text
Producer = 200 ms
Consumer = 1000 ms
```

Record queue occupancy for 10 seconds.

### Exercise 5 — Queue Full Handling

Use:

```cpp
xQueueSend(
  queue,
  &data,
  pdMS_TO_TICKS(100)
);
```

Count the number of failed send operations.

### Exercise 6 — Structured Sensor Data

Create:

```cpp
struct SensorData {

  int raw;

  float voltage;

  unsigned long timestamp;
};
```

Transfer the complete structure through a queue.

### Exercise 7 — Data Processing

The consumer should calculate:

- ADC percentage
- Voltage
- LOW/MEDIUM/HIGH class

and display the results.

### Exercise 8 — Queue Latency

Measure:

$$
T_{\text{queue}}
= t_{\text{receive}}
- t_{\text{send}}.
$$

Collect at least 20 samples.

### Exercise 9 — Queue-Length Comparison

Test queue lengths:

```text
1
5
10
20
```

Compare overflow and latency.

### Exercise 10 — Three-Task System

Construct:

```text
Sensor Task
     ↓
Queue 1
     ↓
Processing Task
     ↓
Queue 2
     ↓
Display Task
```

Demonstrate successful data flow.

---

## 53. Checkpoint Questions

1. What is a producer–consumer system?
2. What is the producer in this lab?
3. What is the consumer?
4. What is a FreeRTOS queue?
5. What does FIFO mean?
6. Which function creates a queue?
7. What does `xQueueSend()` do?
8. What does `xQueueReceive()` do?
9. What happens when a consumer waits on an empty queue?
10. What happens when a producer sends to a full queue?
11. What does `portMAX_DELAY` mean?
12. What does `uxQueueMessagesWaiting()` return?
13. What does `uxQueueSpacesAvailable()` return?
14. Why are queues useful for sensor applications?
15. How is a queue different from a shared variable?
16. How is a queue different from a mutex?
17. Why can blocking be more efficient than polling?
18. What happens when the producer is faster than the consumer?
19. What happens when the consumer is faster than the producer?
20. Why does queue length affect system performance?
21. What is queue latency?
22. How can structured data be transferred using a queue?
23. Why must the queue item size match the sent data type?
24. Why should task priorities reflect timing requirements?
25. What advantage does `vTaskDelayUntil()` provide for periodic sensor acquisition?
26. What is queue overflow?
27. How can queue overflow be detected?
28. What are the advantages of a multi-stage data-processing pipeline?
29. When might dropping sensor samples be preferable to blocking?
30. Why are queues important in real-time embedded-system design?

---

## 54. Expected Results

Students should successfully implement:

```text
Sensor
   ↓
Producer Task
   ↓
FreeRTOS Queue
   ↓
Consumer Task
   ↓
Data Processing
```

The producer should acquire data and send each sample into the queue.

The consumer should receive the samples in FIFO order.

For normal operation:

```text
Produced Sample
      ↓
Queue
      ↓
Consumed Sample
      ↓
Processed Result
```

When the producer becomes faster than the consumer:

```text
Queue Occupancy
      ↑
```

until the queue becomes full.

When the consumer is faster than the producer:

```text
Queue Empty
    ↓
Consumer Blocks
```

until new data arrives.

---

## 55. Submission

Submit:

- Source code for the basic producer–consumer experiment.
- Sensor acquisition source code.
- Structured-data queue implementation.
- Serial Monitor screenshots.
- Queue occupancy results.
- Producer-fast/consumer-slow experiment.
- Producer-slow/consumer-fast experiment.
- Queue-full handling results.
- Queue-latency measurements.
- Queue-length comparison.
- Three-stage task-pipeline implementation.
- Brief discussion of observed producer–consumer behavior.

Suggested result table:

| Experiment | Producer Period | Consumer Period | Queue Length | Result |
|---|---:|---:|---:|---|
| 1 | 1000 ms | Immediate | 5 |  |
| 2 | 500 ms | Immediate | 5 |  |
| 3 | 200 ms | 1000 ms | 5 |  |
| 4 | 1000 ms | 100 ms | 5 |  |

Suggested queue study:

| Queue Length | Average Occupancy | Full Events | Average Latency |
|---:|---:|---:|---:|
| 1 |  |  |  |
| 5 |  |  |  |
| 10 |  |  |  |
| 20 |  |  |  |

---

## 56. Discussion

Students should explain how a FreeRTOS queue separates data acquisition from data processing.

The producer is responsible for generating sensor samples:

```text
Acquire
   ↓
Send
```

while the consumer performs:

```text
Receive
   ↓
Process
```

The queue acts as both a **communication mechanism** and a **buffer** between tasks.

The essential architecture can be summarized as:

$$
\boxed{
\text{Producer}
\rightarrow
\text{Queue}
\rightarrow
\text{Consumer}
}
$$

Unlike a shared variable protected by a mutex, a queue explicitly transfers individual data items between tasks. This improves modularity and helps prevent unsafe concurrent access.

Students should also analyze the relationship among producer rate, consumer rate, queue size, and latency. A fast producer can cause the queue to fill, while a fast consumer may spend much of its time blocked waiting for new data.

---

## 57. Conclusion

In this lab, a producer–consumer system is implemented using FreeRTOS queues. A producer task acquires sensor data and places each sample into a queue, while a consumer task receives and processes the data independently.

Students observe FIFO communication, blocking behavior, queue occupancy, overflow, structured-data transfer, and the effects of different producer and consumer rates. The laboratory demonstrates that queues provide a safe and structured mechanism for inter-task communication without requiring direct shared-variable access.

The progression from the previous labs is:

```text
LAB 5
FreeRTOS Tasks
      ↓
Priorities
      ↓
Preemptive Scheduling
      ↓
LAB 6
Shared Resources
      ↓
Race Conditions
      ↓
Mutex Protection
      ↓
LAB 7
Producer–Consumer
      ↓
FreeRTOS Queue
      ↓
Safe Data Transfer
```

The complete RTOS concept can therefore be summarized as:

```text
Schedule Tasks
      ↓
Protect Resources
      ↓
Transfer Data
```

or:

$$
\boxed{
\text{RTOS Multitasking}
= \text{Scheduling}
+
\text{Synchronization}
+
\text{Inter-Task Communication}
}
$$

This prepares students for the next lab on **FreeRTOS software timers and memory management**, where periodic events, heap usage, task memory, and resource constraints can be investigated.
