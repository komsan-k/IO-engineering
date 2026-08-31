# LAB 15 — Real-Time Edge Inference & Gesture-Controlled Actuation

## 1. Objective

The objective of this lab is to:

1. Run a trained TinyML gesture-recognition model directly on the ESP32.
2. Acquire live MPU6050 accelerometer and gyroscope data.
3. Build fixed-length IMU windows for real-time inference.
4. Apply the same normalization used during model training.
5. Feed live sensor data into the TinyML model.
6. Perform gesture classification continuously on the MCU.
7. Interpret model output probabilities or confidence scores.
8. Apply a confidence threshold before accepting a prediction.
9. Control LEDs according to predicted gesture classes.
10. Measure inference latency and system responsiveness.
11. Compare continuous and overlapping inference windows.
12. Evaluate the reliability of live edge inference.
13. Design a complete sensor-to-decision-to-actuation pipeline.

---

## 2. Background

In the previous lab, a TinyML gesture-recognition model was trained, optimized, quantized, and prepared for deployment.

This lab focuses on **real-time edge inference**.

The complete system becomes:

```text
Physical Gesture
      ↓
MPU6050
      ↓
Live IMU Sampling
      ↓
Windowing
      ↓
Normalization
      ↓
TinyML Model
      ↓
Gesture Classification
      ↓
Decision Logic
      ↓
LED Control
```

The important difference is that the model now processes **live sensor data** instead of stored test samples.

The MCU therefore performs:

```text
Sense
  ↓
Process
  ↓
Infer
  ↓
Decide
  ↓
Act
```

without requiring cloud-based processing.

---

## 3. Edge Intelligence Architecture

The system architecture is:

```text
                Human Gesture
                     ↓
                  MPU6050
                     ↓
             ax, ay, az, gx, gy, gz
                     ↓
                 ESP32 MCU
                     ↓
            Fixed-Length Window
                     ↓
                Normalization
                     ↓
              TinyML Inference
                     ↓
            Predicted Gesture
                     ↓
             Confidence Check
                     ↓
               Decision Logic
                     ↓
                  LEDs
```

This is an example of a **closed-loop intelligent embedded system**.

---

## 4. Hardware & Software Requirements

### Hardware

- ESP32 development board
- MPU6050 IMU
- Breadboard
- Jumper wires
- Two or more LEDs
- Current-limiting resistors
- USB cable

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- MPU6050 library
- TensorFlow Lite for Microcontrollers or compatible embedded inference runtime
- Trained gesture model from LAB 14

### Required Files

The firmware should include:

```text
gesture_model.h
normalization parameters
class labels
```

The model header contains the quantized or floating-point TinyML model.

---

## 5. Gesture Classes and LED Actions

Assume the following gesture classes:

| Class | Gesture | LED Action |
|---:|---|---|
| 0 | Stationary | No change |
| 1 | Left | LED 1 ON |
| 2 | Right | LED 2 ON |
| 3 | Up | Both LEDs ON |
| 4 | Down | Both LEDs OFF |

The mapping can be represented as:

```text
LEFT
 ↓
LED 1 ON

RIGHT
 ↓
LED 2 ON

UP
 ↓
LED 1 + LED 2 ON

DOWN
 ↓
LED 1 + LED 2 OFF

STATIONARY
 ↓
No Change
```

---

## 6. Real-Time Input Window

The model developed in LAB 14 expects:

```text
100 samples × 6 IMU channels
```

Therefore:

$$
\mathbf{X}
\in
\mathbb{R}^{100\times6}.
$$

If the sampling frequency is:

$$
f_s=50~\text{Hz},
$$

then the duration of one inference window is:

$$
T_w
= \frac{N}{f_s}.
$$

For:

$$
N=100,
$$

we obtain:

$$
T_w
= \frac{100}{50}
=
2~\text{s}.
$$

Therefore, each classification initially uses approximately 2 seconds of sensor data.

---

## 7. Experiment 1 — Connect the LEDs

Example ESP32 pins:

```cpp
#define LED1 25
#define LED2 26
```

Initialize:

```cpp
pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);

digitalWrite(LED1, LOW);
digitalWrite(LED2, LOW);
```

The output subsystem is:

```text
ESP32 GPIO 25
      ↓
    LED 1

ESP32 GPIO 26
      ↓
    LED 2
```

Use suitable current-limiting resistors with external LEDs.

---

