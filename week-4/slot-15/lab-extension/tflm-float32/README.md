# LAB — LDR Classification using TFLM Float32
## Dark, Normal, and Bright Classification with ESP32 + TensorFlow Lite for Microcontrollers

## 1. Objective

The objective of this lab is to:

1. Read LDR values using the ESP32 ADC.
2. Prepare labeled data for three classes:
   - **Bright**
   - **Normal**
   - **Dark**
3. Train a small ANN using TensorFlow/Keras.
4. Convert the trained model to a **Float32 TensorFlow Lite model**.
5. Convert the `.tflite` model into a C/C++ byte array.
6. Deploy the model using **TensorFlow Lite for Microcontrollers (TFLM)**.
7. Run real-time inference on the ESP32.
8. Measure classification accuracy and inference latency.
9. Compare plain ANN inference with TFLM inference.
10. Prepare the same workflow for later INT8 quantization.

---

## 2. Learning Outcomes

After completing this lab, students should be able to:

- explain the role of TensorFlow Lite for Microcontrollers;
- train a simple neural network;
- convert a Keras model to `.tflite`;
- explain Float32 inference;
- embed a TensorFlow Lite model in MCU firmware;
- configure a Tensor Arena;
- initialize a TFLM interpreter;
- pass sensor data to the model input;
- invoke the model;
- interpret the output probabilities;
- measure inference time on the ESP32.

---

## 3. System Architecture

```text
Ambient Light
      ↓
     LDR
      ↓
ESP32 ADC
      ↓
Normalization
      ↓
TFLM Float32 Model
      ↓
ANN Inference
      ↓
Bright / Normal / Dark
```

The complete development workflow is:

```text
LDR Dataset
     ↓
TensorFlow / Keras
     ↓
ANN Training
     ↓
Float32 Model
     ↓
TensorFlow Lite Converter
     ↓
model.tflite
     ↓
Convert to C Array
     ↓
TFLM
     ↓
ESP32
     ↓
Real-Time Inference
```

---

## 4. Hardware Requirements

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable

Optional:

```text
GPIO 16 → Bright LED
GPIO 17 → Normal LED
GPIO 18 → Dark LED
```

---

## 5. Software Requirements

- Arduino IDE or PlatformIO
- Python 3
- TensorFlow
- NumPy
- Pandas
- TensorFlow Lite for Microcontrollers support for the selected ESP32 environment

---

## 6. LDR Circuit

```text
3.3 V
  |
 LDR
  |
  +------ GPIO 36
  |
10 kΩ
  |
 GND
```

For this lab, assume:

```text
Bright : 0–99
Normal : 100–499
Dark   : 500–1000
```

These ranges are examples. Students should verify the actual ADC response experimentally.

---

## 7. Dataset

Use a balanced dataset:

```text
Bright = 200 samples
Normal = 200 samples
Dark   = 200 samples

Total = 600 samples
```

Example:

```csv
light_value,label
32,bright
75,bright
250,normal
420,normal
650,dark
910,dark
```

---

## 8. Label Encoding

Use:

```text
0 → Dark
1 → Normal
2 → Bright
```

For this lab:

```python
label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}
```

---

## 9. Input Normalization

Because the example dataset spans:

```text
0 ... 1000
```

normalize the LDR input using:

\[
x_n=\frac{x}{1000}
\]

Therefore:

```text
0    → 0.000
100  → 0.100
500  → 0.500
1000 → 1.000
```

The exact same preprocessing must be used on the ESP32.

---

## 10. ANN Architecture

Use a small ANN:

```text
1 Input
   ↓
4 Hidden Neurons
Sigmoid
   ↓
3 Output Neurons
Softmax
   ↓
Dark / Normal / Bright
```

Architecture:

```text
1–4–3
```

Parameter count:

\[
1\times4+4+4\times3+3=23
\]

So the model contains:

```text
23 trainable parameters
```

---

## 11. TensorFlow Training Code

