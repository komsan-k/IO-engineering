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


# ============================================================
# 21. Full INT8 TensorFlow Lite Quantization
# ============================================================
#
# This creates a fully quantized TFLite model using a
# representative dataset from the normalized training samples.
# Input and output tensors are INT8.
#


def representative_dataset():

    for i in range(
        min(
            200,
            len(X_train)
        )
    ):

        sample = X_train[
            i:i + 1
        ].astype(
            np.float32
        )

        yield [
            sample
        ]


int8_converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)


int8_converter.optimizations = [
    tf.lite.Optimize.DEFAULT
]


int8_converter.representative_dataset = (
    representative_dataset
)


int8_converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]


int8_converter.inference_input_type = (
    tf.int8
)


int8_converter.inference_output_type = (
    tf.int8
)


int8_tflite_model = (
    int8_converter.convert()
)


with open(
    "ldr_ann_model_int8.tflite",
    "wb"
) as f:

    f.write(
        int8_tflite_model
    )


print(
    "\nSaved INT8 TensorFlow Lite model:"
)

print(
    "ldr_ann_model_int8.tflite"
)


# ============================================================
# 22. Inspect TFLite INT8 Input/Output Quantization
# ============================================================

interpreter = tf.lite.Interpreter(
    model_content=int8_tflite_model
)


interpreter.allocate_tensors()


input_details = (
    interpreter.get_input_details()
)


output_details = (
    interpreter.get_output_details()
)


input_scale, input_zero_point = (
    input_details[0]["quantization"]
)


output_scale, output_zero_point = (
    output_details[0]["quantization"]
)


print(
    "\nTFLite INT8 Quantization Parameters:"
)

print(
    "Input dtype:",
    input_details[0]["dtype"]
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
    "Output dtype:",
    output_details[0]["dtype"]
)

print(
    "Output scale:",
    output_scale
)

print(
    "Output zero point:",
    output_zero_point
)


# ============================================================
# 23. Manual Symmetric INT8 Quantization for Arduino
# ============================================================
#
# This section converts the trained Float32 parameters into a
# simple Arduino-friendly INT8 representation.
#
# Educational inference convention:
#
# normalized input 0..1
#      -> INT8 0..127
# INT8 input x INT8 W1
#      -> INT32 accumulator
# sigmoid in Float32
# hidden 0..1
#      -> INT8 0..127
# INT8 hidden x INT8 W2
#      -> INT32 accumulator
#
# Biases are stored as INT32.
#
# This manual format is separate from TensorFlow Lite's internal
# tensor quantization parameters.
#


def quantize_symmetric_int8(
    values
):

    values = np.asarray(
        values,
        dtype=np.float32
    )


    max_abs = np.max(
        np.abs(
            values
        )
    )


    if max_abs == 0.0:

        scale = np.float32(
            1.0
        )

    else:

        scale = np.float32(
            max_abs / 127.0
        )


    quantized = np.round(
        values / scale
    )


    quantized = np.clip(
        quantized,
        -127,
        127
    ).astype(
        np.int8
    )


    return quantized, scale


# ------------------------------------------------------------
# Fixed activation scales for this educational implementation
# ------------------------------------------------------------

MANUAL_INPUT_SCALE = np.float32(
    1.0 / 127.0
)


MANUAL_HIDDEN_SCALE = np.float32(
    1.0 / 127.0
)


# ------------------------------------------------------------
# Quantize W1 and W2
# ------------------------------------------------------------

W1_INT8, W1_SCALE = (
    quantize_symmetric_int8(
        W1
    )
)


W2_INT8, W2_SCALE = (
    quantize_symmetric_int8(
        W2
    )
)


# ------------------------------------------------------------
# Quantize biases to INT32
# ------------------------------------------------------------

B1_SCALE = np.float32(
    MANUAL_INPUT_SCALE
    *
    W1_SCALE
)


B2_SCALE = np.float32(
    MANUAL_HIDDEN_SCALE
    *
    W2_SCALE
)


B1_INT32 = np.round(
    b1 / B1_SCALE
).astype(
    np.int32
)


B2_INT32 = np.round(
    b2 / B2_SCALE
).astype(
    np.int32
)


# ============================================================
# 24. INT8 Arduino Export Helpers
# ============================================================


def print_int8_array_1d(
    name,
    values,
    size_name
):

    values = np.asarray(
        values
    ).flatten()


    print(
        f"const int8_t {name}[{size_name}] ="
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
            f"  {int(value)}{comma}"
        )


    print(
        "};"
    )