## 8. Experiment 2 — Verify the MPU6050

Initialize the MPU6050:

```cpp
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {

  Serial.begin(115200);

  Wire.begin();

  if (!mpu.begin()) {

    Serial.println(
      "MPU6050 not found"
    );

    while (1) {
      delay(10);
    }
  }

  Serial.println(
    "MPU6050 ready"
  );
}
```

Verify that live sensor readings are available before integrating the TinyML model.

---

## 9. Experiment 3 — Create the Live IMU Buffer

Define:

```cpp
#define WINDOW_SIZE 100
#define NUM_FEATURES 6
```

Create a buffer:

```cpp
float imuWindow[
    WINDOW_SIZE
][
    NUM_FEATURES
];
```

The six channels are:

```text
0 → ax
1 → ay
2 → az
3 → gx
4 → gy
5 → gz
```

The buffer therefore stores:

```text
Sample 1   → ax ay az gx gy gz
Sample 2   → ax ay az gx gy gz
...
Sample 100 → ax ay az gx gy gz
```

---

## 10. Experiment 4 — Live IMU Sampling

Use a sampling interval of:

```cpp
#define SAMPLE_INTERVAL_MS 20
```

This approximately corresponds to:

$$
f_s
= \frac{1}{0.020}
= 50~\text{Hz}.
$$

Example:

```cpp
void collectIMUWindow() {

  for (
    int i = 0;
    i < WINDOW_SIZE;
    i++
  ) {

    sensors_event_t a;
    sensors_event_t g;
    sensors_event_t temp;

    mpu.getEvent(
      &a,
      &g,
      &temp
    );

    imuWindow[i][0] =
        a.acceleration.x;

    imuWindow[i][1] =
        a.acceleration.y;

    imuWindow[i][2] =
        a.acceleration.z;

    imuWindow[i][3] =
        g.gyro.x;

    imuWindow[i][4] =
        g.gyro.y;

    imuWindow[i][5] =
        g.gyro.z;

    delay(
      SAMPLE_INTERVAL_MS
    );
  }
}
```

The acquisition process is:

```text
Sample 1
   ↓
20 ms
   ↓
Sample 2
   ↓
20 ms
   ↓
...
   ↓
Sample 100
```

---

## 11. Sampling Timing

The ideal sample time is:

$$
t_i=iT_s.
$$

With:

$$
T_s=20~\text{ms},
$$

sample 50 occurs at approximately:

$$
t_{50}
= 50\times20~\text{ms}
=
1000~\text{ms}.
$$

Consistent sampling is important because the model was trained using a specific time scale.

---

## 12. Experiment 5 — Use Training Normalization Parameters

The live sensor data must be processed using the **same normalization parameters** used during training.

For example:

```cpp
float meanValues[6] = {
    // ax
    // ay
    // az
    // gx
    // gy
    // gz
};

float stdValues[6] = {
    // ax
    // ay
    // az
    // gx
    // gy
    // gz
};
```

Normalize using:

```cpp
float normalized =
    (
      imuWindow[i][j]
      -
      meanValues[j]
    )
    /
    stdValues[j];
```

Mathematically:

$$
x'
= \frac{x-\mu}{\sigma}.
$$

---

## 13. Why Preprocessing Must Match

The complete ML pipeline must be consistent:

```text
Training Data
     ↓
Normalization
     ↓
Model Training

Live Data
     ↓
Same Normalization
     ↓
Model Inference
```

If the MCU uses different preprocessing:

```text
Training Distribution
        ≠
Deployment Distribution
```

then classification accuracy can decrease significantly.

---

## 14. Experiment 6 — Load the TinyML Model

Include the model:

```cpp
#include "gesture_model.h"
```

Conceptually, the TensorFlow Lite Micro initialization process is:

```text
Model Array
    ↓
TFLite Model
    ↓
Operator Resolver
    ↓
Tensor Arena
    ↓
Interpreter
    ↓
Allocate Tensors
```

The exact setup code depends on the TensorFlow Lite Micro library version and the operators used by the trained model.

---

## 15. Tensor Arena

TensorFlow Lite Micro requires a memory area called the **tensor arena**.

Example:

```cpp
constexpr int kTensorArenaSize =
    60 * 1024;

uint8_t tensorArena[
    kTensorArenaSize
];
```

The tensor arena stores temporary runtime tensors.

Its required size depends on:

- Model architecture
- Input tensor size
- Output tensor size
- Intermediate tensors
- Operators