```python
import tensorflow as tf
import numpy as np
import pandas as pd

df = pd.read_csv(
    "light_dataset.csv"
)

X = df[
    "light_value"
].values.astype(
    np.float32
)

X = (
    X / 1000.0
).reshape(
    -1,
    1
)

label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}

y = df[
    "label"
].map(
    label_map
).values.astype(
    np.int32
)

np.random.seed(
    42
)

indices = np.arange(
    len(X)
)

np.random.shuffle(
    indices
)

X = X[
    indices
]

y = y[
    indices
]

n = len(X)

train_end = int(
    0.70 * n
)

val_end = int(
    0.85 * n
)

X_train = X[
    :train_end
]

y_train = y[
    :train_end
]

X_val = X[
    train_end:val_end
]

y_val = y[
    train_end:val_end
]

X_test = X[
    val_end:
]

y_test = y[
    val_end:
]

model = tf.keras.Sequential([
    tf.keras.layers.Input(
        shape=(1,)
    ),

    tf.keras.layers.Dense(
        4,
        activation="sigmoid"
    ),

    tf.keras.layers.Dense(
        3,
        activation="softmax"
    )
])

model.compile(
    optimizer=tf.keras.optimizers.Adam(
        learning_rate=0.01
    ),
    loss="sparse_categorical_crossentropy",
    metrics=[
        "accuracy"
    ]
)

model.summary()

history = model.fit(
    X_train,
    y_train,
    validation_data=(
        X_val,
        y_val
    ),
    epochs=100,
    batch_size=16,
    verbose=1
)

loss, accuracy = model.evaluate(
    X_test,
    y_test,
    verbose=0
)

print(
    "Test Accuracy:",
    accuracy
)

model.save(
    "ldr_float32.keras"
)
```

---

## 12. Convert to Float32 TensorFlow Lite

```python
converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

tflite_model = converter.convert()

with open(
    "ldr_float32.tflite",
    "wb"
) as f:

    f.write(
        tflite_model
    )
```

Because no integer quantization is requested, the model remains a Float32 TensorFlow Lite model.

---

## 13. Verify the TFLite Model in Python

```python
interpreter = tf.lite.Interpreter(
    model_path="ldr_float32.tflite"
)

interpreter.allocate_tensors()

input_details = (
    interpreter.get_input_details()
)

output_details = (
    interpreter.get_output_details()
)

test_value = np.array(
    [[0.25]],
    dtype=np.float32
)

interpreter.set_tensor(
    input_details[0]["index"],
    test_value
)

interpreter.invoke()

output = interpreter.get_tensor(
    output_details[0]["index"]
)

print(
    output
)
```

---

## 14. Convert `.tflite` to C Array

Using `xxd`:

```bash
xxd -i ldr_float32.tflite > model_data.cc
```

This produces code similar to:

```cpp
unsigned char ldr_float32_tflite[] =
{
  0x20, 0x00, 0x00, 0x00,
  ...
};

unsigned int ldr_float32_tflite_len =
  2456;
```

Change the model array to:

```cpp
const unsigned char
ldr_float32_tflite[] =
{
  ...
};
```

---

## 15. Model Data Files

Organize the Arduino project as:

```text
LDR_TFLM_Float32/
│
├── LDR_TFLM_Float32.ino
├── model_data.h
└── model_data.cpp
```

Example `model_data.h`:

```cpp
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

extern const unsigned char
ldr_float32_tflite[];

extern const unsigned int
ldr_float32_tflite_len;

#endif
```

---

## 16. TFLM Runtime Architecture

```text
model_data
    ↓
GetModel()
    ↓
Op Resolver
    ↓
Tensor Arena
    ↓
MicroInterpreter
    ↓
AllocateTensors()
    ↓
Input Tensor
    ↓
Invoke()
    ↓
Output Tensor
```

---

## 17. Tensor Arena

TFLM uses a preallocated memory region called the **Tensor Arena**.

Example:

```cpp
constexpr int
kTensorArenaSize =
  8 * 1024;

alignas(16)
uint8_t tensorArena[
  kTensorArenaSize
];
```

The Tensor Arena stores intermediate tensors and other runtime data.

If the arena is too small:

```text
AllocateTensors() fails
```

---

## 18. Operator Resolver

The ANN uses operations such as:

```text
Fully Connected
Logistic / Sigmoid
Softmax
```

Conceptually:

```cpp
tflite::MicroMutableOpResolver<3>
resolver;

resolver.AddFullyConnected();
resolver.AddLogistic();
resolver.AddSoftmax();
```

The exact resolver configuration should match the operators present in the generated `.tflite` model and the TFLM version being used.

