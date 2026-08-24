# 📘 Theory: Real-Time ML Inference on MCUs  
## Latency, Execution Efficiency, and Memory Management — ESP32 MCU Case Study

---

## 🧩 1. Introduction

**Real-time machine-learning inference on microcontrollers** combines embedded-system constraints with machine-learning computation. Unlike cloud AI, an MCU such as the **ESP32** must acquire sensor data, preprocess it, execute the ML model, and produce a decision within limited:

- CPU performance,
- RAM,
- flash memory,
- power,
- execution time.

A typical real-time TinyML system is:

```text
Physical Environment
        │
        ▼
      Sensor
        │
        ▼
      ESP32
        │
        ├── Sampling
        ├── Preprocessing
        ├── Feature Extraction
        ├── ML Inference
        └── Decision
                │
                ▼
          Actuator / Network
```

The primary design requirement is not simply high ML accuracy. The system must produce the correct result **within the required time constraint**.

$$
\boxed{
\text{Real-Time TinyML} =
\text{Accurate Inference}
+
\text{Timing Constraint}
+
\text{Efficient Execution}
+
\text{Controlled Memory}
}
$$

---

# 🧠 2. What Is ML Inference?

Machine-learning development consists of two major stages:

```text
TRAINING

Dataset
   │
   ▼
Learning Algorithm
   │
   ▼
Trained Model
```

and:

```text
INFERENCE

New Sensor Data
       │
       ▼
Trained Model
       │
       ▼
Prediction
```

Training is normally performed on a workstation or cloud system.

The ESP32 typically performs **inference**.

Mathematically:

$$
\hat{y}=f(\mathbf{x};\theta)
$$

where:

- $\mathbf{x}$ = input feature vector,
- $\theta$ = trained model parameters,
- $f(\cdot)$ = model,
- $\hat{y}$ = predicted output.

---

# ⏱️ 3. What Does Real-Time Mean?

A real-time system must produce its result before a required **deadline**.

If:

$$
T_{\text{response}}\le T_{\text{deadline}}
$$

the timing requirement is satisfied.

For example, if an activity-recognition system must update every:

$$
100~\text{ms}
$$

then:

$$
T_{\text{response}}\le100~\text{ms}
$$

should normally be maintained.

Real-time therefore does **not necessarily mean extremely fast**.

It means:

$$
\boxed{
\text{Correct Result}
+
\text{Correct Time}
}
$$

---

# ⚡ 4. Hard vs. Soft Real-Time Systems

Real-time systems can broadly be divided into two categories.

| Type | Deadline Behavior | Example |
|---|---|---|
| Hard real-time | Missing deadline is unacceptable | Critical motor protection |
| Soft real-time | Occasional deadline miss may be tolerated | Activity recognition |
| Best effort | No strict timing guarantee | Background analytics |

Many ESP32 TinyML applications are **soft real-time systems**.

Examples include:

- gesture recognition,
- activity classification,
- environmental monitoring,
- keyword detection,
- predictive maintenance.

---

# 🔄 5. Real-Time ML Processing Pipeline

A practical inference cycle contains more than the neural-network execution itself.

```text
Sensor Sampling
      │
      ▼
Data Buffering
      │
      ▼
Preprocessing
      │
      ▼
Feature Extraction
      │
      ▼
Model Inference
      │
      ▼
Postprocessing
      │
      ▼
Decision
      │
      ▼
Output
```

Therefore, total processing latency can be represented as:

$$
T_{\text{total}} =
T_{\text{sample}}
+
T_{\text{pre}}
+
T_{\text{feature}}
+
T_{\text{infer}}
+
T_{\text{post}}
+
T_{\text{output}}
$$

---

# ⏱️ 6. Inference Latency

**Inference latency** is the time required to execute one model inference.

$$
\boxed{
T_{\text{infer}} =
t_{\text{end}}-t_{\text{start}}
}
$$

Example:

```cpp
unsigned long startTime = micros();

interpreter->Invoke();

unsigned long endTime = micros();

Serial.print("Inference time = ");
Serial.print(endTime - startTime);
Serial.println(" us");
```

If:

```text
Start = 10,000 µs
End   = 14,500 µs
```

then:

$$
T_{\text{infer}} =
14,500-10,000 =
4,500~\mu s
$$

or:

$$
T_{\text{infer}}=4.5~\text{ms}
$$

---

# 📊 7. Latency vs. Throughput

Latency and throughput are related but different.

### Latency

Time required for one inference:

$$
T_{\text{infer}}
$$

### Throughput

Number of inferences that can be executed per second.

For a simple sequential inference loop:

$$
R_{\text{infer}}
\approx
\frac{1}{T_{\text{infer}}}
$$

If:

$$
T_{\text{infer}}=10~\text{ms}
$$

then the theoretical model-only throughput is approximately:

$$
R_{\text{infer}} =
\frac{1}{0.01} =
100
$$

inferences/s.

Actual application throughput can be lower because sampling, preprocessing, communication, and scheduling also consume time.

---

# 📏 8. Sampling Period and Deadline

Suppose a sensor operates at:

$$
f_s=100~\text{Hz}
$$

The sampling period is:

$$
T_s=\frac{1}{f_s}
$$

Therefore:

$$
T_s=
\frac{1}{100} =
0.01~\text{s}
$$

or:

$$
T_s=10~\text{ms}
$$

If processing must occur for every sample, the relevant computation should generally complete within the available timing budget.

---

# 🪟 9. Window-Based Inference

Many TinyML systems do not classify every individual sensor sample.

Instead, multiple samples are grouped into a window.

For:

$$
f_s=100~\text{Hz}
$$

and:

$$
T_w=1~\text{s}
$$

