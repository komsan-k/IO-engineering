# 📘 Theory: Model Optimization, INT8 Quantization, Pruning, and TensorFlow Lite for Microcontrollers  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Deploying machine-learning models on microcontrollers introduces challenges that are different from running AI models on PCs, GPUs, or cloud servers. An ESP32 has limited:

- RAM,
- flash memory,
- CPU performance,
- energy resources,
- numerical precision,
- real-time processing capability.

A neural network that works well on a desktop computer may therefore be too large or computationally expensive for direct deployment on an ESP32.

**Model optimization** addresses this problem by reducing the computational and memory requirements of a trained model while attempting to preserve acceptable prediction accuracy.

Two important optimization techniques are:

1. **Quantization** — representing model parameters and computations with lower numerical precision.
2. **Pruning** — removing model parameters that contribute relatively little to the model output.

For microcontroller deployment, an important software framework is **TensorFlow Lite for Microcontrollers (TFLM)**.

The overall workflow can be represented as:

```text
Training Dataset
       │
       ▼
TensorFlow Model
       │
       ▼
Model Optimization
       │
       ├── INT8 Quantization
       ├── Pruning
       └── Other Optimizations
       │
       ▼
TFLite Model
       │
       ▼
TensorFlow Lite for Microcontrollers
       │
       ▼
      ESP32
       │
       ▼
On-Device Inference
```

Thus:

$$
\boxed{
\text{Embedded AI} =
\text{Trained Model}
+
\text{Model Optimization}
+
\text{MCU Inference Runtime}
}
$$

---

# 🧠 2. Why Is Model Optimization Necessary?

Consider a neural network containing:

$$
N_p
$$

parameters.

If each parameter is represented using 32-bit floating-point format:

$$
32~\text{bits}=4~\text{bytes}
$$

The approximate parameter storage is:

$$
M_{\text{FP32}} =
4N_p
$$

bytes.

For example, a model with:

$$
N_p=100,000
$$

parameters requires approximately:

$$
M_{\text{FP32}} =
100,000\times4
$$

$$
M_{\text{FP32}} =
400,000~\text{bytes}
$$

or approximately:

$$
400~\text{kB}
$$

for parameters alone.

Additional memory may also be required for:

- activations,
- tensor buffers,
- intermediate calculations,
- input/output tensors,
- runtime structures.

Therefore, parameter size alone does not represent the complete runtime memory requirement.

---

# 💾 3. Embedded AI Resource Constraints

A TinyML model must satisfy several constraints simultaneously:

```text
                 TinyML Model
                      │
       ┌──────────────┼──────────────┐
       ▼              ▼              ▼
    Memory       Computation       Energy
       │              │              │
       ▼              ▼              ▼
   Flash/RAM       Latency       Battery Life
```

The optimization problem can be expressed conceptually as:

$$
\boxed{
\text{Optimize}
\{
\text{Accuracy},
\text{Memory},
\text{Latency},
\text{Energy}
\}
}
$$

These objectives may conflict with one another.

---

# ⚖️ 4. Accuracy vs. Resource Consumption

A larger neural network can sometimes provide better accuracy, but usually requires more resources.

```text
Model Complexity
      ▲
      │
      │                    ● Large Model
      │
      │             ●
      │       ●
      │ ● Small Model
      └────────────────────────────►
            Resource Usage
```

TinyML therefore focuses on finding a suitable balance:

$$
\boxed{
\text{TinyML Model} =
\text{Acceptable Accuracy}
+
\text{Low Resource Cost}
}
$$

---

# 🔢 5. What Is Quantization?

**Quantization** reduces the numerical precision used to represent model parameters and, depending on the quantization scheme, activations and computations.

A conventional model may use:

$$
\text{FP32}
$$

while an optimized model may use:

$$
\text{INT8}
$$

Conceptually:

```text
FP32 Model
    │
    ▼
Quantization
    │
    ▼
INT8 Model
```

FP32 uses:

$$
32~\text{bits}
$$

per value, while INT8 uses:

$$
8~\text{bits}
$$

per value.

Therefore:

$$
\frac{32}{8}=4
$$

so parameter storage can theoretically be reduced by approximately a factor of four when moving from FP32 parameters to INT8 parameters, before considering metadata and other model components.

---

# 📊 6. Floating-Point Representation

A neural-network weight may be represented as:

```text
0.725341
-0.138271
1.294632
```

These values are commonly represented as 32-bit floating-point numbers during training.

Advantages include:

- large dynamic range,
- high numerical precision,
- convenient gradient-based training.

However, FP32 may require more memory and computation than lower-precision representations.

---

# 🔢 7. INT8 Representation

INT8 uses signed 8-bit integers:

$$
q\in[-128,127]
$$

Instead of storing:

```text
0.725341
```

