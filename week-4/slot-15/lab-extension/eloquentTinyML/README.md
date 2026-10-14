# LAB — LDR Classification using EloquentTinyML
## Dark, Normal, and Bright Classification with ESP32

## 1. Objective

The objective of this lab is to:

1. Read LDR measurements using the ESP32 ADC.
2. Prepare a labeled dataset for:
   - **Bright**
   - **Normal**
   - **Dark**
3. Train a small ANN using TensorFlow/Keras.
4. Convert the trained model into a TensorFlow Lite model.
5. Export the model as a C array.
6. Deploy the model using **EloquentTinyML**.
7. Perform real-time ANN inference on the ESP32.
8. Measure inference latency.
9. Compare EloquentTinyML with plain ANN and direct TFLM approaches.

EloquentTinyML is an Arduino-oriented wrapper around TensorFlow Lite for Microcontrollers that simplifies model loading and inference.

---

## 2. Learning Outcomes

After completing this lab, students should be able to:

- explain the purpose of EloquentTinyML;
- train a small TensorFlow neural network;
- convert a Keras model into TensorFlow Lite;
- export a `.tflite` model as an embedded C array;
- configure EloquentTinyML for ESP32;
- load a TinyML model;
- pass LDR data to the network;
- obtain class scores;
- select the predicted class;
- measure inference time.

---

## 3. Why EloquentTinyML?

Direct TFLM programming normally requires several components:

```text
Model
 ↓
Operator Resolver
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

EloquentTinyML provides a higher-level interface:

```text
Model
 ↓
EloquentTinyML
 ↓
begin()
 ↓
predict()
 ↓
Classification Result
```

This makes TinyML easier to introduce in an Arduino-oriented embedded-systems course.

---

## 4. System Architecture

```text
Ambient Light
      ↓
     LDR
      ↓
ESP32 ADC
      ↓
Normalization
      ↓
EloquentTinyML
      ↓
TFLite Model
      ↓
ANN Inference
      ↓
Dark / Normal / Bright
```

Development workflow:

```text
LDR Dataset
     ↓
TensorFlow / Keras
     ↓
ANN Training
     ↓
TensorFlow Lite
     ↓
C Array
     ↓
EloquentTinyML
     ↓
ESP32
     ↓
Real-Time Classification
```

---

## 5. Hardware Requirements

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

## 6. Software Requirements

- Arduino IDE
- ESP32 Arduino board package
- Python
- TensorFlow
- NumPy
- Pandas
- EloquentTinyML
- ESP32 TFLM runtime required by the installed EloquentTinyML version

Current EloquentTinyML versions may use a runtime-specific library such as `tflm_esp32` for ESP32 targets. Older examples may use a different API, so students should follow the examples that match the installed library version.

---

## 7. Install EloquentTinyML

In Arduino IDE:

```text
Sketch
 ↓
Include Library
 ↓
Manage Libraries
 ↓
Search: EloquentTinyML
```

Install the current EloquentTinyML library.

For a current ESP32 runtime, also install the corresponding TensorFlow Lite Micro runtime if required by the installed version, for example:

```text
tflm_esp32
```

---

## 8. LDR Circuit

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

For this lab, use the teaching ranges:

```text
Bright : 0–99
Normal : 100–499
Dark   : 500–1000
```

Actual thresholds should be calibrated from the real circuit.

---

## 9. Dataset

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
34,bright
82,bright
180,normal
425,normal
650,dark
920,dark
```

---

## 10. Class Encoding

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

---

## 11. Input Normalization

For the current dataset:

```text
0 ... 1000
```

normalize with:

\[
x_n=\frac{x}{1000}
\]

Examples:

```text
50   → 0.050
250  → 0.250
800  → 0.800
```

The ESP32 must use the same preprocessing as the Python training code.

---

## 12. ANN Architecture

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

Parameter count:

\[
1\times4+4+4\times3+3=23
\]

---

## 13. TensorFlow Training Code

