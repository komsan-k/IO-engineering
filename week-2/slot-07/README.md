# 📘 Theory: Thread-Safe Data Exchange Using Message Queues

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Modern ESP32 applications frequently execute several activities concurrently using **FreeRTOS tasks**. For example, an IoT system may contain independent tasks for:

- sensor acquisition,
- signal processing,
- actuator control,
- display updates,
- Wi-Fi communication,
- MQTT publishing,
- data logging,
- alarm handling.

These tasks often need to exchange data.

For example:

```text
Sensor Task
     │
     │ Temperature Data
     ▼
Processing Task
     │
     │ Processed Result
     ▼
MQTT Task
```

A simple implementation might use a global variable shared between tasks. However, concurrent access to shared variables can introduce **race conditions**, inconsistent data, and synchronization problems.

FreeRTOS provides **message queues** as a structured and thread-safe mechanism for exchanging data between tasks.

The fundamental concept is:

$$
\boxed{
\text{Producer Task}
\rightarrow
\text{Queue}
\rightarrow
\text{Consumer Task}
}
$$

For ESP32 applications, queues are especially useful because they combine:

$$
\boxed{
\text{Data Transfer}
+
\text{Synchronization}
+
\text{Task Blocking}
}
$$

into a single RTOS mechanism.

---

# 🧠 2. Why Do Tasks Need to Exchange Data?

Consider an ESP32 environmental monitoring system:

```text
                 ESP32
                   │
               FreeRTOS
                   │
       ┌───────────┼───────────┐
       ▼           ▼           ▼
 Sensor Task   Display Task   MQTT Task
```

The Sensor Task may acquire:

```text
Temperature = 27.5 °C
Humidity    = 65 %
```

These values may need to be sent to:

```text
Processing Task
Display Task
MQTT Task
```

Therefore, the application requires a safe mechanism for transferring information between independently scheduled tasks.

---

# ⚠️ 3. The Shared-Variable Approach

A simple implementation might use:

```cpp
float temperature;
```

The sensor task writes:

```cpp
temperature = readTemperature();
```

while another task reads:

```cpp
Serial.println(temperature);
```

Conceptually:

```text
Sensor Task ──────┐
                  │ Write
                  ▼
             temperature
                  ▲
                  │ Read
Display Task ─────┘
```

This creates a **shared-memory relationship** between the tasks.

If synchronization is not correctly implemented, problems can occur.

---

# 🚨 4. Problems with Shared Data

Multiple tasks accessing shared data can introduce:

- race conditions,
- inconsistent values,
- lost updates,
- difficult timing dependencies,
- complicated synchronization logic.

The general problem is:

$$
\boxed{
\text{Multiple Tasks}
+
\text{Shared Memory}
+
\text{Unsynchronized Access}
\rightarrow
\text{Possible Race Condition}
}
$$

For simple variables this may appear manageable, but the problem becomes more significant when exchanging structures containing several related values.

---

# 📦 5. What Is a Message Queue?

A **message queue** is an RTOS-managed data structure that allows one execution context to send data while another receives it.

Conceptually:

```text
Producer
   │
   ▼
┌─────────────────────┐
│        Queue        │
│                     │
│ [M1][M2][M3][M4]   │
│                     │
└─────────────────────┘
   │
   ▼
Consumer
```

The producer inserts messages into the queue.

The consumer removes messages from the queue.

Therefore,

$$
\boxed{
\text{Queue} =
\text{Thread-Safe Message Buffer}
}
$$

---

# 🔄 6. Producer-Consumer Model

The queue naturally implements the **producer-consumer model**.

### Producer

Generates data.

Examples:

- sensor task,
- communication receiver,
- ISR,
- ADC acquisition task.

### Consumer

Processes data.

Examples:

- display task,
- MQTT task,
- controller,
- data logger.

The architecture is:

```text
        Producer
           │
           ▼
       Send Data
           │
           ▼
     ┌───────────┐
     │   Queue   │
     └───────────┘
           │
           ▼
      Receive Data
           │
           ▼
        Consumer
```

---

# 🧠 7. Why Queues Are Thread-Safe

FreeRTOS manages the internal queue operations so that multiple tasks can safely interact with the queue through its API.

Instead of application tasks directly modifying the same shared variable:

```text
Task A ─────┐
            ▼
        Shared Data
            ▲
Task B ─────┘
```

the application uses:

```text
Task A
  │
  ▼
Queue API
  │
  ▼
Queue
  │
  ▼
Queue API
  │
  ▼
Task B
```

Thus, the RTOS manages synchronization around the queue operations.

---

# 🔐 8. Shared Memory vs. Message Passing

Two common inter-task communication approaches are:

### Shared Memory

```text
Task A ────► Shared Variable ◄──── Task B
```

### Message Passing

```text
Task A ────► Queue ────► Task B
```

The message-passing approach reduces the need for application code to coordinate direct access to the same data object.

---

# 📊 9. Shared Variables vs. Queues

| Characteristic | Shared Variable | FreeRTOS Queue |
|---|---|---|
| Data transfer | Yes | Yes |
| Built-in synchronization | No | Yes |
| Multiple messages | Difficult | Yes |
| FIFO buffering | No | Yes |
| Task blocking | Requires extra mechanism | Built in |
| Race-condition protection | Developer responsibility | Queue operations managed by RTOS |
| Producer-consumer design | Manual | Natural |
| Data history | Usually latest value only | Multiple queued values |
| Scalability | Limited | Better |