def print_int32_array_1d(
    name,
    values,
    size_name
):

    values = np.asarray(
        values
    ).flatten()


    print(
        f"const int32_t {name}[{size_name}] ="
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
            f"  {int(value)}{comma}"
        )


    print(
        "};"
    )



def print_int8_array_2d(
    name,
    matrix,
    row_name,
    col_name
):

    matrix = np.asarray(
        matrix
    )


    print(
        f"const int8_t {name}"
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
            str(
                int(value)
            )
            for value in matrix[i]
        )


        comma = (
            ","
            if i < matrix.shape[0] - 1
            else ""
        )


        print(
            f"  {{ {row} }}{comma}"
        )


    print(
        "};"
    )


# ============================================================
# 25. Print Arduino-Compatible INT8 Parameters
# ============================================================

print(
    "\n\n"
    "// ========================================"
)

print(
    "// INT8 ANN Parameters for Arduino"
)

print(
    "// Architecture: 1 -> 4 -> 3"
)

print(
    "// INT8 weights + INT32 biases"
)

print(
    "// ========================================\n"
)


print(
    f"const float INPUT_SCALE = "
    f"{MANUAL_INPUT_SCALE:.10f}f;"
)


print(
    f"const float HIDDEN_SCALE = "
    f"{MANUAL_HIDDEN_SCALE:.10f}f;"
)


print(
    f"const float W1_SCALE = "
    f"{W1_SCALE:.10f}f;"
)


print(
    f"const float B1_SCALE = "
    f"{B1_SCALE:.10f}f;"
)


print(
    f"const float W2_SCALE = "
    f"{W2_SCALE:.10f}f;"
)


print(
    f"const float B2_SCALE = "
    f"{B2_SCALE:.10f}f;"
)


print(
    "\n// W1 shape = 1 x 4"
)


print_int8_array_1d(
    "W1_INT8",
    W1_INT8[0],
    "HIDDEN_SIZE"
)


print(
    "\n// b1 shape = 4"
)


print_int32_array_1d(
    "B1_INT32",
    B1_INT32,
    "HIDDEN_SIZE"
)


print(
    "\n// W2 shape = 4 x 3"
)


print_int8_array_2d(
    "W2_INT8",
    W2_INT8,
    "HIDDEN_SIZE",
    "OUTPUT_SIZE"
)


print(
    "\n// b2 shape = 3"
)


print_int32_array_1d(
    "B2_INT32",
    B2_INT32,
    "OUTPUT_SIZE"
)


# ============================================================
# 26. Save INT8 Arduino Header File
# ============================================================


def format_int8_array_1d(
    name,
    values,
    size_name
):

    values = np.asarray(
        values
    ).flatten()


    lines = [
        f"const int8_t {name}[{size_name}] =",
        "{"
    ]


    for i, value in enumerate(
        values
    ):

        comma = (
            ","
            if i < len(values) - 1
            else ""
        )


        lines.append(
            f"  {int(value)}{comma}"
        )


    lines.append(
        "};"
    )


    return "\n".join(
        lines
    )



def format_int32_array_1d(
    name,
    values,
    size_name
):

    values = np.asarray(
        values
    ).flatten()


    lines = [
        f"const int32_t {name}[{size_name}] =",
        "{"
    ]


    for i, value in enumerate(
        values
    ):

        comma = (
            ","
            if i < len(values) - 1
            else ""
        )


        lines.append(
            f"  {int(value)}{comma}"
        )


    lines.append(
        "};"
    )


    return "\n".join(
        lines
    )