```python
import tensorflow as tf
import numpy as np
import pandas as pd

# --------------------------------
# Load Dataset
# --------------------------------

df = pd.read_csv(
    "light_dataset.csv"
)

# --------------------------------
# Prepare Input
# --------------------------------

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

# --------------------------------
# Encode Labels
# --------------------------------

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

# --------------------------------
# Shuffle
# --------------------------------

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

# --------------------------------
# Split Dataset
# --------------------------------

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

# --------------------------------
# Model
# --------------------------------

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

# --------------------------------
# Train
# --------------------------------

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

# --------------------------------
# Evaluate
# --------------------------------

loss, accuracy = model.evaluate(
    X_test,
    y_test,
    verbose=0
)

print(
    "Test Accuracy:",
    accuracy
)
```

---

## 14. Convert the Model to TensorFlow Lite

```python
converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

tflite_model = converter.convert()

with open(
    "ldr_model.tflite",
    "wb"
) as f:

    f.write(
        tflite_model
    )
```

---

## 15. Export Model as C Array

One option is:

```bash
xxd -i ldr_model.tflite > ldr_model.h
```

Conceptual output:

```cpp
unsigned char ldr_model_tflite[] =
{
  0x20, 0x00, 0x00, 0x00,
  ...
};
```

---

## 16. Arduino Project Structure

```text
LDR_EloquentTinyML/
│
├── LDR_EloquentTinyML.ino
└── ldr_model.h
```

---

## 17. EloquentTinyML Programming Model

A current EloquentTinyML API may use:

```cpp
Eloquent::TF::Sequential
```

The workflow is:

```text
Create Sequential object
       ↓
setNumInputs()
       ↓
setNumOutputs()
       ↓
begin(model)
       ↓
predict(input)
       ↓
classification / output()
```

---

## 18. Include Libraries

A current-style ESP32 application may use:

```cpp
#include "ldr_model.h"

#include <tflm_esp32.h>

#include <EloquentTinyML.h>
```

Exact include names can vary by EloquentTinyML/runtime release, so use the examples shipped with the installed version when compiling.

---

## 19. Define Model Configuration

For the `1–4–3` ANN:

```cpp
#define NUM_INPUTS  1
#define NUM_OUTPUTS 3
```

Example configuration:

```cpp
#define NUM_OPS     4
#define ARENA_SIZE  (8 * 1024)
```

The Tensor Arena size may need adjustment.

---

## 20. Create EloquentTinyML Model Object

Conceptually:

```cpp
Eloquent::TF::Sequential<
    NUM_OPS,
    ARENA_SIZE
> tf;
```

---

## 21. Initialize the Model

Inside `setup()`:

```cpp
tf.setNumInputs(
    NUM_INPUTS
);

tf.setNumOutputs(
    NUM_OUTPUTS
);

if (
    !tf.begin(
        ldr_model_tflite
    ).isOk()
)
{
    Serial.println(
        "Model initialization failed"
    );

    while (true)
    {
        delay(
            1000
        );
    }
}
```

---

## 22. Prepare LDR Input

Read the LDR:

```cpp
int adcValue =
  analogRead(
    LDR_PIN
  );
```

Normalize:

```cpp
float normalized =
  adcValue /
  1000.0f;
```

Prepare the ANN input:

```cpp
float input[
  NUM_INPUTS
];

input[0] =
  normalized;
```

---

## 23. Run Prediction

Conceptually:

```cpp
if (
    !tf.predict(
        input
    ).isOk()
)
{
    Serial.println(
        "Prediction failed"
    );

    return;
}
```

---

## 24. Obtain Classification

```cpp
int predictedClass =
  tf.classification;
```

Individual outputs can be read conceptually using:

```cpp
tf.output(i)
```

---

## 25. Class Names

```cpp
const char* classNames[] =
{
  "Dark",
  "Normal",
  "Bright"
};
```

Display:

```cpp
Serial.print(
  "Prediction: "
);

Serial.println(
  classNames[
    predictedClass
  ]
);
```

