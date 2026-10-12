import numpy as np

# ============================================================
# 1. Reproducibility
# ============================================================

np.random.seed(42)


# ============================================================
# 2. Create Mock-Up LDR Dataset
# ============================================================
#
# ESP32 ADC:
# 0 ... 4095
#
# Class 0 = Dark
# Class 1 = Normal
# Class 2 = Bright
#
# Example distributions:
#
# Dark   -> around 500
# Normal -> around 1700
# Bright -> around 3300
#

SAMPLES_PER_CLASS = 600


dark_raw = np.random.normal(
    loc=500,
    scale=180,
    size=SAMPLES_PER_CLASS
)


normal_raw = np.random.normal(
    loc=1700,
    scale=300,
    size=SAMPLES_PER_CLASS
)


bright_raw = np.random.normal(
    loc=3300,
    scale=250,
    size=SAMPLES_PER_CLASS
)


# Limit to ESP32 12-bit ADC range

dark_raw = np.clip(
    dark_raw,
    0,
    4095
)

normal_raw = np.clip(
    normal_raw,
    0,
    4095
)

bright_raw = np.clip(
    bright_raw,
    0,
    4095
)


# Combine data

X_raw = np.concatenate(
    [
        dark_raw,
        normal_raw,
        bright_raw
    ]
).astype(
    np.float32
)


# Labels

y = np.concatenate(
    [
        np.zeros(
            SAMPLES_PER_CLASS,
            dtype=np.int64
        ),

        np.ones(
            SAMPLES_PER_CLASS,
            dtype=np.int64
        ),

        np.full(
            SAMPLES_PER_CLASS,
            2,
            dtype=np.int64
        )
    ]
)


# ============================================================
# 3. Shuffle Dataset
# ============================================================

indices = np.random.permutation(
    len(y)
)

X_raw = X_raw[
    indices
]

y = y[
    indices
]


# ============================================================
# 4. Normalize LDR Data
# ============================================================
#
# ADC:
#
# 0 ... 4095
#
# becomes:
#
# 0.0 ... 1.0
#

X = (
    X_raw / 4095.0
).reshape(
    -1,
    1
).astype(
    np.float32
)


# ============================================================
# 5. One-Hot Encode Labels
# ============================================================
#
# Dark:
# [1, 0, 0]
#
# Normal:
# [0, 1, 0]
#
# Bright:
# [0, 0, 1]
#

NUM_CLASSES = 3

Y = np.eye(
    NUM_CLASSES,
    dtype=np.float32
)[
    y
]


# ============================================================
# 6. Train / Test Split
# ============================================================

split_index = int(
    0.8 * len(X)
)


X_train = X[
    :split_index
]

Y_train = Y[
    :split_index
]

y_train = y[
    :split_index
]


X_test = X[
    split_index:
]

Y_test = Y[
    split_index:
]

y_test = y[
    split_index:
]


print(
    "Training samples:",
    len(X_train)
)

print(
    "Testing samples:",
    len(X_test)
)


# ============================================================
# 7. ANN Architecture
# ============================================================
#
# 1 Input
#    ↓
# 4 Hidden Neurons
#    ↓
# 3 Output Neurons
#
# Dark / Normal / Bright
#

INPUT_SIZE = 1

HIDDEN_SIZE = 4

OUTPUT_SIZE = 3


# ============================================================
# 8. Initialize Float32 Weights
# ============================================================

W1 = (
    np.random.randn(
        INPUT_SIZE,
        HIDDEN_SIZE
    )
    * 0.5
).astype(
    np.float32
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
    )
    * 0.5
).astype(
    np.float32
)


b2 = np.zeros(
    (
        1,
        OUTPUT_SIZE
    ),
    dtype=np.float32
)


# ============================================================
# 9. Activation Functions
# ============================================================

def sigmoid(
    z
):

    z = np.clip(
        z,
        -20,
        20
    )

    return (
        1.0
        /
        (
            1.0
            +
            np.exp(
                -z
            )
        )
    )


def softmax(
    z
):

    z = (
        z
        -
        np.max(
            z,
            axis=1,
            keepdims=True
        )
    )


    exp_z = np.exp(
        z
    )


    return (
        exp_z
        /
        np.sum(
            exp_z,
            axis=1,
            keepdims=True
        )
    )


# ============================================================
# 10. Float32 Training
# ============================================================

LEARNING_RATE = 0.5

EPOCHS = 8000


print(
    "\nTraining Float32 ANN..."
)


