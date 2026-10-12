# LAB 14 — ANN Light Classification with Plain Code
## Dark, Normal, and Bright Using ESP32 + LDR

## 1. Objective

The objective of this lab is to:

1. Use the **LDR dataset collected in the previous lab**.
2. Prepare LDR measurements for artificial neural network (ANN) training.
3. Normalize the ESP32 ADC value.
4. Encode three light classes:
   - **Dark**
   - **Normal**
   - **Bright**
5. Build a small ANN using **plain Python/NumPy code** without TensorFlow.
6. Train the ANN using the LDR dataset.
7. Evaluate the classifier using training and test data.
8. Export the trained ANN weights and biases.
9. Implement ANN inference using **plain Arduino C++ code** on the ESP32.
10. Perform real-time light classification from the LDR.
11. Compare ANN classification with the threshold-based method from the previous lab.

---

## 2. Background

In the previous lab, the ESP32 measured an LDR through the ADC and generated a labeled dataset:

```text
timestamp,light_value,label
```

with three classes:

```text
Dark
Normal
Bright
```

The next step is to replace the manually selected thresholds with a small **Artificial Neural Network (ANN)**.

The workflow is:

```text
LDR Dataset
    ↓
Data Preparation
    ↓
Normalization
    ↓
ANN Training
    ↓
Weights + Biases
    ↓
ESP32
    ↓
Real-Time LDR Measurement
    ↓
ANN Inference
    ↓
Dark / Normal / Bright
```

This lab intentionally uses **plain code** so students can see how a neural network works internally.

---

## 3. System Architecture

### Training Phase

```text
LDR CSV Dataset
      ↓
Read ADC Values
      ↓
Normalize
      ↓
ANN Training
      ↓
Learn Weights and Biases
      ↓
Export Parameters
```

### Deployment Phase

```text
LDR
 ↓
ESP32 ADC
 ↓
Normalize
 ↓
ANN Forward Pass
 ↓
Output Scores
 ↓
Predicted Class
 ↓
Dark / Normal / Bright
```

---

## 4. Hardware & Software Requirements

### Hardware

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable

### Software

- Arduino IDE or PlatformIO
- Python
- NumPy
- Pandas
- Matplotlib
- Scikit-learn only for optional dataset splitting/evaluation

> TensorFlow is **not required** in this lab.

---

## 5. Input Dataset

Use the LDR dataset from the previous lab.

Example:

```csv
timestamp,light_value,label
1000,425,dark
1100,440,dark
1200,1350,normal
1300,1450,normal
1400,3100,bright
1500,3250,bright
```

The ANN input is:

```text
light_value
```

and the target output is one of:

```text
dark
normal
bright
```

---

## 6. Class Encoding

Use:

```text
0 -> dark
1 -> normal
2 -> bright
```

One-hot encoding becomes:

```text
dark   -> [1, 0, 0]
normal -> [0, 1, 0]
bright -> [0, 0, 1]
```

---

## 7. Input Normalization

The ESP32 ADC is approximately:

```text
0 to 4095
```

Normalize the input using:

\[
x_n = \frac{x}{4095}
\]

Therefore:

```text
ADC = 0    -> 0.0
ADC = 2048 -> ~0.50
ADC = 4095 -> 1.0
```

Python:

```python
x_norm = light_value / 4095.0
```

Arduino:

```cpp
float x =
  lightValue / 4095.0f;
```

The same normalization must be used during training and deployment.

---

## 8. ANN Architecture

Use a very small network:

```text
1 Input
  ↓
4 Hidden Neurons
  ↓
3 Output Neurons
  ↓
Dark / Normal / Bright
```

Architecture:

```text
LDR ADC
  |
  v
Input x
  |
  +---- Hidden 1
  +---- Hidden 2
  +---- Hidden 3
  +---- Hidden 4
           |
           v
   +-------+-------+
   |       |       |
  Dark   Normal  Bright
```

The network contains:

```text
Input neurons  = 1
Hidden neurons = 4
Output neurons = 3
```

---

## 9. ANN Mathematics

For each hidden neuron:

\[
z_j = xw_j+b_j
\]

Use the sigmoid activation:

\[
\sigma(z)
= \frac{1}{1+e^{-z}}
\]

Thus:

\[
h_j=\sigma(z_j)
\]

