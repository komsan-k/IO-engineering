# 📘 Theory: TinyML Fundamentals, Feature Extraction, and Dataset Quality  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

**Tiny Machine Learning (TinyML)** brings machine-learning inference to **microcontrollers and resource-constrained embedded devices**. Instead of continuously sending raw sensor data to a cloud server for analysis, an embedded system can process the data locally and make decisions at the edge.

The ESP32 is well suited to introductory TinyML applications because it combines:

- microcontroller processing,
- memory and peripheral interfaces,
- ADC and digital sensors,
- Wi-Fi and Bluetooth/BLE connectivity,
- FreeRTOS support,
- low-power operating modes.

A typical TinyML system can be represented as:

```text
Physical Environment
        │
        ▼
      Sensor
        │
        ▼
      ESP32
        │
        ▼
 Signal Processing
        │
        ▼
Feature Extraction
        │
        ▼
 TinyML Model
        │
        ▼
   Prediction
        │
        ▼
Decision / Action
```

The complete process can be summarized as:

$$
\boxed{
\text{Sensor Data}
\rightarrow
\text{Features}
\rightarrow
\text{ML Model}
\rightarrow
\text{Inference}
\rightarrow
\text{Decision}
}
$$

Three important concepts are central to practical TinyML development:

$$
\boxed{
\text{TinyML System}
=
\text{Efficient Model}
+
\text{Useful Features}
+
\text{Quality Dataset}
}
$$

---

# 🧠 2. What Is TinyML?

**TinyML** refers to machine-learning techniques designed to operate on highly resource-constrained embedded platforms such as microcontrollers.

Traditional cloud-based machine learning often follows:

```text
Sensor
   │
   ▼
Microcontroller
   │
   ▼
Internet
   │
   ▼
Cloud Server
   │
   ▼
Machine Learning
   │
   ▼
Prediction
```

TinyML moves the inference stage toward the physical device:

```text
Sensor
   │
   ▼
Microcontroller
   │
   ▼
ML Inference
   │
   ▼
Decision
```

Thus:

$$
\boxed{
\text{TinyML}
=
\text{Machine Learning}
+
\text{Embedded Computing}
}
$$

---

# 🌐 3. Cloud AI vs. Edge AI vs. TinyML

Machine-learning computation can be performed at different locations.

| Approach | Processing Location | Typical Platform |
|---|---|---|
| Cloud AI | Remote data center | GPU/CPU server |
| Edge AI | Near data source | Gateway, SBC, edge computer |
| TinyML | Directly on embedded device | MCU |

A simplified hierarchy is:

```text
Cloud AI
   ▲
   │ Internet
   │
Edge AI
   ▲
   │ Local Network
   │
TinyML
   ▲
   │
 Sensor
```

TinyML represents computation very close to the physical sensing process.

---

# ⚡ 4. Why TinyML?

TinyML can provide several advantages for embedded systems:

- reduced communication requirements,
- lower network bandwidth usage,
- lower inference latency,
- improved operation without Internet connectivity,
- greater data locality,
- event-driven communication,
- potentially lower system energy consumption.

For example:

```text
Traditional IoT

Sensor
  │
  ▼
ESP32
  │
  │ Send all samples
  ▼
Cloud
  │
  ▼
AI Model
```

TinyML approach:

```text
Sensor
  │
  ▼
ESP32
  │
  ├── Feature Extraction
  └── ML Inference
         │
         ▼
      Decision
         │
         ├── Normal → No transmission
         │
         └── Abnormal → Send alert
```

---

# 🔋 5. TinyML and Low-Power IoT

TinyML can complement low-power operation by reducing unnecessary wireless communication.

For example:

$$
\boxed{
\text{Sensor}
\rightarrow
\text{TinyML}
\rightarrow
\text{Important Event?}
}
$$

If the result is normal:

```text
Normal
  │
  ▼
Sleep
```

If an abnormal condition is detected:

```text
Abnormal
   │
   ▼
Activate Radio
   │
   ▼
Transmit Alert
   │
   ▼
Sleep
```

This architecture is useful for battery-operated IoT nodes.

---

# 🧱 6. Typical TinyML Pipeline

A complete TinyML development pipeline usually consists of:

```text
1. Problem Definition
        │
        ▼
2. Data Collection
        │
        ▼
3. Data Cleaning
        │
        ▼
4. Preprocessing
        │
        ▼
5. Feature Extraction
        │
        ▼
6. Model Training
        │
        ▼
7. Model Evaluation
        │
        ▼
8. Model Optimization
        │
        ▼
9. MCU Deployment
        │
        ▼
10. On-Device Inference
```

An important distinction is that **model training** is normally performed on a computer or cloud platform, while the ESP32 performs **inference** using the trained model.

---

# 🎓 7. Training vs. Inference

### Training

Training determines model parameters from a dataset:

$$
\boxed{
\text{Training Data}
\rightarrow
\text{Learning Algorithm}
\rightarrow
\text{Trained Model}
}
$$

Training can require substantial memory, processing power, and computation time.

### Inference

Inference uses the trained model to make a prediction:

$$
\boxed{
\text{New Input}
\rightarrow
\text{Trained Model}
\rightarrow
\text{Prediction}
}
$$

The ESP32 normally performs this stage.

---

# 💻 8. TinyML Development Workflow for ESP32

```text
ESP32 / Sensors
      │
      ▼
Collect Dataset
      │
      ▼
PC / Python
      │
      ├── Clean Data
      ├── Extract Features
      ├── Train Model
      └── Evaluate Model
              │
              ▼
       Optimize Model
              │
              ▼
       Deploy to ESP32
              │
              ▼
      On-Device Inference
```

---

# 📊 9. Machine-Learning Tasks

Common TinyML tasks include:

- classification,
- regression,
- anomaly detection,
- keyword recognition,
- activity recognition,
- gesture recognition,
- predictive maintenance,
- environmental monitoring.

For example:

```text
Accelerometer
     │
     ▼
   ESP32
     │
     ▼
Activity Classifier
     │
     ├── Sitting
     ├── Walking
     └── Running
```

---

# 🏷️ 10. Classification

Classification predicts a discrete category.

$$
\mathbf{x}
=
[x_1,x_2,\ldots,x_n]
$$

$$
\hat{y}=f(\mathbf{x})
$$

---

# 📈 11. Regression

Regression predicts a continuous numerical value:

$$
\hat{y}=f(\mathbf{x})
$$

---

# 🚨 12. Anomaly Detection

```text
Sensor Data
    │
    ▼
TinyML Model
    │
    ├── Normal
    │
    └── Anomaly
```

---

# 📡 13. Sensors for ESP32 TinyML

| Sensor | Data | Possible TinyML Application |
|---|---|---|
| MPU6050 | Acceleration + angular velocity | Activity recognition |
| Microphone | Audio waveform | Keyword recognition |
| Temperature sensor | Temperature | Anomaly detection |
| LDR | Light level | Environmental classification |
| Current sensor | Electrical current | Fault detection |
| Pressure sensor | Pressure | Condition monitoring |
| Camera | Image | Visual classification |

---

# 📐 14. Raw Sensor Data

For an accelerometer:

$$
a_x[n],\quad a_y[n],\quad a_z[n]
$$

Example:

```text
Time     Ax      Ay      Az
0.00     0.12    0.03    0.98
0.01     0.15    0.05    1.02
0.02     0.20    0.07    1.10
0.03     0.17    0.04    1.05
...
```

---

# 🧩 15. What Is a Feature?

A **feature** is a measurable property extracted from raw data that helps a machine-learning model distinguish between patterns or classes.

```text
Raw Accelerometer Data
         │
         ▼
Feature Extraction
         │
         ├── Mean
         ├── Variance
         ├── RMS
         ├── Maximum
         └── Energy
```