---

## 19. ESP32 TFLM Float32 Inference Flow

```text
analogRead()
     ↓
Normalize
     ↓
input->data.f[0]
     ↓
Invoke()
     ↓
output->data.f[]
     ↓
Argmax
     ↓
Class
```

---

## 20. Core TFLM Setup

A typical TFLM setup contains:

```cpp
const tflite::Model* model =
    tflite::GetModel(
        ldr_float32_tflite
    );
```

Then create a resolver and interpreter, allocate tensors, and obtain the input/output tensor pointers.

---

## 21. Example ESP32 Inference Logic

```cpp
int adcValue =
  analogRead(
    LDR_PIN
  );

float normalized =
  adcValue /
  1000.0f;

input->data.f[0] =
  normalized;

unsigned long startTime =
  micros();

TfLiteStatus status =
  interpreter->Invoke();

unsigned long inferenceTime =
  micros()
  -
  startTime;
```

Then read:

```cpp
float dark =
  output->data.f[0];

float normal =
  output->data.f[1];

float bright =
  output->data.f[2];
```

---

## 22. Argmax Classification

```cpp
int predictedClass =
  0;

float bestScore =
  output->data.f[0];

for (
  int i = 1;
  i < 3;
  i++
)
{
  if (
    output->data.f[i]
    >
    bestScore
  )
  {
    bestScore =
      output->data.f[i];

    predictedClass =
      i;
  }
}
```

Map:

```text
0 → Dark
1 → Normal
2 → Bright
```

---

## 23. Example Serial Output

```text
ADC: 42
Normalized: 0.042

Dark   : 0.008
Normal : 0.014
Bright : 0.978

Prediction: Bright
Inference: 126 us
```

Another example:

```text
ADC: 325
Normalized: 0.325

Dark   : 0.021
Normal : 0.956
Bright : 0.023

Prediction: Normal
```

---

## 24. Float32 Data Type

The input tensor should normally have:

```text
type = kTfLiteFloat32
```

The ESP32 input is therefore written using:

```cpp
input->data.f[0]
```

Likewise, the output is read through:

```cpp
output->data.f[k]
```

---

## 25. Experiment 1 — Train the Float32 ANN

| Metric | Result |
|---|---:|
| Training accuracy | |
| Validation accuracy | |
| Test accuracy | |
| Final loss | |

---

## 26. Experiment 2 — Inspect the Model

Record:

```text
Input type
Output type
Input shape
Output shape
Model size
```

Expected conceptually:

```text
Input  : Float32
Output : Float32
Input  : [1,1]
Output : [1,3]
```

---

## 27. Experiment 3 — Test TFLite on PC

Test:

```text
ADC = 25
ADC = 250
ADC = 750
```

Expected classes:

```text
25  → Bright
250 → Normal
750 → Dark
```

| ADC | Normalized | Predicted |
|---:|---:|---|
| 25 | 0.025 | |
| 250 | 0.250 | |
| 750 | 0.750 | |

---

## 28. Experiment 4 — Deploy on ESP32

```text
LDR
 ↓
ADC
 ↓
Normalize
 ↓
TFLM Float32
 ↓
Class
```

Record at least 30 predictions.

---

## 29. Experiment 5 — Measure Inference Latency

```cpp
unsigned long start =
  micros();

interpreter->Invoke();

unsigned long elapsed =
  micros()
  -
  start;
```

| Test | Inference Time (µs) |
|---:|---:|
| 1 | |
| 2 | |
| 3 | |
| Mean | |

---

## 30. Experiment 6 — Memory Usage

Record:

```text
Flash usage
Static RAM
Tensor Arena
Model size
```

| Resource | Usage |
|---|---:|
| `.tflite` model | |
| Tensor Arena | |
| Flash | |
| RAM | |

---

## 31. Experiment 7 — Plain ANN vs TFLM

| Feature | Plain C++ ANN | TFLM Float32 |
|---|---|---|
| Runtime | None | TFLM |
| Model format | Hard-coded weights | `.tflite` |
| Easy model replacement | Lower | Higher |
| Memory overhead | Very low | Higher |
| TensorFlow workflow | Manual | Native |
| Larger models | More difficult | Easier |

For a tiny `1–4–3` ANN, plain C++ may be simpler.