---

# 🧱 10. Queue Structure

A FreeRTOS queue has two important design parameters:

1. **Queue length**
2. **Message size**

Suppose:

$$
N=5
$$

and each message is an integer.

Conceptually:

```text
Queue Length = 5

┌────┬────┬────┬────┬────┐
│ M1 │ M2 │ M3 │ M4 │ M5 │
└────┴────┴────┴────┴────┘
```

The queue can store up to five messages before becoming full.

---

# 🔢 11. Queue Capacity

Let

$$
N_Q
$$

represent the maximum number of messages.

Then the current number of stored messages satisfies:

$$
0\leq N\leq N_Q.
$$

When

$$
N=0,
$$

the queue is empty.

When

$$
N=N_Q,
$$

the queue is full.

---

# 📥 12. FIFO Operation

FreeRTOS queues normally operate according to **First-In, First-Out (FIFO)** ordering when standard send operations are used.

Suppose messages arrive:

```text
10 → 20 → 30
```

The queue becomes:

```text
Front                  Back
  │                      │
  ▼                      ▼
┌────┬────┬────┐
│ 10 │ 20 │ 30 │
└────┴────┴────┘
```

The first value received is:

$$
10.
$$

Then:

$$
20,
$$

followed by:

$$
30.
$$

Thus,

$$
\boxed{
\text{First In}
\rightarrow
\text{First Out}
}
$$

---

# ⚙️ 13. Creating a FreeRTOS Queue

A queue is created using:

```cpp
xQueueCreate()
```

For example:

```cpp
QueueHandle_t sensorQueue;

sensorQueue = xQueueCreate(
    5,
    sizeof(int)
);
```

This creates a queue capable of storing:

$$
5
$$

integer messages.

---

# 🧩 14. Queue Handle

The variable:

```cpp
QueueHandle_t sensorQueue;
```

stores a reference to the queue.

Conceptually:

```text
sensorQueue
     │
     ▼
┌─────────────────────┐
│   FreeRTOS Queue    │
│                     │
│ [ ][ ][ ][ ][ ]     │
└─────────────────────┘
```

Tasks use this handle when sending or receiving messages.

---

# 📤 15. Sending Data to a Queue

A task can send data using:

```cpp
xQueueSend()
```

Example:

```cpp
int sensorValue = 100;

xQueueSend(
    sensorQueue,
    &sensorValue,
    portMAX_DELAY
);
```

Conceptually:

```text
sensorValue
    │
    ▼
xQueueSend()
    │
    ▼
┌───────────────┐
│ Sensor Queue  │
└───────────────┘
```

---

# 📥 16. Receiving Data from a Queue

A consumer can receive data using:

```cpp
xQueueReceive()
```

Example:

```cpp
int receivedValue;

xQueueReceive(
    sensorQueue,
    &receivedValue,
    portMAX_DELAY
);
```

Conceptually:

```text
┌───────────────┐
│ Sensor Queue  │
└───────────────┘
       │
       ▼
xQueueReceive()
       │
       ▼
receivedValue
```

---

# 🧪 17. Basic Producer-Consumer Example

```cpp
QueueHandle_t sensorQueue;

void SensorTask(void *pvParameters)
{
    int value = 0;

    while (1)
    {
        value = analogRead(34);

        xQueueSend(
            sensorQueue,
            &value,
            portMAX_DELAY
        );

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

Consumer:

```cpp
void DisplayTask(void *pvParameters)
{
    int value;

    while (1)
    {
        if (xQueueReceive(
                sensorQueue,
                &value,
                portMAX_DELAY) == pdTRUE)
        {
            Serial.print("Received: ");
            Serial.println(value);
        }
    }
}
```

---

# 🔄 18. Execution Flow

The previous example follows:

```text
Sensor Task
    │
    ▼
analogRead()
    │
    ▼
xQueueSend()
    │
    ▼
┌─────────────┐
│    Queue    │
└─────────────┘
    │
    ▼
xQueueReceive()
    │
    ▼
Display Task
    │
    ▼
Serial Output
```

---

# 💤 19. Blocking on an Empty Queue

Suppose the consumer calls:

```cpp
xQueueReceive(
    sensorQueue,
    &value,
    portMAX_DELAY
);
```

while the queue is empty.

The consumer does not need to repeatedly check the queue.

Instead:

```text
Consumer Task
      │
      ▼
Queue Empty?
      │
     Yes
      │
      ▼
   Blocked
      │
      │ Message Arrives
      ▼
    Ready
      │
      ▼
Receive Message
```

This is much more CPU-efficient than polling.

---

# ⚡ 20. Polling vs. Blocking

A polling implementation might use:

```cpp
while (queueIsEmpty())
{
    // Keep checking
}
```

This consumes CPU time.

With a FreeRTOS queue:

```cpp
xQueueReceive(
    queue,
    &data,
    portMAX_DELAY
);
```

the task can enter the **Blocked** state.

Therefore,

$$
\boxed{
\text{Blocking}
\rightarrow
\text{CPU Available to Other Tasks}
}
$$

---

# 📦 21. Blocking on a Full Queue

The producer can also block if the queue is full.

Suppose:

```text
Queue Capacity = 3