for epoch in range(
    EPOCHS
):

    # --------------------------------------------------------
    # Forward Propagation
    # --------------------------------------------------------

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


    probabilities = softmax(
        Z2
    )


    # --------------------------------------------------------
    # Cross-Entropy Loss
    # --------------------------------------------------------

    loss = (
        -np.mean(
            np.sum(
                Y_train
                *
                np.log(
                    probabilities
                    +
                    1e-8
                ),

                axis=1
            )
        )
    )


    # --------------------------------------------------------
    # Output Gradient
    # --------------------------------------------------------

    dZ2 = (
        probabilities
        -
        Y_train
    ) / len(
        X_train
    )


    dW2 = (
        A1.T
        @
        dZ2
    )


    db2 = np.sum(
        dZ2,
        axis=0,
        keepdims=True
    )


    # --------------------------------------------------------
    # Hidden-Layer Gradient
    # --------------------------------------------------------

    dA1 = (
        dZ2
        @
        W2.T
    )


    dZ1 = (
        dA1
        *
        A1
        *
        (
            1.0
            -
            A1
        )
    )


    dW1 = (
        X_train.T
        @
        dZ1
    )


    db1 = np.sum(
        dZ1,
        axis=0,
        keepdims=True
    )


    # --------------------------------------------------------
    # Gradient Descent
    # --------------------------------------------------------

    W1 -= (
        LEARNING_RATE
        *
        dW1
    )


    b1 -= (
        LEARNING_RATE
        *
        db1
    )


    W2 -= (
        LEARNING_RATE
        *
        dW2
    )


    b2 -= (
        LEARNING_RATE
        *
        db2
    )


    if (
        epoch % 500
        ==
        0
    ):

        predictions_train = np.argmax(
            probabilities,
            axis=1
        )


        train_accuracy = np.mean(
            predictions_train
            ==
            y_train
        )


        print(
            f"Epoch {epoch:4d}"
            f" | Loss = {loss:.6f}"
            f" | Accuracy = {train_accuracy:.4f}"
        )


# ============================================================
# 11. Float32 Inference Function
# ============================================================

def predict_float32(
    X_input
):

    hidden = sigmoid(
        X_input @ W1
        +
        b1
    )


    logits = (
        hidden @ W2
        +
        b2
    )


    probabilities = softmax(
        logits
    )


    return probabilities


# ============================================================
# 12. Float32 Test Accuracy
# ============================================================

float_probabilities = predict_float32(
    X_test
)


float_predictions = np.argmax(
    float_probabilities,
    axis=1
)


float_accuracy = np.mean(
    float_predictions
    ==
    y_test
)


print(
    "\n================================"
)

print(
    "Float32 Test Accuracy:",
    float_accuracy
)

print(
    "================================"
)


# ============================================================
# 13. Symmetric INT8 Quantization Function
# ============================================================
#
# q = round(x / scale)
#
# scale = max(abs(x)) / 127
#
# INT8 range:
#
# -127 ... 127
#

def quantize_symmetric(
    values
):

    max_value = np.max(
        np.abs(
            values
        )
    )


    if (
        max_value
        ==
        0
    ):

        scale = 1.0

    else:

        scale = (
            max_value
            /
            127.0
        )


    quantized = np.round(
        values
        /
        scale
    )


    quantized = np.clip(
        quantized,
        -127,
        127
    ).astype(
        np.int8
    )


    return (
        quantized,
        np.float32(
            scale
        )
    )


# ============================================================
# 14. Quantize Float32 Weights to INT8
# ============================================================

W1_int8, W1_scale = quantize_symmetric(
    W1
)


W2_int8, W2_scale = quantize_symmetric(
    W2
)


# ============================================================
# 15. Input Quantization
# ============================================================
#
# Normalized LDR:
#
# 0.0 ... 1.0
#
# mapped to:
#
# 0 ... 127
#

INPUT_SCALE = np.float32(
    1.0
    /
    127.0
)


# Hidden sigmoid output:
#
# 0.0 ... 1.0
#
# also mapped to:
#
# 0 ... 127
#

HIDDEN_SCALE = np.float32(
    1.0
    /
    127.0
)


# ============================================================
# 16. Quantize Biases to INT32
# ============================================================
#
# Bias scale:
#
# input_scale × weight_scale
#

b1_scale = (
    INPUT_SCALE
    *
    W1_scale
)


b1_int32 = np.round(
    b1
    /
    b1_scale
).astype(
    np.int32
)


b2_scale = (
    HIDDEN_SCALE
    *
    W2_scale
)


b2_int32 = np.round(
    b2
    /
    b2_scale
).astype(
    np.int32
)


