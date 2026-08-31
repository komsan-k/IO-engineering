# LAB 13 — IMU Gesture Data Collection & Machine-Learning Dataset Preparation

## 1. Objective

The objective of this lab is to:

1. Understand how **IMU sensor data** can represent human motion and gestures.
2. Interface an **MPU6050 IMU** with the ESP32.
3. Acquire three-axis accelerometer and gyroscope measurements.
4. Sample IMU signals at a controlled sampling rate.
5. Define and label several gesture classes.
6. Record multiple gesture repetitions for machine-learning training.
7. Organize time-series IMU measurements into fixed-size windows.
8. Export the collected measurements in **CSV format**.
9. Prepare training, validation, and test datasets.
10. Visualize and inspect IMU signals before model training.

---

## 2. Background

TinyML systems use machine-learning models directly on resource-constrained embedded devices.

A typical TinyML workflow begins with **data collection**:

```text
Physical Motion
      ↓
IMU Sensor
      ↓
ESP32
      ↓
Data Acquisition
      ↓
Gesture Labeling
      ↓
Dataset
      ↓
Model Training
      ↓
TinyML Model
      ↓
ESP32 Inference
```

In this lab, an ESP32 and an IMU are used to collect motion data for a future **gesture-classification model**.

An IMU typically provides:

```text
Accelerometer
ax, ay, az
     +
Gyroscope
gx, gy, gz
```

Therefore, each sensor sample can be represented by the feature vector:

$$
\mathbf{x}(t)
= [
a_x(t),
a_y(t),
a_z(t),
g_x(t),
g_y(t),
g_z(t)
].
$$

---

## 3. Gesture Recognition Architecture

The overall gesture-recognition system is:

```text
Hand Gesture
     ↓
MPU6050
     ↓
Accelerometer + Gyroscope
     ↓
ESP32
     ↓
Time-Series Samples
     ↓
Gesture Dataset
     ↓
Machine-Learning Model
     ↓
Gesture Classification
```

This lab focuses on the **data acquisition and dataset preparation** stages.

---

## 4. Hardware & Software Requirements

### Hardware

- ESP32 development board
- MPU6050 IMU sensor
- USB cable
- Breadboard
- Jumper wires
- Computer

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- Serial Plotter
- Spreadsheet software or Python environment for dataset inspection

### Arduino Libraries

A suitable MPU6050 library may be used.

For example:

```cpp
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
```

Create the sensor object:

```cpp
Adafruit_MPU6050 mpu;
```

---

## 5. MPU6050 Sensor

The MPU6050 combines:

- 3-axis accelerometer
- 3-axis gyroscope

The six motion variables are:

| Sensor | X-axis | Y-axis | Z-axis |
|---|---|---|---|
| Accelerometer | $a_x$ | $a_y$ | $a_z$ |
| Gyroscope | $g_x$ | $g_y$ | $g_z$ |

A measurement vector can therefore be written as:

$$
\mathbf{x}
= [a_x,a_y,a_z,g_x,g_y,g_z].
$$

The accelerometer describes **linear acceleration**, while the gyroscope describes **angular velocity**.

---

## 6. MPU6050 and ESP32 Connection

The MPU6050 normally communicates with the ESP32 through **I²C**.

A typical connection is:

| MPU6050 | ESP32 |
|---|---|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

Architecture:

```text
MPU6050
   │
   │ I²C
   ▼
ESP32
   │
   │ USB Serial
   ▼
Computer
```

> Verify the voltage requirements of the specific MPU6050 breakout board before connecting it.

---

## 7. Experiment 1 — Detect the MPU6050

Start by initializing the IMU.

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
    "MPU6050 detected"
  );
}

void loop() {}
```

### Expected Output

```text
MPU6050 detected
```

If the sensor cannot be detected, check:

```text
Power
 ↓
Ground
 ↓
SDA
 ↓
SCL
 ↓
I²C Address
```

---

## 8. Experiment 2 — Read Accelerometer Data

Create sensor-event variables:

```cpp
sensors_event_t a;
sensors_event_t g;
sensors_event_t temp;
```

Read the sensor:

```cpp
mpu.getEvent(
  &a,
  &g,
  &temp
);
```

Display acceleration:

```cpp
Serial.print(a.acceleration.x);
Serial.print(",");