For the output layer:

\[
o_k
=
\sum_j h_jW_{jk}+b_k
\]

The predicted class is the output neuron with the largest score:

\[
\hat{y}
= \arg\max_k(o_k)
\]

For this simple lab, the output layer uses raw scores rather than a full softmax during ESP32 inference.

---

# 10. Experiment 1 — Load the Dataset

```python
import pandas as pd

df = pd.read_csv(
    "light_dataset.csv"
)

print(
    df.head()
)

print(
    df["label"].value_counts()
)
```

Expected structure:

```text
dark      500
normal    500
bright    500
```

The exact values depend on the previous lab.

---

# 11. Experiment 2 — Prepare Input and Labels

```python
import numpy as np

X = df[
    "light_value"
].values.astype(
    np.float32
)

X = X / 4095.0

X = X.reshape(
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
).values
```

---

## 12. One-Hot Encoding

```python
Y = np.zeros(
    (
        len(y),
        3
    ),
    dtype=np.float32
)

for i in range(
    len(y)
):
    Y[
        i,
        y[i]
    ] = 1.0
```

Example:

```text
dark   -> [1, 0, 0]
normal -> [0, 1, 0]
bright -> [0, 0, 1]
```

---

## 13. Training and Test Split

A simple split is:

```text
Training = 80%
Testing  = 20%
```

Example:

```python
from sklearn.model_selection import train_test_split

X_train, X_test, Y_train, Y_test = train_test_split(
    X,
    Y,
    test_size=0.20,
    random_state=42,
    stratify=y
)
```

---

# 14. Plain ANN Training Code

The following implementation uses only **NumPy**.

```python
import numpy as np
import pandas as pd

# --------------------------------
# Load Dataset
# --------------------------------

df = pd.read_csv(
    "light_dataset.csv"
)

X = df[
    "light_value"
].values.astype(
    np.float32
)

X = X / 4095.0

X = X.reshape(
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
).values


Y = np.zeros(
    (
        len(y),
        3
    ),
    dtype=np.float32
)

for i in range(
    len(y)
):
    Y[
        i,
        y[i]
    ] = 1.0


# --------------------------------
# Shuffle Dataset
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

Y = Y[
    indices
]


# --------------------------------
# Train / Test Split
# --------------------------------

split =
    int(
        0.8 * len(X)
    )

X_train = X[
    :split
]

Y_train = Y[
    :split
]

X_test = X[
    split:
]

Y_test = Y[
    split:
]


# --------------------------------
# ANN Dimensions
# --------------------------------

INPUT_SIZE = 1
HIDDEN_SIZE = 4
OUTPUT_SIZE = 3


# --------------------------------
# Initialize Weights
# --------------------------------

W1 = np.random.randn(
    INPUT_SIZE,
    HIDDEN_SIZE
).astype(
    np.float32
) * 0.5

b1 = np.zeros(
    (
        1,
        HIDDEN_SIZE
    ),
    dtype=np.float32
)

W2 = np.random.randn(
    HIDDEN_SIZE,
    OUTPUT_SIZE
).astype(
    np.float32
) * 0.5

b2 = np.zeros(
    (
        1,
        OUTPUT_SIZE
    ),
    dtype=np.float32
)


# --------------------------------
# Activation Functions
# --------------------------------

def sigmoid(z):

    return 1.0 / (
        1.0 +
        np.exp(
            -z
        )
    )


def sigmoid_derivative(a):

    return a * (
        1.0 - a
    )


# --------------------------------
# Training Parameters
# --------------------------------

learning_rate =
    0.5

epochs =
    5000


# --------------------------------
# Training Loop
# --------------------------------

for epoch in range(
    epochs
):

    # Forward Pass

    Z1 = (
        X_train @ W1
    ) + b1

    A1 = sigmoid(
        Z1
    )

    Z2 = (
        A1 @ W2
    ) + b2

    # Sigmoid outputs
    A2 = sigmoid(
        Z2
    )


    # --------------------------------
    # Mean Squared Error
    # --------------------------------

    error =
        A2 - Y_train

    loss =
        np.mean(
            error ** 2
        )


    # --------------------------------
    # Backpropagation
    # --------------------------------

    dA2 =
        2.0 * error / len(
            X_train
        )

    dZ2 =
        dA2 * sigmoid_derivative(
            A2
        )

    dW2 =
        A1.T @ dZ2

    db2 =
        np.sum(
            dZ2,
            axis=0,
            keepdims=True
        )

    dA1 =
        dZ2 @ W2.T

    dZ1 =
        dA1 * sigmoid_derivative(
            A1
        )

    dW1 =
        X_train.T @ dZ1

    db1 =
        np.sum(
            dZ1,
            axis=0,
            keepdims=True
        )


    # --------------------------------
    # Gradient Descent
    # --------------------------------

    W1 -= (
        learning_rate * dW1
    )

    b1 -= (
        learning_rate * db1
    )

    W2 -= (
        learning_rate * dW2
    )

    b2 -= (
        learning_rate * db2
    )


    if epoch % 500 == 0:

        print(
            "Epoch:",
            epoch,
            "Loss:",
            loss
        )


# --------------------------------
# Test Prediction
# --------------------------------

H_test = sigmoid(
    X_test @ W1 + b1
)

O_test = sigmoid(
    H_test @ W2 + b2
)

predicted = np.argmax(
    O_test,
    axis=1
)

actual = np.argmax(
    Y_test,
    axis=1
)

accuracy = np.mean(
    predicted == actual
)

print(
    "Test accuracy:",
    accuracy
)


# --------------------------------
# Print Parameters
# --------------------------------

print(
    "W1 =",
    W1
)

print(
    "b1 =",
    b1
)

print(
    "W2 =",
    W2
)

print(
    "b2 =",
    b2
)
```

