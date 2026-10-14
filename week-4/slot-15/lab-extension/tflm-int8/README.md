# LAB — LDR Classification using TFLM INT8
## Dark, Normal, and Bright Classification with ESP32 + TensorFlow Lite for Microcontrollers

## 1. Objective

The objective of this lab is to:

1. Read LDR values using the ESP32 ADC.
2. Prepare labeled LDR data for three classes: **Bright**, **Normal**, and **Dark**.
3. Train a small ANN using TensorFlow/Keras.
4. Convert the Float32 ANN to a **fully quantized INT8 TensorFlow Lite model**.
5. Understand **scale** and **zero point** used by INT8 quantization.
6. Convert the `.tflite` model to a C/C++ byte array.
7. Deploy the INT8 model using **TensorFlow Lite for Microcontrollers (TFLM)**.
8. Perform real-time light classification on the ESP32.
9. Measure inference latency and memory usage.
10. Compare Float32 and INT8 TFLM models.

---

## 2. Learning Outcomes

After completing this lab, students should be able to:

- explain INT8 post-training quantization;
- prepare representative calibration data;
- convert a Keras model to a fully quantized INT8 model;
- inspect tensor scale and zero point;
- quantize a Float32 sensor input into INT8;
- dequantize an INT8 output;
- deploy a TFLM INT8 model on an ESP32;
- compare Float32 and INT8 model size, speed, and accuracy.

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
Float32 Value
      ↓
INT8 Quantization
      ↓
TFLM INT8 Model
      ↓
INT8 Output
      ↓
Dequantization
      ↓
Dark / Normal / Bright
```

The development workflow is:

```text
LDR Dataset
     ↓
TensorFlow / Keras
     ↓
Float32 ANN Training
     ↓
Representative Dataset
     ↓
INT8 Quantization
     ↓
INT8 .tflite
     ↓
C Array
     ↓
TFLM
     ↓
ESP32
```

---

## 4. Hardware Requirements

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable

Optional outputs:

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
- TensorFlow Lite for Microcontrollers

Install Python packages:

```bash
pip install tensorflow numpy pandas
```

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

These are teaching ranges. Actual values should be calibrated experimentally.

---

## 7. Dataset

Use approximately:

```text
Bright = 200 samples
Normal = 200 samples
Dark   = 200 samples

Total = 600 samples
```

Example:

```csv
light_value,label
25,bright
72,bright
185,normal
430,normal
610,dark
920,dark
```

---

## 8. Label Encoding

Use:

```text
0 → Dark
1 → Normal
2 → Bright
```

Python:

```python
label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}
```

The same ordering must be used by the ESP32 application.

---

## 9. Input Normalization

The teaching dataset ranges approximately from:

```text
0 ... 1000
```

Normalize using:

\[
x_n = \frac{x}{1000}
\]

Example:

```text
ADC = 25   → 0.025
ADC = 250  → 0.250
ADC = 750  → 0.750
```

The TensorFlow model is trained using these Float32 normalized values.

---

## 10. ANN Architecture

Use:

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

Total parameters:

\[
1\times4 + 4 + 4\times3 + 3 = 23
\]

---

## 11. Float32 TensorFlow Training

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

model.fit(
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
    "Float32 Test Accuracy:",
    accuracy
)
```

---

## 12. Representative Dataset

Full INT8 quantization requires representative input samples.

```python
def representative_dataset():

    for i in range(
        len(X_train)
    ):

        sample = X_train[
            i:i+1
        ].astype(
            np.float32
        )

        yield [
            sample
        ]
```

The representative dataset allows TensorFlow Lite to estimate the dynamic ranges required to convert Float32 activations into INT8 values.

---

## 13. Convert to Fully INT8 TFLite

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

tflite_int8_model = (
    converter.convert()
)

with open(
    "ldr_int8.tflite",
    "wb"
) as f:

    f.write(
        tflite_int8_model
    )

print(
    "Saved: ldr_int8.tflite"
)
```

The resulting model uses:

```text
INT8 weights
INT8 activations
INT8 input
INT8 output
```

---

## 14. INT8 Quantization Theory

A Float32 value is converted to an integer using:

\[
q =
\operatorname{round}
\left(
\frac{x}{s}
\right)
+
z
\]

where:

- \(x\) = Float32 value
- \(q\) = INT8 value
- \(s\) = quantization scale
- \(z\) = zero point

For INT8:

```text
-128 ... 127
```

The inverse conversion is:

\[
x =
(q-z)s
\]

---

## 15. Inspect Input and Output Quantization

```python
interpreter = tf.lite.Interpreter(
    model_path="ldr_int8.tflite"
)

interpreter.allocate_tensors()

input_details = (
    interpreter.get_input_details()
)

output_details = (
    interpreter.get_output_details()
)