Serial.print(a.acceleration.y);
Serial.print(",");

Serial.println(a.acceleration.z);
```

The output may appear as:

```text
0.21,-0.35,9.72
0.25,-0.40,9.68
0.30,-0.45,9.65
```

---

## 9. Experiment 3 — Read Gyroscope Data

Display angular velocity:

```cpp
Serial.print(g.gyro.x);
Serial.print(",");

Serial.print(g.gyro.y);
Serial.print(",");

Serial.println(g.gyro.z);
```

Example:

```text
0.02,-0.01,0.05
0.04,-0.03,0.10
0.12,-0.08,0.35
```

Rotating the sensor should produce noticeable changes in the gyroscope measurements.

---

## 10. Experiment 4 — Read All Six IMU Channels

Combine the accelerometer and gyroscope measurements:

```cpp
void loop() {

  sensors_event_t a;
  sensors_event_t g;
  sensors_event_t temp;

  mpu.getEvent(
    &a,
    &g,
    &temp
  );

  Serial.print(
    a.acceleration.x
  );

  Serial.print(",");

  Serial.print(
    a.acceleration.y
  );

  Serial.print(",");

  Serial.print(
    a.acceleration.z
  );

  Serial.print(",");

  Serial.print(
    g.gyro.x
  );

  Serial.print(",");

  Serial.print(
    g.gyro.y
  );

  Serial.print(",");

  Serial.println(
    g.gyro.z
  );

  delay(20);
}
```

Each line contains:

```text
ax,ay,az,gx,gy,gz
```

For example:

```text
0.24,-0.31,9.71,0.02,-0.01,0.05
```

---

## 11. Sampling Rate

Machine-learning datasets require a consistent sampling interval.

If the sampling interval is:

$$
T_s,
$$

the sampling frequency is:

$$
f_s=\frac{1}{T_s}.
$$

For:

$$
T_s=20~\text{ms}=0.02~\text{s},
$$

the sampling frequency is:

$$
f_s=\frac{1}{0.02}=50~\text{Hz}.
$$

Therefore:

```cpp
delay(20);
```

approximately corresponds to:

```text
50 samples/second
```

for a simple introductory implementation.

---

## 12. Experiment 5 — Controlled Sampling

Define:

```cpp
#define SAMPLE_INTERVAL_MS 20
```

Then:

```cpp
delay(
  SAMPLE_INTERVAL_MS
);
```

The data-acquisition pipeline becomes:

```text
MPU6050
   ↓
Sample Every 20 ms
   ↓
50 Hz IMU Stream
   ↓
Serial Output
```

For more precise timing, students can later replace `delay()` with timer- or `millis()`-based sampling.

---

## 13. Timestamping IMU Measurements

Each sample should ideally contain a timestamp.

Use:

```cpp
unsigned long timestamp =
    millis();
```

Output:

```cpp
Serial.print(timestamp);
Serial.print(",");
```

A complete record becomes:

```text
timestamp,ax,ay,az,gx,gy,gz
```

Example:

```text
1020,0.24,-0.31,9.71,0.02,-0.01,0.05
1040,0.28,-0.35,9.65,0.04,-0.02,0.08
1060,0.35,-0.42,9.51,0.08,-0.04,0.15
```

---

## 14. Gesture Classes

Define a small set of clearly distinguishable gestures.

For example:

| Class ID | Gesture |
|---:|---|
| 0 | Stationary |
| 1 | Left |
| 2 | Right |
| 3 | Up |
| 4 | Down |

Another possible dataset is:

```text
Stationary
Swipe Left
Swipe Right
Rotate
Shake
```

The same gesture definition should be used consistently throughout data collection.

---

## 15. Gesture Labels

Machine-learning training requires each sample or sequence to have a known **label**.

For example:

```text
left
right
up
down
stationary
```

The dataset may contain:

```text
timestamp,ax,ay,az,gx,gy,gz,label
```

Example:

```text
1020,0.24,-0.31,9.71,0.02,-0.01,0.05,left
1040,0.30,-0.42,9.55,0.04,-0.03,0.10,left
1060,1.20,-0.80,8.90,0.15,-0.10,0.42,left
```

---

## 16. Experiment 6 — Collect a Single Gesture

Choose:

```text
Gesture = LEFT
```

Perform the gesture repeatedly while recording the IMU data.

The sequence is:

```text
Prepare
   ↓