> Python does not allow a line break immediately after `=` in ordinary assignments. If copying the code, write assignments such as `split = int(0.8 * len(X))`, `learning_rate = 0.5`, and `epochs = 5000` on one line. The expanded formatting above is for explanation.

---

# 15. Clean Executable Python Version

Use this version directly:

```python
import numpy as np
import pandas as pd

df = pd.read_csv("light_dataset.csv")

X = df["light_value"].values.astype(np.float32)
X = (X / 4095.0).reshape(-1, 1)

label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}

y = df["label"].map(label_map).values

Y = np.zeros((len(y), 3), dtype=np.float32)

for i in range(len(y)):
    Y[i, y[i]] = 1.0

np.random.seed(42)

indices = np.arange(len(X))
np.random.shuffle(indices)

X = X[indices]
Y = Y[indices]

split = int(0.8 * len(X))

X_train = X[:split]
Y_train = Y[:split]

X_test = X[split:]
Y_test = Y[split:]

INPUT_SIZE = 1
HIDDEN_SIZE = 4
OUTPUT_SIZE = 3

W1 = (
    np.random.randn(
        INPUT_SIZE,
        HIDDEN_SIZE
    ).astype(np.float32) * 0.5
)

b1 = np.zeros(
    (1, HIDDEN_SIZE),
    dtype=np.float32
)

W2 = (
    np.random.randn(
        HIDDEN_SIZE,
        OUTPUT_SIZE
    ).astype(np.float32) * 0.5
)

b2 = np.zeros(
    (1, OUTPUT_SIZE),
    dtype=np.float32
)

def sigmoid(z):

    return 1.0 / (
        1.0 + np.exp(-z)
    )

def sigmoid_derivative(a):

    return a * (
        1.0 - a
    )

learning_rate = 0.5
epochs = 5000

for epoch in range(epochs):

    # Forward propagation
    Z1 = X_train @ W1 + b1
    A1 = sigmoid(Z1)

    Z2 = A1 @ W2 + b2
    A2 = sigmoid(Z2)

    # Loss
    error = A2 - Y_train
    loss = np.mean(error ** 2)

    # Backpropagation
    dA2 = 2.0 * error / len(X_train)

    dZ2 = (
        dA2 *
        sigmoid_derivative(A2)
    )

    dW2 = A1.T @ dZ2

    db2 = np.sum(
        dZ2,
        axis=0,
        keepdims=True
    )

    dA1 = dZ2 @ W2.T

    dZ1 = (
        dA1 *
        sigmoid_derivative(A1)
    )

    dW1 = X_train.T @ dZ1

    db1 = np.sum(
        dZ1,
        axis=0,
        keepdims=True
    )

    # Gradient descent
    W1 -= learning_rate * dW1
    b1 -= learning_rate * db1

    W2 -= learning_rate * dW2
    b2 -= learning_rate * db2

    if epoch % 500 == 0:

        print(
            f"Epoch {epoch}, "
            f"Loss = {loss:.6f}"
        )

# Test
H_test = sigmoid(
    X_test @ W1 + b1
)

O_test = sigmoid(
    H_test @ W2 + b2
)

predicted = np.argmax(
    O_test,
    axis=1
)

actual = np.argmax(
    Y_test,
    axis=1
)

accuracy = np.mean(
    predicted == actual
)

print(
    "Test accuracy:",
    accuracy
)

print("\nW1 =")
print(W1)

print("\nb1 =")
print(b1)

print("\nW2 =")
print(W2)

print("\nb2 =")
print(b2)
```