If the tensor arena is too small, tensor allocation fails.

---

## 16. Experiment 7 — Fill the Input Tensor

For a Float32 model:

```cpp
int index = 0;

for (
  int i = 0;
  i < WINDOW_SIZE;
  i++
) {

  for (
    int j = 0;
    j < NUM_FEATURES;
    j++
  ) {

    float normalized =
        (
          imuWindow[i][j]
          -
          meanValues[j]
        )
        /
        stdValues[j];

    input->data.f[index++] =
        normalized;
  }
}
```

The flattening order must match the training model.

The input flow is:

```text
100 × 6
   ↓
Flatten
   ↓
600 Input Values
```

---

## 17. INT8 Input Conversion

If the deployed model uses INT8 inputs, first normalize the sensor value and then quantize it.

The quantization equation is:

$$
q
= \operatorname{round}
\left(
\frac{x}{s}
\right)
+
z,
$$

where:

- $x$ is the normalized floating-point value
- $s$ is the input scale
- $z$ is the input zero point

Example concept:

```cpp
float scale =
    input->params.scale;

int zeroPoint =
    input->params.zero_point;

int q =
    round(
      normalized / scale
    )
    +
    zeroPoint;
```

The value should then be constrained to the INT8 range:

$$
-128\leq q\leq127.
$$

---

## 18. Experiment 8 — Run TinyML Inference

Measure inference start time:

```cpp
unsigned long startTime =
    micros();
```

Run:

```cpp
TfLiteStatus status =
    interpreter->Invoke();
```

Measure completion:

```cpp
unsigned long endTime =
    micros();
```

Then:

```cpp
unsigned long inferenceTime =
    endTime - startTime;
```

The inference latency is:

$$
T_{\text{inf}}
= t_{\text{end}}
- t_{\text{start}}.
$$

---

## 19. Experiment 9 — Read Model Outputs

For a floating-point model, output values may be accessed as:

```cpp
float score0 =
    output->data.f[0];

float score1 =
    output->data.f[1];

float score2 =
    output->data.f[2];

float score3 =
    output->data.f[3];

float score4 =
    output->data.f[4];
```

For example:

```text
stationary = 0.03
left       = 0.80
right      = 0.07
up         = 0.05
down       = 0.05
```

The predicted class is the maximum score.

---

## 20. Argmax Classification

The predicted class is:

$$
\hat{y}
= \arg\max_k p_k.
$$

Example:

$$
\mathbf{p}
= [
0.03,
0.80,
0.07,
0.05,
0.05
].
$$

The largest value is:

$$
0.80.
$$

Therefore:

```text
Predicted Gesture = LEFT
```

---

## 21. Experiment 10 — Find Maximum Score

Example:

```cpp
int predictedClass = 0;
float maxScore = scores[0];

for (
  int i = 1;
  i < NUM_CLASSES;
  i++
) {

  if (
    scores[i] > maxScore
  ) {

    maxScore =
        scores[i];

    predictedClass =
        i;
  }
}
```

The algorithm is:

```text
Read All Class Scores
        ↓
Find Maximum
        ↓
Predicted Class
        +
Maximum Score
```

---

## 22. Confidence Threshold

Not every model output should immediately control hardware.

Define a threshold:

```cpp
#define CONFIDENCE_THRESHOLD 0.70
```

Decision logic:

```text
Confidence ≥ 0.70
       ↓
Accept Gesture

Confidence < 0.70
       ↓
Reject / Unknown
```

Mathematically:

$$
\text{Decision}
= \begin{cases}
\hat{y}, & C\geq T_C,\\
\text{Unknown}, & C<T_C.
\end{cases}
$$

where:

- $C$ = maximum class score
- $T_C$ = confidence threshold

---

## 23. Experiment 11 — Gesture-to-LED Mapping

Create:

```cpp
void controlLEDs(
    int predictedClass
) {

  switch (
    predictedClass
  ) {

    case 1:

      digitalWrite(
        LED1,
        HIGH
      );

      digitalWrite(
        LED2,
        LOW
      );

      break;

    case 2:

      digitalWrite(
        LED1,
        LOW
      );

      digitalWrite(
        LED2,
        HIGH
      );

      break;

    case 3:

      digitalWrite(
        LED1,
        HIGH
      );

      digitalWrite(
        LED2,
        HIGH
      );

      break;

    case 4:

      digitalWrite(
        LED1,
        LOW
      );

      digitalWrite(
        LED2,
        LOW
      );

      break;

    default:
      break;
  }
}
```

