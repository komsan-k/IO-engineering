import tensorflow as tf
import numpy as np
import pandas as pd

# ============================================================
# 1. Configuration
# ============================================================

TOTAL_SAMPLES = 600
SAMPLES_PER_CLASS = TOTAL_SAMPLES // 3
RANDOM_SEED = 42

ADC_MAX = 1000.0

HIDDEN_SIZE = 4
OUTPUT_SIZE = 3

EPOCHS = 100
BATCH_SIZE = 16

CLASS_NAMES = np.array([
    "dark",
    "normal",
    "bright"
])


# ============================================================
# 2. Create Mock LDR Dataset
# ============================================================
#
# Total samples = 600
#
# Bright : 0   to 99
# Normal : 100 to 499
# Dark   : 500 to 1000
#
# 200 samples per class
#

rng = np.random.default_rng(
    RANDOM_SEED
)


bright_values = rng.integers(
    low=0,
    high=100,
    size=SAMPLES_PER_CLASS
)


normal_values = rng.integers(
    low=100,
    high=500,
    size=SAMPLES_PER_CLASS
)


dark_values = rng.integers(
    low=500,
    high=1001,
    size=SAMPLES_PER_CLASS
)


df = pd.DataFrame(
    {
        "light_value": np.concatenate(
            [
                bright_values,
                normal_values,
                dark_values
            ]
        ),

        "label": (
            ["bright"] * SAMPLES_PER_CLASS
            +
            ["normal"] * SAMPLES_PER_CLASS
            +
            ["dark"] * SAMPLES_PER_CLASS
        )
    }
)


# Shuffle mock dataset

df = df.sample(
    frac=1.0,
    random_state=RANDOM_SEED
).reset_index(
    drop=True
)


print(
    "\nFirst rows:"
)

print(
    df.head()
)


print(
    "\nClass counts:"
)

print(
    df["label"].value_counts()
)


print(
    "\nValue ranges:"
)

for label in [
    "bright",
    "normal",
    "dark"
]:
    values = df[
        df["label"] == label
    ][
        "light_value"
    ]

    print(
        f"{label:6s}: "
        f"{int(values.min())} "
        f"to "
        f"{int(values.max())}"
    )


# ============================================================
# 3. Clean Labels
# ============================================================

df["label"] = (
    df["label"]
    .astype(str)
    .str.strip()
    .str.lower()
)


# ============================================================
# 4. Prepare Input
# ============================================================

X = (
    df["light_value"]
    .values
    .astype(
        np.float32
    )
)


# Normalize:
# 0 ... 1000
#     ↓
# 0.0 ... 1.0

X = X / ADC_MAX

X = X.reshape(
    -1,
    1
)


# ============================================================
# 5. Encode Labels
# ============================================================

label_map = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}


y = (
    df["label"]
    .map(
        label_map
    )
    .values
)


if np.isnan(
    y
).any():

    raise ValueError(
        "Unknown label found. "
        "Use only: dark, normal, bright"
    )


y = y.astype(
    np.int32
)


# ============================================================
# 6. Shuffle Dataset
# ============================================================

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


# ============================================================
# 7. Train / Validation / Test Split
# ============================================================

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


print(
    "\nDataset split:"
)

print(
    "Training:",
    len(X_train)
)

print(
    "Validation:",
    len(X_val)
)

print(
    "Testing:",
    len(X_test)
)


# ============================================================
# 8. Build ANN Model
# ============================================================
#
# Architecture:
#
# 1 Input
#   ↓
# Dense 4
# Sigmoid
#   ↓
# Dense 3
# Softmax
#

model = tf.keras.Sequential([
    tf.keras.layers.Input(
        shape=(1,)
    ),

    tf.keras.layers.Dense(
        HIDDEN_SIZE,
        activation="sigmoid"
    ),

    tf.keras.layers.Dense(
        OUTPUT_SIZE,
        activation="softmax"
    )
])


# ============================================================
# 9. Compile Model
# ============================================================