TFLM becomes more useful as models become larger or more complex.

---

## 32. Experiment 8 — Threshold vs ANN vs TFLM

Compare:

```text
Threshold
Plain ANN
TFLM Float32
```

| Method | Accuracy | Memory | Inference Time | Complexity |
|---|---:|---:|---:|---|
| Threshold | | | | Low |
| Plain ANN | | | | Medium |
| TFLM Float32 | | | | Medium |

---

## 33. TFLM Training vs Inference

Training occurs on a computer:

```text
Dataset
 ↓
TensorFlow
 ↓
Backpropagation
 ↓
Weights
```

ESP32 performs only inference:

```text
Sensor
 ↓
Model
 ↓
Forward Pass
 ↓
Prediction
```

---

## 34. Why Use Float32 First?

Float32 is useful for the first TFLM lab because:

```text
No quantization mathematics
No scale
No zero point
Direct floating-point input
Direct floating-point output
```

The processing chain is easy to understand:

```text
ADC
 ↓
Normalize
 ↓
Float32 Tensor
 ↓
ANN
 ↓
Float32 Output
```

After this lab, students can extend the same model to:

```text
INT8 Quantization
```

---

## 35. Float32 vs INT8 Preview

```text
Float32 TFLM
    ↓
32-bit model parameters
    ↓
Simple preprocessing

INT8 TFLM
    ↓
8-bit model parameters
    ↓
Scale + Zero Point
    ↓
Smaller model / potentially faster inference
```

---

## 36. Checkpoint Questions

1. What is TensorFlow Lite for Microcontrollers?
2. Why is a TensorFlow model converted to `.tflite`?
3. Why is the `.tflite` model converted to a C array?
4. What is a Tensor Arena?
5. What does `AllocateTensors()` do?
6. What does `Invoke()` do?
7. Why must ESP32 normalization match training normalization?
8. What does `input->data.f[0]` represent?
9. What is the role of the operator resolver?
10. What is the main difference between Float32 TFLM and INT8 TFLM?

---

## 37. Exercises

### Exercise 1 — Train Model

Train:

```text
1 → 4 → 3
```

and record the accuracy.

### Exercise 2 — Convert Model

Generate:

```text
ldr_float32.tflite
```

### Exercise 3 — Inspect Model Size

Measure the `.tflite` file size.

### Exercise 4 — Embed Model

Convert the model to:

```text
model_data.cpp
```

### Exercise 5 — ESP32 Inference

Run the model with real LDR measurements.

### Exercise 6 — Measure Latency

Measure inference in microseconds.

### Exercise 7 — Compare Approaches

Compare:

```text
Threshold
Plain ANN
TFLM Float32
```

---

## 38. Expected Results

The complete system should operate as:

```text
Ambient Light
     ↓
LDR
     ↓
ADC
     ↓
Normalize
     ↓
Float32 TFLM Input
     ↓
ANN 1–4–3
     ↓
Softmax
     ↓
Dark / Normal / Bright
```

Typical results:

```text
ADC = 45
Prediction = Bright

ADC = 310
Prediction = Normal

ADC = 780
Prediction = Dark
```

---

## 39. Submission

Students should submit:

- LDR dataset
- TensorFlow training code
- training history
- test accuracy
- `ldr_float32.keras`
- `ldr_float32.tflite`
- model C array
- ESP32 TFLM source code
- Tensor Arena size
- Flash/RAM usage
- inference latency
- classification test results
- comparison with threshold and plain ANN
- short discussion

---

## 40. Conclusion

This lab introduces the complete **Float32 TFLM deployment workflow**:

```text
LDR Data
   ↓
TensorFlow Training
   ↓
Float32 Keras Model
   ↓
TFLite Conversion
   ↓
.tflite
   ↓
C Array
   ↓
TensorFlow Lite for Microcontrollers
   ↓
ESP32
   ↓
Real-Time Classification
```

The main difference from the earlier plain ANN implementation is that the weights and model structure are no longer manually copied into the Arduino code. Instead, the TensorFlow Lite model is compiled into the embedded application and executed through the TFLM interpreter.

For a very small LDR classifier, this introduces some runtime overhead, but it provides a scalable workflow for later TinyML applications such as IMU gesture recognition, audio classification, anomaly detection, and sensor fusion.