print(
    "Input:"
)

print(
    input_details
)

print(
    "\nOutput:"
)

print(
    output_details
)
```

Get quantization parameters:

```python
input_scale, input_zero_point = (
    input_details[0][
        "quantization"
    ]
)

output_scale, output_zero_point = (
    output_details[0][
        "quantization"
    ]
)

print(
    "Input scale:",
    input_scale
)

print(
    "Input zero point:",
    input_zero_point
)

print(
    "Output scale:",
    output_scale
)

print(
    "Output zero point:",
    output_zero_point
)
```

---

## 16. Test INT8 Model in Python

Suppose:

```text
ADC = 250
```

Normalize:

```python
x_float = np.array(
    [[250 / 1000.0]],
    dtype=np.float32
)
```

Quantize:

```python
x_int8 = np.round(
    x_float /
    input_scale
    +
    input_zero_point
)

x_int8 = np.clip(
    x_int8,
    -128,
    127
).astype(
    np.int8
)
```

Set the tensor:

```python
interpreter.set_tensor(
    input_details[0]["index"],
    x_int8
)

interpreter.invoke()
```

Read INT8 output:

```python
output_int8 = interpreter.get_tensor(
    output_details[0]["index"]
)
```

Dequantize:

```python
output_float = (
    output_int8.astype(
        np.float32
    )
    -
    output_zero_point
) * output_scale
```

Prediction:

```python
predicted_class = np.argmax(
    output_float
)
```

---

## 17. Example INT8 Model Output

Example:

```text
Raw INT8 output:

[-120, 112, -118]
```

After dequantization:

```text
Dark   = 0.02
Normal = 0.95
Bright = 0.03
```

Therefore:

```text
Prediction = Normal
```

---

## 18. Compare Float32 and INT8 Accuracy

Test both models using the same test set.

| Model | Accuracy |
|---|---:|
| Float32 | |
| INT8 | |

A small reduction in accuracy may occur after quantization.

For a simple LDR classification problem, the difference may be very small.

---

## 19. Convert `.tflite` to C Array

Use:

```bash
xxd -i ldr_int8.tflite > model_data.cpp
```

Example:

```cpp
const unsigned char
ldr_int8_tflite[] =
{
  0x20, 0x00, 0x00, 0x00,
  ...
};

const unsigned int
ldr_int8_tflite_len =
  2200;
```

---

## 20. Arduino Project Structure

```text
LDR_TFLM_INT8/
│
├── LDR_TFLM_INT8.ino
├── model_data.h
└── model_data.cpp
```

---

## 21. `model_data.h`

```cpp
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

extern const unsigned char
ldr_int8_tflite[];

extern const unsigned int
ldr_int8_tflite_len;

#endif
```

---

## 22. TFLM INT8 Architecture

```text
ADC Value
    ↓
Float32 Normalization
    ↓
INT8 Quantization
    ↓
Input Tensor
    ↓
TFLM Interpreter
    ↓
INT8 ANN
    ↓
INT8 Output
    ↓
Dequantization
    ↓
Argmax
```

---

## 23. Tensor Arena

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

If the model cannot allocate tensors, increase the arena.

For example:

```cpp
16 * 1024
```

or:

```cpp
32 * 1024
```

depending on the model and runtime.

---

## 24. Operator Resolver

A small ANN may require operators such as:

```text
Fully Connected
Logistic
Softmax
Quantize / Dequantize
```

The exact operators depend on the generated TFLite graph.

A simple teaching configuration can use:

```cpp
tflite::AllOpsResolver resolver;
```

For a production system, a `MicroMutableOpResolver` containing only the required operators can reduce code size.

---

## 25. INT8 Input Tensor

After:

```cpp
interpreter.AllocateTensors();
```

obtain:

```cpp
TfLiteTensor* input =
    interpreter.input(0);

TfLiteTensor* output =
    interpreter.output(0);
```

Check:

```cpp
if (
  input->type !=
  kTfLiteInt8
)
{
  Serial.println(
    "Input is not INT8"
  );
}
```

---

## 26. Get Quantization Parameters on ESP32

TFLM stores:

```cpp
float inputScale =
  input->params.scale;

int inputZeroPoint =
  input->params.zero_point;
```

For output:

```cpp
float outputScale =
  output->params.scale;

int outputZeroPoint =
  output->params.zero_point;
```

These values should come from the model rather than being guessed.

---

## 27. Quantize LDR Input on ESP32

First normalize:

```cpp
float normalized =
  adcValue /
  1000.0f;
```

Then quantize:

```cpp
int32_t quantized =
  roundf(
    normalized /
    inputScale
  )
  +
  inputZeroPoint;
```

Clip to INT8:

```cpp
if (
  quantized > 127
)
{
  quantized =
    127;
}

