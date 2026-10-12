# Assignment — ANN Light Classification with LDR Feature Extraction
## Dark, Normal, and Bright Using ESP32 + Plain ANN Code

## 1. Objective

The objective of this lab is to:

1. Use the **LDR dataset collected in the previous lab**.
2. Group consecutive LDR measurements into fixed-size windows.
3. Extract useful statistical features from each LDR window.
4. Normalize the extracted features.
5. Encode three light classes:
   - **Dark**
   - **Normal**
   - **Bright**
6. Build a small ANN using **plain Python/NumPy code** without TensorFlow.
7. Train and evaluate a **5–4–3 ANN**.
8. Export trained weights and biases.
9. Reproduce the same feature extraction on the ESP32.
10. Perform real-time ANN inference using plain Arduino C++.
11. Compare **raw-value classification**, **feature-based ANN classification**, and the previous **threshold method**.

---

## 2. Background

The previous version of this lab used only one instantaneous LDR reading as the ANN input:

```text
LDR ADC
   ↓
Normalize
   ↓
1 Input
   ↓
ANN
   ↓
Dark / Normal / Bright
```

A single reading is simple, but it may be affected by:

```text
Sensor Noise
Light Flicker
ADC Variation
Temporary Shadows
Small Hand Movements
```

This revised lab introduces **feature extraction**.

Instead of classifying one ADC value, the ESP32 first collects a short window of LDR measurements:

```text
x1, x2, x3, ... , xN
```

and calculates descriptive features.

The new workflow becomes:

```text
LDR
 ↓
ESP32 ADC
 ↓
Fixed-Length Window
 ↓
Feature Extraction
 ↓
Normalization
 ↓
ANN
 ↓
Dark / Normal / Bright
```

Feature extraction converts a sequence of sensor samples into a compact numerical description of the light condition.

---

## 3. Why Use Feature Extraction?

Suppose the LDR produces:

```text
1320
1380
1355
1410
1375
```

Rather than sending all values to the ANN, calculate statistics such as:

```text
Mean
Standard Deviation
Minimum
Maximum
Range
```

These describe both:

- the average illumination level; and
- the stability or variation of the light signal.

Feature extraction can therefore reduce noise and make the classifier more robust.

---

## 4. System Architecture

### Training Phase

```text
LDR CSV Dataset
      ↓
Windowing
      ↓
Feature Extraction
      ↓
Mean
Std
Min
Max
Range
      ↓
Feature Normalization
      ↓
ANN 5–4–3
      ↓
Training
      ↓
Weights + Biases
```

### Deployment Phase

```text
LDR
 ↓
ESP32 ADC
 ↓
Collect 20 Samples
 ↓
Feature Extraction
 ↓
5 Features
 ↓
Normalize
 ↓
ANN 5–4–3
 ↓
Prediction
 ↓
Dark / Normal / Bright
```

---

## 5. Hardware & Software Requirements

### Hardware

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable
- Optional LEDs for class indication

### Software

- Arduino IDE or PlatformIO
- Python
- NumPy
- Pandas
- Matplotlib
- Scikit-learn for optional splitting and evaluation

> TensorFlow is not required.

---

## 6. Input Dataset

Use the LDR dataset collected in the previous lab.

Example:

```csv
timestamp,light_value,label
1000,425,dark
1100,440,dark
1200,438,dark
1300,1350,normal
1400,1450,normal
1500,1420,normal
1600,3100,bright
1700,3250,bright
1800,3180,bright
```

The original dataset contains:

```text
timestamp
light_value
label
```

The revised ANN will no longer use only one `light_value`.

Instead, several samples are converted into one **feature vector**.

---

## 7. Windowing

Use:

```text
Sampling interval = 100 ms
Sampling rate     ≈ 10 Hz
Window size       = 20 samples
```

Therefore:

```text
Window duration
= 20 / 10
= 2 seconds
```

A window is:

```text
[x1, x2, ..., x20]
```