Start Recording
   ↓
Perform LEFT Gesture
   ↓
Stop Recording
   ↓
Save Samples
```

Repeat the same gesture several times.

For example:

```text
LEFT_01
LEFT_02
LEFT_03
...
LEFT_20
```

---

## 17. Experiment 7 — Collect Multiple Gesture Classes

Repeat the process for all gesture classes.

Example:

```text
Stationary
    ↓
20 repetitions

Left
    ↓
20 repetitions

Right
    ↓
20 repetitions

Up
    ↓
20 repetitions

Down
    ↓
20 repetitions
```

The dataset should contain approximately balanced numbers of examples for each class.

---

## 18. Complete IMU Data-Collection Program

```cpp
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

#define SAMPLE_INTERVAL_MS 20

const char* gestureLabel =
    "left";

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
    "timestamp,ax,ay,az,gx,gy,gz,label"
  );

  delay(1000);
}

void loop() {

  sensors_event_t a;
  sensors_event_t g;
  sensors_event_t temp;

  mpu.getEvent(
    &a,
    &g,
    &temp
  );

  unsigned long timestamp =
      millis();

  Serial.print(timestamp);
  Serial.print(",");

  Serial.print(
    a.acceleration.x
  );
  Serial.print(",");

  Serial.print(
    a.acceleration.y
  );
  Serial.print(",");

  Serial.print(
    a.acceleration.z
  );
  Serial.print(",");

  Serial.print(
    g.gyro.x
  );
  Serial.print(",");

  Serial.print(
    g.gyro.y
  );
  Serial.print(",");

  Serial.print(
    g.gyro.z
  );
  Serial.print(",");

  Serial.println(
    gestureLabel
  );

  delay(
    SAMPLE_INTERVAL_MS
  );
}
```

---

## 19. CSV Dataset Format

The serial output can be stored as a **Comma-Separated Values (CSV)** file.

Example:

```csv
timestamp,ax,ay,az,gx,gy,gz,label
1000,0.21,-0.35,9.72,0.02,-0.01,0.05,left
1020,0.25,-0.40,9.68,0.04,-0.03,0.10,left
1040,0.45,-0.62,9.20,0.12,-0.08,0.35,left
1060,1.25,-1.10,8.70,0.30,-0.15,0.80,left
```

CSV is useful because it can be imported into:

```text
Python
Excel
MATLAB
Machine-Learning Tools
```

---

## 20. Dataset Structure

A simple dataset structure is:

```text
gesture_dataset/
│
├── stationary/
│   ├── stationary_01.csv
│   ├── stationary_02.csv
│   └── ...
│
├── left/
│   ├── left_01.csv
│   ├── left_02.csv
│   └── ...
│
├── right/
│   ├── right_01.csv
│   ├── right_02.csv
│   └── ...
│
├── up/
│   └── ...
│
└── down/
    └── ...
```

This organization makes the class labels explicit.

---

## 21. Time-Series Gesture Windows

A gesture is normally represented by a **sequence of samples**, rather than a single IMU measurement.

Suppose:

$$
f_s=50~\text{Hz}
$$

and the gesture window is:

$$
T_w=2~\text{s}.
$$

The number of samples per gesture window is:

$$
N=f_sT_w.
$$

Therefore:

$$
N=50\times2=100.
$$

Each gesture example contains:

```text
100 time steps × 6 IMU channels
```

or mathematically:

$$
\mathbf{X}\in\mathbb{R}^{100\times6}.
$$

---

## 22. Gesture Window Representation

A single gesture can be represented as:

$$
\mathbf{X}
= \begin{bmatrix}
a_x(1) & a_y(1) & a_z(1) & g_x(1) & g_y(1) & g_z(1)\\
a_x(2) & a_y(2) & a_z(2) & g_x(2) & g_y(2) & g_z(2)\\
\vdots & \vdots & \vdots & \vdots & \vdots & \vdots\\
a_x(N) & a_y(N) & a_z(N) & g_x(N) & g_y(N) & g_z(N)
\end{bmatrix}.
$$

The corresponding label is:

$$
y\in
\{
\text{left},
\text{right},
\text{up},
\text{down},
\text{stationary}
\}.
$$

Therefore, machine-learning training uses pairs:

$$
(\mathbf{X},y).
$$

---

## 23. Experiment 8 — Fixed-Length Gesture Recording

Use:

```text
Sampling Rate = 50 Hz
Gesture Duration = 2 s
```

Therefore:

```text
Samples per Gesture = 100
```

The recording process becomes:

```text
Ready
  ↓