the system stores an integer representation such as:

```text
92
```

along with quantization parameters that relate the integer to the original real-valued range.

---

# 📐 8. Quantization Mathematical Model

A common affine quantization relationship is:

$$
r=S(q-Z)
$$

where:

- $r$ = real-valued quantity,
- $q$ = quantized integer,
- $S$ = scale,
- $Z$ = zero-point.

The quantization operation can be expressed approximately as:

$$
q = round(\frac{r}{S}) +Z
$$

followed by clipping to the supported integer range.

Thus:

$$
\boxed{
\text{Real Value}
\leftrightarrow
\text{Integer Value}
}
$$

through the scale and zero-point parameters.

---

# 📏 9. Scale Factor

Suppose the real-value range is:

$$
[r_{\min},r_{\max}]
$$

and the quantized range is:

$$
[q_{\min},q_{\max}]
$$

A scale can be calculated approximately as:

$$
S=
\frac{
r_{\max}-r_{\min}
}{
q_{\max}-q_{\min}
}
$$

For INT8:

$$
q_{\min}=-128
$$

and:

$$
q_{\max}=127
$$

giving:

$$
q_{\max}-q_{\min}=255
$$

---

# 🎯 10. Zero-Point

The **zero-point** shifts the integer representation so that real zero can be represented appropriately.

Conceptually:

```text
Real Values

-1.0 -------- 0 -------- +1.0

              │
              ▼

INT8 Values

-128 -------- Z -------- 127
```

The zero-point is particularly important for asymmetric quantization.

---

# 🧮 11. Quantization Example

Suppose:

$$
S=0.01
$$

and:

$$
Z=0
$$

For:

$$
r=0.75
$$

the quantized value is:

$$
q= round(\frac{0.75}{0.01})
$$

$$
q=75
$$

The reconstructed value is:

$$
\hat{r}=0.01(75)=0.75
$$

---

# ⚠️ 12. Quantization Error

Not every floating-point number can be represented exactly using INT8.

Define quantization error as:

$$
e_q=r-\hat{r}
$$

where:

- $r$ = original value,
- $\hat{r}$ = reconstructed quantized value.

Therefore:

$$
\boxed{
\text{Lower Precision}
\rightarrow
\text{Quantization Error}
}
$$

The goal is to reduce model size and computational requirements without causing unacceptable accuracy degradation.

---

# 📊 13. FP32 vs. INT8

| Property | FP32 | INT8 |
|---|---:|---:|
| Bits/value | 32 | 8 |
| Bytes/value | 4 | 1 |
| Numerical precision | Higher | Lower |
| Parameter storage | Larger | Smaller |
| Integer execution | No | Possible |
| Quantization error | No INT8 quantization error | Present |
| MCU suitability | Depends on model/hardware | Often attractive |

---

# 🧠 14. Types of Quantization

Common model-optimization approaches include:

```text
Quantization
    │
    ├── Dynamic Range Quantization
    │
    ├── Float16 Quantization
    │
    └── Full Integer Quantization
              │
              ▼
             INT8
```

For microcontroller-oriented deployment, **full integer INT8 quantization** is particularly important because supported models can perform inference primarily using integer operations.

---

# 🔄 15. Post-Training Quantization

**Post-Training Quantization (PTQ)** applies quantization after the model has already been trained.

Workflow:

```text
Dataset
   │
   ▼
Train FP32 Model
   │
   ▼
Trained Model
   │
   ▼
Post-Training Quantization
   │
   ▼
INT8 TFLite Model
```

Advantages include:

- relatively simple workflow,
- no need to retrain from the beginning,
- reduced model size.

However, some models may experience noticeable accuracy loss.

---

# 📚 16. Representative Dataset

Full integer quantization normally requires a **representative dataset** for calibration.

Conceptually:

```text
Training Data
     │
     ▼
Representative Samples
     │
     ▼
TFLite Converter
     │
     ▼
Activation Range Calibration
     │
     ▼
INT8 Model
```

The representative dataset should reflect realistic model inputs.

It does not necessarily need to contain the entire training dataset.

---

# 🎯 17. Why Representative Data Matters

Suppose the deployed sensor input normally covers:

$$
-2g\le a\le2g
$$

but the representative dataset only contains:

$$
-0.1g\le a\le0.1g
$$

The calibration may not represent the real operating range well.

Therefore:

$$
\boxed{
\text{Representative Calibration Data}
\rightarrow
\text{Better Quantization Calibration}
}
$$

---

# 🧠 18. Quantization-Aware Training

**Quantization-Aware Training (QAT)** simulates quantization effects during model training.

```text
Training Data
      │
      ▼
FP32 Training
      │
      +
Simulated Quantization
      │
      ▼
Quantization-Aware Model
      │
      ▼
INT8 Conversion
```