Each window receives one class label:

```text
dark
normal
bright
```

---

## 8. LDR Feature Set

For each 20-sample window, calculate five features:

```text
1. Mean
2. Standard deviation
3. Minimum
4. Maximum
5. Range
```

The feature vector is:

$$
\mathbf{f}
= [
\mu,
\sigma,
x_{\min},
x_{\max},
R
]
$$

where:

\[
R=x_{\max}-x_{\min}.
\]

Therefore, each ANN example contains:

```text
5 features
```

instead of one raw ADC value.

---

## 9. Feature 1 — Mean

The mean describes the average light level:

\[
\mu
= \frac{1}{N}
\sum_{i=1}^{N}x_i.
\]

Example:

```text
Window:
400, 420, 430, 410, 440

Mean:
420
```

Mean is usually the most important feature for separating Dark, Normal, and Bright conditions.

---

## 10. Feature 2 — Standard Deviation

The standard deviation describes signal variability:

\[
\sigma
= \sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
(x_i-\mu)^2
}.
\]

A stable light source may produce:

```text
Small standard deviation
```

while a flickering or changing source may produce:

```text
Large standard deviation
```

---

## 11. Feature 3 — Minimum

\[
x_{\min}
= \min(x_1,x_2,\ldots,x_N)
\]

This indicates the lowest light reading in the window.

---

## 12. Feature 4 — Maximum

\[
x_{\max}
= \max(x_1,x_2,\ldots,x_N)
\]

This indicates the highest light reading in the window.

---

## 13. Feature 5 — Range

\[
R = x_{\max}
- x_{\min}
\]

Range gives a simple measure of variation.

Example:

```text
Minimum = 1300
Maximum = 1480

Range = 180
```

---

## 14. Feature Vector

One LDR window becomes:

```text
20 Raw Samples
      ↓
Feature Extraction
      ↓
[Mean, Std, Min, Max, Range]
```

Example:

```text
[1382.5, 46.2, 1305, 1460, 155]
```

The ANN therefore changes from:

```text
1–4–3
```

to:

```text
5–4–3
```

---

## 15. Feature Normalization

Because ESP32 ADC measurements are approximately:

```text
0 ... 4095
```

normalize each feature using 4095:

\[
f_n
= \frac{f}{4095}.
\]

For example:

```text
Mean  = 1400 -> 0.342
Std   = 100  -> 0.024
Min   = 1250 -> 0.305
Max   = 1550 -> 0.379
Range = 300  -> 0.073
```

The normalized feature vector becomes:

```text
[0.342, 0.024, 0.305, 0.379, 0.073]
```

The same normalization must be used in Python and on the ESP32.

---

## 16. Class Encoding

Use:

```text
0 -> dark
1 -> normal
2 -> bright
```

One-hot representation:

```text
dark   -> [1, 0, 0]
normal -> [0, 1, 0]
bright -> [0, 0, 1]
```

---

## 17. ANN Architecture

The revised ANN is:

```text
5 Input Features
      ↓
4 Hidden Neurons
      ↓
3 Output Neurons
      ↓
Dark / Normal / Bright
```

Input layer:

```text
x1 = Mean
x2 = Standard Deviation
x3 = Minimum
x4 = Maximum
x5 = Range
```

Architecture:

```text
 Mean ─────┐
 Std ──────┤
 Min ──────┤
 Max ──────┤──► Hidden Layer ───► Dark
 Range ────┘      4 neurons      ► Normal
                                  ► Bright
```

---

## 18. ANN Mathematics

For hidden neuron \(j\):

\[
z_j
= \sum_{i=1}^{5}
x_iW_{ij}
+
b_j.
\]

Sigmoid activation:

\[
h_j
= \frac{1}
{1+e^{-z_j}}.
\]

For output neuron \(k\):

\[
o_k
= \sum_{j=1}^{4}
h_jW_{jk}
+
b_k.
\]

The predicted class is:

\[
\hat{y}
= \arg\max_k(o_k).
\]

---

# 19. Experiment 1 — Extract Features from One Window

Python example:

```python
import numpy as np

window = np.array([
    1320, 1380, 1355, 1410, 1375,
    1390, 1365, 1420, 1340, 1400,
    1370, 1385, 1415, 1360, 1395,
    1335, 1405, 1375, 1380, 1390
], dtype=np.float32)

mean_value = np.mean(window)
std_value = np.std(window)
min_value = np.min(window)
max_value = np.max(window)
range_value = max_value - min_value

features = np.array([
    mean_value,
    std_value,
    min_value,
    max_value,
    range_value
])

print(features)
```

---

# 20. Experiment 2 — Create Feature Dataset

The following code converts the raw LDR dataset into feature windows.

```python
import numpy as np
import pandas as pd

WINDOW_SIZE = 20

df = pd.read_csv(
    "light_dataset.csv"
)

feature_rows = []

for label in [
    "dark",
    "normal",
    "bright"
]:

    values = df[
        df["label"] == label
    ]["light_value"].values.astype(
        np.float32
    )

    number_of_windows = (
        len(values) // WINDOW_SIZE
    )

    for w in range(
        number_of_windows
    ):

        start = (
            w * WINDOW_SIZE
        )

        end = (
            start + WINDOW_SIZE
        )

        window = values[
            start:end
        ]

        mean_value = np.mean(
            window
        )

        std_value = np.std(
            window
        )

        min_value = np.min(
            window
        )

        max_value = np.max(
            window
        )

        range_value = (
            max_value -
            min_value
        )

        feature_rows.append([
            mean_value,
            std_value,
            min_value,
            max_value,
            range_value,
            label
        ])

feature_df = pd.DataFrame(
    feature_rows,
    columns=[
        "mean",
        "std",
        "min",
        "max",
        "range",
        "label"
    ]
)

feature_df.to_csv(
    "ldr_feature_dataset.csv",
    index=False
)

print(
    feature_df.head()
)

print(
    feature_df["label"].value_counts()
)
```

> If the previous dataset contains a `recording_id`, it is preferable to form windows within each recording rather than concatenating unrelated recordings.

---

## 21. Feature Dataset Format

The output is:

```csv
mean,std,min,max,range,label
425.2,18.4,395,455,60,dark
438.9,21.0,400,472,72,dark
1390.4,52.2,1280,1490,210,normal
1455.7,48.5,1350,1540,190,normal
3150.3,70.1,3005,3290,285,bright
```

This dataset is now ready for ANN training.

---

# 22. Experiment 3 — Normalize Feature Dataset

```python
import numpy as np
import pandas as pd

df = pd.read_csv(
    "ldr_feature_dataset.csv"
)

feature_names = [
    "mean",
    "std",
    "min",
    "max",
    "range"
]

X = df[
    feature_names
].values.astype(
    np.float32
)

X = X / 4095.0

print(
    X[:5]
)
```

---

# 23. Complete Plain Python ANN Training Code — 5–4–3