the number of samples is:

$$
N=f_sT_w=100
$$

The processing structure becomes:

```text
Sensor Samples
      │
      ▼
 ┌───────────┐
 │  Window   │
 │ 100 Samples
 └───────────┘
      │
      ▼
Feature Extraction
      │
      ▼
ML Inference
```

---

# 🔄 10. Sliding-Window Inference

Windows may overlap:

```text
Sensor Stream
────────────────────────────────────►

Window 1
|---------------|

        Window 2
        |---------------|

                Window 3
                |---------------|
```

If a prediction is generated every:

$$
T_h
$$

seconds, where $T_h$ is the hop interval, then the inference rate is approximately:

$$
f_{\text{prediction}} =
\frac{1}{T_h}
$$

---

# 🚦 11. End-to-End Response Time

Model latency alone does not determine application responsiveness.

Suppose:

$$
T_{\text{sample}}=20~\text{ms}
$$

$$
T_{\text{pre}}=2~\text{ms}
$$

$$
T_{\text{feature}}=3~\text{ms}
$$

$$
T_{\text{infer}}=8~\text{ms}
$$

$$
T_{\text{post}}=1~\text{ms}
$$

Then:

$$
T_{\text{total}} =
20+2+3+8+1
$$

$$
T_{\text{total}} =
34~\text{ms}
$$

This is more meaningful than reporting only:

$$
T_{\text{infer}}=8~\text{ms}
$$

---

# 📉 12. Jitter

Real-time systems should also consider **timing variation**.

If inference times are:

```text
Run 1 = 5.1 ms
Run 2 = 5.2 ms
Run 3 = 7.8 ms
Run 4 = 5.0 ms
Run 5 = 6.4 ms
```

the average latency does not describe the complete timing behavior.

A simple range-based jitter measure is:

$$
J=
T_{\max}-T_{\min}
$$

However, real-time evaluation should preferably examine the latency distribution and worst-case behavior rather than relying only on one jitter definition.

---

# 🎯 13. Worst-Case Execution Time

For deadline-sensitive systems, average execution time is insufficient.

A practical measured approximation is:

$$
T_{\text{max}} =
\max
\{
T_1,T_2,\ldots,T_N
\}
$$

A timing requirement may be expressed as:

$$
T_{\text{max}}
<
T_{\text{deadline}}
$$

True **worst-case execution-time analysis** can require more rigorous methods than simply observing the maximum measured value.

---

# ⚙️ 14. Execution Efficiency

Execution efficiency concerns how effectively the MCU uses available computing resources.

Important factors include:

- model architecture,
- numerical precision,
- operator implementation,
- memory access,
- feature-extraction complexity,
- task scheduling,
- communication overhead.

Conceptually:

$$
\boxed{
\text{Execution Efficiency} =
\frac{\text{Useful ML Computation}}
{\text{Available Processing Resources}}
}
$$

---

# 🧮 15. Multiply-Accumulate Operations

Neural networks rely heavily on **multiply-accumulate (MAC)** operations:

$$
y =
\sum_{i=1}^{N}w_ix_i+b
$$

Each term requires multiplication and accumulation.

For a fully connected layer:

$$
N_{\text{MAC}}
\approx
N_{\text{in}}N_{\text{out}}
$$

For:

$$
N_{\text{in}}=64
$$

and:

$$
N_{\text{out}}=32
$$

approximately:

$$
64\times32=2048
$$

MAC operations are required.

---

# 🔢 16. INT8 Execution

Model quantization can convert suitable FP32 models into INT8 models.

```text
FP32
 │
 ▼
Quantization
 │
 ▼
INT8
```

Potential advantages include:

- reduced model storage,
- reduced memory bandwidth,
- efficient integer arithmetic,
- potentially reduced inference latency.

A simplified affine quantization model is:

$$
r=S(q-Z)
$$

where:

- $r$ = real value,
- $q$ = integer value,
- $S$ = scale,
- $Z$ = zero-point.

---

# ⚡ 17. Quantization and Execution Efficiency

FP32 parameter storage requires approximately:

$$
4N_p
$$

bytes.

INT8 parameter storage requires approximately:

$$
N_p
$$

bytes.

Thus, parameter storage can theoretically decrease by approximately:

$$
4\times
$$

although complete model and runtime memory reductions depend on model structure and implementation.

---

# 🪶 18. Reduce Model Complexity

A direct way to improve inference performance is to reduce the model itself.

Possible techniques include:

- fewer layers,
- fewer neurons,
- fewer convolution filters,
- smaller input dimensions,
- feature selection,
- INT8 quantization,
- appropriate pruning.

For a dense layer:

$$
N_p =
N_{\text{in}}N_{\text{out}}
+
N_{\text{out}}
$$

Reducing either dimension reduces parameter count.

---

# 🎯 19. Feature Extraction and Execution Efficiency

Suppose raw sensor input contains:

$$
300
$$

values.

Feature extraction converts this to:

$$
9
$$

features.

```text
300 Raw Values
      │
      ▼
Feature Extraction
      │
      ▼
9 Features
      │
      ▼
Small ANN
```

This can significantly reduce neural-network complexity.

However, feature extraction itself also consumes CPU time.

Therefore:

$$
T_{\text{total}} =
T_{\text{feature}}
+
T_{\text{infer}}
+\cdots
$$

must still be evaluated.

---

# 💾 20. Memory Management

Memory is one of the most important constraints in MCU-based ML.

A simplified ESP32 application may use:

```text
Memory
  │
  ├── Program / Model Storage
  │
  └── RAM
       │
       ├── Stack
       ├── Heap
       ├── Global Variables
       ├── Sensor Buffers
       └── ML Tensor Arena
```