model.compile(
    optimizer=tf.keras.optimizers.Adam(
        learning_rate=0.01
    ),

    loss="sparse_categorical_crossentropy",

    metrics=[
        "accuracy"
    ]
)


print(
    "\nModel Summary:"
)

model.summary()


# ============================================================
# 10. Train Model
# ============================================================

history = model.fit(
    X_train,
    y_train,

    validation_data=(
        X_val,
        y_val
    ),

    epochs=EPOCHS,

    batch_size=BATCH_SIZE,

    verbose=1
)


# ============================================================
# 11. Evaluate Model
# ============================================================

test_loss, test_accuracy = model.evaluate(
    X_test,
    y_test,
    verbose=0
)


print(
    "\n================================"
)

print(
    "Test Loss:",
    test_loss
)

print(
    "Test Accuracy:",
    test_accuracy
)

print(
    "================================"
)


# ============================================================
# 12. Predictions
# ============================================================

predictions = model.predict(
    X_test,
    verbose=0
)


predicted_classes = np.argmax(
    predictions,
    axis=1
)


print(
    "\nSample Predictions:"
)


for i in range(
    min(
        10,
        len(X_test)
    )
):

    print(
        f"Normalized ADC = "
        f"{X_test[i][0]:.4f}"
        f" | Actual = "
        f"{CLASS_NAMES[y_test[i]]}"
        f" | Predicted = "
        f"{CLASS_NAMES[predicted_classes[i]]}"
        f" | Probabilities = "
        f"{predictions[i]}"
    )


# ============================================================
# 13. Test Selected ADC Values
# ============================================================

test_adc = np.array(
    [
        25,
        75,
        100,
        250,
        499,
        500,
        650,
        800,
        950,
        1000
    ],
    dtype=np.float32
)


test_input = (
    test_adc
    /
    ADC_MAX
).reshape(
    -1,
    1
)


test_predictions = model.predict(
    test_input,
    verbose=0
)


print(
    "\nSelected ADC Tests:"
)


for i in range(
    len(test_adc)
):

    predicted_index = np.argmax(
        test_predictions[i]
    )


    print(
        f"ADC = "
        f"{int(test_adc[i]):4d}"
        f" | Norm = "
        f"{test_input[i,0]:.4f}"
        f" | Prediction = "
        f"{CLASS_NAMES[predicted_index]}"
        f" | P = "
        f"{test_predictions[i]}"
    )


# ============================================================
# 14. Save Keras Model
# ============================================================

model.save(
    "ldr_ann_model.keras"
)


print(
    "\nSaved:"
)

print(
    "ldr_ann_model.keras"
)


# ============================================================
# 15. Convert to TensorFlow Lite
# ============================================================

converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)


tflite_model = converter.convert()


with open(
    "ldr_ann_model.tflite",
    "wb"
) as f:

    f.write(
        tflite_model
    )


print(
    "ldr_ann_model.tflite"
)


# ============================================================
# 16. Get Trained Weights
# ============================================================

weights = model.get_weights()


W1 = weights[0]
b1 = weights[1]

W2 = weights[2]
b2 = weights[3]


print(
    "\nTensorFlow Shapes:"
)

print(
    "W1:",
    W1.shape
)

print(
    "b1:",
    b1.shape
)

print(
    "W2:",
    W2.shape
)

print(
    "b2:",
    b2.shape
)


# ============================================================
# 17. Arduino Export Helper — 1D Array
# ============================================================

def print_arduino_array_1d(
    name,
    values,
    size_name
):

    print(
        f"float {name}[{size_name}] ="
    )

    print(
        "{"
    )


    for i, value in enumerate(
        values
    ):

        comma = (
            ","
            if i < len(values) - 1
            else ""
        )


        print(
            f"  {value:.8f}f"
            f"{comma}"
        )


    print(
        "};"
    )


# ============================================================
# 18. Arduino Export Helper — 2D Array
# ============================================================