```python
import numpy as np
import pandas as pd

# ==================================================
# Load Feature Dataset
# ==================================================

df = pd.read_csv(
    "ldr_feature_dataset.csv"
)

feature_names = [
    "mean",
    "std",
    "min",
    "max",
    "range"
]

X = df[
    feature_names
].values.astype(
    np.float32
)

# Normalize
X = X / 4095.0


# ==================================================
# Encode Classes
# ==================================================

label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}

y = df[
    "label"
].map(
    label_map
).values

Y = np.eye(
    3,
    dtype=np.float32
)[y]


# ==================================================
# Shuffle
# ==================================================

np.random.seed(
    42
)

indices = np.random.permutation(
    len(X)
)

X = X[
    indices
]

Y = Y[
    indices
]

y = y[
    indices
]


# ==================================================
# Train / Test Split
# ==================================================

split = int(
    0.8 * len(X)
)

X_train = X[
    :split
]

Y_train = Y[
    :split
]

y_train = y[
    :split
]

X_test = X[
    split:
]

Y_test = Y[
    split:
]

y_test = y[
    split:
]


# ==================================================
# ANN Architecture
# 5 -> 4 -> 3
# ==================================================

INPUT_SIZE = 5
HIDDEN_SIZE = 4
OUTPUT_SIZE = 3


# ==================================================
# Initialize Parameters
# ==================================================

W1 = (
    np.random.randn(
        INPUT_SIZE,
        HIDDEN_SIZE
    ).astype(
        np.float32
    ) * 0.5
)

b1 = np.zeros(
    (
        1,
        HIDDEN_SIZE
    ),
    dtype=np.float32
)

W2 = (
    np.random.randn(
        HIDDEN_SIZE,
        OUTPUT_SIZE
    ).astype(
        np.float32
    ) * 0.5
)

b2 = np.zeros(
    (
        1,
        OUTPUT_SIZE
    ),
    dtype=np.float32
)


# ==================================================
# Functions
# ==================================================

def sigmoid(z):

    z = np.clip(
        z,
        -20,
        20
    )

    return (
        1.0 /
        (
            1.0 +
            np.exp(-z)
        )
    )


def softmax(z):

    shifted = (
        z -
        np.max(
            z,
            axis=1,
            keepdims=True
        )
    )

    exp_z = np.exp(
        shifted
    )

    return (
        exp_z /
        np.sum(
            exp_z,
            axis=1,
            keepdims=True
        )
    )


# ==================================================
# Training
# ==================================================

LEARNING_RATE = 0.3
EPOCHS = 5000

for epoch in range(
    EPOCHS
):

    # Forward
    Z1 = (
        X_train @ W1
        +
        b1
    )

    A1 = sigmoid(
        Z1
    )

    Z2 = (
        A1 @ W2
        +
        b2
    )

    A2 = softmax(
        Z2
    )

    # Cross-entropy
    loss = -np.mean(
        np.sum(
            Y_train *
            np.log(
                A2 + 1e-8
            ),
            axis=1
        )
    )

    # Backpropagation
    dZ2 = (
        A2 -
        Y_train
    ) / len(
        X_train
    )

    dW2 = (
        A1.T @ dZ2
    )

    db2 = np.sum(
        dZ2,
        axis=0,
        keepdims=True
    )

    dA1 = (
        dZ2 @ W2.T
    )

    dZ1 = (
        dA1 *
        A1 *
        (
            1.0 -
            A1
        )
    )

    dW1 = (
        X_train.T @ dZ1
    )

    db1 = np.sum(
        dZ1,
        axis=0,
        keepdims=True
    )

    # Update
    W1 -= (
        LEARNING_RATE *
        dW1
    )

    b1 -= (
        LEARNING_RATE *
        db1
    )

    W2 -= (
        LEARNING_RATE *
        dW2
    )

    b2 -= (
        LEARNING_RATE *
        db2
    )

    if epoch % 500 == 0:

        pred_train = np.argmax(
            A2,
            axis=1
        )

        train_accuracy = np.mean(
            pred_train ==
            y_train
        )

        print(
            f"Epoch {epoch:4d}"
            f" | Loss = {loss:.6f}"
            f" | Accuracy = {train_accuracy:.4f}"
        )


# ==================================================
# Test
# ==================================================

H_test = sigmoid(
    X_test @ W1
    +
    b1
)

O_test = softmax(
    H_test @ W2
    +
    b2
)

predicted = np.argmax(
    O_test,
    axis=1
)

accuracy = np.mean(
    predicted ==
    y_test
)

print(
    "\nTest Accuracy:",
    accuracy
)


# ==================================================
# Export Parameters
# ==================================================

print(
    "\nW1 ="
)

print(
    W1
)

print(
    "\nb1 ="
)

print(
    b1
)

print(
    "\nW2 ="
)

print(
    W2
)

print(
    "\nb2 ="
)

print(
    b2
)
```