$$
\mathbf{x}
=
[
\mu,
\sigma^2,
x_{\mathrm{RMS}},
x_{\max},
E
]
$$

---

# 🎯 16. Why Feature Extraction?

Feature extraction can:

- reduce data dimensionality,
- reduce memory requirements,
- reduce computational requirements,
- remove irrelevant information,
- emphasize useful patterns,
- improve classification performance,
- simplify the model.

$$
\boxed{
\text{Raw Data}
\rightarrow
\text{Feature Extraction}
\rightarrow
\text{Compact Representation}
}
$$

---

# 🪟 17. Windowing Sensor Data

$$
N=f_sT_w
$$

For:

$$
f_s=100~\text{Hz},\qquad T_w=1~\text{s}
$$

then:

$$
N=100
$$

samples per window.

---

# 🔄 18. Overlapping Windows

```text
Signal
────────────────────────────────────►

Window 1
|-------------|

       Window 2
       |-------------|

              Window 3
              |-------------|
```

---

# 📊 19. Mean

$$
\mu
=
\frac{1}{N}
\sum_{i=1}^{N}x_i
$$

---

# 📈 20. Variance

$$
\sigma^2
=
\frac{1}{N}
\sum_{i=1}^{N}
(x_i-\mu)^2
$$

---

# 📏 21. Standard Deviation

$$
\sigma
=
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
(x_i-\mu)^2
}
$$

---

# ⚡ 22. Root Mean Square

$$
x_{\mathrm{RMS}}
=
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
x_i^2
}
$$

---

# 📊 23. Minimum and Maximum

$$
x_{\min}=\min(x_1,\ldots,x_N)
$$

$$
x_{\max}=\max(x_1,\ldots,x_N)
$$

$$
R=x_{\max}-x_{\min}
$$

---

# 🔋 24. Signal Energy

$$
E
=
\sum_{i=1}^{N}x_i^2
$$

$$
E_{\text{avg}}
=
\frac{1}{N}
\sum_{i=1}^{N}x_i^2
$$

---

# 📐 25. Magnitude Feature

$$
a_{\text{mag}}
=
\sqrt{
a_x^2+a_y^2+a_z^2
}
$$

---

# 📉 26. Zero-Crossing Rate

```text
     /\      /\
----/--\----/--\---- 0
  /      \/
```

---

# 🌊 27. Time-Domain Features

| Feature | Description |
|---|---|
| Mean | Average value |
| Variance | Signal dispersion |
| Standard deviation | Signal variability |
| RMS | Effective magnitude |
| Minimum | Lowest value |
| Maximum | Highest value |
| Range | Max − Min |
| Energy | Signal activity |
| Zero crossings | Sign-change behavior |

---

# 🌐 28. Frequency-Domain Features

$$
X[k]
=
\sum_{n=0}^{N-1}
x[n]
e^{-j2\pi kn/N}
$$

```text
Time-Domain Signal
        │
        ▼
       FFT
        │
        ▼
Frequency Spectrum
        │
        ▼
Frequency Features
```

---

# 🎵 29. Frequency-Domain Features

Possible features:

- dominant frequency,
- spectral energy,
- frequency-band energy,
- spectral centroid,
- spectral peaks.

---

# 🎙️ 30. Audio Feature Extraction

```text
Microphone
    │
    ▼
Audio Samples
    │
    ▼
Windowing
    │
    ▼
FFT
    │
    ▼
Spectral Processing
    │
    ▼
Feature Vector
    │
    ▼
TinyML Model
```

---

# 🧹 31. Data Preprocessing

Typical preprocessing includes:

- missing-value handling,
- filtering,
- normalization,
- scaling,
- segmentation,
- outlier inspection,
- synchronization,
- label verification.

$$
\boxed{
\text{Raw Data}
\rightarrow
\text{Cleaning}
\rightarrow
\text{Preprocessing}
\rightarrow
\text{Features}
}
$$

---

# 📏 32. Normalization