def format_int8_array_2d(
    name,
    matrix,
    row_name,
    col_name
):

    matrix = np.asarray(
        matrix
    )


    lines = [
        f"const int8_t {name}[{row_name}][{col_name}] =",
        "{"
    ]


    for i in range(
        matrix.shape[0]
    ):

        row = ", ".join(
            str(
                int(value)
            )
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


int8_header_text = f"""
#ifndef LDR_ANN_WEIGHTS_INT8_H
#define LDR_ANN_WEIGHTS_INT8_H

#include <stdint.h>

#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 3

const float INPUT_SCALE = {MANUAL_INPUT_SCALE:.10f}f;
const float HIDDEN_SCALE = {MANUAL_HIDDEN_SCALE:.10f}f;
const float W1_SCALE = {W1_SCALE:.10f}f;
const float B1_SCALE = {B1_SCALE:.10f}f;
const float W2_SCALE = {W2_SCALE:.10f}f;
const float B2_SCALE = {B2_SCALE:.10f}f;

"""


int8_header_text += (
    "// W1 shape = 1 x 4\n"
)


int8_header_text += format_int8_array_1d(
    "W1_INT8",
    W1_INT8[0],
    "HIDDEN_SIZE"
)


int8_header_text += (
    "\n\n// b1 shape = 4\n"
)


int8_header_text += format_int32_array_1d(
    "B1_INT32",
    B1_INT32,
    "HIDDEN_SIZE"
)


int8_header_text += (
    "\n\n// W2 shape = 4 x 3\n"
)


int8_header_text += format_int8_array_2d(
    "W2_INT8",
    W2_INT8,
    "HIDDEN_SIZE",
    "OUTPUT_SIZE"
)


int8_header_text += (
    "\n\n// b2 shape = 3\n"
)


int8_header_text += format_int32_array_1d(
    "B2_INT32",
    B2_INT32,
    "OUTPUT_SIZE"
)


int8_header_text += "\n\n#endif\n"


with open(
    "ldr_ann_weights_int8.h",
    "w"
) as f:

    f.write(
        int8_header_text
    )


print(
    "\nSaved Arduino INT8 weight file:"
)

print(
    "ldr_ann_weights_int8.h"
)


# ============================================================
# 27. Compare Float32 and Manual INT8 Predictions
# ============================================================
#
# This simulates the same simple hybrid INT8 inference intended
# for an Arduino implementation. Sigmoid remains Float32.
#


def manual_int8_predict(
    x_normalized
):

    # Input 0..1 -> INT8 0..127
    x_q = int(
        np.clip(
            np.round(
                x_normalized
                /
                MANUAL_INPUT_SCALE
            ),
            0,
            127
        )
    )


    hidden_float = np.zeros(
        HIDDEN_SIZE,
        dtype=np.float32
    )


    hidden_q = np.zeros(
        HIDDEN_SIZE,
        dtype=np.int8
    )


    for j in range(
        HIDDEN_SIZE
    ):

        acc = (
            x_q
            *
            int(
                W1_INT8[0, j]
            )
            +
            int(
                B1_INT32[j]
            )
        )


        z = (
            acc
            *
            B1_SCALE
        )


        h = 1.0 / (
            1.0
            +
            np.exp(
                -z
            )
        )


        hidden_float[j] = h


        hidden_q[j] = np.int8(
            np.clip(
                np.round(
                    h
                    /
                    MANUAL_HIDDEN_SCALE
                ),
                0,
                127
            )
        )


    logits = np.zeros(
        OUTPUT_SIZE,
        dtype=np.float32
    )


    for k in range(
        OUTPUT_SIZE
    ):

        acc = int(
            B2_INT32[k]
        )


        for j in range(
            HIDDEN_SIZE
        ):

            acc += (
                int(
                    hidden_q[j]
                )
                *
                int(
                    W2_INT8[j, k]
                )
            )


        logits[k] = (
            acc
            *
            B2_SCALE
        )


    shifted = (
        logits
        -
        np.max(
            logits
        )
    )


    exp_values = np.exp(
        shifted
    )


    probabilities = (
        exp_values
        /
        np.sum(
            exp_values
        )
    )


    predicted_class = int(
        np.argmax(
            probabilities
        )
    )


    return predicted_class, probabilities


print(
    "\nFloat32 vs Manual INT8 Test:"
)


int8_correct = 0


for i in range(
    len(X_test)
):

    int8_class, int8_prob = (
        manual_int8_predict(
            float(
                X_test[i, 0]
            )
        )
    )


    if int8_class == int(
        y_test[i]
    ):

        int8_correct += 1


manual_int8_accuracy = (
    int8_correct
    /
    len(X_test)
)


print(
    "Float32 Test Accuracy:",
    test_accuracy
)


print(
    "Manual INT8 Test Accuracy:",
    manual_int8_accuracy
)


print(
    "\nGenerated model files:"
)

print(
    "ldr_ann_model.keras"
)

print(
    "ldr_ann_model.tflite"
)

print(
    "ldr_ann_model_int8.tflite"
)

print(
    "ldr_ann_weights.h"
)

print(
    "ldr_ann_weights_int8.h"
)
