# LAB 14 — TinyML Gesture Recognition: Training, Optimization & MCU Deployment

## 1. Objective

The objective of this lab is to:

1. Use the IMU gesture dataset collected in the previous lab.
2. Prepare fixed-length IMU windows for machine-learning training.
3. Normalize and encode the input data.
4. Build a compact neural network for gesture classification.
5. Train and validate the TinyML model.
6. Evaluate classification accuracy using a test dataset.
7. Convert the trained model to TensorFlow Lite.
8. Apply post-training quantization for MCU deployment.
9. Compare floating-point and quantized model size and accuracy.
10. Convert the TensorFlow Lite model to a C array.
11. Integrate the model into ESP32 firmware.
12. Perform real-time gesture inference on the MCU.
13. Measure inference latency and memory requirements.
14. Use the predicted gesture to control an embedded output.

---

## 2. Background

In the previous lab, IMU data were collected from the MPU6050 and organized into labeled gesture windows.

The next stage of the TinyML workflow is:

```text
IMU Dataset
     ↓
Data Preparation
     ↓
Model Training
     ↓
Model Evaluation
     ↓
Optimization
     ↓
TensorFlow Lite
     ↓
Quantization
     ↓
MCU Deployment
     ↓
Real-Time Inference
```

TinyML differs from conventional machine learning because the trained model must operate within the limited resources of a microcontroller.

Typical constraints include:

- Limited Flash memory
- Limited RAM
- Limited CPU performance
- Low power consumption
- Real-time response requirements

Therefore, a TinyML model should be:

```text
Accurate
   +
Small
   +
Fast
   +
Memory Efficient
```

---

## 3. System Architecture

The complete system is:

```text
                   Training Phase

IMU Gesture Dataset
        ↓
Preprocessing
        ↓
Neural Network Training
        ↓
Model Evaluation
        ↓
Quantization
        ↓
TensorFlow Lite Model
        ↓
C/C++ Model Array

────────────────────────────────────

                   Deployment Phase

MPU6050
   ↓
ESP32
   ↓
100 × 6 IMU Window
   ↓
Normalization
   ↓
TinyML Model
   ↓
Inference
   ↓
Gesture Class
   ↓
Embedded Action
```

---

## 4. Hardware & Software Requirements

### Hardware

- ESP32 development board
- MPU6050 IMU
- USB cable
- Optional LEDs
- Optional buzzer or actuator

### Training Software

- Python
- TensorFlow
- NumPy
- Pandas
- Matplotlib
- Scikit-learn

### Embedded Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- TensorFlow Lite for Microcontrollers or a compatible embedded inference runtime

---

## 5. Input Dataset

Use the dataset generated in LAB 13.

A raw CSV file may contain:

```csv
gesture_id,sample_id,timestamp,ax,ay,az,gx,gy,gz,label
1,1,1000,0.21,-0.35,9.72,0.02,-0.01,0.05,left
1,2,1020,0.25,-0.40,9.68,0.04,-0.03,0.10,left
1,3,1040,0.45,-0.62,9.20,0.12,-0.08,0.35,left
```

Each gesture sequence contains:

```text
100 samples × 6 channels
```

Therefore:

$$
\mathbf{X}
\in
\mathbb{R}^{100\times6}.
$$

The six input channels are:

$$
[a_x,a_y,a_z,g_x,g_y,g_z].
$$

---

## 6. Gesture Classes

Assume five gesture classes:

```text
0 → stationary
1 → left
2 → right
3 → up
4 → down
```

The classification output is:

$$
\hat{y}
= \arg\max_k p_k,
$$

where $p_k$ is the predicted probability of gesture class $k$.

---

## 7. Experiment 1 — Load the Dataset

Example Python code:

```python
import pandas as pd

df = pd.read_csv("gesture_dataset.csv")

print(df.head())
print(df.shape)
print(df["label"].value_counts())
```

Expected output may resemble:

```text
stationary    5000
left          5000
right         5000
up            5000
down          5000
```

The exact values depend on the collected dataset.