$$
x'
=
\frac{x-x_{\min}}
{x_{\max}-x_{\min}}
$$

---

# 📐 33. Standardization

$$
z
=
\frac{x-\mu}{\sigma}
$$

Training and deployment must use the same preprocessing parameters.

---

# 📚 34. What Is a Dataset?

$$
D=
\{
(\mathbf{x}_1,y_1),
(\mathbf{x}_2,y_2),
\ldots,
(\mathbf{x}_N,y_N)
\}
$$

Example:

| Mean Acceleration | Variance | RMS | Label |
|---:|---:|---:|---|
| 0.98 | 0.02 | 1.01 | Standing |
| 1.03 | 0.35 | 1.21 | Walking |
| 1.10 | 0.82 | 1.67 | Running |

---

# 🏷️ 35. Labels

Incorrect labels introduce incorrect information into the learning process.

$$
\boxed{
\text{Good Sensors}
+
\text{Wrong Labels}
\rightarrow
\text{Poor Training Data}
}
$$

---

# ⭐ 36. Dataset Quality

$$
\boxed{
\text{Model Quality}
\propto
\text{Data Quality}
}
$$

Important factors:

- correctness,
- representativeness,
- sufficient quantity,
- class balance,
- label accuracy,
- diversity,
- consistency,
- coverage of real operating conditions.

---

# 🎯 37. Representativeness

Training data should represent deployment conditions.

---

# ⚖️ 38. Class Balance

Example:

```text
Normal  = 950 samples
Fault   = 50 samples
```

A model predicting only `Normal` achieves:

$$
\frac{950}{1000}\times100=95\%
$$

accuracy but fails completely to detect faults.

---

# 📊 39. Class Distribution

Balanced:

```text
Standing = 1000
Walking  = 1000
Running  = 1000
```

Imbalanced:

```text
Standing = 2500
Walking  = 400
Running  = 100
```

---

# 🧹 40. Noisy Data

Possible issues:

- electrical noise,
- motion artifacts,
- communication errors,
- missing samples,
- sensor saturation,
- calibration errors,
- incorrect timestamps.

---

# 🚫 41. Missing Data

Example:

```text
Time    Temperature
0       28.1
1       28.2
2       ---
3       28.4
```

---

# 🏷️ 42. Label Noise

```text
Actual activity: WALKING
Dataset label:   RUNNING
```

---

# 🔀 43. Dataset Splitting

```text
Complete Dataset
      │
      ├── Training Set
      ├── Validation Set
      └── Test Set
```

Illustrative split:

```text
Training   = 70%
Validation = 15%
Test       = 15%
```

---

# 🏋️ 44. Training Set

$$
\boxed{
D_{\text{train}}
\rightarrow
\text{Learning Algorithm}
\rightarrow
\text{Model Parameters}
}
$$

---

# 🔧 45. Validation Set

Used for:

- selecting architecture,
- choosing hyperparameters,
- determining training duration,
- comparing feature sets,
- monitoring overfitting.

---

# 🧪 46. Test Set

```text
Trained Model
      │
      ▼
Unseen Test Data
      │
      ▼
Final Performance
```

---

# ⚠️ 47. Data Leakage

```text
Continuous Sensor Recording
          │
          ▼
Overlapping Windows
          │
          ├── Window A → Training
          └── Nearly Identical Window B → Test
```

---

# 📚 48. Data Diversity

Examples:

```text
Different users
Different speeds
Different body sizes
Different sensor orientations
Different environments
Different sessions
```

---

# 🔄 49. Data Augmentation

Possible techniques:

- realistic noise,
- time shifting,
- amplitude scaling,
- inertial-data rotation,
- cropping,
- controlled time warping.

$$
\boxed{
\text{Useful Augmentation}
=
\text{Realistic Variation}
+
\text{Correct Label}
}
$$

---

# 🧠 50. Feature Selection

$$
\boxed{
\text{Feature Selection}
\rightarrow
\text{Smaller Input}
\rightarrow
\text{Potentially Smaller Model}
}
$$