---

# 16. ANN Forward Propagation

For one normalized LDR value:

```text
x
 ↓
W1 + b1
 ↓
Sigmoid
 ↓
4 Hidden Values
 ↓
W2 + b2
 ↓
Sigmoid
 ↓
3 Output Scores
 ↓
Maximum Score
 ↓
Predicted Class
```

---

## 17. Export the Trained Parameters

After training, Python prints:

```text
W1
b1
W2
b2
```

For example:

```text
W1 =
[[ ... ... ... ... ]]

b1 =
[[ ... ... ... ... ]]

W2 =
[[ ... ... ... ]
 [ ... ... ... ]
 [ ... ... ... ]
 [ ... ... ... ]]

b2 =
[[ ... ... ... ]]
```

Copy the actual trained values into the ESP32 program.

---

# 18. Plain Arduino ANN Inference Code

The ESP32 does **not** require TensorFlow Lite for this small network.

```cpp
#include <Arduino.h>
#include <math.h>

#define LDR_PIN 36

#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 3


// --------------------------------
// Replace these values with
// parameters learned in Python
// --------------------------------

float W1[HIDDEN_SIZE] =
{
  1.0,
  2.0,
  3.0,
  4.0
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


// ========================================
// Sigmoid
// ========================================

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


// ========================================
// ANN Prediction
// ========================================

int predictANN(
  int adcValue
)
{
  float x =
    adcValue / 4095.0f;


  float hidden[
    HIDDEN_SIZE
  ];


  // --------------------------------
  // Hidden Layer
  // --------------------------------

  for (
    int j = 0;
    j < HIDDEN_SIZE;
    j++
  )
  {
    float z =
      x * W1[j]
      +
      b1[j];


    hidden[j] =
      sigmoid(
        z
      );
  }


  // --------------------------------
  // Output Layer
  // --------------------------------

  float output[
    OUTPUT_SIZE
  ];


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


    output[k] =
      sigmoid(
        z
      );
  }


  // --------------------------------
  // Argmax
  // --------------------------------

  int bestClass =
    0;


  float bestScore =
    output[0];


  for (
    int k = 1;
    k < OUTPUT_SIZE;
    k++
  )
  {
    if (
      output[k]
      >
      bestScore
    )
    {
      bestScore =
        output[k];

      bestClass =
        k;
    }
  }


  return
    bestClass;
}


// ========================================
// Class Name
// ========================================

const char* className(
  int classID
)
{
  if (
    classID == 0
  )
  {
    return "dark";
  }


  if (
    classID == 1
  )
  {
    return "normal";
  }


  return "bright";
}


// ========================================
// Setup
// ========================================

void setup()
{
  Serial.begin(
    115200
  );

  pinMode(
    LDR_PIN,
    INPUT
  );
}


// ========================================
// Loop
// ========================================

void loop()
{
  int lightValue =
    analogRead(
      LDR_PIN
    );


  int predictedClass =
    predictANN(
      lightValue
    );


  Serial.print(
    "ADC: "
  );

  Serial.print(
    lightValue
  );

  Serial.print(
    " -> "
  );

  Serial.println(
    className(
      predictedClass
    )
  );


  delay(
    500
  );
}
```

> The weights shown above are placeholders. Replace them with the values obtained from training.

---

# 19. Mapping Python Weights to Arduino

Python:

```text
W1 shape = (1,4)
```

Arduino:

```cpp
float W1[4]
```

Python:

```text
b1 shape = (1,4)
```

Arduino:

```cpp
float b1[4]
```