┌────┬────┬────┐
│ M1 │ M2 │ M3 │
└────┴────┴────┘

FULL
```

A producer attempting to send another message may:

- wait,
- timeout,
- or fail immediately,

depending on the configured wait time.

---

# ⏱️ 22. Queue Timeout

Instead of waiting forever:

```cpp
portMAX_DELAY
```

a timeout can be specified:

```cpp
pdMS_TO_TICKS(100)
```

Example:

```cpp
if (xQueueSend(
        sensorQueue,
        &value,
        pdMS_TO_TICKS(100)) == pdTRUE)
{
    Serial.println("Data sent");
}
else
{
    Serial.println("Queue full");
}
```

---

# ⏳ 23. Receive Timeout

Similarly:

```cpp
if (xQueueReceive(
        sensorQueue,
        &value,
        pdMS_TO_TICKS(500)) == pdTRUE)
{
    Serial.println(value);
}
else
{
    Serial.println("No data");
}
```

The task waits for up to:

$$
500~ms.
$$

---

# 📊 24. Queue States

A queue can conceptually exist in three conditions:

### Empty

```text
[ ][ ][ ][ ]
```

### Partially Filled

```text
[M1][M2][ ][ ]
```

### Full

```text
[M1][M2][M3][M4]
```

These conditions affect producer and consumer behavior.

---

# 🧠 25. Queue as Synchronization

A queue does more than store data.

It also synchronizes tasks.

Suppose the consumer waits for data:

```text
Consumer
   │
   ▼
Blocked
```

The producer sends:

```text
Producer
   │
   ▼
xQueueSend()
```

The waiting consumer can then become Ready.

Thus:

$$
\boxed{
\text{Queue Message}
\rightarrow
\text{Data + Scheduling Event}
}
$$

---

# 🔐 26. Thread-Safe Data Exchange

Suppose the producer generates:

```cpp
int value = 123;
```

The queue stores a copy of the message data for ordinary queue-by-value usage.

Conceptually:

```text
Producer Variable
      │
      │ Copy
      ▼
┌─────────────┐
│ Queue Item  │
└─────────────┘
      │
      │ Copy
      ▼
Consumer Variable
```

This reduces direct shared-memory dependency between producer and consumer.

---

# 🧱 27. Sending Structures

Queues can transfer more than integers.

Suppose sensor data contains:

```cpp
typedef struct
{
    float temperature;
    float humidity;
    int light;
} SensorData;
```

A queue can be created using:

```cpp
sensorQueue = xQueueCreate(
    10,
    sizeof(SensorData)
);
```

This allows each queue entry to contain a complete sensor record.

---

# 📡 28. ESP32 Sensor Structure Example

```cpp
typedef struct
{
    float temperature;
    float humidity;
    uint32_t timestamp;
} SensorMessage;
```

Producer:

```cpp
SensorMessage msg;

msg.temperature = 27.5;
msg.humidity = 65.0;
msg.timestamp = millis();

xQueueSend(
    sensorQueue,
    &msg,
    portMAX_DELAY
);
```

Consumer:

```cpp
SensorMessage received;

xQueueReceive(
    sensorQueue,
    &received,
    portMAX_DELAY
);
```

---

# 📊 29. Why Structures Are Useful

Suppose temperature and humidity must represent the same measurement instant.

Using independent global variables:

```cpp
float temperature;
float humidity;
```

may create consistency problems if one value changes while another task is reading them.

A structure groups the values:

```text
SensorMessage
├── Temperature
├── Humidity
└── Timestamp
```

The complete message can then be transferred together.

---

# 🔄 30. Multiple Producers

A queue may receive messages from multiple producers.

```text
Sensor Task A ────┐
                  │
Sensor Task B ────┼────► Queue ─────► Processing Task
                  │
Sensor Task C ────┘
```

For example:

```text
Temperature Sensor
        │
        ▼
      Queue
        ▲
        │
Humidity Sensor
```

The queue serializes message insertion through the RTOS queue API.

---

# 📥 31. Multiple Consumers

Depending on the architecture, multiple tasks may attempt to receive from the same queue:

```text
                 Queue
                /     \
               ▼       ▼
        Consumer A   Consumer B
```

However, a message removed by one consumer is generally no longer available to another consumer.

Therefore, one queue with multiple consumers represents **work distribution**, not automatic broadcasting.

---

# 📢 32. Queue Is Not a Broadcast Mechanism

Suppose:

```text
Message M1
    │
    ▼
  Queue
   / \
  ▼   ▼
 A     B