Conceptually:

$$
\boxed{
\text{QAT} =
\text{Training}
+
\text{Quantization Effects}
}
$$

QAT can help recover accuracy when simple PTQ produces excessive degradation.

---

# 🆚 19. PTQ vs. QAT

| Feature | PTQ | QAT |
|---|---|---|
| Quantization stage | After training | During/following training |
| Complexity | Lower | Higher |
| Additional training | Usually no | Yes |
| Calibration data | Often required for full INT8 | Training process incorporates quantization simulation |
| Accuracy preservation | Model-dependent | Often better for sensitive models |
| Development time | Shorter | Longer |

A practical workflow often begins with PTQ and moves to QAT only if necessary.

---

# ✂️ 20. What Is Pruning?

**Pruning** removes or suppresses neural-network parameters that have relatively little contribution to the model.

Consider:

$$
W=
[
0.81,
0.002,
-0.64,
0.0004,
0.53
]
$$

Very small weights may contribute relatively little.

After pruning:

$$
W'=
[
0.81,
0,
-0.64,
0,
0.53
]
$$

Thus:

$$
\boxed{
\text{Pruning} =
\text{Remove Less-Important Parameters}
}
$$

---

# 🎯 21. Why Pruning?

Potential benefits include:

- increased sparsity,
- reduced number of effective parameters,
- opportunities for model compression,
- possible computational savings when supported by the runtime/hardware.

However, an important point is:

> Setting weights to zero does not automatically guarantee faster inference or lower RAM usage on every MCU implementation.

Actual gains depend on whether the deployment runtime and hardware exploit sparse representations.

---

# 📉 22. Magnitude-Based Pruning

A common approach removes weights with small absolute magnitude.

For a threshold:

$$
T
$$

a simplified rule is:

$$
w_i' = \begin{cases}
0, & |w_i| < T,\\
w_i, & |w_i| \geq T. \end{cases}
$$

Example:

```text
Original

0.72   0.01   -0.65   0.003   0.81

Pruned

0.72   0      -0.65   0       0.81
```

---

# 📊 23. Sparsity

Sparsity describes the proportion of zero-valued parameters.

A simple definition is:

$$
\text{Sparsity} =
\frac{
N_{\text{zero}}
}{
N_{\text{total}}
}
$$

For:

$$
N_{\text{zero}}=700
$$

and:

$$
N_{\text{total}}=1000
$$

then:

$$
\text{Sparsity}=0.7=70\%
$$

---

# 📈 24. Pruning Schedule

Removing too many parameters immediately can severely damage model performance.

Instead, pruning can be gradually increased during training:

```text
Training
   │
   ├── 10% sparsity
   │
   ├── 30% sparsity
   │
   ├── 50% sparsity
   │
   └── 70% sparsity
```

This gives the network an opportunity to adapt.

---

# ⚖️ 25. Pruning Trade-Off

```text
Increasing Sparsity
       │
       ▼
More Weights Removed
       │
       ├── Potential Compression Benefit
       │
       └── Potential Accuracy Loss
```

Therefore:

$$
\boxed{
\text{Pruning Level} =
\text{Compression}
\leftrightarrow
\text{Accuracy}
}
$$

---

# 🔗 26. Combining Pruning and Quantization

Pruning and quantization can be used together.

```text
FP32 Model
    │
    ▼
Pruning
    │
    ▼
Sparse Model
    │
    ▼
Quantization
    │
    ▼
INT8 Model
```

Conceptually:

$$
\boxed{
\text{Optimized Model} =
\text{Pruning}
+
\text{Quantization}
}
$$

However, deployment benefits must be measured on the target ESP32 rather than assumed.

---

# 📦 27. TensorFlow Lite

TensorFlow models are commonly developed using:

```python
tf.keras
```

A trained model can be converted to TensorFlow Lite:

```text
TensorFlow / Keras Model
         │
         ▼
   TFLite Converter
         │
         ▼
      .tflite
```

The `.tflite` file stores the model in a compact format suitable for supported inference runtimes.

---

# 🤏 28. TensorFlow Lite for Microcontrollers

**TensorFlow Lite for Microcontrollers (TFLM)** is designed to execute supported TensorFlow Lite models on microcontrollers and other highly constrained devices.

The architecture can be represented as:

```text
TensorFlow Model
       │
       ▼
TFLite Converter
       │
       ▼
model.tflite
       │
       ▼
TensorFlow Lite
for Microcontrollers
       │
       ▼
ESP32
```

TFLM is designed around embedded constraints and avoids assumptions such as a full desktop operating system.

---

# 🧱 29. TFLM Architecture

A simplified TFLM inference architecture is:

```text
┌─────────────────────────────┐
│      ESP32 Application      │
├─────────────────────────────┤
│        TFLM Runtime         │
├─────────────────────────────┤
│       Operator Kernels      │
├─────────────────────────────┤
│      Tensor Arena / RAM     │
├─────────────────────────────┤
│          ESP32 MCU          │
└─────────────────────────────┘
```

---

# 💾 30. Model Storage on ESP32

A `.tflite` model can be converted into a C/C++ byte array and compiled into firmware.

Conceptually:

```text
model.tflite
     │
     ▼
Binary-to-C Conversion
     │
     ▼
model_data.cc
     │
     ▼
ESP32 Firmware
```

Example:

```cpp
const unsigned char model_data[] = {
    0x20, 0x00, 0x00, 0x00,
    // ...
};
```

---

# 🔧 31. Converting a TFLite Model to a C Array

A commonly used command-line approach on Unix-like systems is:

```bash
xxd -i model.tflite > model_data.cc
```

This generates a byte array that can be included in an embedded project.

The exact development workflow depends on the build environment.

---

# 🧠 32. Tensor Arena

TFLM uses a preallocated memory region commonly called the **tensor arena** for runtime tensor storage and related working memory.

Example:

```cpp
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
```

Conceptually:

```text
ESP32 RAM
   │
   ├── Application Variables
   ├── Stack
   ├── Heap
   └── Tensor Arena
          │
          ├── Input Tensor
          ├── Activations
          └── Intermediate Data
```

Tensor-arena sizing is an important part of embedded deployment.

---

# ⚠️ 33. Tensor Arena Too Small

If the tensor arena is too small:

```text
Model Initialization
       │
       ▼
Memory Allocation
       │
       ▼
FAILED
```

If it is unnecessarily large:

```text
Large Tensor Arena
       │
       ▼
Wasted RAM
```

Therefore:

$$
\boxed{
\text{Tensor Arena}
\approx
\text{Enough Memory for Model Runtime}
}
$$

with suitable margin for the application.

---

# ⚙️ 34. TFLM Operators

A neural network consists of operators such as:

- fully connected,
- convolution,
- pooling,
- reshape,
- softmax,
- activation functions.

TFLM must provide implementations for the operators required by the deployed model.

Conceptually:

```text
Model
 │
 ├── Conv2D
 ├── ReLU
 ├── Pooling
 ├── Fully Connected
 └── Softmax
```

---

# 🧩 35. Operator Resolver

A microcontroller application can register only the required operators.

Conceptually:

```cpp
tflite::MicroMutableOpResolver<4> resolver;

resolver.AddFullyConnected();
resolver.AddRelu();
resolver.AddSoftmax();
resolver.AddReshape();
```

This approach avoids including unnecessary operator registrations.

The exact operators must match the converted model.

---

# 🔄 36. TFLM Inference Process

The inference process can be represented as:

```text
Initialize TFLM
      │
      ▼
Load Model
      │
      ▼
Allocate Tensors
      │
      ▼
Read Sensor
      │
      ▼
Preprocess Input
      │
      ▼
Fill Input Tensor
      │
      ▼
Invoke Model
      │
      ▼
Read Output Tensor
      │
      ▼
Decision
```

---

# 📥 37. Input Tensor

Suppose the model expects:

$$
\mathbf{x} =
[x_1,x_2,x_3,x_4]
$$

The ESP32 must place these values into the model's input tensor.

For an INT8 model:

```text
Sensor Values
     │
     ▼
Preprocessing
     │
     ▼
INT8 Quantization
     │
     ▼
Input Tensor
```

---

# 📤 38. Output Tensor

For three-class classification:

```text
Class 0 → Standing
Class 1 → Walking
Class 2 → Running
```

the output might conceptually be:

$$
\mathbf{y} =
[0.05,0.90,0.05]
$$

The largest score corresponds to:

```text
Walking
```

For quantized models, the raw output tensor may contain integer values that need to be interpreted using the output tensor's quantization parameters.

---

# 📐 39. Quantized ESP32 Input

Suppose preprocessing produces:

$$
x=0.73
$$

For scale:

$$
S=0.01
$$

and zero-point:

$$
Z=-3
$$

the quantized input is:

$$
q= round(
\frac{0.73}{0.01})-3
$$

$$
q=70
$$

provided this lies within the INT8 range.

The ESP32 should use the quantization parameters associated with the actual model tensor.

---

# 🧮 40. Dequantizing Output

For an output integer:

$$
q=80
$$

with:

$$
S=0.01,\qquad Z=-10
$$

the real-valued representation is:

$$
r=S(q-Z)
$$

$$
r=0.01(80+10)
$$

$$
r=0.90
$$

---

# 🚶 41. ESP32 TinyML Case Study

Consider an activity-recognition system using an MPU6050.