---

## 24. New Weight Dimensions

The revised model uses:

```text
5 Inputs
4 Hidden Neurons
3 Outputs
```

Therefore:

```text
W1 shape = 5 × 4
b1 shape = 4

W2 shape = 4 × 3
b2 shape = 3
```

Parameter count:

\[
5\times4 + 4 + 4\times3 + 3
= 39.
\]

Thus:

```text
Total ANN parameters = 39
```

For Float32:

```text
39 × 4 bytes
= 156 bytes
```

for model parameters.

---

# 25. Feature Extraction on ESP32

The ESP32 must calculate exactly the same features used during Python training.

Use:

```cpp
#define WINDOW_SIZE 20
#define NUM_FEATURES 5
```

Buffer:

```cpp
float ldrWindow[
  WINDOW_SIZE
];
```

---

## 26. Arduino Feature Extraction Function

```cpp
#include <math.h>

void extractFeatures(
  float samples[],
  int n,
  float features[]
)
{
  float sum =
    0.0f;

  float minValue =
    samples[0];

  float maxValue =
    samples[0];


  for (
    int i = 0;
    i < n;
    i++
  )
  {
    float x =
      samples[i];

    sum +=
      x;


    if (
      x < minValue
    )
    {
      minValue =
        x;
    }


    if (
      x > maxValue
    )
    {
      maxValue =
        x;
    }
  }


  float meanValue =
    sum / n;


  float variance =
    0.0f;


  for (
    int i = 0;
    i < n;
    i++
  )
  {
    float d =
      samples[i] -
      meanValue;

    variance +=
      d * d;
  }


  variance /=
    n;


  float stdValue =
    sqrtf(
      variance
    );


  float rangeValue =
    maxValue -
    minValue;


  features[0] =
    meanValue;

  features[1] =
    stdValue;

  features[2] =
    minValue;

  features[3] =
    maxValue;

  features[4] =
    rangeValue;
}
```

---

## 27. Normalize Features on ESP32

```cpp
void normalizeFeatures(
  float features[]
)
{
  for (
    int i = 0;
    i < 5;
    i++
  )
  {
    features[i] =
      features[i] /
      4095.0f;
  }
}
```

The feature ordering must be identical to Python:

```text
0 -> Mean
1 -> Standard deviation
2 -> Minimum
3 -> Maximum
4 -> Range
```

---

# 28. Plain Arduino ANN Forward Pass — 5–4–3

Replace the placeholder weights with the values learned in Python.