---

# 🔬 51. Feature Extraction vs. Feature Selection

$$
\boxed{
\text{Extraction}
\neq
\text{Selection}
}
$$

---

# 🧮 52. Neural Network Fundamentals

$$
z
=
\sum_{i=1}^{n}w_ix_i+b
$$

$$
y=f(z)
$$

---

# 🧠 53. Simple ANN Architecture

```text
Features
   │
   ▼
Input Layer
   │
   ▼
Hidden Layer
   │
   ▼
Output Layer
   │
   ▼
Prediction
```

---

# ⚙️ 54. Resource Constraints on ESP32

Important constraints include:

- RAM,
- flash memory,
- CPU performance,
- inference latency,
- energy consumption.

$$
\boxed{
\text{TinyML Design}
=
\text{Accuracy}
+
\text{Memory Efficiency}
+
\text{Computational Efficiency}
+
\text{Energy Efficiency}
}
$$

---

# 💾 55. Model Memory

$$
M_{\text{parameters}}
=
N_pB
$$

---

# 🔢 56. Quantization

```text
32-bit Floating Point Model
           │
           ▼
      Quantization
           │
           ▼
      8-bit Model
```

Potential benefits:

- smaller model,
- lower memory,
- efficient integer arithmetic,
- potentially faster inference.

---

# ⚖️ 57. TinyML Design Trade-Off

$$
\boxed{
\text{Best TinyML Model}
=
\text{Sufficient Accuracy}
+
\text{Feasible Embedded Cost}
}
$$

---

# 📊 58. Confusion Matrix

| Actual \ Predicted | Standing | Walking | Running |
|---|---:|---:|---:|
| Standing | 90 | 8 | 2 |
| Walking | 5 | 88 | 7 |
| Running | 1 | 9 | 90 |

---

# 🎯 59. Accuracy

$$
\text{Accuracy}
=
\frac{\text{Number of Correct Predictions}}
{\text{Total Predictions}}
$$

For binary classification:

$$
\text{Accuracy}
=
\frac{TP+TN}
{TP+TN+FP+FN}
$$

---

# 📈 60. Precision and Recall

$$
\text{Precision}
=
\frac{TP}
{TP+FP}
$$

$$
\text{Recall}
=
\frac{TP}
{TP+FN}
$$

---

# ⚖️ 61. F1-Score

$$
F_1
=
2
\frac{
\text{Precision}\times\text{Recall}
}{
\text{Precision}+\text{Recall}
}
$$

---

# 📉 62. Overfitting

$$
\boxed{
\text{Memorization}
\neq
\text{Generalization}
}
$$

---

# 📈 63. Underfitting

Possible causes:

- insufficient model capacity,
- weak features,
- inadequate training,
- poor-quality data.

---

# 🧪 64. On-Device Validation

```text
Offline Evaluation
       │
       ▼
Model Accuracy
       │
       ▼
Deploy to ESP32
       │
       ▼
On-Device Testing
       │
       ├── Accuracy
       ├── Latency
       ├── RAM
       ├── Flash
       └── Energy
```

---

# ⏱️ 65. Inference Latency

$$
T_{\text{inference}}
=
t_{\text{end}}-t_{\text{start}}
$$

---

# 📦 66. ESP32 TinyML Architecture

```text
MPU6050
   │
   │ I²C
   ▼
ESP32
   │
   ├── Sampling
   ├── Window Buffer
   ├── Feature Extraction
   ├── TinyML Inference
   └── Decision
          │
          ├── LED
          ├── BLE
          ├── Wi-Fi
          └── MQTT
```

---

# 🚶 67. Case Study: Activity Recognition

```text
MPU6050
   │
   ▼
Accelerometer Samples
   │
   ▼
1-Second Window
   │
   ▼
Feature Extraction
   │
   ├── Mean
   ├── Standard Deviation
   ├── RMS
   └── Energy
   │
   ▼
Classifier
   │
   ▼
Activity
```