Start
  ↓
Collect 100 Samples
  ↓
Stop
  ↓
Assign Label
  ↓
Save Gesture
```

Repeat for each gesture.

---

## 24. Dataset Size

Suppose there are:

$$
C=5
$$

gesture classes and:

$$
R=50
$$

recordings per class.

The total number of gesture examples is:

$$
N_{\text{examples}}
= C\times R.
$$

Therefore:

$$
N_{\text{examples}}
= 5\times50
=
250.
$$

If each example contains 100 samples:

$$
N_{\text{samples}}
= 250\times100
= 25{,}000.
$$

Thus, the complete dataset contains:

```text
250 gesture sequences
25,000 IMU samples
```

---

## 25. Balanced Dataset

A balanced dataset contains approximately the same number of examples for each class.

For example:

| Gesture | Number of Examples |
|---|---:|
| Stationary | 50 |
| Left | 50 |
| Right | 50 |
| Up | 50 |
| Down | 50 |
| **Total** | **250** |

A strongly unbalanced dataset can bias the classifier toward classes with more training examples.

---

## 26. Experiment 9 — Visualizing IMU Data

Before model training, inspect the recorded signals.

For example, plot:

```text
ax
ay
az
gx
gy
gz
```

against time.

A gesture may produce a pattern such as:

```text
Acceleration
    ▲
    │        /\
    │       /  \
    │______/    \______
    │
    └──────────────────► Time
```

Different gestures should ideally produce distinguishable temporal patterns.

---

## 27. Signal Magnitude

The acceleration magnitude can be calculated as:

$$
a_{\text{mag}}
= \sqrt{
a_x^2+a_y^2+a_z^2
}.
$$

Similarly, gyroscope magnitude is:

$$
g_{\text{mag}}
= \sqrt{
g_x^2+g_y^2+g_z^2
}.
$$

These quantities can help describe overall motion intensity independent of individual axes.

---

## 28. Basic Feature Extraction

For a window containing $N$ samples, simple statistical features can be calculated.

### Mean

$$
\mu_x
= \frac{1}{N}
\sum_{i=1}^{N}x_i.
$$

### Variance

$$
\sigma_x^2
= \frac{1}{N}
\sum_{i=1}^{N}
(x_i-\mu_x)^2.
$$

### Root Mean Square

$$
x_{\text{RMS}}
= \sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
x_i^2
}.
$$

### Maximum

$$
x_{\max}
= \max(x_1,x_2,\ldots,x_N).
$$

### Minimum

$$
x_{\min}
= \min(x_1,x_2,\ldots,x_N).
$$

These features can later be used with lightweight classifiers.

---

## 29. Raw Data vs. Extracted Features

Two machine-learning approaches are possible.

### Raw Time-Series Input

```text
IMU
 ↓
100 × 6 Samples
 ↓
Neural Network
 ↓
Gesture
```

### Feature-Based Input

```text
IMU
 ↓
Window
 ↓
Feature Extraction
 ↓
Mean / RMS / Variance / Max
 ↓
Classifier
 ↓
Gesture
```

| Approach | Advantages | Limitations |
|---|---|---|
| Raw time series | Preserves motion patterns | Larger model/input |
| Extracted features | Lower computational cost | May lose temporal information |

---

## 30. Data Normalization

Machine-learning models often benefit from normalized input.

A common standardization method is:

$$
x'
= \frac{x-\mu}{\sigma},
$$

where:

- $x$ = original value
- $\mu$ = mean
- $\sigma$ = standard deviation
- $x'$ = standardized value

Another simple approach is min-max normalization:

$$
x'
= \frac{x-x_{\min}}
{x_{\max}-x_{\min}}.
$$

Normalization parameters should be derived from the **training data** and then applied consistently to validation, test, and deployment data.

---

## 31. Training, Validation, and Test Sets

The dataset should be separated before model evaluation.

One possible division is:

```text
Training   = 70%
Validation = 15%
Testing    = 15%
```

For 250 gesture examples:

$$
N_{\text{train}}
\approx175,
$$

$$
N_{\text{validation}}
\approx38,
$$

and

$$
N_{\text{test}}
\approx37.
$$

The exact integer allocation can vary slightly while preserving the intended proportions.

The roles are:

| Dataset | Purpose |
|---|---|
| Training | Learn model parameters |
| Validation | Tune model/hyperparameters |
| Test | Final evaluation |

---

## 32. Avoiding Data Leakage

Gesture windows from the same recording should not be randomly divided across training and test sets if they are highly overlapping.

For example:

```text
Gesture Recording
      ↓