if (
  quantized < -128
)
{
  quantized =
    -128;
}
```

Write input:

```cpp
input->data.int8[0] =
  (int8_t)quantized;
```

---

## 28. Run TFLM Inference

```cpp
unsigned long startTime =
  micros();

TfLiteStatus status =
  interpreter.Invoke();

unsigned long inferenceTime =
  micros()
  -
  startTime;
```

Check:

```cpp
if (
  status != kTfLiteOk
)
{
  Serial.println(
    "Invoke failed"
  );
}
```

---

## 29. Read INT8 Output

```cpp
int8_t qDark =
  output->data.int8[0];

int8_t qNormal =
  output->data.int8[1];

int8_t qBright =
  output->data.int8[2];
```

---

## 30. Dequantize Output

```cpp
float dark =
  (
    qDark
    -
    outputZeroPoint
  )
  *
  outputScale;

float normal =
  (
    qNormal
    -
    outputZeroPoint
  )
  *
  outputScale;

float bright =
  (
    qBright
    -
    outputZeroPoint
  )
  *
  outputScale;
```

---

## 31. Classification

Create:

```cpp
float scores[3] =
{
  dark,
  normal,
  bright
};
```

Then:

```cpp
int predictedClass =
  0;

float bestScore =
  scores[0];

for (
  int i = 1;
  i < 3;
  i++
)
{
  if (
    scores[i] >
    bestScore
  )
  {
    bestScore =
      scores[i];

    predictedClass =
      i;
  }
}
```

Class mapping:

```text
0 → Dark
1 → Normal
2 → Bright
```

---

## 32. Complete Embedded Processing Flow

```text
analogRead()
     ↓
ADC
     ↓
ADC / 1000
     ↓
Float32 normalized value
     ↓
x / input_scale + zero_point
     ↓
INT8
     ↓
input->data.int8[0]
     ↓
Invoke()
     ↓
output->data.int8[]
     ↓
(output - zero_point) × scale
     ↓
Float32 Scores
     ↓
Argmax
     ↓
Class
```

---

## 33. Example Serial Output

```text
ADC: 252
Normalized: 0.2520

INT8 Input: -64

Input Scale: 0.00392157
Input Zero Point: -128

Dark   : 0.018
Normal : 0.961
Bright : 0.021

Prediction: Normal

Inference Time: 87 us
```

The actual scale, zero point, scores, and execution time depend on the generated model and board.

---

## 34. Experiment 1 — Train Float32 ANN

| Metric | Result |
|---|---:|
| Training accuracy | |
| Validation accuracy | |
| Test accuracy | |
| Loss | |

---

## 35. Experiment 2 — INT8 Quantization

Generate:

```text
ldr_int8.tflite
```

Record:

| Parameter | Value |
|---|---:|
| Input scale | |
| Input zero point | |
| Output scale | |
| Output zero point | |

---

## 36. Experiment 3 — Compare Model Size

Measure:

```text
Float32 .tflite
INT8 .tflite
```

| Model | Size |
|---|---:|
| Float32 | |
| INT8 | |

INT8 weights typically require approximately one quarter of the storage of Float32 weights themselves, although the total `.tflite` file size also includes model metadata and operators.

---

## 37. Experiment 4 — Compare Accuracy

| Model | Accuracy |
|---|---:|
| Float32 | |
| INT8 | |

Calculate:

\[
\Delta A
=
A_{\mathrm{INT8}}
-
A_{\mathrm{Float32}}
\]

---

## 38. Experiment 5 — ESP32 INT8 Inference

Test:

```text
ADC = 25
ADC = 250
ADC = 750
```

Expected:

```text
25  → Bright
250 → Normal
750 → Dark
```

| ADC | Float Input | INT8 Input | Predicted |
|---:|---:|---:|---|
| 25 | 0.025 | | |
| 250 | 0.250 | | |
| 750 | 0.750 | | |

---

## 39. Experiment 6 — Measure Inference Latency

| Test | Float32 TFLM | INT8 TFLM |
|---:|---:|---:|
| 1 | | |
| 2 | | |
| 3 | | |
| Mean | | |

Discuss whether INT8 is faster on the selected ESP32.

---

## 40. Experiment 7 — Memory Comparison

| Resource | Float32 | INT8 |
|---|---:|---:|
| Model size | | |
| Tensor Arena | | |
| Flash | | |
| RAM | | |

---

## 41. Experiment 8 — Classification Comparison

Compare:

```text
Threshold
Plain Float32 ANN
TFLM Float32
TFLM INT8
```

| Method | Accuracy | Memory | Latency | Complexity |
|---|---:|---:|---:|---|
| Threshold | | | | Low |
| Plain ANN | | | | Medium |
| TFLM Float32 | | | | Medium |
| TFLM INT8 | | | | Higher |

---

## 42. Float32 vs INT8

### Float32

```text
ADC
 ↓