The intelligent control chain becomes:

```text
Gesture
   ↓
Model Prediction
   ↓
Class ID
   ↓
Decision Logic
   ↓
LED State
```

---

## 24. Experiment 12 — Reject Low-Confidence Predictions

Example:

```cpp
if (
  maxScore >=
  CONFIDENCE_THRESHOLD
) {

  controlLEDs(
    predictedClass
  );

}
else {

  Serial.println(
    "Unknown gesture"
  );
}
```

This reduces the possibility that noise or incomplete gestures activate an LED.

---

## 25. Complete Runtime Loop

The conceptual runtime loop is:

```cpp
void loop() {

  collectIMUWindow();

  preprocessInput();

  runInference();

  int gesture =
      getPrediction();

  if (
    confidence >=
    CONFIDENCE_THRESHOLD
  ) {

    controlLEDs(
      gesture
    );
  }

  printResults();
}
```

Architecture:

```text
loop()
  ↓
Collect Sensor Window
  ↓
Normalize
  ↓
Inference
  ↓
Prediction
  ↓
Confidence Check
  ↓
LED Control
  ↓
Repeat
```

---

## 26. Block-Based Inference

The simplest implementation waits for a complete window before performing inference.

```text
Collect 2 s
    ↓
Infer
    ↓
Collect Next 2 s
    ↓
Infer
```

This is known as **non-overlapping window inference**.

Advantages:

- Simple implementation
- Low computation rate
- Easy to understand

Limitation:

- Slow response to gestures occurring near window boundaries

---

## 27. Sliding-Window Inference

A more responsive approach uses overlapping windows.

For example:

```text
Window Size = 100 Samples
Window Step = 25 Samples
```

This means:

```text
Window 1 → Samples   1–100
Window 2 → Samples  26–125
Window 3 → Samples  51–150
Window 4 → Samples  76–175
```

The overlap is:

$$
N_{\text{overlap}}
= N_{\text{window}}
- N_{\text{step}}.
$$

Therefore:

$$
N_{\text{overlap}}
= 100-25
= 75.
$$

---

## 28. Overlap Percentage

The overlap percentage is:

$$
P_{\text{overlap}}
= \frac{
N_{\text{window}}
- N_{\text{step}}
}{
N_{\text{window}}
}
\times100\%.
$$

For a window of 100 samples and step of 25:

$$
P_{\text{overlap}}
= \frac{100-25}{100}
\times100\%
= 75\%.
$$

---

## 29. Experiment 13 — Compare Window Strategies

Test:

| Test | Window Size | Step Size | Overlap |
|---:|---:|---:|---:|
| 1 | 100 | 100 | 0% |
| 2 | 100 | 50 | 50% |
| 3 | 100 | 25 | 75% |

Record:

| Window Strategy | Response Time | Accuracy | MCU Load |
|---|---:|---:|---:|
| 0% overlap |  |  |  |
| 50% overlap |  |  |  |
| 75% overlap |  |  |  |

Increasing overlap can improve responsiveness but also increases the number of inference operations.

---

## 30. Classification Rate

If the step size is $N_s$ samples and the sampling frequency is $f_s$, the inference update interval is:

$$
T_u
= \frac{N_s}{f_s}.
$$

For:

$$
N_s=25
$$

and:

$$
f_s=50~\text{Hz},
$$

we obtain:

$$
T_u
= \frac{25}{50}
= 0.5~\text{s}.
$$

The classification update rate is:

$$
f_u
= \frac{1}{T_u}
= 2~\text{Hz}.
$$

---

## 31. Experiment 14 — Prediction Smoothing

A single incorrect prediction can cause unwanted LED switching.

Prediction smoothing can be used.

For example:

```text
Prediction 1 = LEFT
Prediction 2 = LEFT
Prediction 3 = RIGHT
Prediction 4 = LEFT
Prediction 5 = LEFT
```

Majority decision:

```text
Final Gesture = LEFT
```

This can improve system stability.

---

## 32. Majority Vote

For the last $M$ predictions:

$$
\hat{y}_{\text{final}}
= \operatorname{mode}
\{
\hat{y}_1,
\hat{y}_2,
\dots,
\hat{y}_M
\}.
$$

For example:

```text
M = 5
```

provides a five-prediction voting window.