---

## 8. Experiment 2 — Inspect Dataset Quality

Check for missing values:

```python
print(df.isnull().sum())
```

Inspect available labels:

```python
print(df["label"].unique())
```

Check class balance:

```python
print(
    df["label"].value_counts()
)
```

A balanced dataset should contain approximately similar numbers of gesture examples in each class.

---

## 9. Experiment 3 — Construct Gesture Windows

Each `gesture_id` represents one gesture sequence.

Example:

```python
import numpy as np

features = [
    "ax",
    "ay",
    "az",
    "gx",
    "gy",
    "gz"
]

X = []
y = []

for gesture_id, group in df.groupby(
    "gesture_id"
):
    data = group[features].values

    if len(data) == 100:
        X.append(data)
        y.append(
            group["label"].iloc[0]
        )

X = np.array(X)

print(X.shape)
```

Expected structure:

```text
(number_of_gestures, 100, 6)
```

For example:

```text
(250, 100, 6)
```

---

## 10. Label Encoding

Convert text labels into integer classes.

```python
from sklearn.preprocessing import LabelEncoder

encoder = LabelEncoder()

y_encoded = encoder.fit_transform(y)

print(
    encoder.classes_
)
```

Possible output:

```text
['down', 'left', 'right', 'stationary', 'up']
```

The exact integer mapping should be recorded because the MCU must use the same mapping.

---

## 11. Training, Validation, and Test Split

Separate the dataset into independent subsets.

For example:

```text
Training   = 70%
Validation = 15%
Testing    = 15%
```

Example:

```python
from sklearn.model_selection import train_test_split

X_train, X_temp, y_train, y_temp = train_test_split(
    X,
    y_encoded,
    test_size=0.30,
    stratify=y_encoded,
    random_state=42
)

X_val, X_test, y_val, y_test = train_test_split(
    X_temp,
    y_temp,
    test_size=0.50,
    stratify=y_temp,
    random_state=42
)
```

The data flow becomes:

```text
Complete Dataset
      ↓
 ┌────┴─────┐
 │          │
70%       30%
Train      Temporary
             ↓
        ┌────┴────┐
        │         │
      15%       15%
 Validation    Test
```

---

## 12. Data Normalization

IMU channels can have different numerical ranges.

Standardization is:

$$
x'
= \frac{x-\mu}{\sigma}.
$$

The mean and standard deviation should be calculated using the **training dataset only**.

Example:

```python
mean = X_train.mean(
    axis=(0, 1),
    keepdims=True
)

std = X_train.std(
    axis=(0, 1),
    keepdims=True
)

std[std == 0] = 1.0

X_train_norm = (
    X_train - mean
) / std

X_val_norm = (
    X_val - mean
) / std

X_test_norm = (
    X_test - mean
) / std
```

This avoids leaking test-set information into the training process.

---

## 13. Save Normalization Parameters

The same normalization must later be applied on the MCU.

For each channel, store:

```text
Mean
Standard deviation
```

For example:

| Channel | Mean | Standard Deviation |
|---|---:|---:|
| ax |  |  |
| ay |  |  |
| az |  |  |
| gx |  |  |
| gy |  |  |
| gz |  |  |

The ESP32 will later calculate:

$$
x_i'
= \frac{x_i-\mu_i}{\sigma_i}.
$$

---

## 14. Experiment 4 — Build a Compact Neural Network

A simple fully connected neural network can be used for the first TinyML model.

The input shape is:

```text
100 × 6
```

Flatten it into:

$$
100\times6=600
$$

features.

Example:

```python
import tensorflow as tf

model = tf.keras.Sequential([
    tf.keras.layers.Input(
        shape=(100, 6)
    ),

    tf.keras.layers.Flatten(),

    tf.keras.layers.Dense(
        64,
        activation="relu"
    ),

    tf.keras.layers.Dense(
        32,
        activation="relu"
    ),

    tf.keras.layers.Dense(
        5,
        activation="softmax"
    )
])
```

Architecture:

```text
100 × 6 IMU Window
        ↓
     Flatten
        ↓
      600
        ↓
   Dense 64
        ↓
   Dense 32
        ↓
   Dense 5
        ↓
Gesture Class
```

---

## 15. Neural Network Mathematics

For a dense layer:

$$
\mathbf{z}
= \mathbf{W}\mathbf{x}
+
\mathbf{b}.
$$

The ReLU activation is:

$$
f(z)
= \max(0,z).
$$

The output layer uses softmax:

$$
p_k
= \frac{e^{z_k}}
{\sum_{j=1}^{C}e^{z_j}},
$$

where:

- $C$ = number of gesture classes
- $p_k$ = probability of class $k$

The predicted class is:

$$
\hat{y}
= \arg\max_k p_k.
$$

---

## 16. Experiment 5 — Compile the Model

```python
model.compile(
    optimizer="adam",
    loss="sparse_categorical_crossentropy",
    metrics=["accuracy"]
)

model.summary()
```

The summary provides:

- Number of layers
- Output shapes
- Number of parameters

Model complexity is important because the final model must fit into MCU memory.

---

## 17. Experiment 6 — Train the Model

Example:

```python
history = model.fit(
    X_train_norm,
    y_train,
    validation_data=(
        X_val_norm,
        y_val
    ),
    epochs=50,
    batch_size=16
)
```

During training, monitor:

```text
Training Loss
Training Accuracy
Validation Loss
Validation Accuracy
```

---

## 18. Training Accuracy

Classification accuracy is:

$$
\text{Accuracy}
= \frac{
N_{\text{correct}}
}{
N_{\text{total}}
}
\times100\%.
$$

For example, if 92 of 100 gestures are classified correctly:

$$
\text{Accuracy}
= \frac{92}{100}
\times100\%
= 92\%.
$$

---

## 19. Experiment 7 — Plot Training History

```python
import matplotlib.pyplot as plt

plt.figure()

plt.plot(
    history.history["accuracy"],
    label="Training"
)

plt.plot(
    history.history["val_accuracy"],
    label="Validation"
)

plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.legend()
plt.show()
```

A second plot can show loss:

```python
plt.figure()

plt.plot(
    history.history["loss"],
    label="Training"
)

plt.plot(
    history.history["val_loss"],
    label="Validation"
)

plt.xlabel("Epoch")
plt.ylabel("Loss")
plt.legend()
plt.show()
```

Inspect whether the model is:

```text
Underfitting
     ↓
Well Fitted
     ↓
Overfitting
```

---

## 20. Overfitting

Overfitting occurs when training performance continues improving while validation performance deteriorates.

Typical behavior:

```text
Training Accuracy
      ↑
      │          ─────────
      │       /
      │     /
      │   /
      └──────────────────► Epoch

Validation Accuracy
      ↑
      │       ─────
      │      /     \
      │     /       \
      │
      └──────────────────► Epoch
```

Possible strategies include:

- Fewer epochs
- Smaller network
- Dropout
- More training data
- Better gesture consistency
- Data augmentation

---

## 21. Experiment 8 — Add Early Stopping

```python
early_stop = tf.keras.callbacks.EarlyStopping(
    monitor="val_loss",
    patience=8,
    restore_best_weights=True
)
```

Train:

```python
history = model.fit(
    X_train_norm,
    y_train,
    validation_data=(
        X_val_norm,
        y_val
    ),
    epochs=100,
    batch_size=16,
    callbacks=[early_stop]
)
```

Early stopping reduces unnecessary training once validation performance stops improving.

---

## 22. Experiment 9 — Evaluate the Floating-Point Model

```python
test_loss, test_accuracy = model.evaluate(
    X_test_norm,
    y_test
)

print(
    "Test accuracy:",
    test_accuracy
)
```

Record:

| Metric | Result |
|---|---:|
| Training accuracy |  |
| Validation accuracy |  |
| Test accuracy |  |
| Test loss |  |

The test dataset should only be used for final evaluation.

---