```

If Task A receives `M1`, Task B normally does not receive the same queue item.

Therefore,

$$
\boxed{
\text{One Queue Message}
\rightarrow
\text{One Receive Operation}
}
$$

For broadcasting, an application may use:

- multiple queues,
- event groups,
- publish-subscribe architecture,
- another appropriate communication mechanism.

---

# 🚦 33. Queue Length Selection

Queue length affects system behavior.

Let:

- $\lambda_p$ = average production rate,
- $\lambda_c$ = average consumption rate.

If:

$$
\lambda_p > \lambda_c
$$

for a sustained period, the queue tends to fill.

If:

$$
\lambda_c \geq \lambda_p,
$$

the consumer can generally keep pace with the producer.

Therefore, queue length should consider:

- message arrival rate,
- consumer processing time,
- burst behavior,
- available RAM.

---

# 🧮 34. Queue Memory Requirement

A simplified estimate of message-storage memory is:

$$
M_Q
\approx
N_Q\times S_M
$$

where:

- $M_Q$ = message-storage memory,
- $N_Q$ = queue length,
- $S_M$ = message size.

For example:

$$
N_Q=10
$$

and

$$
S_M=16~bytes.
$$

Then:

$$
M_Q
\approx
10\times16 =
160~bytes.
$$

Additional RTOS control overhead is also required.

---

# ⚖️ 35. Queue Length Trade-Off

### Queue Too Small

```text
Producer
   │
   ▼
Queue Full
   │
   ▼
Blocked / Message Not Sent
```

### Queue Too Large

```text
Large Queue
    │
    ▼
Higher RAM Usage
```

Therefore:

$$
\boxed{
\text{Queue Length} =
\text{Buffering Requirement}
\leftrightarrow
\text{Memory Cost}
}
$$

---

# 🔢 36. Checking Queue Occupancy

FreeRTOS provides functions that can report queue state.

For example:

```cpp
uxQueueMessagesWaiting(sensorQueue);
```

Conceptually:

```text
Queue

[M1][M2][M3][ ][ ]

Messages Waiting = 3
```

This can be useful for:

- debugging,
- performance analysis,
- congestion monitoring.

---

# 📨 37. Sending to the Front or Back

Queues can support inserting data at different positions.

Common operations include conceptually:

```text
Send to Back
```

and

```text
Send to Front
```

For example:

```text
Existing Queue:

[M1][M2][M3]

Send M4 to Back:

[M1][M2][M3][M4]
```

while sending urgent data to the front could produce:

```text
[M4][M1][M2][M3]
```

The exact API should be selected according to the required message-ordering behavior.

---

# 🔍 38. Peeking at a Queue

Sometimes a task needs to inspect the next message without removing it.

Conceptually:

```text
Before Peek:

[M1][M2][M3]

Read M1

After Peek:

[M1][M2][M3]
```

This differs from receiving:

```text
Before Receive:

[M1][M2][M3]

After Receive:

[M2][M3]
```

FreeRTOS provides queue-peek functionality for this purpose.

---

# 🔄 39. Queue vs. Semaphore

A semaphore primarily represents synchronization or resource availability.

```text
Event
  │
  ▼
Semaphore
  │
  ▼
Task
```

A queue transfers actual message data:

```text
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

Therefore:

$$
\boxed{
\text{Semaphore}
\rightarrow
\text{Synchronization}
}
$$

while

$$
\boxed{
\text{Queue}
\rightarrow
\text{Data Transfer + Synchronization}
}
$$

---

# 🔒 40. Queue vs. Mutex

A mutex protects a shared resource:

```text
Task A
  │
Take Mutex
  │
  ▼
Shared Resource
  │
Give Mutex
```

A queue avoids direct sharing by transferring messages:

```text
Task A
  │
  ▼
Queue
  │
  ▼
Task B
```

Thus:

$$
\boxed{
\text{Mutex}
\rightarrow
\text{Protect Shared State}
}
$$

$$
\boxed{
\text{Queue}
\rightarrow
\text{Exchange Messages}
}
$$

---

# 📊 41. Queue vs. Mutex vs. Semaphore

| Mechanism | Main Purpose | Transfers Data? | Ownership? | Typical Use |
|---|---|---:|---:|---|
| Queue | Message exchange | Yes | No | Sensor → processing |
| Binary semaphore | Event signaling | No | No | ISR → task |
| Counting semaphore | Count events/resources | No | No | Resource pool |
| Mutex | Resource protection | No | Yes | Shared I²C bus |

---

# 🧠 42. Message Passing vs. Locking

Suppose a sensor task produces information for a controller.

### Shared Memory

```text
Sensor Task
     │
     ▼
Shared Structure
     ▲
     │
Controller Task
```

This may require a mutex.

### Message Passing

```text
Sensor Task
     │
     ▼
    Queue
     │
     ▼
Controller Task
```

The second design can reduce direct coupling between the tasks.

---

# 🔬 43. ESP32 Multicore Advantage

Some ESP32 devices can execute tasks on different CPU cores.

```text
Core 0                     Core 1
  │                           │
Sensor Task             Processing Task
  │                           ▲
  └────────► Queue ───────────┘
```

The queue provides a structured communication mechanism even when tasks may execute simultaneously.

Therefore:

$$
\boxed{
\text{Multicore Tasks}
+
\text{Message Queue}
\rightarrow
\text{Controlled Data Exchange}
}
$$

---

# ⚠️ 44. Passing Pointers Through Queues

Queues can also contain pointers, but this requires careful design.

For example:

```cpp
SensorData *ptr;
```

If a pointer is placed into a queue:

```text
Producer
   │
   ▼
Pointer
   │
   ▼
Queue
   │
   ▼
Consumer
```

the underlying data may still be shared.

Questions then arise:

- Who owns the memory?
- Who may modify it?
- Who frees it?
- Is it still valid when received?

Therefore:

$$
\boxed{
\text{Queueing a Pointer}
\neq
\text{Automatically Eliminating Shared-Memory Problems}
}
$$