```text
MPU6050
   │
   │ I²C
   ▼
ESP32
   │
   ▼
Sampling
   │
   ▼
Feature Extraction
   │
   ▼
INT8 Quantization
   │
   ▼
TFLM Model
   │
   ▼
Prediction
   │
   ├── Standing
   ├── Walking
   └── Running
```

---

# 📊 42. Feature Vector

Suppose the model uses:

$$
\mathbf{x} =
[
\mu_x,
\sigma_x,
RMS_x,
\mu_y,
\sigma_y,
RMS_y,
\mu_z,
\sigma_z,
RMS_z
]
$$

The input layer therefore contains:

$$
9
$$

features.

---

# 🧠 43. ANN Example

A small network might be:

```text
9 Inputs
   │
   ▼
Dense Layer
16 Neurons
   │
   ▼
ReLU
   │
   ▼
Dense Layer
3 Neurons
   │
   ▼
Output
```

This network can classify:

```text
Standing
Walking
Running
```

---

# 💻 44. Training Model in TensorFlow

A simplified Python model is:

```python
import tensorflow as tf

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(9,)),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(3, activation='softmax')
])

model.compile(
    optimizer='adam',
    loss='sparse_categorical_crossentropy',
    metrics=['accuracy']
)
```

Training:

```python
model.fit(
    X_train,
    y_train,
    validation_data=(X_val, y_val),
    epochs=50
)
```

---

# 🔄 45. Basic TFLite Conversion

```python
converter = tf.lite.TFLiteConverter.from_keras_model(model)

tflite_model = converter.convert()

with open("model.tflite", "wb") as f:
    f.write(tflite_model)
```

---

# 🔢 46. INT8 Conversion

```python
def representative_dataset():
    for i in range(100):
        yield [X_train[i:i+1].astype("float32")]
```

Then:

```python
converter = tf.lite.TFLiteConverter.from_keras_model(model)

converter.optimizations = [
    tf.lite.Optimize.DEFAULT
]

converter.representative_dataset = representative_dataset

converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]

converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

quantized_model = converter.convert()
```

Save:

```python
with open("model_int8.tflite", "wb") as f:
    f.write(quantized_model)
```

---

# 📦 47. Compare Model Sizes

```python
import os

fp32_size = os.path.getsize("model.tflite")
int8_size = os.path.getsize("model_int8.tflite")

print("FP32:", fp32_size)
print("INT8:", int8_size)
```

| Model | File Size | Accuracy |
|---|---:|---:|
| FP32 | | |
| INT8 | | |

---

# ✂️ 48. Pruning with TensorFlow Model Optimization

```text
Original Model
      │
      ▼
Pruning Wrapper
      │
      ▼
Pruning-Aware Training
      │
      ▼
Strip Pruning
      │
      ▼
Optimized Model
```

Example:

```python
import tensorflow_model_optimization as tfmot

prune_low_magnitude = (
    tfmot.sparsity.keras.prune_low_magnitude
)
```

---

# 📉 49. Pruning Schedule Example

```python
pruning_params = {
    "pruning_schedule":
        tfmot.sparsity.keras.PolynomialDecay(
            initial_sparsity=0.0,
            final_sparsity=0.50,
            begin_step=0,
            end_step=1000
        )
}
```

Then:

```python
model_for_pruning = prune_low_magnitude(
    model,
    **pruning_params
)
```

---

# 🧹 50. Strip Pruning

```python
model_for_export = (
    tfmot.sparsity.keras.strip_pruning(
        model_for_pruning
    )
)
```

---

# 📊 51. Evaluate Before and After Optimization

| Metric | FP32 | INT8 | Pruned | Pruned + INT8 |
|---|---:|---:|---:|---:|
| Accuracy | | | | |
| Model size | | | | |
| RAM use | | | | |
| Inference time | | | | |
| Energy/inference | | | | |

---

# ⏱️ 52. Measuring Inference Time on ESP32

```cpp
unsigned long startTime = micros();

interpreter->Invoke();

unsigned long endTime = micros();

Serial.print("Inference time: ");
Serial.print(endTime - startTime);
Serial.println(" us");
```

$$
T_{\text{inference}} =
t_{\text{end}}-t_{\text{start}}
$$

---

# 💾 53. Measuring Memory Usage

$$
M_{\text{total}} =
M_{\text{model}}
+
M_{\text{tensor}}
+
M_{\text{application}}
+
M_{\text{runtime}}
$$

---

# ⚡ 54. Measuring Energy per Inference

$$
E=
\int V(t)I(t)\,dt
$$

For approximately constant voltage:

$$
E\approx VI_{\text{avg}}T
$$

---

# 📊 55. Performance per Resource

$$
\boxed{
\text{Embedded ML Quality} =
f(
\text{Accuracy},
\text{Latency},
\text{Memory},
\text{Energy}
)
}
$$