---

# 📐 68. Feature Vector Example

$$
\mathbf{x}
=
[
\mu_x,\sigma_x,RMS_x,
\mu_y,\sigma_y,RMS_y,
\mu_z,\sigma_z,RMS_z
]
$$

---

# 💻 69. ESP32 Feature Extraction Example

```cpp
float calculateRMS(float *data, int N)
{
    float sum = 0.0;

    for (int i = 0; i < N; i++)
    {
        sum += data[i] * data[i];
    }

    return sqrt(sum / N);
}
```

```cpp
float calculateMean(float *data, int N)
{
    float sum = 0.0;

    for (int i = 0; i < N; i++)
    {
        sum += data[i];
    }

    return sum / N;
}
```

---

# 🔄 70. Training and Deployment Consistency

$$
\boxed{
\text{Training Preprocessing}
=
\text{Deployment Preprocessing}
}
$$

---

# 🧪 71. Dataset Collection Experiment

Collect:

| Activity | Duration | Number of Trials |
|---|---:|---:|
| Standing | 2 min | 5 |
| Walking | 2 min | 5 |
| Running | 2 min | 5 |

Record:

```text
timestamp, ax, ay, az, gx, gy, gz, label
```

Example:

```csv
0.00,0.02,0.01,0.99,0.1,0.2,0.1,standing
0.01,0.03,0.02,1.01,0.2,0.1,0.2,standing
```

---

# 🔍 72. Dataset Inspection Exercise

Inspect:

- number of samples,
- missing values,
- class distribution,
- minimum values,
- maximum values,
- mean values,
- possible sensor errors,
- incorrect labels.

---

# 📊 73. Feature Extraction Exercise

Calculate:

$$
\mu,\quad \sigma,\quad RMS,\quad x_{\max},\quad x_{\min}
$$

---

# 🧪 74. Dataset Split Exercise

Example:

```text
Training   = 70%
Validation = 15%
Test       = 15%
```

Check:

1. Are all classes represented?
2. Are samples from the same recording leaking across sets?
3. Are test examples independent?
4. Is the class distribution appropriate?
5. Does the split represent deployment conditions?

---

# 🤖 75. Model Training Exercise

Train a small classifier such as:

- logistic regression,
- decision tree,
- small neural network.

```python
model.fit(X_train, y_train)
```

```python
prediction = model.predict(X_test)
```

---

# 📊 76. Compare Feature Sets

| Model | Features | Accuracy | Model Size | Inference Time |
|---|---:|---:|---:|---:|
| A | Mean | | | |
| B | Mean + STD | | | |
| C | Mean + STD + RMS + Energy | | | |

---

# ⚠️ 77. Dataset Quality Experiment

Compare:

```text
Dataset A
Clean + Balanced

Dataset B
Clean + Imbalanced

Dataset C
Noisy + Incorrect Labels
```

Evaluate:

- accuracy,
- precision,
- recall,
- F1-score,
- confusion matrix.

---

# 🚀 78. Advanced Exercise — ESP32 Activity Recognition

### Hardware

```text
MPU6050
   │
   │ I²C
   ▼
ESP32
```

### Processing

```text
Accelerometer
     │
     ▼
Sampling
     │
     ▼
Windowing
     │
     ▼
Feature Extraction
     │
     ▼
TinyML Model
     │
     ▼
Classification
```

### Output

```text
Standing → LED 1
Walking  → LED 2
Running  → LED 3
```

Optional:

```text
Prediction
    │
    ▼
BLE / Wi-Fi / MQTT
    │
    ▼
Dashboard
```

---

# 🪞 79. TinyML and Digital Twins

```text
Physical System
      │
      ▼
Sensors
      │
      ▼
ESP32
      │
      ▼
TinyML
      │
      ▼
State Classification
      │
      ▼
MQTT
      │
      ▼
Digital Twin
```

---

# 📡 80. TinyML + MQTT