For introductory ESP32 applications, passing small values or structures by value is often simpler.

---

# 💾 45. Copy-by-Value Trade-Off

Copying data into a queue improves separation but introduces copying overhead.

For small messages:

```text
int
float
small struct
```

this overhead is usually manageable.

For very large data:

```text
Large image
Audio buffer
Large network packet
```

copying may become inefficient.

Thus:

$$
\boxed{
\text{Small Messages}
\rightarrow
\text{Queue by Value}
}
$$

while larger systems may require carefully designed buffer ownership or zero-copy strategies.

---

# ⚡ 46. Queue from an Interrupt

A hardware interrupt may need to communicate with a FreeRTOS task.

Conceptually:

```text
GPIO / Timer / ADC Event
          │
          ▼
         ISR
          │
          ▼
      Queue Message
          │
          ▼
      Worker Task
```

FreeRTOS provides ISR-safe queue operations specifically for interrupt context.

The architecture follows:

$$
\boxed{
\text{ISR}
\rightarrow
\text{Short Event Capture}
\rightarrow
\text{Queue}
\rightarrow
\text{Task Processing}
}
$$

---

# 🚨 47. ISR Design Principle

Avoid performing large amounts of processing inside the ISR.

Instead:

```text
Interrupt
    │
    ▼
Read Essential Data
    │
    ▼
Send to Queue
    │
    ▼
Return from ISR
```

Then:

```text
Worker Task
    │
    ▼
Receive Queue
    │
    ▼
Process Data
```

This keeps interrupt latency low.

---

# 🧪 48. ESP32 Button Event Example

Architecture:

```text
Push Button
    │
    ▼
GPIO Interrupt
    │
    ▼
ISR
    │
    ▼
Event Queue
    │
    ▼
Button Task
    │
    ▼
Process Event
```

A queue can be useful when the event contains information such as:

```cpp
typedef struct
{
    int pin;
    uint32_t time;
} ButtonEvent;
```

Unlike a simple binary semaphore, the queue can carry the event data itself.

---

# 📡 49. Sensor-to-MQTT Architecture

A common IoT design is:

```text
Sensor
   │
   ▼
Sensor Task
   │
   ▼
Sensor Queue
   │
   ▼
MQTT Task
   │
   ▼
Wi-Fi
   │
   ▼
MQTT Broker
```

The Sensor Task does not need to perform network communication directly.

This improves modularity.

---

# 🖥️ 50. Sensor-to-Display Architecture

```text
Temperature Sensor
       │
       ▼
   Sensor Task
       │
       ▼
      Queue
       │
       ▼
  Display Task
       │
       ▼
      OLED
```

The sensor acquisition rate and display-update rate can be managed independently.

---

# 🎛️ 51. Sensor-to-Control Architecture

For a control application:

```text
Sensor
  │
  ▼
Acquisition Task
  │
  ▼
Queue
  │
  ▼
Control Task
  │
  ▼
PWM Output
  │
  ▼
Actuator
```

The queue separates:

- data acquisition,
- control computation,
- actuator interface.

---

# 🚦 52. Smart Traffic ESP32 Example

Consider a smart traffic controller.

```text
Vehicle Sensor
      │
      ▼
 Sensor Task
      │
      ▼
Vehicle Queue
      │
      ▼
Traffic FSM Task
      │
      ▼
Traffic Lights
```

The sensor task can generate messages such as:

```cpp
typedef struct
{
    int lane;
    int vehicleCount;
    uint32_t timestamp;
} TrafficMessage;
```

The traffic-control task receives these messages and updates the FSM.

---

# 🧠 53. Multiple Sensor Architecture

Consider:

```text
Temperature Task ─────┐
                      │
Humidity Task ─────────┼────► Sensor Queue
                      │
Light Task ────────────┘
```

Each message may contain a type field:

```cpp
typedef struct
{
    int sensorID;
    float value;
    uint32_t timestamp;
} SensorMessage;
```

The consumer can determine the message source using:

```cpp
sensorID
```

---

# 📋 54. Message Design

A good message should contain enough information for the consumer to process it independently.

Example:

```cpp
typedef struct
{
    uint8_t sensorID;
    float value;
    uint32_t timestamp;
} SensorMessage;
```

Conceptually:

```text
Message
├── Source ID
├── Value
└── Timestamp
```

Additional fields may include:

- status,
- sequence number,
- error code,
- message type.

---

# 🔢 55. Sequence Numbers

A sequence number can help detect lost or reordered application-level messages.

Example:

```cpp
typedef struct
{
    uint32_t sequence;
    float temperature;
} SensorMessage;
```

Messages:

```text
100
101
102
104
```

The missing:

```text
103
```

may indicate that a message was not processed or recorded as expected.

---

# ⏱️ 56. Timestamps

Timestamps help identify when data was generated.

```cpp
msg.timestamp = millis();
```

For example:

```text
Temperature = 27.5 °C
Timestamp   = 12500 ms
```

This is especially useful when queue buffering introduces a delay between data production and consumption.

---

# 📊 57. Queue Latency

Let:

- $t_s$ = time message is sent,
- $t_r$ = time message is received.

Queue waiting latency can be defined as:

$$
T_Q=t_r-t_s.
$$