# ============================================================
# 17. Simulated INT8 Inference
# ============================================================
#
# Matrix multiplications use:
#
# INT8 × INT8 -> INT32
#
# Sigmoid and final Softmax are evaluated in Float32.
#
# This is useful for teaching how quantized inference works.
#

def predict_int8(
    X_input
):

    # --------------------------------------------------------
    # Quantize Input
    # --------------------------------------------------------

    X_int8 = np.round(
        X_input
        /
        INPUT_SCALE
    )


    X_int8 = np.clip(
        X_int8,
        0,
        127
    ).astype(
        np.int8
    )


    # --------------------------------------------------------
    # Layer 1 INT8 × INT8
    # --------------------------------------------------------

    accumulator1 = (
        X_int8.astype(
            np.int32
        )
        @
        W1_int8.astype(
            np.int32
        )
        +
        b1_int32
    )


    # Convert accumulator back to real scale

    Z1 = (
        accumulator1.astype(
            np.float32
        )
        *
        b1_scale
    )


    # Sigmoid

    hidden_float = sigmoid(
        Z1
    )


    # --------------------------------------------------------
    # Quantize Hidden Activations
    # --------------------------------------------------------

    hidden_int8 = np.round(
        hidden_float
        /
        HIDDEN_SCALE
    )


    hidden_int8 = np.clip(
        hidden_int8,
        0,
        127
    ).astype(
        np.int8
    )


    # --------------------------------------------------------
    # Layer 2 INT8 × INT8
    # --------------------------------------------------------

    accumulator2 = (
        hidden_int8.astype(
            np.int32
        )
        @
        W2_int8.astype(
            np.int32
        )
        +
        b2_int32
    )


    # Convert back to float logits

    logits = (
        accumulator2.astype(
            np.float32
        )
        *
        b2_scale
    )


    probabilities = softmax(
        logits
    )


    return probabilities


# ============================================================
# 18. INT8 Test Accuracy
# ============================================================

int8_probabilities = predict_int8(
    X_test
)


int8_predictions = np.argmax(
    int8_probabilities,
    axis=1
)


int8_accuracy = np.mean(
    int8_predictions
    ==
    y_test
)


print(
    "\n================================"
)

print(
    "INT8 Test Accuracy:",
    int8_accuracy
)

print(
    "================================"
)


# ============================================================
# 19. Compare Float32 and INT8
# ============================================================

print(
    "\nModel Comparison"
)

print(
    "--------------------------------"
)

print(
    f"Float32 Accuracy : "
    f"{float_accuracy * 100:.2f}%"
)

print(
    f"INT8 Accuracy    : "
    f"{int8_accuracy * 100:.2f}%"
)

print(
    f"Accuracy Loss    : "
    f"{(float_accuracy - int8_accuracy) * 100:.2f}%"
)


# ============================================================
# 20. Print Float32 Parameters
# ============================================================

print(
    "\nFloat32 W1:"
)

print(
    W1
)


print(
    "\nFloat32 b1:"
)

print(
    b1
)


print(
    "\nFloat32 W2:"
)

print(
    W2
)


print(
    "\nFloat32 b2:"
)

print(
    b2
)


# ============================================================
# 21. Print INT8 Parameters
# ============================================================

print(
    "\nINT8 W1:"
)

print(
    W1_int8
)


print(
    "W1 scale:",
    W1_scale
)


print(
    "\nINT32 b1:"
)

print(
    b1_int32
)


print(
    "\nINT8 W2:"
)

print(
    W2_int8
)


print(
    "W2 scale:",
    W2_scale
)


print(
    "\nINT32 b2:"
)

print(
    b2_int32
)


# ============================================================
# 22. Test Selected ADC Values
# ============================================================

class_names = [
    "Dark",
    "Normal",
    "Bright"
]


test_adc_values = np.array(
    [
        300,
        500,
        800,
        1300,
        1700,
        2200,
        2800,
        3300,
        3800
    ],
    dtype=np.float32
)


test_normalized = (
    test_adc_values
    /
    4095.0
).reshape(
    -1,
    1
)


float_test = predict_float32(
    test_normalized
)


int8_test = predict_int8(
    test_normalized
)


print(
    "\n========================================"
)

print(
    "ADC Classification Comparison"
)

print(
    "========================================"
)


for i in range(
    len(
        test_adc_values
    )
):

    float_class = np.argmax(
        float_test[i]
    )


    int8_class = np.argmax(
        int8_test[i]
    )


    print(
        f"ADC = {int(test_adc_values[i]):4d}"
        f" | Norm = {test_normalized[i,0]:.3f}"
        f" | Float32 = {class_names[float_class]:6s}"
        f" | INT8 = {class_names[int8_class]:6s}"
    )