The exact memory organization varies among ESP32 family members.

---

# 📦 21. Flash vs. RAM

### Flash

Typically stores:

- firmware,
- constants,
- model data,
- static resources.

### RAM

Typically stores:

- variables,
- stacks,
- heaps,
- sensor buffers,
- intermediate tensors,
- runtime state.

A model fitting in flash does **not** guarantee that it can execute successfully.

The runtime must also fit in available RAM.

---

# 🧠 22. Tensor Arena

TensorFlow Lite for Microcontrollers commonly uses a statically allocated **tensor arena**.

Example:

```cpp
constexpr int kTensorArenaSize = 60 * 1024;

static uint8_t tensor_arena[kTensorArenaSize];
```

The tensor arena provides working memory for inference.

```text
Tensor Arena
     │
     ├── Input Tensor
     ├── Output Tensor
     ├── Activations
     └── Intermediate Buffers
```

---

# 📐 23. Memory Budget

A conceptual RAM budget is:

$$
M_{\text{RAM}} =
M_{\text{tensor}}
+
M_{\text{sensor}}
+
M_{\text{stack}}
+
M_{\text{heap}}
+
M_{\text{application}}
$$

The design must satisfy:

$$
M_{\text{RAM}}
<
M_{\text{available}}
$$

with suitable safety margin.

---

# ⚠️ 24. Dynamic Memory Allocation

Dynamic allocation uses functions or mechanisms such as:

```cpp
malloc()
```

```cpp
new
```

and corresponding deallocation.

Repeated dynamic allocation can introduce concerns such as:

- fragmentation,
- allocation failure,
- variable execution time,
- difficult memory debugging.

These characteristics may be undesirable in long-running real-time embedded applications.

---

# 🧱 25. Static Memory Allocation

Static allocation reserves memory before runtime use.

Example:

```cpp
static float sensorBuffer[100];
```

and:

```cpp
static uint8_t tensorArena[60 * 1024];
```

Advantages include:

- predictable memory use,
- reduced fragmentation risk,
- deterministic allocation behavior.

For real-time TinyML, static or carefully bounded memory allocation is often preferred.

---

# 📊 26. Sensor Buffer

For:

$$
N=100
$$

samples with three accelerometer axes:

$$
N_{\text{values}}=100\times3=300
$$

If each sample uses a 32-bit float:

$$
M=
300\times4
$$

$$
M=1200~\text{bytes}
$$

A simple buffer is:

```cpp
float accelBuffer[100][3];
```

---

# 🔄 27. Circular Buffer

Streaming systems can use a **circular buffer**.

```text
┌─────────────────────────────┐
│ 0 │ 1 │ 2 │ ... │ N-2 │ N-1│
└─────────────────────────────┘
  ▲                         │
  └─────────────────────────┘
```

When the final position is reached, writing returns to the beginning.

A write index can be updated as:

$$
i_{\text{next}} =
(i+1)\bmod N
$$

This avoids repeatedly shifting an entire sample array.

---

# ⚡ 28. Why Circular Buffers Improve Efficiency

A less efficient approach may shift data:

```cpp
for (int i = 0; i < N - 1; i++) {
    buffer[i] = buffer[i + 1];
}
```

This requires:

$$
O(N)
$$

data movement for each update.

A circular-buffer index update is approximately:

$$
O(1)
$$

for inserting a new sample.

---

# 🧵 29. Real-Time Tasks on ESP32

ESP32 development commonly uses FreeRTOS-based task scheduling.

A TinyML application can be decomposed into:

```text
Task 1
Sensor Acquisition
       │
       ▼
Task 2
Preprocessing
       │
       ▼
Task 3
ML Inference
       │
       ▼
Task 4
Communication
```

This separation can improve software organization and scheduling control.

---

# 🚦 30. Task Priorities

Different operations have different timing requirements.

| Task | Priority | Function |
|---|---:|---|
| Sensor acquisition | High | Maintain sampling |
| ML inference | High/medium | Produce prediction |
| Control | High | Respond to prediction |
| Communication | Medium | Send results |
| Logging | Low | Diagnostic output |

Priorities should be assigned according to application timing requirements rather than arbitrarily.

---

# ⚠️ 31. Blocking Operations

Blocking code can delay real-time tasks.

Example:

```cpp
delay(1000);
```

During this delay, the calling task does no useful application work.

Similarly:

```cpp
while (!client.connected()) {
    // wait indefinitely
}
```

may delay processing.

A better architecture separates timing-critical processing from potentially slow network operations.

---

# 📡 32. Communication vs. ML Inference

Consider:

```text
Sensor
   │
   ▼
ML Inference
   │
   ▼
Prediction
   │
   ▼
Wi-Fi / MQTT
```

If MQTT transmission blocks for:

$$
500~\text{ms}
$$

while the ML deadline is:

$$
100~\text{ms}
$$

a single sequential task may miss the deadline.

A better architecture can be:

```text
           ┌── ML Task
Sensor ────┤
           └── Communication Task
```

with queues or other synchronization mechanisms between tasks.

---

# 📬 33. Message Queues

FreeRTOS queues provide thread-safe communication.

```text
ML Task
   │
   │ Prediction
   ▼
 Queue
   │
   ▼
Communication Task
```

Example concept:

```cpp
xQueueSend(predictionQueue, &prediction, 0);
```

The inference task can continue without directly performing all communication work.

---

# 🔐 34. Shared Data and Race Conditions

Suppose:

```text
Sensor Task ──────┐
                  ▼
              Shared Buffer
                  ▲
Inference Task ───┘
```

If both tasks access the buffer simultaneously, inconsistent data can occur.

Solutions include:

- mutexes,
- critical sections,
- queues,
- double buffering.

---

# 🔁 35. Double Buffering

Double buffering uses two data buffers:

```text
Buffer A → Sensor Writing

Buffer B → ML Processing
```

Then:

```text
Buffer A → ML Processing

Buffer B → Sensor Writing
```

This can allow acquisition and processing to overlap while reducing data conflicts.

---

# ⚡ 36. Pipelined Execution

Instead of:

```text
Sample → Process → Infer → Communicate → Sample
```

a pipelined system can perform:

```text
Time ─────────────────────────────────────►

Sensor:      [W1] [W2] [W3] [W4]

Inference:        [I1] [I2] [I3]

Network:               [N1] [N2]
```

where:

- `W` = window acquisition,
- `I` = inference,
- `N` = network operation.

This improves system-level concurrency.

---

# 🧮 37. CPU Utilization

For a periodic task with execution time:

$$
C_i
$$

and period:

$$
T_i
$$

a simple utilization estimate is:

$$
U_i=\frac{C_i}{T_i}
$$

For several periodic tasks:

$$
U=
\sum_{i=1}^{n}
\frac{C_i}{T_i}
$$

Example:

| Task | Execution | Period | Utilization |
|---|---:|---:|---:|
| Sampling | 1 ms | 10 ms | 0.10 |
| Inference | 8 ms | 50 ms | 0.16 |
| Communication | 5 ms | 100 ms | 0.05 |

Therefore:

$$
U=0.10+0.16+0.05=0.31
$$

or:

$$
31\%
$$

This is a useful first estimate, although actual schedulability depends on scheduling policy, priorities, blocking, interrupts, and other overhead.

---

# ⏲️ 38. Hardware Timers

Hardware timers can support periodic sensor acquisition.

Conceptually:

```text
Hardware Timer
      │
      ▼
Timer Event
      │
      ▼
Sampling Trigger
```

For:

$$
f_s=100~\text{Hz}
$$

the required period is:

$$
T_s=10~\text{ms}
$$

Timers can provide more controlled sampling than large blocking delays.

---

# ⚠️ 39. Keep ISRs Short

An interrupt service routine should generally perform only essential work.

Avoid performing:

- ML inference,
- network communication,
- large loops,
- complex feature extraction,

directly inside a time-critical ISR.

A preferred pattern is:

```text
Timer Interrupt
      │
      ▼
Set Flag / Notify Task
      │
      ▼
Exit ISR
      │
      ▼
Normal Task Performs Processing
```

---

# 🧠 40. TensorFlow Lite for Microcontrollers

A simplified TFLM architecture is:

```text
Application
    │
    ▼
TFLM Interpreter
    │
    ▼
Operator Kernels
    │
    ▼
Tensor Arena
    │
    ▼
ESP32 Hardware
```

Typical workflow:

```text
Load Model
    │
    ▼
Register Operators
    │
    ▼
Allocate Tensors
    │
    ▼
Prepare Input
    │
    ▼
Invoke()
    │
    ▼
Read Output
```

---

# 🚀 41. TFLM Initialization

Initialization should generally occur once during application startup.

Conceptually:

```cpp
void setup() {
    initializeModel();
    allocateTensors();
}
```

rather than repeatedly inside:

```cpp
void loop()
```

Repeated model initialization can waste processing time and complicate memory behavior.

---

# 📥 42. Input Processing

A real-time inference cycle can be:

```text
Sensor
   │
   ▼
Feature Extraction
   │
   ▼
Normalization
   │
   ▼
Quantization
   │
   ▼
Input Tensor
```

The same preprocessing used during model development should be reproduced on the ESP32.

$$
\boxed{
\text{Training Preprocessing} =
\text{Deployment Preprocessing}
}
$$

---

# 🔢 43. INT8 Input Conversion

For an INT8 input tensor:

$$
q= round( \frac{x}{S})+Z
$$

The result should be clipped to:

$$
[-128,127]
$$

before being stored as an `int8_t`.

Example concept:

```cpp
int32_t q = round(x / scale) + zeroPoint;

q = constrain(q, -128, 127);

input->data.int8[i] = (int8_t)q;
```

---

# 📤 44. Output Processing

For classification:

```text
Model Output
    │
    ▼
Class Scores
    │
    ▼
Find Maximum
    │
    ▼
Predicted Class
```

Mathematically:

$$
\hat{c} =
\arg\max_i y_i
$$

---

# 🧮 45. Efficient Argmax

Instead of unnecessary sorting, the largest output can be found with a single pass:

```cpp
int bestClass = 0;

for (int i = 1; i < NUM_CLASSES; i++) {
    if (output[i] > output[bestClass]) {
        bestClass = i;
    }
}
```

Complexity:

$$
O(N)
$$

No complete sort is required.

---

# 🔋 46. Execution Efficiency and Energy

Energy per inference can be approximated as:

$$
E_{\text{infer}} =
VI_{\text{avg}}T_{\text{infer}}
$$

Reducing execution time may reduce energy per inference, although total device energy also depends on:

- sensor operation,
- radio activity,
- idle periods,
- sleep states.

---

# 💤 47. Duty-Cycled Inference

An MCU does not always need to remain fully active.

```text
Sleep
  │
  ▼
Wake
  │
  ▼
Collect Data
  │
  ▼
Run Inference
  │
  ▼
Take Action
  │
  ▼
Sleep
```

The active duty cycle can be approximated by:

$$
D=
\frac{T_{\text{active}}}
{T_{\text{total}}}
$$

---

# 📊 48. Average Current

For active and sleep states:

$$
I_{\text{avg}} =
D I_{\text{active}}
+
(1-D)I_{\text{sleep}}
$$