---

# 🧠 56. Model Complexity

For a fully connected layer:

$$
N_W=N_{\text{in}}N_{\text{out}}
$$

Including biases:

$$
N_P=
N_{\text{in}}N_{\text{out}}
+
N_{\text{out}}
$$

---

# 🧮 57. Example Parameter Count

For:

```text
Input = 9
Hidden = 16
Output = 3
```

Layer 1:

$$
N_1=9(16)+16=160
$$

Layer 2:

$$
N_2=16(3)+3=51
$$

Total:

$$
N_P=160+51=211
$$

parameters.

---

# 🪶 58. Model Architecture Optimization

Optimization can begin by:

- reducing layers,
- reducing neurons,
- reducing filters,
- reducing input dimensions,
- extracting compact features,
- selecting useful features.

$$
\boxed{
\text{Efficient Architecture} >
\text{Needlessly Large Model + Optimization}
}
$$

---

# 🎯 59. Feature Extraction as Optimization

Suppose raw data contains:

$$
N=300
$$

samples.

Feature extraction may reduce this to:

$$
N_f=9
$$

features.

Thus:

$$
300\rightarrow9
$$

input values.

---

# 🔄 60. Complete TinyML Optimization Pipeline

```text
Raw Dataset
     │
     ▼
Data Cleaning
     │
     ▼
Feature Extraction
     │
     ▼
Train FP32 Model
     │
     ▼
Baseline Evaluation
     │
     ▼
Model Optimization
     │
     ├── Architecture Reduction
     ├── Pruning
     └── INT8 Quantization
     │
     ▼
TFLite Model
     │
     ▼
TFLM Deployment
     │
     ▼
ESP32 Testing
     │
     ├── Accuracy
     ├── Latency
     ├── Flash
     ├── RAM
     └── Energy
```

---

# ⚠️ 61. Operator Compatibility

```text
TensorFlow Model
      │
      ▼
TFLite Conversion
      │
      ▼
Check Operators
      │
      ▼
TFLM Support?
    /       \
  Yes        No
   │          │
Deploy     Modify Model
```

---

# 🔍 62. Verify the Converted Model

```text
Keras Model
     │
     ▼
Evaluate
     │
     ▼
Convert
     │
     ▼
TFLite Model
     │
     ▼
Evaluate Again
```

Compare:

$$
A_{\text{FP32}}
$$

with:

$$
A_{\text{INT8}}
$$

$$
\Delta A =
A_{\text{INT8}} -
A_{\text{FP32}}
$$

---

# 📉 63. Acceptable Accuracy Loss

Suppose:

$$
A_{\text{FP32}}=95.2\%
$$

and:

$$
A_{\text{INT8}}=94.8\%
$$

Then:

$$
\Delta A=-0.4
$$

percentage points.

---

# 🔬 64. Optimization Experiment

Train four variants:

```text
Model A → FP32 Baseline
Model B → INT8
Model C → Pruned
Model D → Pruned + INT8
```

Compare:

- accuracy,
- model size,
- RAM,
- latency,
- energy.

---

# 🧪 65. Exercise 1 — Parameter Memory

Given:

$$
N_p=250,000
$$

Calculate approximate parameter memory for:

1. FP32,
2. INT8.

---

# 🧪 66. Exercise 2 — Quantization

Given:

$$
r=0.64,\quad S=0.01,\quad Z=-5
$$

Calculate:

$$
q= round(\frac{r}{S})+Z
$$

Then reconstruct:

$$
\hat r=S(q-Z)
$$

and calculate:

$$
e_q=r-\hat r
$$

---

# 🧪 67. Exercise 3 — Pruning

Given:

$$
W=
[
0.75,
0.02,
-0.60,
0.005,
0.40,
-0.01
]
$$

and:

$$
T=0.03
$$

apply threshold pruning and calculate resulting sparsity.

---

# 🧪 68. Exercise 4 — Compare FP32 and INT8

| Metric | FP32 | INT8 |
|---|---:|---:|
| Test accuracy | | |
| File size | | |
| Tensor arena | | |
| Inference time | | |

---

# 🧪 69. Exercise 5 — Representative Dataset

Compare calibration using:

```text
Dataset A
10 samples

Dataset B
100 representative samples

Dataset C
100 samples from only one class
```

---

# 🧪 70. Exercise 6 — Pruning Level

| Sparsity | Accuracy | File Size | Inference Time |
|---:|---:|---:|---:|
| 0% | | | |
| 25% | | | |
| 50% | | | |
| 75% | | | |
| 90% | | | |

---

# 🚀 71. Advanced Exercise — ESP32 INT8 Activity Classifier

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
Normalization
   │
   ▼
INT8 Conversion
   │
   ▼
TFLM
   │
   ▼