## 23. Experiment 10 — Confusion Matrix

Generate predictions:

```python
import numpy as np

predictions = model.predict(
    X_test_norm
)

y_pred = np.argmax(
    predictions,
    axis=1
)
```

Create a confusion matrix:

```python
from sklearn.metrics import confusion_matrix

cm = confusion_matrix(
    y_test,
    y_pred
)

print(cm)
```

Example structure:

| Actual \ Predicted | Stationary | Left | Right | Up | Down |
|---|---:|---:|---:|---:|---:|
| Stationary |  |  |  |  |  |
| Left |  |  |  |  |  |
| Right |  |  |  |  |  |
| Up |  |  |  |  |  |
| Down |  |  |  |  |  |

The confusion matrix reveals which gestures are difficult for the classifier to distinguish.

---

## 24. TinyML Model Optimization

A floating-point TensorFlow model may be too large or computationally expensive for a small MCU.

Optimization techniques include:

```text
Model Reduction
     ↓
Quantization
     ↓
Smaller Memory
     ↓
Faster Inference
```

Common optimization strategies include:

- Reducing layer size
- Reducing number of layers
- Quantization
- Pruning
- Feature reduction

This lab focuses primarily on **post-training quantization**.

---

## 25. TensorFlow Lite Conversion

Convert the trained Keras model:

```python
converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

tflite_model = converter.convert()
```

Save:

```python
with open(
    "gesture_model.tflite",
    "wb"
) as f:
    f.write(
        tflite_model
    )
```

The pipeline becomes:

```text
Keras Model
    ↓
TFLite Converter
    ↓
gesture_model.tflite
```

---

## 26. Experiment 11 — Measure Model Size

```python
import os

size_bytes = os.path.getsize(
    "gesture_model.tflite"
)

print(
    "Model size:",
    size_bytes,
    "bytes"
)
```

Convert to kilobytes:

$$
S_{\text{kB}}
= \frac{S_{\text{bytes}}}{1024}.
$$

Record the floating-point TFLite model size.

---

## 27. Post-Training Quantization

Quantization reduces numerical precision.

A common transformation is:

```text
Float32
   ↓
INT8
```

Float32 weights typically require:

```text
32 bits / parameter
```

while INT8 weights require:

```text
8 bits / parameter
```

This can substantially reduce model-storage requirements.

---

## 28. Quantization Model

A quantized value can be represented approximately as:

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

- $x$ = floating-point value
- $q$ = quantized integer
- $s$ = scale
- $z$ = zero point

Dequantization is:

$$
x
\approx
s(q-z).
$$

---

## 29. Experiment 12 — Dynamic-Range Quantization

A simple optimization is:

```python
converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

converter.optimizations = [
    tf.lite.Optimize.DEFAULT
]

quant_model = converter.convert()
```

Save:

```python
with open(
    "gesture_model_quant.tflite",
    "wb"
) as f:
    f.write(
        quant_model
    )
```

Compare:

```text
gesture_model.tflite
gesture_model_quant.tflite
```

---

## 30. Full Integer Quantization

For integer-only inference, a representative dataset is required.

```python
def representative_dataset():

    for i in range(
        min(
            100,
            len(X_train_norm)
        )
    ):

        sample = X_train_norm[
            i:i+1
        ].astype(
            np.float32
        )

        yield [sample]
```

Configure:

```python
converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

converter.optimizations = [
    tf.lite.Optimize.DEFAULT
]

converter.representative_dataset = (
    representative_dataset
)

converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]

converter.inference_input_type = (
    tf.int8
)

converter.inference_output_type = (
    tf.int8
)

int8_model = converter.convert()
```

Save:

```python
with open(
    "gesture_model_int8.tflite",
    "wb"
) as f:

    f.write(
        int8_model
    )
```

---

## 31. Float vs. Quantized Model

Record the results:

| Model | Numeric Format | Size | Accuracy |
|---|---|---:|---:|
| Original Keras | Float32 |  |  |
| TFLite | Float32 |  |  |
| Quantized TFLite | Quantized |  |  |
| Full integer | INT8 |  |  |