Python:

```text
W2 shape = (4,3)
```

Arduino:

```cpp
float W2[4][3]
```

Python:

```text
b2 shape = (1,3)
```

Arduino:

```cpp
float b2[3]
```

---

## 20. Experiment 3 — Test ANN with Known LDR Values

Try:

```text
Dark sample
Normal sample
Bright sample
```

Record:

| Test | ADC Value | Actual Class | ANN Prediction | Correct |
|---:|---:|---|---|---|
| 1 |  | Dark |  |  |
| 2 |  | Normal |  |  |
| 3 |  | Bright |  |  |

---

## 21. Classification Accuracy

Accuracy is:

\[
\text{Accuracy}
= \frac{
N_{\text{correct}}
}{
N_{\text{total}}
}
\times100\%
\]

For example:

```text
Correct = 93
Total   = 100
```

then:

```text
Accuracy = 93%
```

---

## 22. Experiment 4 — Confusion Matrix

For three classes:

| Actual \ Predicted | Dark | Normal | Bright |
|---|---:|---:|---:|
| Dark |  |  |  |
| Normal |  |  |  |
| Bright |  |  |  |

The confusion matrix identifies which light classes overlap.

---

## 23. Experiment 5 — ANN vs Threshold Classification

Compare the previous threshold classifier with the ANN.

| Method | Dark | Normal | Bright | Overall Accuracy |
|---|---:|---:|---:|---:|
| Threshold |  |  |  |  |
| ANN |  |  |  |  |

Threshold method:

```text
ADC
 ↓
T1 / T2
 ↓
Class
```

ANN method:

```text
ADC
 ↓
Normalization
 ↓
Hidden Layer
 ↓
Output Layer
 ↓
Class
```

---

## 24. Why Use ANN for a Simple LDR?

For only one sensor and three clearly separated classes, a threshold method may be simpler.

However, this ANN lab is useful because it introduces:

```text
Training
Weights
Biases
Activation Functions
Forward Propagation
Backpropagation
Classification
Embedded Inference
```

The same method can later be extended to multiple inputs.

---

## 25. Extend to Multiple Inputs

A future system may use:

```text
LDR
Temperature
Humidity
Motion
```

Then the ANN input becomes:

```text
[x1, x2, x3, x4]
```

instead of only one light value.

---

## 26. Real-Time System

```text
Ambient Light
      ↓
     LDR
      ↓
ESP32 ADC
      ↓
Normalize
      ↓
Plain ANN
      ↓
Prediction
      ↓
Dark / Normal / Bright
```

---

## 27. Optional LED Output

Use three LEDs:

```text
Dark   -> LED 1
Normal -> LED 2
Bright -> LED 3
```

Example:

```cpp
#define LED_DARK   16
#define LED_NORMAL 17
#define LED_BRIGHT 18
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

## 28. Experiment 6 — Classification with LED Indicators

Architecture:

```text
LDR
 ↓
ESP32
 ↓
ANN
 ↓
Predicted Class
 ↓
+--------+--------+
|        |        |
Dark   Normal   Bright
LED1    LED2     LED3
```

---

## 29. Inference Latency

Measure ANN execution time:

```cpp
unsigned long startTime =
  micros();

int predictedClass =
  predictANN(
    lightValue
  );

unsigned long endTime =
  micros();

unsigned long latency =
  endTime - startTime;
```

Display:

```cpp
Serial.print(
  "Inference time: "
);

Serial.print(
  latency
);

Serial.println(
  " us"
);
```

---

## 30. Memory Requirement

This small ANN stores only:

```text
W1 : 4 values
b1 : 4 values
W2 : 12 values
b2 : 3 values
```

Total:

```text
23 floating-point parameters
```

For 32-bit float:

```text
23 × 4 bytes = 92 bytes
```

excluding temporary variables and program code.

This is much smaller than a general-purpose neural-network runtime.

---

## 31. Experiment 7 — Change Hidden-Layer Size

Compare:

```text
Model A
1 -> 2 -> 3

Model B
1 -> 4 -> 3