Classification
   │
   ├── Standing
   ├── Walking
   └── Running
```

Measure:

- model size,
- tensor arena,
- inference latency,
- classification accuracy.

---

# 📡 72. TinyML + MQTT

```text
TFLM Prediction
       │
       ▼
Activity
       │
       ▼
MQTT Publish
       │
       ▼
Broker
       │
       ▼
Dashboard
```

Example:

```json
{
  "device": "esp32-ai-01",
  "activity": "walking",
  "confidence": 0.93
}
```

---

# 🔋 73. Optimized TinyML + Sleep Modes

```text
Deep Sleep
    │
    ▼
Wake
    │
    ▼
Sensor Sampling
    │
    ▼
Feature Extraction
    │
    ▼
INT8 TFLM Inference
    │
    ▼
Important Event?
   / \
 No   Yes
 │     │
Sleep  Wi-Fi / BLE / LPWAN
        │
        ▼
       Sleep
```

---

# 🪞 74. TinyML and Digital Twins

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
INT8 TinyML
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

---

# 🔐 75. Optimization and Reliability

Important questions include:

- Does accuracy remain acceptable?
- Are minority classes still detected?
- Does quantization change false-negative rates?
- Does pruning remove important behavior?
- Is inference deterministic enough for the application?
- Does the model fit available memory?
- Is latency acceptable?

$$
\boxed{
\text{Optimization}
\neq
\text{Minimize Size at Any Cost}
}
$$

---

# 📊 76. Optimization Metrics

| Metric | Description |
|---|---|
| Accuracy | Overall prediction correctness |
| Precision | Reliability of positive predictions |
| Recall | Ability to detect actual positives |
| F1-score | Balance between precision and recall |
| Model size | Flash/storage requirement |
| Tensor arena | TFLM working-memory requirement |
| Inference latency | Time per prediction |
| Energy/inference | Energy required per prediction |
| Sparsity | Fraction of zero parameters |

---

# 💬 77. Discussion Questions

1. Why is model optimization important for ESP32 TinyML?
2. What hardware constraints affect TinyML models?
3. What is quantization?
4. What is INT8?
5. How does INT8 differ from FP32?
6. Why can INT8 reduce model storage?
7. What is quantization error?
8. What is a quantization scale?
9. What is a zero-point?
10. What is affine quantization?
11. What is Post-Training Quantization?
12. What is a representative dataset?
13. Why is representative calibration data important?
14. What is Quantization-Aware Training?
15. How does QAT differ from PTQ?
16. When might QAT be useful?
17. What is pruning?
18. What is magnitude-based pruning?
19. What is sparsity?
20. Why can excessive pruning reduce accuracy?
21. Why does sparsity not automatically make an ESP32 model faster?
22. Can pruning and quantization be combined?
23. What is TensorFlow Lite?
24. What is TensorFlow Lite for Microcontrollers?
25. What is a `.tflite` model?
26. How can a `.tflite` model be embedded in firmware?
27. What is a tensor arena?
28. Why is tensor-arena sizing important?
29. What is an operator in TFLM?
30. Why must model operators be supported?
31. What is an input tensor?
32. What is an output tensor?
33. How are INT8 inputs generated?
34. How are INT8 outputs interpreted?
35. Why should a converted model be evaluated before deployment?
36. How is inference latency measured?
37. Why should RAM and flash be measured separately?
38. Why should energy per inference be measured?
39. How can feature extraction reduce model complexity?
40. Why should optimized models be tested on the actual ESP32?

---

# 🧾 78. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain why model optimization is required for microcontrollers,
- identify ESP32 resource constraints,
- distinguish FP32 and INT8 representations,
- explain quantization,
- calculate simple affine quantization values,
- calculate quantization error,
- explain scale and zero-point,
- describe Post-Training Quantization,
- explain representative-dataset calibration,
- describe Quantization-Aware Training,
- compare PTQ and QAT,
- explain neural-network pruning,
- calculate model sparsity,
- describe magnitude-based pruning,
- explain pruning schedules,
- combine pruning and quantization conceptually,
- explain TensorFlow Lite,
- explain TensorFlow Lite for Microcontrollers,
- convert a Keras model to TFLite,
- perform basic INT8 conversion,
- embed a TFLite model in MCU firmware,
- explain the tensor arena,
- identify required TFLM operators,
- prepare input tensors,
- interpret quantized output tensors,
- calculate neural-network parameter counts,
- compare FP32 and INT8 model sizes,
- measure inference latency,
- evaluate RAM and flash requirements,
- compare optimized models using accuracy and resource metrics,
- deploy an optimized TinyML model to ESP32.

---

# 🔑 79. Key Concepts

### Model Optimization

$$
\boxed{
\text{Model Optimization} =
\text{Reduce Resource Usage}
+
\text{Preserve Required Performance}
}
$$

### FP32 Parameter Memory

$$
\boxed{
M_{\text{FP32}}
\approx
4N_p
}
$$

### INT8 Parameter Memory

$$
\boxed{
M_{\text{INT8}}
\approx
N_p
}
$$

### Affine Quantization

$$
\boxed{
q = round(\frac{r}{S})+Z
}
$$

### Dequantization

$$
\boxed{
r=S(q-Z)
}
$$

### Quantization Error

$$
\boxed{
e_q=r-\hat r
}
$$

### Sparsity

$$
\boxed{
\text{Sparsity} =
\frac{N_{\text{zero}}}{N_{\text{total}}}
}
$$

### Parameter Count

$$
\boxed{
N_P =
N_{\text{in}}N_{\text{out}}
+
N_{\text{out}}
}
$$

### Inference Latency

$$
\boxed{
T_{\text{inference}} =
t_{\text{end}}-t_{\text{start}}
}
$$

### Embedded Model Evaluation

$$
\boxed{
\text{Performance} =
f(
\text{Accuracy},
\text{Memory},
\text{Latency},
\text{Energy}
)
}
$$

---

# 🎯 80. Summary

TinyML models must operate within the limited memory, computation, and energy resources available on microcontrollers such as the ESP32. **Model optimization** is therefore an important stage between model development and embedded deployment.

Two major optimization techniques are:

$$
\boxed{
\text{Optimization} =
\text{Quantization}
+
\text{Pruning}
}
$$

**INT8 quantization** replaces higher-precision numerical representations with 8-bit integer representations. A common affine model is:

$$
r=S(q-Z)
$$

which allows real-valued model quantities to be represented using integers together with scale and zero-point parameters.

Compared with FP32 parameter storage:

$$
32~\text{bits}
\rightarrow8~\text{bits}
$$

can theoretically reduce parameter storage by approximately:

$$
4\times
$$

although the complete `.tflite` model and runtime memory will not necessarily shrink by exactly this factor.

**Pruning** introduces sparsity by removing or zeroing less-important model parameters:

$$
w_i' = \begin{cases}
0, & |w_i| < T,\\
w_i, & |w_i| \geq T. \end{cases}
$$

However, sparsity does not automatically produce faster MCU inference. Actual improvements depend on runtime and hardware support.

**TensorFlow Lite for Microcontrollers** provides a framework for executing supported TensorFlow Lite models on resource-constrained embedded devices.

The complete ESP32 workflow can be represented as:

```text
Dataset
   │
   ▼