Reducing both inference time and radio-active time can improve battery life.

---

# 🌡️ 49. Thermal and Sustained Workload Considerations

Continuous ML inference increases processor activity.

For always-on systems, designers should consider:

- clock frequency,
- CPU utilization,
- radio activity,
- environmental temperature,
- sustained power consumption.

The fastest possible execution mode is not necessarily the best system-level operating point.

---

# 🧪 50. Profiling

Optimization should begin with measurement.

Useful quantities include:

```text
Sampling Time
Preprocessing Time
Feature Extraction Time
Inference Time
Postprocessing Time
Communication Time
```

Example:

```cpp
uint32_t t0 = micros();

readSensors();

uint32_t t1 = micros();

extractFeatures();

uint32_t t2 = micros();

interpreter->Invoke();

uint32_t t3 = micros();
```

Then:

$$
T_{\text{sensor}}=t_1-t_0
$$

$$
T_{\text{feature}}=t_2-t_1
$$

$$
T_{\text{infer}}=t_3-t_2
$$

---

# 📊 51. Timing Profile Table

| Stage | Time |
|---|---:|
| Sensor acquisition | |
| Preprocessing | |
| Feature extraction | |
| Quantization | |
| Inference | |
| Postprocessing | |
| Communication | |
| **Total** | |

---

# 🔍 52. Bottleneck Analysis

Suppose:

```text
Sensor         2 ms
Features      25 ms
Inference      5 ms
Communication 50 ms
```

Optimizing inference from:

$$
5~\text{ms}\rightarrow3~\text{ms}
$$

provides only a small total improvement.

The main bottlenecks are feature extraction and communication.

Therefore:

$$
\boxed{
\text{Profile First}
\rightarrow
\text{Optimize Bottleneck}
}
$$

---

# 🧠 53. Memory Profiling

Students should measure:

- model size,
- tensor-arena size,
- sensor-buffer size,
- free heap,
- minimum free heap during execution,
- stack usage where available.

A simple ESP32 Arduino example is:

```cpp
Serial.print("Free heap: ");
Serial.println(ESP.getFreeHeap());
```

Memory should be observed during sustained operation, not only immediately after boot.

---

# ⚠️ 54. Memory Leak

A memory leak occurs when allocated memory is not properly released or becomes unreachable.

```text
Start
Free RAM = 200 kB

After 1 hour
Free RAM = 180 kB

After 5 hours
Free RAM = 120 kB

Eventually
Allocation Failure
```

Long-duration testing is therefore important for embedded AI systems.

---

# 📉 55. Fragmentation

Repeated allocation and deallocation can produce fragmented free memory:

```text
RAM

[Used][Free][Used][Free][Used][Free]
```

Even if total free memory appears sufficient, a large contiguous allocation may fail.

Static buffers and controlled allocation strategies can reduce this risk.

---

# 📚 56. Memory Reuse

Temporary buffers can sometimes be reused when their lifetimes do not overlap.

Instead of:

```text
Buffer A → Preprocessing
Buffer B → Features
Buffer C → Temporary Result
```

a carefully designed system may reuse memory:

```text
Shared Workspace
      │
      ├── Preprocessing
      ├── Feature Calculation
      └── Temporary Processing
```

This requires careful analysis to avoid overwriting data still in use.

---

# ⚡ 57. Avoid Unnecessary Data Copies

Consider:

```text
Sensor Buffer
     │ copy
     ▼
Processing Buffer
     │ copy
     ▼
ML Input Buffer
```

Repeated copying consumes:

- CPU cycles,
- memory bandwidth,
- additional RAM.

Where safe and supported, processing data in place or writing directly into the intended buffer can improve efficiency.

---

# 📐 58. Data-Type Selection

Using an unnecessarily large data type wastes memory.

For 1,000 values:

### `float`

$$
1000\times4=4000~\text{bytes}
$$

### `int16_t`

$$
1000\times2=2000~\text{bytes}
$$

### `int8_t`

$$
1000\times1=1000~\text{bytes}
$$

The selected type must still provide sufficient range and precision.

---

# 🔢 59. Fixed-Point and Integer Processing

Some preprocessing operations can potentially use integer arithmetic instead of floating point.

```text
Floating-Point Pipeline

Sensor → float → float features → FP model
```

versus:

```text
Integer Pipeline

Sensor → integer → fixed/integer features → INT8 model
```

Integer processing may improve efficiency, but scaling, overflow, precision, and numerical behavior must be carefully managed.

---

# 🧠 60. Model Optimization Hierarchy

A useful optimization sequence is:

```text
1. Measure Baseline
       │
       ▼
2. Reduce Input Complexity
       │
       ▼
3. Simplify Model
       │
       ▼
4. Quantize Model
       │
       ▼
5. Optimize Memory
       │
       ▼
6. Optimize Scheduling
       │
       ▼
7. Measure Again
```

Optimization should be evidence-based.

---

# 🚶 61. Case Study: Real-Time Activity Recognition

Consider:

```text
MPU6050
   │
   ▼
ESP32
   │
   ▼
100-Hz Sampling
   │
   ▼
1-Second Window
   │
   ▼
Feature Extraction
   │
   ▼
INT8 ANN
   │
   ▼
Standing / Walking / Running
```

Assume:

$$
f_s=100~\text{Hz}
$$

and a prediction is required every:

$$
250~\text{ms}
$$

The system must continuously collect samples while producing predictions at approximately:

$$
4~\text{predictions/s}
$$

---

# 📊 62. Example Timing Budget

Suppose the prediction deadline is:

$$
T_D=250~\text{ms}
$$

Measured processing times are:

| Operation | Time |
|---|---:|
| Window management | 2 ms |
| Feature extraction | 8 ms |
| Quantization | 1 ms |
| TFLM inference | 12 ms |
| Postprocessing | 1 ms |
| Queue operation | 1 ms |

Total processing:

$$
T_P =
2+8+1+12+1+1
$$

$$
T_P=25~\text{ms}
$$

Timing margin:

$$
T_M=T_D-T_P
$$

$$
T_M=250-25=225~\text{ms}
$$

---

# 📈 63. Deadline Utilization

Define the fraction of the deadline consumed by processing as:

$$
U_D=
\frac{T_P}{T_D}
$$

For the previous example:

$$
U_D=
\frac{25}{250} =
0.10
$$

or:

$$
10\%
$$

This leaves substantial timing margin for other system operations.

---

# 📡 64. Separate Communication from Inference

A robust architecture is:

```text
                    ┌───────────────┐
                    │ Sensor Task   │
                    └───────┬───────┘
                            │
                            ▼
                         Buffer
                            │
                            ▼
                    ┌───────────────┐
                    │ TinyML Task   │
                    └───────┬───────┘
                            │
                            ▼
                          Queue
                            │
                            ▼
                    ┌───────────────┐
                    │ MQTT / Wi-Fi  │
                    │     Task      │
                    └───────────────┘
```

Network delays are therefore less likely to directly stall the inference task.

---

# 🔄 65. FreeRTOS Architecture Example

Conceptually:

```cpp
void sensorTask(void *parameter) {
    while (true) {
        readSensor();
        updateBuffer();
        vTaskDelay(...);
    }
}

void mlTask(void *parameter) {
    while (true) {
        if (windowReady()) {
            extractFeatures();
            runInference();
        }
    }
}

void networkTask(void *parameter) {
    while (true) {
        sendPrediction();
    }
}
```

In a practical system, tasks should use queues, notifications, timers, or semaphores rather than uncontrolled busy waiting.

---

# 🎯 66. Event-Driven Inference

Not every sample needs to trigger full ML processing.

```text
Sensor
   │
   ▼
Simple Threshold
   │
   ├── No Event → Continue
   │
   └── Event
         │
         ▼
      ML Inference
```

This can reduce:

- CPU utilization,
- energy consumption,
- unnecessary inference operations.

---

# 🧠 67. Cascaded Intelligence

A two-stage architecture can be:

```text
Sensor
   │
   ▼
Simple Detector
   │
   ▼
Possible Event?
   │
   ├── No → Sleep / Continue
   │
   └── Yes
         │
         ▼
     TinyML Model
         │
         ▼
   Detailed Classification
```

This is useful when important events are relatively rare.

---

# 📊 68. Real-Time Accuracy

A model may have high offline accuracy but poor real-world performance because of:

- delayed sampling,
- dropped samples,
- inconsistent preprocessing,
- buffer corruption,
- timing jitter,
- sensor noise,
- deployment-domain differences.

Therefore:

$$
\boxed{
\text{Offline Accuracy}
\neq
\text{Complete Real-Time Performance}
}
$$

---

# 🧪 69. On-Device Evaluation

| Category | Metric |
|---|---|
| ML | Accuracy |
| ML | Precision |
| ML | Recall |
| ML | F1-score |
| Timing | Mean inference latency |
| Timing | Maximum measured latency |
| Timing | Deadline misses |
| Memory | Flash usage |
| Memory | RAM usage |
| Memory | Tensor arena |
| System | CPU utilization |
| Energy | Energy/inference |

---

# 🚨 70. Deadline Miss Rate

If:

$$
N_D
$$

predictions are evaluated and:

$$
N_M
$$

miss their deadlines, define:

$$
R_M=
\frac{N_M}{N_D}
$$

For:

$$
N_D=10,000
$$

and:

$$
N_M=20
$$

then:

$$
R_M=
\frac{20}{10000} =
0.002
$$

or:

$$
0.2\%
$$

For a hard real-time requirement, even a very small deadline-miss rate may be unacceptable.

---

# 🔬 71. Lab-Style Performance Experiment

Students can compare:

```text
Configuration A
FP32 Model

Configuration B
INT8 Model

Configuration C
INT8 + Optimized Buffering

Configuration D
INT8 + FreeRTOS Task Separation
```

Record:

| Configuration | Accuracy | Latency | RAM | Deadline Misses |
|---|---:|---:|---:|---:|
| FP32 | | | | |
| INT8 | | | | |
| INT8 + Buffer Optimization | | | | |
| INT8 + Task Optimization | | | | |

---

# 🧪 72. Exercise 1 — Calculate Latency

Given:

```text
Preprocessing      = 3 ms
Feature extraction = 8 ms
Inference          = 12 ms
Postprocessing     = 2 ms
```

Calculate:

$$
T_{\text{processing}}
$$

If the deadline is:

$$
50~\text{ms}
$$

determine whether the system meets the processing deadline.

---

# 🧪 73. Exercise 2 — Calculate Throughput

If:

$$
T_{\text{infer}}=8~\text{ms}
$$

calculate:

$$
R_{\text{infer}} =
\frac{1}{T_{\text{infer}}}
$$

---

# 🧪 74. Exercise 3 — Calculate RAM

Given:

```text
Tensor arena      = 70 kB
Sensor buffer     = 8 kB
Application data  = 15 kB
Other runtime RAM = 25 kB
```

estimate:

$$
M_{\text{RAM}}
$$

and discuss why safety margin is necessary.

---

# 🧪 75. Exercise 4 — Circular Buffer

Implement a circular buffer for:

$$
N=100
$$

accelerometer samples.

Use:

$$
i_{\text{next}} =
(i+1)\bmod100
$$