Normalize
 ↓
Float32 Input
 ↓
Float32 ANN
 ↓
Float32 Output
```

### INT8

```text
ADC
 ↓
Normalize
 ↓
Quantize
 ↓
INT8 Input
 ↓
INT8 ANN
 ↓
INT8 Output
 ↓
Dequantize
```

The INT8 workflow adds:

```text
Scale
Zero Point
Quantization
Dequantization
```

but reduces numerical precision and usually decreases model storage.

---

## 43. Why Use INT8?

INT8 is especially useful for microcontrollers because it can provide:

```text
Smaller model
Lower memory requirements
Integer arithmetic
Potentially faster inference
Lower energy consumption
```

The actual speed improvement depends on the processor and optimized kernels available for the target.

---

## 44. Common Error — Incorrect Input Quantization

Do **not** directly write:

```cpp
input->data.int8[0] =
  adcValue;
```

This is incorrect.

The correct pipeline is:

```text
ADC
 ↓
Normalize
 ↓
Apply scale and zero point
 ↓
INT8
```

---

## 45. Common Error — Hard-Coding Scale

Avoid assuming:

```cpp
scale = 1.0 / 127.0
```

unless the actual model uses this value.

Instead use:

```cpp
input->params.scale
```

and:

```cpp
input->params.zero_point
```

because the TFLite converter determines the quantization parameters.

---

## 46. Common Error — Wrong Tensor Type

Float32 uses:

```cpp
input->data.f[0]
```

INT8 uses:

```cpp
input->data.int8[0]
```

Similarly:

```cpp
output->data.int8[k]
```

must be used for an INT8 output tensor.

---

## 47. Checkpoint Questions

1. What is INT8 quantization?
2. Why is a representative dataset required?
3. What is quantization scale?
4. What is zero point?
5. What is the numerical range of signed INT8?
6. Why must the LDR value first be normalized before quantization?
7. Why should the ESP32 obtain scale and zero point from the model?
8. What is the difference between `input->data.f` and `input->data.int8`?
9. Why can INT8 reduce memory usage?
10. What trade-off may occur when converting Float32 to INT8?

---

## 48. Exercises

### Exercise 1 — Train Float32 Model

Train:

```text
1 → 4 → 3
```

### Exercise 2 — INT8 Conversion

Generate:

```text
ldr_int8.tflite
```

### Exercise 3 — Inspect Quantization

Report:

```text
Input Scale
Input Zero Point
Output Scale
Output Zero Point
```

### Exercise 4 — Model Size

Compare:

```text
Float32 vs INT8
```

### Exercise 5 — ESP32 Deployment

Run real-time INT8 inference.

### Exercise 6 — Latency

Compare Float32 and INT8 inference times.

### Exercise 7 — Accuracy

Compare:

```text
Float32 Accuracy
INT8 Accuracy
```

---

## 49. Expected Results

The final embedded system should implement:

```text
Ambient Light
      ↓
LDR
      ↓
ADC
      ↓
Normalize
      ↓
Quantize to INT8
      ↓
TFLM INT8 ANN
      ↓
INT8 Output
      ↓
Dequantize
      ↓
Dark / Normal / Bright
```

Example:

```text
ADC: 63
Prediction: Bright

ADC: 320
Prediction: Normal

ADC: 820
Prediction: Dark
```

---

## 50. Submission

Students should submit:

- LDR dataset
- TensorFlow training code
- Float32 model accuracy
- representative dataset function
- INT8 conversion code
- `ldr_int8.tflite`
- input scale and zero point
- output scale and zero point
- C-array model
- ESP32 TFLM INT8 source code
- model size comparison
- memory usage
- Float32/INT8 inference latency
- Float32/INT8 accuracy comparison
- Serial Monitor results
- short discussion

---

## 51. Conclusion

This lab extends Float32 TFLM deployment into a **fully quantized INT8 TinyML implementation**.

The complete process is:

```text
LDR Dataset
     ↓
Float32 ANN Training
     ↓
Representative Dataset
     ↓
Post-Training Quantization
     ↓
INT8 TFLite Model
     ↓
C Array
     ↓
TFLM
     ↓
ESP32
     ↓
Real-Time INT8 Inference
```

The most important concept introduced in this lab is the transformation between floating-point sensor values and quantized integer tensors:

```text
Float32
   ↓
Scale + Zero Point
   ↓
INT8
```

and for the model output:

```text
INT8
   ↓
Scale + Zero Point
   ↓
Float32 Score
```

This provides a practical foundation for deploying larger TinyML applications on resource-constrained microcontrollers while reducing model storage and potentially improving inference efficiency.