Window 1
Window 2
Window 3
```

should preferably remain within the same dataset partition.

Otherwise:

```text
Nearly Identical Data
      ↓
Training + Testing
      ↓
Artificially High Accuracy
```

This is known as **data leakage**.

---

## 33. Experiment 10 — Dataset Quality Inspection

Inspect the collected dataset for:

- Missing samples
- Incorrect labels
- Sensor saturation
- Duplicate recordings
- Inconsistent gesture duration
- Irregular sampling intervals
- Extreme outliers
- Class imbalance

Suggested table:

| Gesture | Samples | Recordings | Missing Data | Valid |
|---|---:|---:|---:|---|
| Stationary |  |  |  |  |
| Left |  |  |  |  |
| Right |  |  |  |  |
| Up |  |  |  |  |
| Down |  |  |  |  |

---

## 34. Experiment 11 — Dataset Export

Export the final dataset using CSV.

A combined dataset may have:

```csv
gesture_id,sample_id,timestamp,ax,ay,az,gx,gy,gz,label
1,1,1000,0.21,-0.35,9.72,0.02,-0.01,0.05,left
1,2,1020,0.25,-0.40,9.68,0.04,-0.03,0.10,left
1,3,1040,0.45,-0.62,9.20,0.12,-0.08,0.35,left
```

The additional `gesture_id` field identifies which samples belong to the same gesture recording.

This is preferable to relying only on the class label when multiple repetitions are collected.

---

## 35. Complete Dataset Pipeline

The complete workflow is:

```text
Physical Gesture
      ↓
MPU6050
      ↓
ESP32
      ↓
Sampling
      ↓
ax, ay, az, gx, gy, gz
      ↓
Timestamp
      ↓
Gesture Label
      ↓
Fixed-Length Window
      ↓
CSV Dataset
      ↓
Quality Inspection
      ↓
Train / Validation / Test
      ↓
Machine-Learning Training
```

---

## 36. Experiments

### Experiment 1 — IMU Detection

Connect the MPU6050 and verify successful I²C communication.

### Experiment 2 — Accelerometer

Display:

```text
ax, ay, az
```

Move the sensor and observe the changes.

### Experiment 3 — Gyroscope

Display:

```text
gx, gy, gz
```

Rotate the sensor and observe the changes.

### Experiment 4 — Six-Axis IMU

Output:

```text
ax,ay,az,gx,gy,gz
```

### Experiment 5 — Sampling Rate

Configure:

```text
50 Hz
```

and verify the approximate sampling interval.

### Experiment 6 — Gesture Label

Add:

```text
left
right
up
down
stationary
```

to the data records.

### Experiment 7 — Gesture Recording

Collect multiple repetitions of each gesture.

### Experiment 8 — Fixed Windows

Record:

```text
100 samples / gesture
```

at 50 Hz.

### Experiment 9 — CSV Export

Save all recordings in CSV format.

### Experiment 10 — Dataset Inspection

Check class balance, labels, missing data, and signal quality.

---

## 37. Exercises

### Exercise 1 — Basic IMU Logger

Create:

```text
MPU6050
   ↓
ESP32
   ↓
ax, ay, az
gx, gy, gz
   ↓