The goal is to obtain:

```text
Smaller Model
     +
Similar Accuracy
```

---

## 32. Model Compression Ratio

The model-size reduction can be calculated as:

$$
R
= \frac{
S_{\text{float}}
}{
S_{\text{quant}}
}.
$$

For example:

$$
S_{\text{float}}
= 100~\text{kB}
$$

and:

$$
S_{\text{quant}}
= 30~\text{kB}.
$$

Then:

$$
R
= \frac{100}{30}
\approx
3.33.
$$

The quantized model is approximately 3.33 times smaller.

---

## 33. Accuracy Loss Due to Quantization

Define:

$$
\Delta A
= A_{\text{float}}
-
A_{\text{quant}}.
$$

For example:

$$
A_{\text{float}}
= 94.0\%
$$

and:

$$
A_{\text{quant}}
= 93.2\%.
$$

Then:

$$
\Delta A
= 0.8\%.
$$

A small reduction in accuracy may be acceptable if memory and inference efficiency improve substantially.

---

## 34. Experiment 13 — Evaluate the Quantized Model

Use the TensorFlow Lite interpreter:

```python
interpreter = tf.lite.Interpreter(
    model_path=
    "gesture_model_int8.tflite"
)

interpreter.allocate_tensors()

input_details = (
    interpreter.get_input_details()
)

output_details = (
    interpreter.get_output_details()
)

print(input_details)
print(output_details)
```

For a fully quantized model, the input requires INT8 conversion using the input scale and zero point.

Conceptually:

$$
q_{\text{input}}
= \operatorname{round}
\left(
\frac{x}{s_{\text{input}}}
\right)
+
z_{\text{input}}.
$$

---

## 35. Export Model as a C Array

Microcontroller firmware usually embeds the `.tflite` file as a byte array.

One possible command-line method is:

```bash
xxd -i gesture_model_int8.tflite > gesture_model.h
```

The output resembles:

```cpp
unsigned char gesture_model_int8_tflite[] = {
  0x20, 0x00, 0x00, 0x00,
  0x54, 0x46, 0x4c, 0x33
  // ...
};

unsigned int gesture_model_int8_tflite_len =
    12345;
```

The resulting header can be included in MCU firmware.

---

## 36. Embedded Model Architecture

The MCU inference pipeline becomes:

```text
MPU6050
    ↓
Collect 100 Samples
    ↓
6 IMU Channels
    ↓
Normalization
    ↓
Input Tensor
    ↓
TinyML Interpreter
    ↓
Neural Network
    ↓
Output Tensor
    ↓
Gesture Prediction
```

---

## 37. Experiment 14 — Collect Inference Window on ESP32

Create:

```cpp
#define WINDOW_SIZE 100
#define NUM_FEATURES 6
```

Buffer:

```cpp
float imuWindow[
    WINDOW_SIZE
][
    NUM_FEATURES
];
```

Collect:

```text
100 samples
```

at:

```text
50 Hz
```

giving:

$$
T_w
= \frac{100}{50}
= 2~\text{s}.
$$

---

## 38. Embedded Normalization

The ESP32 must reproduce the training preprocessing.

Store:

```cpp
float meanValues[6] = {
    // ax, ay, az, gx, gy, gz
};

float stdValues[6] = {
    // ax, ay, az, gx, gy, gz
};
```

Normalize:

```cpp
normalized =
    (value - meanValues[j]) /
    stdValues[j];
```

This step is essential.

If training and MCU preprocessing are different, model accuracy can decrease significantly.

---

## 39. Input Tensor

For a floating-point model:

```cpp
input->data.f[index] =
    normalized;
```

For a quantized INT8 model, the normalized value must be quantized using the model input tensor parameters.

Conceptually:

$$
q
= \operatorname{round}
\left(
\frac{x}{s}
\right)
+
z.
$$

Then constrain the result to the supported INT8 range.

---

## 40. Experiment 15 — Run Inference