For example:

$$
t_s=100~ms
$$

and

$$
t_r=115~ms.
$$

Then:

$$
T_Q=15~ms.
$$

This metric can be useful in real-time system analysis.

---

# 📈 58. Queue Throughput

If $N$ messages are processed during time interval $T$, throughput can be estimated as:

$$
R_Q=\frac{N}{T}.
$$

For example, if:

$$
N=1000
$$

messages are processed in:

$$
T=10~s,
$$

then:

$$
R_Q=100~\text{messages/s}.
$$

---

# ⚠️ 59. Queue Overflow

If producers generate data faster than the consumer can process it:

$$
R_P>R_C,
$$

the queue may eventually become full.

```text
Producer →→→→→→ Queue → Consumer
                 │
                 ▼
                FULL
```

Possible consequences include:

- producer blocking,
- send timeout,
- rejected messages,
- increased latency.

---

# 🛠️ 60. Handling Queue Overflow

Possible strategies include:

- increase queue length,
- increase consumer priority,
- reduce production rate,
- improve consumer processing speed,
- discard stale data where appropriate,
- use an overwrite-style design for latest-value applications.

The correct strategy depends on whether every message is important.

---

# 🔄 61. Event Stream vs. Latest Value

This distinction is important.

### Event Stream

Every event matters.

Example:

```text
Button presses
Packets
Commands
Transactions
```

A queue is highly suitable because messages should be preserved in order.

### Latest Value

Only the newest measurement matters.

Example:

```text
Current temperature
Current motor speed
Current battery voltage
```

Storing a long history in a queue may not always be necessary.

System architecture should reflect the data semantics.

---

# 🔐 62. Queue and Data Ownership

For ordinary by-value messages:

```text
Producer owns local data
       │
       ▼
Queue receives copy
       │
       ▼
Consumer receives copy
```

This provides clear ownership boundaries.

The producer can continue modifying its local variable without changing the already queued copy.

---

# 🧩 63. Task Decoupling

Without a queue:

```text
Sensor Task
     │
     ▼
Directly calls network code
     │
     ▼
Wi-Fi delays sensor task
```

With a queue:

```text
Sensor Task
     │
     ▼
Queue
     │
     ▼
Network Task
```

The tasks become more independent.

This property is called **decoupling**.

---

# 🧠 64. Temporal Decoupling

Producer and consumer do not necessarily have to execute at the same instant.

```text
Producer runs
    │
    ▼
Message stored
    │
    ▼
Producer continues

       Later...

Consumer runs
    │
    ▼
Message received
```

Therefore, a queue provides **temporal decoupling** between tasks.

---

# 🏗️ 65. Modular ESP32 Architecture

A queue-based system can be organized as:

```text
                 ESP32
                   │
               FreeRTOS
                   │
     ┌─────────────┼──────────────┐
     ▼             ▼              ▼
 Sensor Task   Control Task    MQTT Task
     │             ▲              ▲
     ▼             │              │
 Sensor Queue ─────┘              │
                                  │
 Result Queue ────────────────────┘
```

Each task has a clear responsibility.

---

# 🆚 66. Super Loop vs. Queue-Based RTOS

### Super Loop

```text
Read Sensor
    │
    ▼
Process
    │
    ▼
Display
    │
    ▼
MQTT
    │
    ▼
Repeat
```

### Queue-Based RTOS

```text
Sensor Task ──► Queue ──► Processing Task
                              │
                              ▼
                            Queue
                           /     \
                          ▼       ▼
                    Display    MQTT
```

The second architecture supports greater modularity and concurrency.

---

# 📊 67. Design Comparison

| Feature | Shared Global Data | Queue-Based Design |
|---|---|---|
| Coupling | Higher | Lower |
| Synchronization | Manual | Integrated |
| Buffering | Manual | Built in |
| Blocking | Separate mechanism | Built in |
| Message ordering | Manual | FIFO support |
| Debugging | Can be difficult | Structured |
| Multitasking scalability | Moderate | Good |
| Producer-consumer model | Manual | Natural |

---

# ⚠️ 68. Common Queue Programming Errors

Common mistakes include:

1. Creating the wrong queue item size.
2. Using the wrong data type during receive.
3. Ignoring queue-creation failure.
4. Using a queue that has not been created.
5. Choosing an insufficient queue length.
6. Blocking indefinitely where a timeout is required.
7. Sending messages faster than they can be consumed.
8. Passing pointers without defining memory ownership.
9. Using one queue when true broadcasting is required.
10. Performing too much processing in an ISR.
11. Ignoring return values from queue APIs.
12. Assuming a queue automatically solves every shared-memory issue.

---

# 🛡️ 69. Checking Queue Creation

Queue creation should be checked.

```cpp
sensorQueue = xQueueCreate(
    10,
    sizeof(SensorMessage)
);

if (sensorQueue == NULL)
{
    Serial.println("Queue creation failed");
}
```

This improves system robustness.

---

# 🔍 70. Checking Send Results

Instead of assuming success:

```cpp
xQueueSend(queue, &data, 0);
```

check:

```cpp
if (xQueueSend(queue, &data, 0) == pdTRUE)
{
    // Message accepted
}
else
{
    // Queue full
}
```

---

# 🔍 71. Checking Receive Results

Similarly:

```cpp
if (xQueueReceive(
        queue,
        &data,
        pdMS_TO_TICKS(100)) == pdTRUE)
{
    // Valid message received
}
else
{
    // Timeout
}
```

This allows predictable error handling.

---

# 🧠 72. Queue Design Procedure

A practical design flow is:

```text
Identify Producer
       │
       ▼
Identify Consumer
       │
       ▼
Define Message
       │
       ▼
Determine Queue Length
       │
       ▼
Determine Blocking Policy
       │
       ▼
Assign Task Priorities
       │
       ▼
Implement
       │
       ▼
Measure Queue Behavior
```

---

# 📋 73. Queue Design Table

Before implementation, students can define:

| Queue | Producer | Consumer | Message | Length | Timeout |
|---|---|---|---|---:|---|
| SensorQueue | Sensor Task | Processing Task | `SensorData` | 10 | 100 ms |
| MQTTQueue | Processing Task | MQTT Task | `MQTTMessage` | 5 | 500 ms |
| EventQueue | ISR | Control Task | `Event` | 10 | Infinite receive |

This makes the communication architecture explicit.

---

# 📊 74. Monitoring Queue Performance

Useful metrics include:

- queue occupancy,
- maximum occupancy,
- send failures,
- receive timeouts,
- message latency,
- throughput,
- producer rate,
- consumer rate.

A conceptual monitoring table is:

| Metric | Meaning |
|---|---|
| Occupancy | Current queued messages |
| Peak occupancy | Maximum observed queue depth |
| Send failures | Messages not accepted |
| Receive timeout | Consumer received no message |
| Latency | Send-to-receive delay |
| Throughput | Messages processed per second |

---

# 🧪 75. Complete ESP32 Queue Example

```cpp
#include <Arduino.h>

QueueHandle_t sensorQueue;

void SensorTask(void *pvParameters)
{
    int sensorValue;

    while (1)
    {
        sensorValue = analogRead(34);

        if (xQueueSend(
                sensorQueue,
                &sensorValue,
                pdMS_TO_TICKS(100)) == pdTRUE)
        {
            Serial.println("Sensor data sent");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ProcessingTask(void *pvParameters)
{
    int receivedValue;

    while (1)
    {
        if (xQueueReceive(
                sensorQueue,
                &receivedValue,
                portMAX_DELAY) == pdTRUE)
        {
            Serial.print("Received = ");
            Serial.println(receivedValue);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    sensorQueue = xQueueCreate(
        10,
        sizeof(int)
    );

    if (sensorQueue == NULL)
    {
        Serial.println("Queue creation failed");
        return;
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

void loop()
{
}
```

---

# 🔄 76. Program Operation

The program executes conceptually as:

```text
ESP32 Starts
    │
    ▼
Create Queue
    │
    ▼
Create Sensor Task
    │
    ▼
Create Processing Task
    │
    ▼
FreeRTOS Scheduler
    │
    ├───────────────┐
    ▼               ▼
Sensor Task    Processing Task
    │               │
analogRead()        Wait
    │               │
    ▼               │
xQueueSend() ───────┘
                    │
                    ▼
              xQueueReceive()
                    │
                    ▼
                 Process
```

---

# 🚀 77. Advanced ESP32 IoT Architecture

A more complete application could use:

```text
                   ESP32
                     │
                 FreeRTOS
                     │
        ┌────────────┼─────────────┐
        ▼            ▼             ▼
  Sensor Task   Control Task   Network Task
        │            ▲             ▲
        ▼            │             │
   SensorQueue ──────┘             │
                                   │
        ControlQueue ──────────────┘
                                   │
                                   ▼
                              MQTT Broker
```

Each task exchanges structured messages rather than directly modifying unrelated task state.

---

# 💬 78. Discussion Questions

1. What is a message queue?
2. Why are queues useful in FreeRTOS?
3. What is the producer-consumer model?
4. Why can global variables cause race conditions?
5. How does a queue reduce direct shared-memory access?
6. What does thread-safe data exchange mean?
7. What is FIFO?
8. What happens when a consumer reads an empty queue?
9. What happens when a producer writes to a full queue?
10. Why are blocking queues CPU-efficient?
11. What is a queue timeout?
12. Can queues transfer structures?
13. What is the difference between a queue and semaphore?
14. What is the difference between a queue and mutex?
15. Can several producers use the same queue?
16. What happens when several consumers share one queue?
17. Why is a queue not automatically a broadcast mechanism?
18. What determines the required queue length?
19. Why can passing pointers through a queue be dangerous?
20. Why should ISR processing remain short?

---

# 🧠 79. Exercise 1 — Basic Queue

Create a queue with:

$$
N=5
$$

entries where each entry is an integer.

Write the required:

```cpp
QueueHandle_t
```

and:

```cpp
xQueueCreate()
```

statements.

---

# 📤 80. Exercise 2 — Producer

Create a task that generates:

```cpp
int value;
```

every:

$$
500~ms
$$

and sends it to a queue.

Architecture:

```text
Producer
   │
   ▼
Queue
```

---

# 📥 81. Exercise 3 — Consumer

Create another task that:

1. waits for a message,
2. receives the integer,
3. prints it using Serial.

Architecture:

```text
Queue
  │
  ▼
Consumer
  │
  ▼
Serial
```