```text
Sensor
  │
  ▼
ESP32
  │
  ▼
TinyML
  │
  ▼
Prediction
  │
  ▼
MQTT Publish
  │
  ▼
Broker
  │
  ├── Dashboard
  ├── Database
  └── Alert System
```

Example:

```json
{
  "device": "esp32-01",
  "activity": "walking",
  "confidence": 0.94
}
```

---

# 🔋 81. TinyML + Sleep Modes

```text
Deep Sleep
    │
    ▼
Wake
    │
    ▼
Collect Sensor Data
    │
    ▼
Feature Extraction
    │
    ▼
Inference
    │
    ▼
Important Event?
   / \
 No   Yes
 │     │
Sleep  Transmit
        │
        ▼
       Sleep
```

---

# 💬 82. Discussion Questions

1. What is TinyML?
2. How does TinyML differ from cloud AI?
3. How does TinyML relate to edge AI?
4. Why is the ESP32 useful for TinyML?
5. What is the difference between training and inference?
6. Why is training usually performed off-device?
7. What is a feature?
8. Why is feature extraction important?
9. What is windowing?
10. Why are overlapping windows used?
11. What is the mean of a signal?
12. What does variance represent?
13. What is RMS?
14. What is signal energy?
15. What is acceleration magnitude?
16. What are time-domain features?
17. What are frequency-domain features?
18. What is an FFT?
19. What is data preprocessing?
20. Why must preprocessing be identical during training and deployment?
21. What is normalization?
22. What is standardization?
23. What is dataset quality?
24. Why is class balance important?
25. Why can accuracy be misleading?
26. What is label noise?
27. What is data leakage?
28. Why should a test set remain independent?
29. What is data augmentation?
30. What is the difference between feature extraction and feature selection?
31. What is model quantization?
32. Why is quantization useful for TinyML?
33. What is overfitting?
34. What is underfitting?
35. What is a confusion matrix?
36. What is precision?
37. What is recall?
38. What is F1-score?
39. Why is on-device validation necessary?
40. How can TinyML reduce IoT communication energy?

---

# 🧾 83. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain the fundamentals of TinyML,
- distinguish TinyML, edge AI, and cloud AI,
- explain training and inference,
- describe the TinyML development pipeline,
- collect sensor datasets using ESP32,
- explain feature extraction,
- segment sensor data using windows,
- calculate mean, variance, standard deviation, RMS, and energy,
- explain time-domain and frequency-domain features,
- describe FFT-based feature extraction,
- preprocess sensor datasets,
- normalize and standardize features,
- explain dataset quality,
- identify class imbalance,
- identify label noise,
- identify missing and corrupted data,
- explain data leakage,
- create training, validation, and test sets,
- explain data augmentation,
- distinguish feature extraction from feature selection,
- describe simple neural-network inference,
- explain MCU resource constraints,
- explain model quantization,
- evaluate classification models,
- calculate accuracy, precision, recall, and F1-score,
- interpret a confusion matrix,
- identify overfitting and underfitting,
- measure inference latency,
- deploy a small ML model to an ESP32,
- integrate TinyML with BLE, Wi-Fi, MQTT, and sleep modes.

---

# 🔑 84. Key Concepts

### TinyML

$$
\boxed{
\text{TinyML}
=
\text{Machine Learning}
+
\text{Microcontroller}
}
$$

### TinyML Pipeline

$$
\boxed{
\text{Data}
\rightarrow
\text{Preprocessing}
\rightarrow
\text{Features}
\rightarrow
\text{Model}
\rightarrow
\text{Inference}
}
$$

### Feature Extraction

$$
\boxed{
\text{Raw Sensor Data}
\rightarrow
\text{Feature Extraction}
\rightarrow
\text{Feature Vector}
}
$$

### Mean

$$
\boxed{
\mu=
\frac{1}{N}\sum_{i=1}^{N}x_i
}
$$

### Variance

$$
\boxed{
\sigma^2
=
\frac{1}{N}
\sum_{i=1}^{N}(x_i-\mu)^2
}
$$