```cpp
#include <Arduino.h>
#include <math.h>

#define LDR_PIN 36

#define WINDOW_SIZE 20
#define INPUT_SIZE 5
#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 3

float W1[INPUT_SIZE][HIDDEN_SIZE] =
{
  {0.1, 0.2, 0.3, 0.4},
  {0.1, 0.2, 0.3, 0.4},
  {0.1, 0.2, 0.3, 0.4},
  {0.1, 0.2, 0.3, 0.4},
  {0.1, 0.2, 0.3, 0.4}
};

float b1[HIDDEN_SIZE] =
{
  0.0,
  0.0,
  0.0,
  0.0
};

float W2[HIDDEN_SIZE][OUTPUT_SIZE] =
{
  { 1.0,  0.0, -1.0},
  { 0.5,  0.5, -0.5},
  {-0.5,  1.0,  0.5},
  {-1.0,  0.0,  1.0}
};

float b2[OUTPUT_SIZE] =
{
  0.0,
  0.0,
  0.0
};

const char* classNames[OUTPUT_SIZE] =
{
  "Dark",
  "Normal",
  "Bright"
};


float sigmoid(
  float x
)
{
  return
    1.0f /
    (
      1.0f +
      expf(
        -x
      )
    );
}


void extractFeatures(
  float samples[],
  int n,
  float features[]
)
{
  float sum =
    0.0f;

  float minValue =
    samples[0];

  float maxValue =
    samples[0];


  for (
    int i = 0;
    i < n;
    i++
  )
  {
    float x =
      samples[i];

    sum +=
      x;


    if (
      x < minValue
    )
    {
      minValue =
        x;
    }


    if (
      x > maxValue
    )
    {
      maxValue =
        x;
    }
  }


  float meanValue =
    sum / n;

  float variance =
    0.0f;


  for (
    int i = 0;
    i < n;
    i++
  )
  {
    float d =
      samples[i] -
      meanValue;

    variance +=
      d * d;
  }


  variance /=
    n;


  features[0] =
    meanValue;

  features[1] =
    sqrtf(
      variance
    );

  features[2] =
    minValue;

  features[3] =
    maxValue;

  features[4] =
    maxValue -
    minValue;
}


int predictANN(
  float features[],
  float outputs[]
)
{
  float hidden[
    HIDDEN_SIZE
  ];


  // Hidden layer

  for (
    int j = 0;
    j < HIDDEN_SIZE;
    j++
  )
  {
    float z =
      b1[j];


    for (
      int i = 0;
      i < INPUT_SIZE;
      i++
    )
    {
      z +=
        features[i]
        *
        W1[i][j];
    }


    hidden[j] =
      sigmoid(
        z
      );
  }


  // Output layer

  for (
    int k = 0;
    k < OUTPUT_SIZE;
    k++
  )
  {
    float z =
      b2[k];


    for (
      int j = 0;
      j < HIDDEN_SIZE;
      j++
    )
    {
      z +=
        hidden[j]
        *
        W2[j][k];
    }


    outputs[k] =
      sigmoid(
        z
      );
  }


  // Argmax

  int bestClass =
    0;

  float bestScore =
    outputs[0];


  for (
    int k = 1;
    k < OUTPUT_SIZE;
    k++
  )
  {
    if (
      outputs[k] >
      bestScore
    )
    {
      bestScore =
        outputs[k];

      bestClass =
        k;
    }
  }


  return
    bestClass;
}


void setup()
{
  Serial.begin(
    115200
  );

  pinMode(
    LDR_PIN,
    INPUT
  );

  Serial.println(
    "LDR Feature ANN Classifier"
  );
}


void loop()
{
  float window[
    WINDOW_SIZE
  ];


  // --------------------------------
  // Collect 20 Samples
  // --------------------------------

  for (
    int i = 0;
    i < WINDOW_SIZE;
    i++
  )
  {
    window[i] =
      analogRead(
        LDR_PIN
      );

    delay(
      100
    );
  }


  // --------------------------------
  // Feature Extraction
  // --------------------------------

  float features[
    INPUT_SIZE
  ];


  extractFeatures(
    window,
    WINDOW_SIZE,
    features
  );


  // Display raw features

  Serial.println(
    "Raw Features"
  );

  Serial.print(
    "Mean: "
  );

  Serial.println(
    features[0]
  );

  Serial.print(
    "Std: "
  );

  Serial.println(
    features[1]
  );

  Serial.print(
    "Min: "
  );

  Serial.println(
    features[2]
  );

  Serial.print(
    "Max: "
  );

  Serial.println(
    features[3]
  );

  Serial.print(
    "Range: "
  );

  Serial.println(
    features[4]
  );


  // --------------------------------
  // Normalize
  // --------------------------------

  for (
    int i = 0;
    i < INPUT_SIZE;
    i++
  )
  {
    features[i] /=
      4095.0f;
  }


  // --------------------------------
  // ANN Inference
  // --------------------------------

  float outputs[
    OUTPUT_SIZE
  ];


  unsigned long startTime =
    micros();


  int predictedClass =
    predictANN(
      features,
      outputs
    );


  unsigned long latency =
    micros() -
    startTime;


  // --------------------------------
  // Result
  // --------------------------------

  Serial.print(
    "Prediction: "
  );

  Serial.println(
    classNames[
      predictedClass
    ]
  );


  Serial.print(
    "Dark: "
  );

  Serial.println(
    outputs[0],
    4
  );


  Serial.print(
    "Normal: "
  );

  Serial.println(
    outputs[1],
    4
  );


  Serial.print(
    "Bright: "
  );

  Serial.println(
    outputs[2],
    4
  );


  Serial.print(
    "Inference Time: "
  );

  Serial.print(
    latency
  );

  Serial.println(
    " us"
  );


  Serial.println(
    "--------------------"
  );
}
```