The trade-off is:

```text
More Smoothing
      ↓
More Stable
      ↓
but
      ↓
Higher Response Delay
```

---

## 33. Experiment 15 — Consecutive-Prediction Confirmation

Another method requires the same gesture to be detected repeatedly.

Example:

```text
LEFT
LEFT
LEFT
 ↓
Accept LEFT
```

but:

```text
LEFT
RIGHT
LEFT
 ↓
Do Not Activate
```

Define:

```cpp
#define REQUIRED_MATCHES 3
```

This is useful when gesture classification directly controls actuators.

---

## 34. End-to-End Response Time

The total response time may include:

$$
T_{\text{total}}
= T_{\text{window}}
+
T_{\text{pre}}
+
T_{\text{inf}}
+
T_{\text{decision}}
+
T_{\text{act}}.
$$

where:

- $T_{\text{window}}$ = sensor acquisition delay
- $T_{\text{pre}}$ = preprocessing time
- $T_{\text{inf}}$ = inference latency
- $T_{\text{decision}}$ = decision time
- $T_{\text{act}}$ = actuator response

For this application, the sensor window may dominate the total response time.

---

## 35. Experiment 16 — Measure Inference Latency

Perform at least 20 inference operations.

Record:

| Test | Gesture | Inference Time ($\mu$s) |
|---:|---|---:|
| 1 | Left |  |
| 2 | Left |  |
| 3 | Right |  |
| ... | ... | ... |
| 20 | Down |  |

Calculate:

$$
\bar{T}_{\text{inf}}
= \frac{1}{N}
\sum_{i=1}^{N}T_i.
$$

Also identify:

$$
T_{\min}
= \min(T_i)
$$

and:

$$
T_{\max}
= \max(T_i).
$$

---

## 36. Experiment 17 — Measure Live Classification Accuracy

Perform each gesture multiple times.

For example:

```text
20 Left gestures
20 Right gestures
20 Up gestures
20 Down gestures
20 Stationary trials
```

Calculate:

$$
\text{Live Accuracy}
= \frac{
N_{\text{correct}}
}{
N_{\text{total}}
}
\times100\%.
$$

Suggested table:

| Gesture | Trials | Correct | Accuracy |
|---|---:|---:|---:|
| Stationary | 20 |  |  |
| Left | 20 |  |  |
| Right | 20 |  |  |
| Up | 20 |  |  |
| Down | 20 |  |  |

---

## 37. Offline vs. Live Accuracy

Compare:

| Metric | Offline Test | Live MCU Test |
|---|---:|---:|
| Accuracy |  |  |
| Data source | Stored | Live |
| Noise | Controlled | Real-world |
| Gesture consistency | Higher | Variable |
| Sensor placement | Fixed | May vary |

It is common for live accuracy to differ from offline test accuracy because real-world conditions introduce more variability.

---

## 38. Experiment 18 — Test Confidence Thresholds

Evaluate:

```text
0.50
0.60
0.70
0.80
0.90
```

Record:

| Threshold | Accepted Predictions | Correct | Rejected | False Activations |
|---:|---:|---:|---:|---:|
| 0.50 |  |  |  |  |
| 0.60 |  |  |  |  |
| 0.70 |  |  |  |  |
| 0.80 |  |  |  |  |
| 0.90 |  |  |  |  |

Increasing the threshold generally causes:

```text
Higher Threshold
      ↓
Fewer Accepted Predictions
      ↓
Potentially Fewer False Activations
```

but may also increase rejected valid gestures.

---

## 39. Acceptance Rate

Define:

$$
R_{\text{accept}}
= \frac{
N_{\text{accepted}}
}{
N_{\text{total}}
}
\times100\%.
$$

A high confidence threshold can lower the acceptance rate.

Therefore, the threshold should balance:

$$
\boxed{
\text{Reliability}
\leftrightarrow
\text{Availability}
}
$$

---

## 40. Experiment 19 — Monitor MCU Memory

Use:

```cpp
Serial.print(
  "Free Heap: "
);

Serial.println(
  ESP.getFreeHeap()
);
```

Record memory before and after model initialization.

Suggested table:

| Stage | Free Heap |
|---|---:|
| Startup |  |
| After IMU initialization |  |
| After model loading |  |
| After tensor allocation |  |
| During inference |  |

This helps identify memory constraints.

---

## 41. Memory Consumption

An approximate RAM requirement can be considered as:

$$
M_{\text{RAM}}
= M_{\text{tensor}}
+
M_{\text{buffer}}
+
M_{\text{runtime}}
+
M_{\text{other}}.
$$

The IMU window alone requires approximately:

$$
100\times6\times4
= 2400~\text{bytes}
$$

when each value is stored as a 32-bit `float`.

Using quantized input buffers can potentially reduce storage requirements.

---

## 42. Experiment 20 — Serial Output for Debugging

Display:

```text
Gesture: LEFT
Confidence: 0.87
Inference: 1432 us
LED1: ON
LED2: OFF
```

Example:

```cpp
Serial.print(
  "Gesture: "
);

Serial.println(
  gestureNames[
    predictedClass
  ]
);

Serial.print(
  "Confidence: "
);

Serial.println(
  maxScore
);

Serial.print(
  "Inference: "
);

Serial.print(
  inferenceTime
);

Serial.println(
  " us"
);
```

This makes the embedded inference pipeline observable during testing.

---

## 43. Complete Edge-Inference Algorithm

```text
START
  ↓
Initialize ESP32
  ↓
Initialize MPU6050
  ↓
Initialize TinyML Model
  ↓
Initialize LEDs
  ↓
Collect IMU Samples
  ↓
Build Input Window
  ↓
Normalize Sensor Data
  ↓
Run TinyML Inference
  ↓
Find Highest Output Score
  ↓
Is Confidence ≥ Threshold?
  ├── No → Ignore / Unknown
  │
  └── Yes
         ↓
     Identify Gesture
         ↓
     Apply Decision Logic
         ↓
       Control LEDs
         ↓
       Repeat
```

---

## 44. Complete Embedded Intelligence Architecture

The completed system can be represented as:

```text
┌─────────────────────────────┐
│        Physical World       │
│        Human Gesture        │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│          Sensing            │
│          MPU6050            │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│       Data Acquisition      │
│      50 Hz IMU Sampling     │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│       Preprocessing         │
│ Windowing + Normalization   │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│     Embedded Intelligence   │
│       TinyML Inference      │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│        Decision Logic       │
│ Confidence + Classification │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│          Actuation          │
│           LEDs              │
└─────────────────────────────┘
```

---

## 45. Experiments

### Experiment 1 — LED Verification

Control two LEDs directly from the ESP32.

### Experiment 2 — Live IMU Acquisition

Read six-axis MPU6050 data at approximately 50 Hz.

### Experiment 3 — Window Buffer

Store 100 consecutive six-axis IMU samples.

### Experiment 4 — Preprocessing

Normalize the live measurements using LAB 14 training parameters.

### Experiment 5 — Model Integration

Load the trained TinyML model into MCU firmware.

### Experiment 6 — Input Tensor

Transfer normalized IMU data to the model input tensor.

### Experiment 7 — Inference

Run the model on the ESP32.

### Experiment 8 — Gesture Prediction

Find the class with the highest model output.

### Experiment 9 — Confidence Filtering

Reject predictions below the selected threshold.

### Experiment 10 — LED Control

Map gestures to LED states.

### Experiment 11 — Sliding Windows

Compare non-overlapping and overlapping inference.

### Experiment 12 — Prediction Smoothing

Apply majority voting or consecutive-prediction confirmation.

### Experiment 13 — Latency Measurement

Measure inference time.

### Experiment 14 — Live Accuracy

Perform multiple gesture trials and calculate classification accuracy.

### Experiment 15 — System Evaluation

Evaluate model accuracy, latency, memory, and actuator response.

---

## 46. Exercises

### Exercise 1 — Live Gesture Classifier

Implement:

```text
MPU6050
   ↓
ESP32
   ↓
TinyML
   ↓
Gesture
```

Display the predicted class through Serial Monitor.

### Exercise 2 — Gesture-Controlled LEDs

Implement:

```text
LEFT
 ↓
LED 1 ON

RIGHT
 ↓
LED 2 ON

UP
 ↓
Both ON

DOWN
 ↓
Both OFF
```

### Exercise 3 — Confidence-Based Control

Use:

```text
Threshold = 0.70
```

and prevent LED control when the model confidence is below the threshold.

### Exercise 4 — Sliding Window

Modify the inference pipeline to use:

```text
Window = 100 samples
Step   = 50 samples
```

Calculate the overlap percentage and classification update rate.

### Exercise 5 — Prediction Stabilization