Serial Monitor
```

Use a 50 Hz sampling rate.

### Exercise 2 — Timestamped Data Logger

Generate:

```csv
timestamp,ax,ay,az,gx,gy,gz
```

and record at least 10 seconds of motion.

### Exercise 3 — Gesture Dataset

Collect the following classes:

```text
Stationary
Left
Right
Up
Down
```

Collect at least 20 repetitions per class.

### Exercise 4 — Fixed-Length Gesture Windows

Use:

```text
Sampling Frequency = 50 Hz
Window Duration = 2 s
```

Calculate the number of samples per window and organize each gesture accordingly.

### Exercise 5 — Signal Magnitude

Calculate:

$$
a_{\text{mag}}
= \sqrt{a_x^2+a_y^2+a_z^2}
$$

and

$$
g_{\text{mag}}
= \sqrt{g_x^2+g_y^2+g_z^2}.
$$

Add these values to the dataset.

### Exercise 6 — Feature Extraction

For each gesture window, calculate:

```text
Mean
Variance
RMS
Maximum
Minimum
```

for each IMU channel.

### Exercise 7 — Complete ML Dataset

Develop the following pipeline:

```text
Gesture
   ↓
MPU6050
   ↓
ESP32
   ↓
50 Hz Sampling
   ↓
6-Axis IMU Data
   ↓
2-Second Window
   ↓
Gesture Label
   ↓
CSV
   ↓
Dataset
```

Prepare the data for the next machine-learning lab.

---

## 38. Checkpoint Questions

1. What is an IMU?
2. What sensors are integrated into the MPU6050?
3. What do $a_x$, $a_y$, and $a_z$ represent?
4. What do $g_x$, $g_y$, and $g_z$ represent?
5. Why are both acceleration and angular velocity useful for gesture recognition?
6. What communication protocol is commonly used between the MPU6050 and ESP32?
7. What is sampling frequency?
8. What sampling frequency corresponds to a 20 ms sampling interval?
9. Why should IMU data be sampled at a consistent rate?
10. Why should each gesture have a label?
11. What is a gesture window?
12. How many samples are collected during 2 seconds at 50 Hz?
13. Why should multiple examples of each gesture be collected?
14. What is a balanced dataset?
15. Why should IMU data be inspected before training?
16. What is acceleration magnitude?
17. What is feature extraction?
18. Why is normalization useful for machine learning?
19. What is the difference between training, validation, and test data?
20. What is data leakage, and why should it be avoided?

---

## 39. Expected Results

Students should successfully acquire six-axis motion data:

```text
MPU6050
   ↓
ESP32
   ↓
ax, ay, az
gx, gy, gz
```

The output should be structured as:

```text
timestamp,ax,ay,az,gx,gy,gz,label
```

The gesture dataset should contain multiple labeled sequences:

```text
Stationary ─┐
Left ───────┤
Right ──────┼──► Gesture Dataset
Up ─────────┤
Down ───────┘
```

Each gesture should contain a fixed-length sequence:

$$
\boxed{
\mathbf{X}\in\mathbb{R}^{100\times6}
}
$$

for a 2-second window sampled at 50 Hz.

The final output should be a structured dataset suitable for machine-learning training.

---

## 40. Submission

Submit the following:

- ESP32 MPU6050 source code.
- Screenshot showing successful MPU6050 detection.
- Raw accelerometer data.
- Raw gyroscope data.
- Six-axis IMU output.
- Sampling-rate verification.
- Gesture definitions.
- Labeled gesture recordings.
- CSV dataset.
- Dataset statistics.
- IMU signal plots.
- Short discussion of dataset quality.

Suggested data-collection table:

| Gesture | Repetitions | Samples per Gesture | Total Samples |
|---|---:|---:|---:|
| Stationary |  | 100 |  |
| Left |  | 100 |  |
| Right |  | 100 |  |
| Up |  | 100 |  |
| Down |  | 100 |  |

Suggested dataset summary:

| Property | Value |
|---|---|
| IMU | MPU6050 |
| Accelerometer axes | 3 |
| Gyroscope axes | 3 |
| Total channels | 6 |
| Sampling frequency | 50 Hz |
| Sampling interval | 20 ms |
| Window duration | 2 s |
| Samples/window | 100 |
| Gesture classes | 5 |
| File format | CSV |

---

## 41. Conclusion

In this lab, the ESP32 and MPU6050 are used to construct a complete **IMU gesture-data acquisition pipeline**. Three-axis acceleration and three-axis angular-velocity measurements are sampled at a controlled rate, timestamped, labeled according to the performed gesture, and exported as CSV data.

The use of fixed-length gesture windows converts the continuous IMU stream into structured machine-learning examples. Careful attention to class balance, consistent gesture execution, sampling rate, labeling, normalization, and data partitioning is important because the quality of the final TinyML model depends strongly on the quality of the training dataset.