Compare its update time with an implementation that shifts all 100 samples.

---

# 🧪 76. Exercise 5 — Measure ESP32 Inference

Run:

```cpp
uint32_t t0 = micros();

interpreter->Invoke();

uint32_t t1 = micros();

Serial.println(t1 - t0);
```

Repeat:

$$
N=1000
$$

times.

Calculate:

- minimum latency,
- maximum latency,
- mean latency,
- standard deviation.

Mean:

$$
\bar{T} =
\frac{1}{N}
\sum_{i=1}^{N}T_i
$$

Standard deviation:

$$
\sigma_T =
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
(T_i-\bar{T})^2
}
$$

---

# 🧪 77. Exercise 6 — Deadline Testing

Set:

$$
T_D=20~\text{ms}
$$

For every inference:

```cpp
if (inferenceTime > deadline) {
    deadlineMisses++;
}
```

Calculate:

$$
R_M=
\frac{N_M}{N_D}\times100\%
$$

---

# 🧪 78. Exercise 7 — Memory Stability

Record free heap every minute for one hour.

| Time | Free Heap |
|---:|---:|
| 0 min | |
| 10 min | |
| 20 min | |
| 30 min | |
| 40 min | |
| 50 min | |
| 60 min | |

A continuous downward trend may indicate a memory-management problem requiring investigation.

---

# 🚀 79. Advanced Exercise — Complete Real-Time TinyML System

Develop:

```text
MPU6050
   │
   ▼
ESP32
   │
   ├── Sensor Task
   │
   ▼
Circular Buffer
   │
   ▼
Feature Extraction
   │
   ▼
INT8 TFLM
   │
   ▼
Prediction Queue
   │
   ├──────────────► LED / Actuator
   │
   └──────────────► MQTT Task
```

Measure:

1. sampling rate,
2. feature-extraction time,
3. inference latency,
4. total processing latency,
5. RAM usage,
6. model size,
7. deadline misses,
8. classification accuracy.

---

# 🪞 80. Real-Time TinyML and Digital Twins

A real-time digital-twin endpoint can be:

```text
Physical System
      │
      ▼
Sensor
      │
      ▼
ESP32
      │
      ▼
Real-Time TinyML
      │
      ▼
State Estimation
      │
      ▼
MQTT
      │
      ▼
Digital Twin
```

The digital twin receives interpreted states such as:

```json
{
  "device": "motor-01",
  "state": "abnormal",
  "confidence": 0.96,
  "inference_ms": 8.4
}
```

---

# 📡 81. Edge Intelligence Architecture

```text
Physical Sensor
      │
      ▼
ESP32 TinyML
      │
      ├── Immediate Local Decision
      │
      ▼
Edge Gateway
      │
      ▼
Cloud / Digital Twin
```

The ESP32 handles time-sensitive inference while higher-level systems perform:

- visualization,
- long-term storage,
- fleet analysis,
- model management.

---

# 🔑 82. Optimization Principles

A practical optimization strategy can be summarized as:

$$
\boxed{
\text{Measure}
\rightarrow
\text{Identify Bottleneck}
\rightarrow
\text{Optimize}
\rightarrow
\text{Measure Again}
}
$$

Useful rules include:

1. Avoid optimizing code that is not a bottleneck.
2. Measure complete end-to-end latency.
3. Consider worst observed latency, not only averages.
4. Minimize unnecessary data copying.
5. Prefer predictable memory allocation.
6. Separate network operations from timing-critical inference.
7. Use efficient buffering.
8. Quantize models when appropriate.
9. Simplify models before adding unnecessary complexity.
10. Test continuously on the actual MCU.

---

# 💬 83. Discussion Questions

1. What is real-time ML inference?
2. How does inference differ from training?
3. What does real-time mean in embedded systems?
4. What is a deadline?
5. What is the difference between hard and soft real time?
6. What is inference latency?
7. What is end-to-end latency?
8. How does throughput differ from latency?
9. What is timing jitter?
10. Why is average latency insufficient?
11. What is worst-case execution time?
12. What is a deadline miss?
13. How can deadline-miss rate be calculated?
14. Why does INT8 quantization help MCU inference?
15. What is a MAC operation?
16. How does model complexity affect latency?
17. How can feature extraction reduce model size?
18. Why must feature-extraction latency also be measured?
19. What is the difference between flash and RAM?
20. What is a tensor arena?
21. Why is static allocation useful?
22. What problems can dynamic allocation cause?
23. What is memory fragmentation?
24. What is a memory leak?
25. What is a circular buffer?
26. Why is a circular buffer efficient?
27. What is double buffering?
28. What is pipelined processing?
29. Why should ML inference usually not execute inside an ISR?
30. How can FreeRTOS improve system organization?
31. What is a message queue?
32. How can queues separate ML and network operations?
33. What is a race condition?
34. Why can Wi-Fi affect real-time performance?
35. What is CPU utilization?
36. What is duty-cycled inference?
37. How does inference latency affect energy?
38. Why should ML models be profiled on the target ESP32?
39. What metrics should be measured for real-time TinyML?
40. Why is offline accuracy insufficient for evaluating a real-time embedded AI system?

---