Model C
1 -> 8 -> 3
```

Record:

| Model | Hidden Neurons | Parameters | Accuracy |
|---|---:|---:|---:|
| A | 2 |  |  |
| B | 4 |  |  |
| C | 8 |  |  |

Discuss whether a larger network is necessary for this simple problem.

---

## 32. Experiments

### Experiment 1 — Dataset Loading

Load the previous LDR CSV dataset.

### Experiment 2 — Normalize Input

Convert:

```text
0...4095
```

to:

```text
0.0...1.0
```

### Experiment 3 — ANN Training

Train:

```text
1 -> 4 -> 3
```

using plain NumPy.

### Experiment 4 — Test Accuracy

Evaluate the held-out test samples.

### Experiment 5 — Export Parameters

Record:

```text
W1
b1
W2
b2
```

### Experiment 6 — ESP32 Deployment

Insert parameters into the Arduino program.

### Experiment 7 — Live Classification

Classify:

```text
Dark
Normal
Bright
```

in real time.

### Experiment 8 — Compare ANN and Thresholds

Compare accuracy and implementation complexity.

### Experiment 9 — Inference Latency

Measure execution time in microseconds.

### Experiment 10 — Hidden-Layer Study

Compare 2, 4, and 8 hidden neurons.

---

## 33. Exercises

### Exercise 1 — Train the ANN

Train the three-class classifier and report:

- training loss
- test accuracy
- final weights
- final biases

### Exercise 2 — Deploy on ESP32

Implement:

```text
LDR
 ↓
ADC
 ↓
Normalization
 ↓
ANN
 ↓
Class
```

### Exercise 3 — LED Output

Use the ANN prediction to activate one of three LEDs.

### Exercise 4 — Compare with Thresholds

Measure:

```text
Threshold Accuracy
ANN Accuracy
```

### Exercise 5 — Evaluate Unknown Conditions

Test the sensor close to the class boundaries and identify which cases are difficult to classify.

### Exercise 6 — Change Network Size

Compare 2, 4, and 8 hidden neurons.

---

## 34. Checkpoint Questions

1. What is an artificial neural network?
2. Why is the LDR ADC value normalized before ANN training?
3. What are the three output classes in this lab?
4. What is the purpose of weights and biases?
5. What does the sigmoid activation function do?
6. What is forward propagation?
7. What is backpropagation?
8. How is the predicted class selected from the three ANN outputs?
9. Why must the ESP32 use the same normalization as the training program?
10. What is the main difference between threshold-based and ANN-based light classification?

---

## 35. Expected Results

Students should train a small ANN:

```text
1 Input
   ↓
4 Hidden Neurons
   ↓
3 Outputs
```

and classify LDR measurements as:

```text
Dark
Normal
Bright
```

The ESP32 should produce output similar to:

```text
ADC: 420  -> dark
ADC: 1450 -> normal
ADC: 3150 -> bright
```

The ANN should execute directly on the ESP32 without TensorFlow Lite.

---

## 36. Submission

Submit:

- LDR CSV dataset
- plain Python ANN training code
- training-loss results
- test accuracy
- confusion matrix
- trained weights and biases
- Arduino ANN source code
- real-time classification output
- ANN inference latency
- ANN vs threshold comparison
- short discussion

Suggested result table:

| Metric | Result |
|---|---:|
| Input features | 1 |
| Hidden neurons | 4 |
| Output classes | 3 |
| ANN parameters | 23 |
| Test accuracy |  |
| Inference latency |  |

Suggested classification table:

| Test | ADC | Actual | Predicted | Correct |
|---:|---:|---|---|---|
| 1 |  | Dark |  |  |
| 2 |  | Normal |  |  |
| 3 |  | Bright |  |  |
| 4 |  | Dark |  |  |
| 5 |  | Bright |  |  |

---

## 37. Conclusion

In this lab, the LDR dataset from the previous lab is transformed into a simple **ANN-based light-classification system**.

The model is deliberately implemented using plain mathematics and plain code:

```text
LDR Measurement
      ↓
Normalization
      ↓
Weights + Biases
      ↓
Sigmoid Activation
      ↓
ANN Output
      ↓
Dark / Normal / Bright
```

Unlike a TensorFlow Lite workflow, this small ANN requires no machine-learning runtime on the ESP32. The trained weights and biases are copied directly into the Arduino program, and inference is performed using ordinary arithmetic operations.

This provides a clear introduction to neural-network training, forward propagation, backpropagation, and embedded inference before moving to more complex TinyML frameworks.