> The ANN parameters in this Arduino example are placeholders. Replace `W1`, `b1`, `W2`, and `b2` with the trained parameters from Python.

---

## 29. Expected ESP32 Output

Example:

```text
Raw Features
Mean: 1412.30
Std: 48.72
Min: 1325.00
Max: 1502.00
Range: 177.00

Prediction: Normal
Dark: 0.0412
Normal: 0.9315
Bright: 0.0627

Inference Time: 38 us
```

---

## 30. Feature Interpretation

Typical behavior may look like:

| Class | Mean | Std | Min | Max | Range |
|---|---:|---:|---:|---:|---:|
| Dark | Low | Low/Medium | Low | Low | Small/Medium |
| Normal | Medium | Low/Medium | Medium | Medium | Small/Medium |
| Bright | High | Low/Medium | High | High | Small/Medium |

The exact values depend on the LDR, resistor, circuit orientation, and illumination.

---

## 31. Experiment — Compare Raw Input vs Features

Compare:

### Model A — Raw Value

```text
1 Input
 ↓
4 Hidden
 ↓
3 Outputs
```

### Model B — Feature Based

```text
5 Features
 ↓
4 Hidden
 ↓
3 Outputs
```

Suggested results:

| Model | Input | Parameters | Test Accuracy | Robustness |
|---|---|---:|---:|---|
| Raw ANN | Single ADC | 23 |  |  |
| Feature ANN | Mean, Std, Min, Max, Range | 39 |  |  |

Discuss whether feature extraction improves classification near class boundaries.

---

## 32. Experiment — Feature Importance

Train models using different feature subsets.

### Case A

```text
Mean only
```

### Case B

```text
Mean + Std
```

### Case C

```text
Mean + Std + Min + Max + Range
```

Compare:

| Feature Set | Inputs | Accuracy |
|---|---:|---:|
| Mean | 1 |  |
| Mean + Std | 2 |  |
| Five Features | 5 |  |

This helps students understand whether every feature contributes useful information.

---

## 33. Experiment — Window Size

Compare:

```text
10 samples = 1 second
20 samples = 2 seconds
50 samples = 5 seconds
```

at approximately 10 Hz.

| Window Size | Duration | Accuracy | Response Time |
|---:|---:|---:|---:|
| 10 | 1 s |  | Fast |
| 20 | 2 s |  | Medium |
| 50 | 5 s |  | Slow |

A larger window may provide more stable features but slower response.

---

## 34. Feature Extraction vs Raw Data

### Raw Input

```text
LDR Value
    ↓
ANN
```

Advantages:

- very simple;
- low latency;
- smallest network.

Limitations:

- sensitive to individual noisy readings.

### Feature-Based Input

```text
LDR Window
    ↓
Statistics
    ↓
ANN
```

Advantages:

- more stable;
- describes signal variability;
- less sensitive to one abnormal sample.

Limitations:

- requires buffering;
- adds response delay;
- requires more calculations.

---

## 35. Experiments

### Experiment 1 — Window Collection

Collect:

```text
20 LDR samples
```

at:

```text
100 ms/sample
```

### Experiment 2 — Feature Extraction

Calculate:

```text
Mean
Standard Deviation
Minimum
Maximum
Range
```

### Experiment 3 — Feature Dataset

Create:

```text
mean,std,min,max,range,label
```