Conceptual TensorFlow Lite Micro code:

```cpp
TfLiteStatus invoke_status =
    interpreter->Invoke();

if (
  invoke_status != kTfLiteOk
) {

  Serial.println(
    "Inference failed"
  );

  return;
}
```

After successful inference, inspect the model output.

---

## 41. Gesture Classification

Suppose the output probabilities are:

```text
stationary = 0.02
left       = 0.85
right      = 0.05
up         = 0.04
down       = 0.04
```

Then:

$$
\hat{y}
= \arg\max
[
0.02,
0.85,
0.05,
0.04,
0.04
].
$$

Therefore:

```text
Predicted Gesture = LEFT
```

---

## 42. Confidence Score

The maximum model output can be treated as a confidence-like score:

$$
C
= \max_k p_k.
$$

For:

$$
C=0.85,
$$

the model strongly favors the predicted class.

A threshold can be introduced:

```text
If confidence ≥ 0.70
    Accept prediction

If confidence < 0.70
    Unknown / Ignore
```

This can reduce unintended actuator commands.

---

## 43. Experiment 16 — Gesture-Controlled LED

Map gestures to actions:

| Gesture | Action |
|---|---|
| Left | LED 1 ON |
| Right | LED 2 ON |
| Up | Both LEDs ON |
| Down | LEDs OFF |
| Stationary | No change |

Example:

```cpp
if (
  predictedClass == LEFT
) {

  digitalWrite(
    LED1,
    HIGH
  );

}
else if (
  predictedClass == RIGHT
) {

  digitalWrite(
    LED2,
    HIGH
  );
}
```

The complete intelligent embedded system becomes:

```text
Hand Gesture
     ↓
MPU6050
     ↓
ESP32
     ↓
TinyML Inference
     ↓
Gesture
     ↓
LED / Actuator
```

---

## 44. Inference Latency

Measure inference time using:

```cpp
unsigned long start =
    micros();

interpreter->Invoke();

unsigned long end =
    micros();

unsigned long latency =
    end - start;
```

Inference latency is:

$$
T_{\text{inf}}
= t_{\text{end}}
- t_{\text{start}}.
$$

Record:

| Test | Gesture | Inference Latency |
|---:|---|---:|
| 1 | Left |  |
| 2 | Right |  |
| 3 | Up |  |
| 4 | Down |  |
| 5 | Stationary |  |

---

## 45. Average Inference Latency

For $N$ tests:

$$
\bar{T}_{\text{inf}}
= \frac{1}{N}
\sum_{i=1}^{N}
T_i.
$$

Lower inference latency improves responsiveness.

---

## 46. Memory Requirements

TinyML deployment involves several memory components:

```text
Flash
  ↓
Model + Program

RAM
  ↓
Input Buffer
Output Buffer
Tensor Arena
Runtime Variables
```

Monitor available ESP32 heap:

```cpp
Serial.println(
  ESP.getFreeHeap()
);
```

Record:

| Resource | Value |
|---|---:|
| Model size |  |
| Free heap before inference |  |
| Free heap after initialization |  |
| Input tensor size |  |
| Tensor arena |  |
| Inference latency |  |

---

## 47. Model Size vs. Accuracy Trade-Off

A larger network may improve classification accuracy but require:

```text
More Flash
More RAM
More Computation
More Energy
```

A smaller model may provide:

```text
Lower Accuracy
        ↓
but
        ↓
Lower Memory
Lower Latency
Lower Energy
```

TinyML design therefore involves balancing:

$$
\boxed{
\text{Accuracy}
\leftrightarrow
\text{Memory}
\leftrightarrow
\text{Latency}
\leftrightarrow
\text{Energy}
}
$$

---

## 48. Experiment 17 — Compare Model Architectures

Train several models.

### Model A

```text
Flatten
  ↓
Dense 32
  ↓
Output
```

### Model B

```text
Flatten
  ↓
Dense 64
  ↓
Dense 32
  ↓
Output
```

### Model C