---

## 26. Display Output Scores

```cpp
Serial.print(
  "Dark: "
);

Serial.println(
  tf.output(0),
  4
);

Serial.print(
  "Normal: "
);

Serial.println(
  tf.output(1),
  4
);

Serial.print(
  "Bright: "
);

Serial.println(
  tf.output(2),
  4
);
```

---

## 27. Measure Inference Time

A current EloquentTinyML API may expose benchmarking support.

Conceptually:

```cpp
Serial.print(
  "Inference time: "
);

Serial.print(
  tf.benchmark.microseconds()
);

Serial.println(
  " us"
);
```

---

## 28. Example Complete Arduino Structure

The exact includes may need minor adjustment for the installed EloquentTinyML/runtime release.

```cpp
#include <Arduino.h>

#include "ldr_model.h"

#include <tflm_esp32.h>
#include <EloquentTinyML.h>


#define LDR_PIN 36

#define NUM_INPUTS  1
#define NUM_OUTPUTS 3

#define NUM_OPS     4
#define ARENA_SIZE  (8 * 1024)


Eloquent::TF::Sequential<
  NUM_OPS,
  ARENA_SIZE
> tf;


const char* classNames[
  NUM_OUTPUTS
] =
{
  "Dark",
  "Normal",
  "Bright"
};


void setup()
{
  Serial.begin(
    115200
  );

  pinMode(
    LDR_PIN,
    INPUT
  );


  tf.setNumInputs(
    NUM_INPUTS
  );

  tf.setNumOutputs(
    NUM_OUTPUTS
  );


  if (
    !tf.begin(
      ldr_model_tflite
    ).isOk()
  )
  {
    Serial.println(
      "Cannot initialize model"
    );

    while (
      true
    )
    {
      delay(
        1000
      );
    }
  }


  Serial.println(
    "EloquentTinyML LDR Classifier"
  );
}


void loop()
{
  int adcValue =
    analogRead(
      LDR_PIN
    );


  float normalized =
    adcValue /
    1000.0f;


  float input[
    NUM_INPUTS
  ] =
  {
    normalized
  };


  if (
    !tf.predict(
      input
    ).isOk()
  )
  {
    Serial.println(
      "Inference error"
    );

    delay(
      1000
    );

    return;
  }


  int predictedClass =
    tf.classification;


  Serial.print(
    "ADC: "
  );

  Serial.println(
    adcValue
  );


  Serial.print(
    "Normalized: "
  );

  Serial.println(
    normalized,
    4
  );


  Serial.print(
    "Dark: "
  );

  Serial.println(
    tf.output(0),
    4
  );


  Serial.print(
    "Normal: "
  );

  Serial.println(
    tf.output(1),
    4
  );


  Serial.print(
    "Bright: "
  );

  Serial.println(
    tf.output(2),
    4
  );


  Serial.print(
    "Prediction: "
  );

  Serial.println(
    classNames[
      predictedClass
    ]
  );


  Serial.print(
    "Inference time: "
  );

  Serial.print(
    tf.benchmark.microseconds()
  );

  Serial.println(
    " us"
  );


  Serial.println(
    "--------------------"
  );


  delay(
    500
  );
}
```

---

## 29. Expected Serial Output

```text
ADC: 52
Normalized: 0.0520

Dark:   0.0121
Normal: 0.0184
Bright: 0.9695

Prediction: Bright
Inference time: 125 us
```

Another example:

```text
ADC: 312
Normalized: 0.3120

Dark:   0.0310
Normal: 0.9467
Bright: 0.0223

Prediction: Normal
Inference time: 123 us
```

---

## 30. Optional LED Output

Define:

```cpp
#define LED_BRIGHT 16
#define LED_NORMAL 17
#define LED_DARK   18
```

Then:

```cpp
digitalWrite(
  LED_DARK,
  predictedClass == 0
);

digitalWrite(
  LED_NORMAL,
  predictedClass == 1
);

digitalWrite(
  LED_BRIGHT,
  predictedClass == 2
);
```