# 🧾 84. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain real-time ML inference,
- distinguish training from inference,
- explain hard and soft real-time systems,
- define timing deadlines,
- calculate inference latency,
- calculate end-to-end processing latency,
- distinguish latency and throughput,
- explain timing jitter,
- evaluate maximum measured latency,
- calculate deadline-miss rates,
- explain execution efficiency,
- calculate basic MAC requirements,
- explain INT8 inference,
- identify ML execution bottlenecks,
- profile an ESP32 TinyML application,
- distinguish flash and RAM requirements,
- explain tensor-arena memory,
- create a memory budget,
- explain static and dynamic allocation,
- identify fragmentation and memory leaks,
- implement circular buffers,
- explain double buffering,
- design pipelined ML processing,
- organize ML applications using FreeRTOS tasks,
- use queues for inter-task communication,
- avoid blocking operations in timing-critical tasks,
- explain why ISRs should remain short,
- measure CPU utilization,
- calculate duty cycle,
- estimate inference energy,
- measure memory stability,
- evaluate real-time TinyML systems using accuracy and timing metrics.

---

# 🔑 85. Key Concepts

### Real-Time Requirement

$$
\boxed{
T_{\text{response}}
\le
T_{\text{deadline}}
}
$$

### End-to-End Processing Time

$$
\boxed{
T_{\text{total}} =
T_{\text{sample}}
+
T_{\text{pre}}
+
T_{\text{feature}}
+
T_{\text{infer}}
+
T_{\text{post}}
+
T_{\text{output}}
}
$$

### Inference Latency

$$
\boxed{
T_{\text{infer}} =
t_{\text{end}} -
t_{\text{start}}
}
$$

### Throughput

$$
\boxed{
R_{\text{infer}}
\approx
\frac{1}{T_{\text{infer}}}
}
$$

### Sampling Period

$$
\boxed{
T_s=\frac{1}{f_s}
}
$$

### Periodic CPU Utilization

$$
\boxed{
U=
\sum_i
\frac{C_i}{T_i}
}
$$

### RAM Budget

$$
\boxed{
M_{\text{RAM}} =
M_{\text{tensor}}
+
M_{\text{buffer}}
+
M_{\text{stack}}
+
M_{\text{heap}}
+
M_{\text{application}}
}
$$

### Circular Buffer

$$
\boxed{
i_{\text{next}} =
(i+1)\bmod N
}
$$

### Deadline Miss Rate

$$
\boxed{
R_M =
\frac{N_M}{N_D}
}
$$

### Energy per Inference

$$
\boxed{
E_{\text{infer}}
\approx
VI_{\text{avg}}T_{\text{infer}}
}
$$

---

# 🎯 86. Summary

Real-time machine-learning inference on an ESP32 requires much more than deploying a trained neural network. The complete embedded system must acquire data, preprocess sensor measurements, execute inference, generate decisions, and potentially communicate results while satisfying timing and memory constraints.

The central requirement is:

$$
\boxed{
T_{\text{response}}
\le
T_{\text{deadline}}
}
$$

Real-time TinyML performance therefore depends on:

```text
Sensor Acquisition
       │
       ▼
Efficient Buffering
       │
       ▼
Feature Extraction
       │
       ▼
Optimized ML Model
       │
       ▼
Efficient Inference
       │
       ▼
Real-Time Decision
```

Three major design areas are particularly important:

$$
\boxed{
\text{Real-Time TinyML} =
\text{Latency}
+
\text{Execution Efficiency}
+
\text{Memory Management}
}
$$

**Latency** should be evaluated at both model and system levels. Reporting only neural-network inference time may hide delays caused by feature extraction, communication, buffering, or task scheduling.

**Execution efficiency** can be improved through:

- compact neural-network architectures,
- INT8 quantization,
- efficient feature extraction,
- reduced data copying,
- appropriate data types,
- task separation,
- event-driven processing.

**Memory management** is equally important. Predictable embedded systems benefit from:

- bounded memory use,
- appropriately sized tensor arenas,
- static buffers,
- circular buffering,
- careful task stacks,
- avoidance of memory leaks and fragmentation.

A practical engineering workflow is therefore:

$$
\boxed{
\text{Profile}
\rightarrow
\text{Identify Bottleneck}
\rightarrow
\text{Optimize}
\rightarrow
\text{Validate on ESP32}
}
$$

The final TinyML system should be evaluated using multiple dimensions:

$$
\boxed{
\text{System Quality} =
f(
\text{Accuracy},
\text{Latency},
\text{Memory},
\text{Deadline Reliability},
\text{Energy}
)
}
$$

These concepts provide the foundation for developing real-time **activity recognition, predictive maintenance, wearable intelligence, anomaly detection, intelligent control, smart sensing, and digital-twin endpoints** using ESP32-class microcontrollers.

---

# 📘 References

1. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*, O'Reilly Media, 2019.  
2. R. David et al., “TensorFlow Lite Micro: Embedded Machine Learning for TinyML Systems,” *Proceedings of Machine Learning and Systems*, vol. 3, pp. 800–811, 2021.  
3. TensorFlow, *TensorFlow Lite for Microcontrollers Documentation*.  
4. Espressif Systems, *ESP32 Series Datasheets and Technical Reference Manuals*.  
5. Espressif Systems, *ESP-IDF Programming Guide*.  
6. Espressif Systems, *ESP-DL User Guide and Documentation*.  
7. FreeRTOS, *FreeRTOS Kernel Documentation*.  
8. J. Lin et al., “MCUNet: Tiny Deep Learning on IoT Devices,” *Advances in Neural Information Processing Systems*, 2020.  
9. B. Jacob et al., “Quantization and Training of Neural Networks for Efficient Integer-Arithmetic-Only Inference,” *IEEE/CVF Conference on Computer Vision and Pattern Recognition*, 2018.  
10. J. L. Hennessy and D. A. Patterson, *Computer Architecture: A Quantitative Approach*, Morgan Kaufmann.  
11. J. W. S. Liu, *Real-Time Systems*, Prentice Hall.  
12. G. C. Buttazzo, *Hard Real-Time Computing Systems: Predictable Scheduling Algorithms and Applications*, Springer.

---