```text
Flatten
  ↓
Dense 128
  ↓
Dense 64
  ↓
Output
```

Compare:

| Model | Parameters | Test Accuracy | TFLite Size |
|---|---:|---:|---:|
| A |  |  |  |
| B |  |  |  |
| C |  |  |  |

Select the smallest model that provides acceptable performance.

---

## 49. Experiment 18 — Float vs. INT8 Deployment

Compare the floating-point and quantized implementations.

Record:

| Metric | Float Model | INT8 Model |
|---|---:|---:|
| Accuracy |  |  |
| Model size |  |  |
| Inference latency |  |  |
| RAM usage |  |  |
| Accuracy loss | — |  |

Discuss whether quantization is beneficial for the target MCU.

---

## 50. Complete TinyML Workflow

The full workflow from LAB 13 to LAB 14 is:

```text
Physical Gestures
      ↓
MPU6050
      ↓
IMU Measurements
      ↓
CSV Dataset
      ↓
Data Cleaning
      ↓
Windowing
      ↓
Normalization
      ↓
Training
      ↓
Validation
      ↓
Testing
      ↓
Optimization
      ↓
INT8 Quantization
      ↓
TFLite Model
      ↓
C Array
      ↓
ESP32
      ↓
Real-Time IMU
      ↓
TinyML Inference
      ↓
Gesture Classification
      ↓
Embedded Action
```

---

## 51. Experiments

### Experiment 1 — Dataset Loading

Load the LAB 13 CSV dataset and inspect class distributions.

### Experiment 2 — Window Construction

Convert the data into:

```text
N × 100 × 6
```

input tensors.

### Experiment 3 — Normalization

Calculate training-set mean and standard deviation and normalize all inputs.

### Experiment 4 — Model Creation

Create a compact neural network for five gesture classes.

### Experiment 5 — Model Training

Train using training and validation data.

### Experiment 6 — Test Evaluation

Calculate test accuracy.

### Experiment 7 — Confusion Matrix

Identify commonly confused gesture classes.

### Experiment 8 — TensorFlow Lite Conversion

Convert the model to `.tflite`.

### Experiment 9 — Quantization

Create a quantized model.

### Experiment 10 — Size Comparison

Compare floating-point and quantized model sizes.

### Experiment 11 — Model Export

Convert the `.tflite` file into a C/C++ byte array.

### Experiment 12 — MCU Deployment

Include the model in ESP32 firmware.

### Experiment 13 — Live Inference

Collect live IMU windows and classify gestures.

### Experiment 14 — Embedded Control

Use gesture predictions to control LEDs or another actuator.

### Experiment 15 — Performance Evaluation

Measure latency, accuracy, and memory consumption.

---

## 52. Exercises

### Exercise 1 — Train a Five-Class Classifier

Train a model for:

```text
Stationary
Left
Right
Up
Down
```

Report:

- Training accuracy
- Validation accuracy
- Test accuracy

### Exercise 2 — Optimize the Model

Reduce network size while maintaining acceptable accuracy.

Compare at least three model architectures.

### Exercise 3 — Quantize the Model

Convert:

```text
Float32 Model
      ↓
INT8 Model
```

Report:

- Original size
- Quantized size
- Compression ratio
- Accuracy change

### Exercise 4 — Deploy on ESP32

Implement:

```text
MPU6050
    ↓
100 Samples
    ↓
Normalization
    ↓
TinyML
    ↓
Gesture
```

Display predictions through the Serial Monitor.

### Exercise 5 — Gesture-Controlled Output

Use at least three gestures to control hardware.

Example:

```text
LEFT
 ↓
LED 1

RIGHT
 ↓
LED 2

DOWN
 ↓
LEDs OFF
```

### Exercise 6 — Performance Benchmark

Measure:

```text
Model Size
RAM Usage
Inference Latency
Classification Accuracy
```

Summarize the TinyML implementation.

### Exercise 7 — Complete Embedded Intelligence System

Implement:

```text
              Human Gesture
                    ↓
                 MPU6050
                    ↓
              Sensor Sampling
                    ↓
                Windowing
                    ↓
              Normalization
                    ↓
              TinyML Model
                    ↓
             Gesture Decision
                    ↓
             Local Actuation
```

The complete system should operate without requiring cloud inference.

---

## 53. Checkpoint Questions

1. What is TinyML?
2. Why are compact models important for microcontrollers?
3. What is the input shape of the gesture model?
4. Why must gesture sequences have a fixed length?
5. Why is normalization required?
6. Why must normalization parameters be calculated from the training set?
7. What is a dense neural-network layer?
8. What does ReLU do?
9. What is the purpose of softmax?
10. How is the predicted gesture selected from the output probabilities?
11. What is classification accuracy?
12. What is overfitting?
13. Why is validation data required?
14. Why should the test set not be used for training?
15. What does a confusion matrix show?
16. What is TensorFlow Lite?
17. What is model quantization?
18. What is the difference between Float32 and INT8?
19. Why can INT8 models be useful on MCUs?
20. What is a representative dataset?
21. Why must MCU preprocessing match training preprocessing?
22. What is inference latency?
23. What is a tensor arena?
24. What is the trade-off between model size and accuracy?
25. Why is local TinyML inference useful for embedded intelligence?

---

## 54. Expected Results

Students should successfully train a gesture classifier using the dataset collected in LAB 13.

The training stage should produce:

```text
Gesture Dataset
      ↓
Neural Network
      ↓
Trained Model
```

The model should then be converted:

```text
Keras Model
     ↓
TensorFlow Lite
     ↓
Quantization
     ↓
INT8 Model
```

Deployment should produce:

```text
MPU6050
    ↓
ESP32
    ↓
TinyML Model
    ↓
Gesture Prediction
```

The final embedded-intelligence system should operate as:

```text
Gesture
   ↓
Sensor
   ↓
Local Machine Learning
   ↓
Decision
   ↓
Actuator
```

Students should demonstrate that the optimized model can run directly on the MCU while maintaining acceptable gesture-classification accuracy.

---

## 55. Submission

Submit the following:

- Python training code.
- Dataset preprocessing code.
- Training and validation accuracy results.
- Training and validation loss plots.
- Test accuracy.
- Confusion matrix.
- Original TensorFlow Lite model.
- Quantized TensorFlow Lite model.
- Model-size comparison.
- C/C++ model array.
- ESP32 inference source code.
- Live gesture-classification results.
- Inference latency measurements.
- Memory-usage measurements.
- Gesture-controlled output demonstration.
- Short discussion of TinyML optimization trade-offs.

Suggested model-performance table:

| Model | Parameters | Size | Test Accuracy | Inference Latency |
|---|---:|---:|---:|---:|
| Float32 |  |  |  |  |
| Quantized |  |  |  |  |
| INT8 |  |  |  |  |

Suggested gesture results:

| Test | Actual Gesture | Predicted Gesture | Confidence | Correct |
|---:|---|---|---:|---|
| 1 | Left |  |  |  |
| 2 | Right |  |  |  |
| 3 | Up |  |  |  |
| 4 | Down |  |  |  |
| 5 | Stationary |  |  |  |

Suggested MCU-performance table:

| Metric | Result |
|---|---:|
| Model size |  |
| Tensor arena |  |
| Free heap |  |
| Average inference latency |  |
| Test accuracy |  |
| Live accuracy |  |

---

## 56. Conclusion

In this lab, the IMU gesture dataset is transformed into a complete **TinyML gesture-recognition system**. The data are organized into fixed-length windows, normalized, and used to train a compact neural-network classifier. The trained model is evaluated using independent validation and test data before being converted to TensorFlow Lite and optimized through quantization.

The optimized model is then deployed on the ESP32, where live MPU6050 measurements are collected, preprocessed, and classified locally. This demonstrates the core concept of **embedded intelligence**, in which sensing, machine-learning inference, decision making, and actuation are performed directly on the MCU without requiring cloud-based processing.