---

## 31. Experiment 1 — Train the Model

| Metric | Result |
|---|---:|
| Training accuracy | |
| Validation accuracy | |
| Test accuracy | |
| Loss | |

---

## 32. Experiment 2 — Convert to TFLite

Generate:

```text
ldr_model.tflite
```

Record the file size.

| Item | Result |
|---|---:|
| Keras model | |
| TFLite model | |

---

## 33. Experiment 3 — Export C Array

Generate:

```text
ldr_model.h
```

Verify that the Arduino project can include the model.

---

## 34. Experiment 4 — Real-Time Classification

Test:

```text
ADC = 30
ADC = 250
ADC = 800
```

Expected:

```text
30  → Bright
250 → Normal
800 → Dark
```

Record:

| ADC | Normalized | Predicted |
|---:|---:|---|
| 30 | 0.030 | |
| 250 | 0.250 | |
| 800 | 0.800 | |

---

## 35. Experiment 5 — Inference Latency

Record:

| Test | Time (µs) |
|---:|---:|
| 1 | |
| 2 | |
| 3 | |
| 4 | |
| 5 | |
| Mean | |

---

## 36. Experiment 6 — Plain ANN vs EloquentTinyML

| Feature | Plain ANN | EloquentTinyML |
|---|---|---|
| Model representation | Arrays | `.tflite` |
| Manual forward propagation | Yes | No |
| TensorFlow workflow | Partial | Direct |
| Runtime required | No | Yes |
| Code simplicity | Simple for tiny models | Easier for scalable models |
| Model replacement | Manual | Easier |

---

## 37. Experiment 7 — Direct TFLM vs EloquentTinyML

### Direct TFLM

```text
GetModel()
 ↓
Resolver
 ↓
Tensor Arena
 ↓
MicroInterpreter
 ↓
AllocateTensors()
 ↓
Input
 ↓
Invoke()
 ↓
Output
```

### EloquentTinyML

```text
Sequential
 ↓
begin()
 ↓
predict()
 ↓
classification
```

Compare:

| Feature | Direct TFLM | EloquentTinyML |
|---|---|---|
| Low-level control | High | Lower |
| Code length | Longer | Shorter |
| Teaching complexity | Higher | Lower |
| Tensor Arena visible | Yes | Abstracted |
| Operator management | Explicit | Simplified |
| Beginner friendly | Moderate | High |

---

## 38. Experiment 8 — Threshold vs TinyML

Compare:

```text
Threshold
Plain ANN
TFLM
EloquentTinyML
```

| Method | Accuracy | Memory | Latency | Complexity |
|---|---:|---:|---:|---|
| Threshold | | | | Low |
| Plain ANN | | | | Low–Medium |
| Direct TFLM | | | | High |
| EloquentTinyML | | | | Medium |

---

## 39. Optional Feature Extraction Extension

The lab can be extended from one LDR measurement to:

```text
Mean
Standard Deviation
Minimum
Maximum
```

using a short LDR window.

The ANN becomes:

```text
4 Inputs
   ↓
8 Hidden Neurons
   ↓
3 Outputs
```

Input:

```text
[mean, std, min, max]
```

---

## 40. Feature-Based Architecture

```text
LDR
 ↓
20 Samples
 ↓
Feature Extraction
 ↓
Mean / Std / Min / Max
 ↓
Normalization
 ↓
EloquentTinyML
 ↓
ANN
 ↓
Dark / Normal / Bright
```

This is a useful next exercise after the single-input model.

---

## 41. EloquentTinyML vs TFLM

EloquentTinyML does not replace the underlying TinyML runtime.

Conceptually:

```text
Application
    ↓
EloquentTinyML
    ↓
TFLM Runtime
    ↓
ESP32
```

Its purpose is to simplify the interface between Arduino application code and the TensorFlow Lite Micro runtime.

---

## 42. Advantages of EloquentTinyML

For teaching and rapid prototyping:

```text
Less boilerplate code
Simpler inference API
Easier model loading
Built-in classification helpers
Built-in benchmarking
Compatible with Arduino workflow
```

---

## 43. Limitations

Students should also understand that:

```text
EloquentTinyML
≠ TensorFlow training framework
```

Training still occurs on the computer.

The library primarily simplifies:

```text
Model Loading
Inference
Classification
Benchmarking
```

It also depends on the underlying TFLM runtime and supported operators.

---

## 44. API Version Note

EloquentTinyML has changed substantially over time.

Older examples may contain:

```cpp
Eloquent::TinyML::TensorFlow::TensorFlow<
    N_INPUTS,
    N_OUTPUTS,
    TENSOR_ARENA_SIZE
>
```

while newer versions may use:

```cpp
Eloquent::TF::Sequential<
    NUM_OPS,
    ARENA_SIZE
>
```

Therefore, students should avoid mixing code examples from different major versions.

---

## 45. Checkpoint Questions

1. What is EloquentTinyML?
2. How is EloquentTinyML related to TFLM?
3. Why is the Keras model converted to TensorFlow Lite?
4. Why must the model be embedded as a C array?
5. What does `tf.begin()` do?
6. What does `tf.predict()` do?
7. What does `tf.classification` represent?
8. Why must the LDR normalization used on ESP32 match training?
9. What is the purpose of the Tensor Arena?
10. What is the main difference between direct TFLM and EloquentTinyML?

---

## 46. Exercises

### Exercise 1 — Train ANN

Train:

```text
1 → 4 → 3
```

### Exercise 2 — Export Model

Generate:

```text
ldr_model.tflite
```

### Exercise 3 — Convert Model

Generate:

```text
ldr_model.h
```

### Exercise 4 — Deploy

Run the model on ESP32 using EloquentTinyML.

### Exercise 5 — Measure Latency

Use the built-in benchmark facility.

### Exercise 6 — LED Classification

Activate LEDs for:

```text
Dark
Normal
Bright
```

### Exercise 7 — Feature Extension

Change the model from:

```text
1 input
```

to:

```text
4 features
```

using:

```text
Mean
Std
Min
Max
```

---

## 47. Expected Results

The final system should operate as:

```text
Ambient Light
      ↓
LDR
      ↓
ADC
      ↓
Normalize
      ↓
EloquentTinyML
      ↓
TFLite ANN
      ↓
Classification
      ↓
Dark / Normal / Bright
```

Example:

```text
ADC: 60
Prediction: Bright

ADC: 280
Prediction: Normal

ADC: 760
Prediction: Dark
```

---

## 48. Submission

Students should submit:

- LDR dataset
- TensorFlow training code
- training accuracy
- validation accuracy
- test accuracy
- `ldr_model.tflite`
- generated model header
- ESP32 EloquentTinyML code
- inference latency
- classification results
- plain ANN vs EloquentTinyML comparison
- direct TFLM vs EloquentTinyML comparison
- short discussion

---

## 49. Conclusion

This lab introduces a simplified TinyML deployment workflow using **EloquentTinyML**:

```text
LDR Data
   ↓
TensorFlow Training
   ↓
TFLite Model
   ↓
C Array
   ↓
EloquentTinyML
   ↓
TFLM Runtime
   ↓
ESP32
   ↓
Real-Time Classification
```

Compared with direct TFLM programming, EloquentTinyML reduces much of the interpreter and inference boilerplate and provides a convenient Arduino-oriented interface. This makes it useful as an intermediate teaching step between a manually coded ANN and lower-level TensorFlow Lite Micro deployment.

A useful course progression is:

```text
Threshold Classification
        ↓
Plain ANN
        ↓
EloquentTinyML
        ↓
Direct TFLM Float32
        ↓
TFLM INT8
        ↓
Feature-Based TinyML
```

This allows students to first understand neural-network arithmetic, then use a simplified deployment framework, and finally examine the lower-level memory and quantization mechanisms used in embedded TinyML.