### RMS

$$
\boxed{
x_{\mathrm{RMS}}
=
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}x_i^2
}
}
$$

### Dataset

$$
\boxed{
D=
\{(\mathbf{x}_i,y_i)\}_{i=1}^{N}
}
$$

### Neural Inference

$$
\boxed{
z=\sum_iw_ix_i+b,
\qquad
y=f(z)
}
$$

### Embedded AI Design

$$
\boxed{
\text{TinyML Performance}
=
f(
\text{Data Quality},
\text{Features},
\text{Model},
\text{Hardware}
)
}
$$

---

# 🎯 85. Summary

TinyML enables machine-learning inference to operate directly on resource-constrained devices such as the **ESP32**, allowing sensor data to be interpreted close to where it is generated.

A typical ESP32 TinyML pipeline is:

$$
\boxed{
\text{Sensor}
\rightarrow
\text{Sampling}
\rightarrow
\text{Windowing}
\rightarrow
\text{Feature Extraction}
\rightarrow
\text{Inference}
\rightarrow
\text{Decision}
}
$$

**Feature extraction** transforms raw sensor measurements into compact and informative representations. Common features include:

- mean,
- variance,
- standard deviation,
- RMS,
- minimum,
- maximum,
- signal energy,
- frequency-domain characteristics.

However, the machine-learning model depends strongly on the quality of the dataset used during development. Important considerations include:

$$
\boxed{
\text{Dataset Quality}
=
\text{Correct Labels}
+
\text{Representative Samples}
+
\text{Class Coverage}
+
\text{Data Diversity}
}
$$

Training, validation, and test datasets should be carefully separated, and **data leakage** should be avoided. For time-series sensor applications, highly overlapping windows from the same recording require particular care when splitting datasets.

TinyML models must also satisfy embedded constraints such as:

- flash usage,
- RAM usage,
- inference latency,
- computational complexity,
- energy consumption.

Therefore, TinyML engineering is not simply about maximizing classification accuracy. A practical objective is:

$$
\boxed{
\text{Effective TinyML}
=
\text{Good Dataset}
+
\text{Useful Features}
+
\text{Compact Model}
+
\text{Efficient Deployment}
}
$$

Combining TinyML with ESP32 connectivity and power-management capabilities enables intelligent systems such as:

```text
Sensor
   │
   ▼
ESP32
   │
   ├── Feature Extraction
   ├── TinyML Inference
   └── Decision
          │
          ├── Local Action
          ├── BLE
          ├── Wi-Fi / MQTT
          └── LPWAN Gateway
```

These principles provide the foundation for developing **activity-recognition systems, predictive-maintenance devices, intelligent sensor nodes, wearable systems, anomaly detectors, low-power IoT devices, and digital-twin endpoints**.

---

# 📘 References

1. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*, O'Reilly Media, 2019.
2. R. David et al., “TensorFlow Lite Micro: Embedded Machine Learning for TinyML Systems,” *Proceedings of Machine Learning and Systems*, vol. 3, pp. 800–811, 2021.
3. J. Lin et al., “MCUNet: Tiny Deep Learning on IoT Devices,” *Advances in Neural Information Processing Systems*, 2020.
4. Espressif Systems, *ESP-DL User Guide and Documentation*.
5. Espressif Systems, *ESP32 Series Datasheets and Technical Reference Manuals*.
6. TensorFlow, *TensorFlow Lite for Microcontrollers Documentation*.
7. ARM, *CMSIS-NN: Efficient Neural Network Kernels for Arm Cortex-M CPUs*.
8. S. Haykin, *Neural Networks and Learning Machines*, Pearson.
9. A. Géron, *Hands-On Machine Learning with Scikit-Learn, Keras, and TensorFlow*, O'Reilly Media.
10. J. P. Vasseur and A. Dunkels, *Interconnecting Smart Objects with IP: The Next Internet*, Morgan Kaufmann.

---