Feature Extraction
   │
   ▼
TensorFlow Training
   │
   ▼
FP32 Model
   │
   ▼
Pruning / INT8 Quantization
   │
   ▼
TFLite Model
   │
   ▼
TFLM
   │
   ▼
ESP32
   │
   ▼
On-Device Inference
```

A successful embedded model should therefore be evaluated not only by accuracy but also by:

$$
\boxed{
\text{TinyML Quality} =
\text{Accuracy}
+
\text{Small Memory}
+
\text{Low Latency}
+
\text{Low Energy}
}
$$

This optimization workflow provides the foundation for deploying **activity recognition, anomaly detection, predictive maintenance, smart sensing, wearable intelligence, digital twins, and low-power intelligent IoT applications** on ESP32-class microcontrollers.

---

# 📘 References

1. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*, O'Reilly Media, 2019.
2. R. David et al., “TensorFlow Lite Micro: Embedded Machine Learning for TinyML Systems,” *Proceedings of Machine Learning and Systems*, vol. 3, pp. 800–811, 2021.
3. TensorFlow, *TensorFlow Lite for Microcontrollers Documentation*.
4. TensorFlow, *TensorFlow Model Optimization Toolkit Documentation*.
5. TensorFlow, *Post-Training Quantization Documentation*.
6. TensorFlow, *Quantization-Aware Training Documentation*.
7. Espressif Systems, *ESP-DL User Guide and Documentation*.
8. Espressif Systems, *ESP32 Series Datasheets and Technical Reference Manuals*.
9. J. Lin et al., “MCUNet: Tiny Deep Learning on IoT Devices,” *Advances in Neural Information Processing Systems*, 2020.
10. S. Han et al., “Learning Both Weights and Connections for Efficient Neural Networks,” *Advances in Neural Information Processing Systems*, 2015.
11. S. Han, H. Mao, and W. J. Dally, “Deep Compression: Compressing Deep Neural Networks with Pruning, Trained Quantization and Huffman Coding,” *International Conference on Learning Representations*, 2016.
12. B. Jacob et al., “Quantization and Training of Neural Networks for Efficient Integer-Arithmetic-Only Inference,” *IEEE/CVF Conference on Computer Vision and Pattern Recognition*, 2018.

---