---

# 🧱 82. Exercise 4 — Structured Message

Define:

```cpp
typedef struct
{
    float temperature;
    float humidity;
    uint32_t timestamp;
} SensorData;
```

Create a queue that stores:

$$
10
$$

`SensorData` messages.

---

# ⏱️ 83. Exercise 5 — Queue Latency

A message is sent at:

$$
t_s=120~ms
$$

and received at:

$$
t_r=145~ms.
$$

Calculate:

$$
T_Q=t_r-t_s.
$$

---

# 📈 84. Exercise 6 — Queue Throughput

A system processes:

$$
500
$$

messages during:

$$
5~s.
$$

Calculate:

$$
R_Q=\frac{N}{T}.
$$

---

# 💾 85. Exercise 7 — Queue Memory

A queue contains:

$$
N_Q=20
$$

messages.

Each message requires:

$$
S_M=12~bytes.
$$

Estimate:

$$
M_Q=N_QS_M.
$$

Ignore RTOS control overhead.

---

# 🚦 86. Exercise 8 — Smart Traffic Queue

Design:

```text
Vehicle Sensor
      │
      ▼
 Sensor Task
      │
      ▼
Traffic Queue
      │
      ▼
Traffic FSM
```

Use:

```cpp
typedef struct
{
    uint8_t lane;
    uint16_t vehicleCount;
    uint32_t timestamp;
} TrafficMessage;
```

Determine:

- producer,
- consumer,
- message size,
- suitable queue length,
- send timeout,
- receive timeout.

---

# 🔬 87. Practical ESP32 Exercise

Design an application with:

```text
TaskSensor
TaskProcessing
TaskDisplay
TaskMQTT
```

Use queues to connect the tasks.

Complete:

| Queue | Producer | Consumer | Data | Length |
|---|---|---|---|---:|
| Sensor Queue | | | | |
| Result Queue | | | | |
| MQTT Queue | | | | |

Then draw the complete data-flow architecture.

---

# 🚀 88. Advanced Exercise

Design an ESP32 monitoring system:

```text
MPU6050
   │
   ▼
IMU Task
   │
   ▼
IMU Queue
   │
   ▼
Processing Task
   │
   ├──────────► Display Queue ──► Display Task
   │
   └──────────► MQTT Queue ─────► MQTT Task
```

Requirements:

1. IMU sampling at 100 Hz.
2. Processing task calculates derived motion information.
3. Display updates at 5 Hz.
4. MQTT publishes at 1 Hz.
5. No direct global-data exchange between these tasks.
6. Queue overflow must be handled.
7. Message timestamps must be included.
8. Queue latency should be measured.

Students should determine:

- message structures,
- queue lengths,
- task priorities,
- blocking policies,
- expected data rates.

---

# 🧾 89. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain thread-safe inter-task communication,
- describe FreeRTOS message queues,
- explain producer-consumer architecture,
- distinguish shared memory from message passing,
- create queues on ESP32,
- send and receive queue messages,
- explain FIFO operation,
- use blocking queue operations,
- configure queue timeouts,
- transfer structured data,
- understand multiple-producer architectures,
- understand multiple-consumer behavior,
- distinguish queues from semaphores and mutexes,
- explain queue capacity and memory trade-offs,
- analyze queue overflow,
- estimate queue latency and throughput,
- explain data ownership,
- identify pointer-sharing risks,
- use queues in ISR-to-task architectures,
- design modular ESP32 FreeRTOS applications.

---

# 📘 90. References

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

# 🔑 91. Key Concepts

### Message Queue

$$
\boxed{
\text{Producer}
\rightarrow
\text{Queue}
\rightarrow
\text{Consumer}
}
$$

### Thread-Safe Data Exchange

$$
\boxed{
\text{Task A}
\rightarrow
\text{RTOS Queue}
\rightarrow
\text{Task B}
}
$$

### Queue Capacity

$$
\boxed{
0\leq N\leq N_Q
}
$$

### FIFO

$$
\boxed{
\text{First In}
\rightarrow
\text{First Out}
}
$$

### Queue Memory

$$
\boxed{
M_Q\approx N_QS_M
}
$$

### Queue Latency

$$
\boxed{
T_Q=t_r-t_s
}
$$

### Queue Throughput

$$
\boxed{
R_Q=\frac{N}{T}
}
$$

### Queue vs. Semaphore

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
\text{Data + Synchronization}
}
$$

### Queue vs. Mutex

$$
\boxed{
\text{Mutex}
\rightarrow
\text{Protect Shared Resource}
}
$$

$$
\boxed{
\text{Queue}
\rightarrow
\text{Transfer Messages}
}
$$

### ESP32 Queue Architecture

$$
\boxed{
\text{Sensor}
\rightarrow
\text{Producer Task}
\rightarrow
\text{Queue}
\rightarrow
\text{Consumer Task}
\rightarrow
\text{Application}
}
$$

FreeRTOS message queues provide an effective mechanism for **thread-safe data exchange in ESP32 systems**. Rather than allowing tasks to depend heavily on directly shared global variables, queue-based message passing transfers data through an RTOS-managed communication channel. This approach combines **buffering, synchronization, blocking, and data transfer**, making queues particularly useful for scalable sensor, control, IoT, and real-time embedded applications.