### Experiment 4 — Normalize Features

Convert all feature values approximately into:

```text
0.0 ... 1.0
```

### Experiment 5 — Train ANN

Train:

```text
5 -> 4 -> 3
```

### Experiment 6 — Test Accuracy

Evaluate held-out feature windows.

### Experiment 7 — ESP32 Deployment

Implement the same feature extraction on ESP32.

### Experiment 8 — Live Classification

Classify:

```text
Dark
Normal
Bright
```

### Experiment 9 — Compare Raw and Feature ANN

Measure accuracy and robustness.

### Experiment 10 — Window-Size Study

Compare 10, 20, and 50 samples.

---

## 36. Exercises

### Exercise 1 — LDR Feature Logger

Generate:

```csv
mean,std,min,max,range,label
```

### Exercise 2 — Feature Visualization

Plot the mean for all three classes.

Then plot:

```text
Mean vs Standard Deviation
```

### Exercise 3 — Feature ANN

Train a:

```text
5–4–3 ANN
```

and report test accuracy.

### Exercise 4 — Feature Selection

Compare:

```text
Mean only
Mean + Std
All 5 features
```

### Exercise 5 — Embedded Inference

Deploy the trained ANN on ESP32.

### Exercise 6 — Robustness

Introduce temporary shadows or light fluctuations and compare:

```text
Raw ANN
Feature ANN
```

---

## 37. Checkpoint Questions

1. What is feature extraction?
2. Why can feature extraction improve LDR classification?
3. What five LDR features are used in this lab?
4. What does the mean describe?
5. What does standard deviation describe?
6. Why is a fixed window required?
7. Why must Python and ESP32 use the same feature order and normalization?
8. Why does the ANN architecture change from 1–4–3 to 5–4–3?
9. What is the trade-off between a larger window and response time?
10. How can feature-based ANN classification be compared fairly with threshold classification?

---

## 38. Expected Results

Students should transform:

```text
20 LDR ADC Samples
```

into:

```text
5 Statistical Features
```

and use:

```text
5 Inputs
   ↓
4 Hidden Neurons
   ↓
3 Outputs
```

to classify:

```text
Dark
Normal
Bright
```

A typical processing pipeline is:

```text
LDR
 ↓
20 Samples
 ↓
Mean / Std / Min / Max / Range
 ↓
Normalize
 ↓
ANN
 ↓
Class
```

---

## 39. Submission

Submit:

- previous LDR raw CSV dataset;
- generated feature dataset;
- Python feature-extraction code;
- plain Python ANN training code;
- training loss;
- test accuracy;
- confusion matrix;
- trained `W1`, `b1`, `W2`, `b2`;
- Arduino feature-extraction and ANN code;
- real-time classification output;
- inference latency;
- comparison of raw ANN and feature ANN;
- short discussion.

Suggested performance table:

| Metric | Raw ANN | Feature ANN |
|---|---:|---:|
| Input variables | 1 | 5 |
| Hidden neurons | 4 | 4 |
| Output classes | 3 | 3 |
| Parameters | 23 | 39 |
| Test accuracy |  |  |
| Inference latency |  |  |
| Window delay | None | 2 s |

---

## 40. Conclusion

This revised lab extends LDR-based ANN classification by adding a **feature-extraction stage**.

Instead of making a decision from one ADC measurement, the system analyzes a short window:

```text
LDR Window
    ↓
Mean
Std
Min
Max
Range
    ↓
Feature Vector
    ↓
ANN 5–4–3
    ↓
Dark / Normal / Bright
```

The approach demonstrates an important embedded-machine-learning concept: raw sensor measurements can often be transformed into compact, meaningful features before classification.

For this simple LDR problem, mean illumination may already separate the three classes well, but standard deviation, minimum, maximum, and range provide additional information about stability and short-term variation. The experiment therefore allows students to compare the benefits and costs of **raw-value inference** versus **feature-based inference** on a microcontroller.