Implement either:

```text
Majority Voting
```

or:

```text
3 Consecutive Matching Predictions
```

Compare the stability with the original implementation.

### Exercise 6 — Performance Benchmark

Measure:

- Offline accuracy
- Live accuracy
- Inference latency
- Window duration
- Update rate
- Model size
- Free heap
- False LED activations

### Exercise 7 — Complete Edge-AI Controller

Develop:

```text
Gesture
   ↓
IMU
   ↓
Live Data
   ↓
TinyML
   ↓
Confidence
   ↓
Decision
   ↓
LED Actuation
```

The system should operate completely on the ESP32 without cloud-based inference.

---

## 47. Checkpoint Questions

1. What is edge inference?
2. Why is live sensor inference different from offline model testing?
3. What is the input size of the gesture model?
4. Why must the sampling rate match the training conditions?
5. What is a fixed-length sensor window?
6. Why must MCU normalization match training normalization?
7. What is a TensorFlow Lite Micro tensor arena?
8. What happens if the tensor arena is too small?
9. What is the purpose of the model input tensor?
10. What does `Invoke()` do?
11. What is an output score?
12. What does `argmax` determine?
13. What is a confidence threshold?
14. Why might a low-confidence prediction be rejected?
15. What is the difference between non-overlapping and overlapping windows?
16. How does window overlap affect response time?
17. What is prediction smoothing?
18. How does majority voting improve stability?
19. What is inference latency?
20. What components contribute to end-to-end response time?
21. Why can live accuracy be lower than offline accuracy?
22. How does a higher confidence threshold affect the acceptance rate?
23. Why should MCU memory usage be monitored?
24. What is the role of decision logic between inference and actuation?
25. Why is this system considered an embedded-intelligence application?

---

## 48. Expected Results

Students should successfully run the trained gesture-recognition model on the ESP32 using live MPU6050 measurements.

The real-time inference pipeline should operate as:

```text
Live Gesture
     ↓
MPU6050
     ↓
100 × 6 IMU Window
     ↓
Normalization
     ↓
TinyML Inference
     ↓
Predicted Gesture
     ↓
Confidence Check
     ↓
LED Control
```

Example behavior:

```text
Swipe Left
    ↓
LEFT = 0.88
    ↓
LED 1 ON
```

```text
Swipe Right
     ↓
RIGHT = 0.91
     ↓
LED 2 ON
```

```text
Gesture Unclear
      ↓
Maximum Score = 0.42
      ↓
Prediction Rejected
      ↓
No LED Change
```

The system should demonstrate that machine-learning inference, decision making, and actuation can all be executed locally on the MCU.

---

## 49. Submission

Submit the following:

- ESP32 real-time inference source code.
- TinyML model header file.
- Normalization parameters.
- Live IMU sampling implementation.
- Input-window implementation.
- Gesture-classification output.
- Confidence-threshold implementation.
- LED-control demonstration.
- Sliding-window experiment.
- Prediction-smoothing experiment.
- Inference latency measurements.
- Live classification accuracy.
- MCU memory measurements.
- Short discussion of edge-inference performance.

Suggested live-classification table:

| Trial | Actual Gesture | Prediction | Confidence | LED Action | Correct |
|---:|---|---|---:|---|---|
| 1 | Left |  |  |  |  |
| 2 | Right |  |  |  |  |
| 3 | Up |  |  |  |  |
| 4 | Down |  |  |  |  |
| 5 | Stationary |  |  |  |  |

Suggested performance table:

| Metric | Result |
|---|---:|
| Sampling frequency | 50 Hz |
| Window size | 100 samples |
| Window duration | 2 s |
| Step size |  |
| Window overlap |  |
| Model size |  |
| Average inference latency |  |
| Offline accuracy |  |
| Live accuracy |  |
| Confidence threshold |  |
| Free heap |  |

---

## 50. Conclusion

In this lab, a trained TinyML gesture-recognition model is executed directly on the ESP32 to classify **live MPU6050 sensor data**. The MCU continuously acquires six-axis IMU measurements, constructs fixed-length sensor windows, applies the same preprocessing used during training, performs local machine-learning inference, and converts the classification result into LED-control actions.

Confidence-based decision making, overlapping windows, and prediction smoothing improve the robustness and responsiveness of the real-time system. Performance is evaluated in terms of classification accuracy, inference latency, memory usage, confidence threshold, and actuator response.