def print_arduino_array_2d(
    name,
    matrix,
    row_name,
    col_name
):

    print(
        f"float {name}"
        f"[{row_name}]"
        f"[{col_name}] ="
    )


    print(
        "{"
    )


    for i in range(
        matrix.shape[0]
    ):

        row = ", ".join(
            f"{value:.8f}f"
            for value in matrix[i]
        )


        comma = (
            ","
            if i < matrix.shape[0] - 1
            else ""
        )


        print(
            f"  {{ {row} }}"
            f"{comma}"
        )


    print(
        "};"
    )


# ============================================================
# 19. Print Arduino-Compatible Parameters
# ============================================================

print(
    "\n\n"
    "// ========================================"
)

print(
    "// Trained ANN Parameters for Arduino"
)

print(
    "// Architecture: 1 -> 4 -> 3"
)

print(
    "// ========================================\n"
)


# ------------------------------------------------------------
# W1
# TensorFlow shape: (1,4)
# Arduino shape: [4]
# ------------------------------------------------------------

print(
    "// W1 shape = 1 x 4"
)


print_arduino_array_1d(
    "W1",
    W1[0],
    "HIDDEN_SIZE"
)


# ------------------------------------------------------------
# b1
# ------------------------------------------------------------

print(
    "\n\n// b1 shape = 4"
)


print_arduino_array_1d(
    "b1",
    b1,
    "HIDDEN_SIZE"
)


# ------------------------------------------------------------
# W2
# TensorFlow shape: (4,3)
# ------------------------------------------------------------

print(
    "\n\n// W2 shape = 4 x 3"
)


print_arduino_array_2d(
    "W2",
    W2,
    "HIDDEN_SIZE",
    "OUTPUT_SIZE"
)


# ------------------------------------------------------------
# b2
# ------------------------------------------------------------

print(
    "\n\n// b2 shape = 3"
)


print_arduino_array_1d(
    "b2",
    b2,
    "OUTPUT_SIZE"
)


# ============================================================
# 20. Also Save Arduino Parameters to Header File
# ============================================================

def format_array_1d(
    name,
    values,
    size_name
):

    lines = []

    lines.append(
        f"float {name}[{size_name}] ="
    )

    lines.append(
        "{"
    )


    for i, value in enumerate(
        values
    ):

        comma = (
            ","
            if i < len(values) - 1
            else ""
        )


        lines.append(
            f"  {value:.8f}f{comma}"
        )


    lines.append(
        "};"
    )


    return "\n".join(
        lines
    )


def format_array_2d(
    name,
    matrix,
    row_name,
    col_name
):

    lines = []


    lines.append(
        f"float {name}"
        f"[{row_name}]"
        f"[{col_name}] ="
    )


    lines.append(
        "{"
    )


    for i in range(
        matrix.shape[0]
    ):

        row = ", ".join(
            f"{value:.8f}f"
            for value in matrix[i]
        )


        comma = (
            ","
            if i < matrix.shape[0] - 1
            else ""
        )


        lines.append(
            f"  {{ {row} }}{comma}"
        )


    lines.append(
        "};"
    )


    return "\n".join(
        lines
    )


header_text = """

#ifndef LDR_ANN_WEIGHTS_H
#define LDR_ANN_WEIGHTS_H

#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 3

"""


header_text += (
    "// W1 shape = 1 x 4\n"
)


header_text += format_array_1d(
    "W1",
    W1[0],
    "HIDDEN_SIZE"
)


header_text += (
    "\n\n"
    "// b1 shape = 4\n"
)


header_text += format_array_1d(
    "b1",
    b1,
    "HIDDEN_SIZE"
)


header_text += (
    "\n\n"
    "// W2 shape = 4 x 3\n"
)


header_text += format_array_2d(
    "W2",
    W2,
    "HIDDEN_SIZE",
    "OUTPUT_SIZE"
)


header_text += (
    "\n\n"
    "// b2 shape = 3\n"
)


header_text += format_array_1d(
    "b2",
    b2,
    "OUTPUT_SIZE"
)


header_text += """

#endif
"""


with open(
    "ldr_ann_weights.h",
    "w"
) as f:

    f.write(
        header_text
    )


print(
    "\nSaved Arduino weight file:"
)

print(
    "ldr_ann_weights.h"
)
