import tensorflow as tf
import numpy as np
import pandas as pd

# ============================================================
# 1. Configuration
# ============================================================

CSV_FILE = "light_dataset.csv"

ADC_MAX = 4095.0

HIDDEN_SIZE = 4
OUTPUT_SIZE = 3

EPOCHS = 100
BATCH_SIZE = 16

RANDOM_SEED = 42

CLASS_NAMES = np.array([
    "dark",
    "normal",
    "bright"
])

LABEL_MAP = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}


# ============================================================
# 2. Reproducibility
# ============================================================

np.random.seed(
    RANDOM_SEED
)

tf.random.set_seed(
    RANDOM_SEED
)


# ============================================================
# 3. Load Dataset
# ============================================================

df = pd.read_csv(
    CSV_FILE
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


# ============================================================
# 4. Clean Labels
# ============================================================

df["label"] = (
    df["label"]
    .astype(str)
    .str.strip()
    .str.lower()
)


# ============================================================
# 5. Prepare Input
# ============================================================

X = (
    df["light_value"]
    .values
    .astype(
        np.float32
    )
)

# Normalize:
#
# 0 ... 4095
#     ↓
# 0.0 ... 1.0

X = X / ADC_MAX

X = X.reshape(
    -1,
    1
)


# ============================================================
# 6. Encode Labels
# ============================================================

y = (
    df["label"]
    .map(
        LABEL_MAP
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
# 7. Shuffle Dataset
# ============================================================

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
# 8. Train / Validation / Test Split
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
# 9. Build ANN Model
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
# Training remains Float32.
# Quantization is performed after training.
# ============================================================

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
# 10. Compile Model
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
# 11. Train Float32 Model
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
# 12. Evaluate Float32 Model
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
    "Float32 Test Loss:",
    test_loss
)

print(
    "Float32 Test Accuracy:",
    test_accuracy
)

print(
    "================================"
)


# ============================================================
# 13. Float32 Predictions
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
    "\nSample Float32 Predictions:"
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
# 14. Save Float32 Keras Model
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
# 15. Representative Dataset for Full INT8 Quantization
# ============================================================
#
# This dataset tells the converter the expected activation
# ranges. Use representative TRAINING samples only.
#
# The generator yields Float32 input because calibration
# occurs before integer input conversion.
# ============================================================

def representative_dataset():

    # Use at most 200 representative samples.
    sample_count = min(
        200,
        len(X_train)
    )

    sample_indices = np.linspace(
        0,
        len(X_train) - 1,
        sample_count,
        dtype=int
    )

    for i in sample_indices:

        sample = X_train[
            i:i + 1
        ].astype(
            np.float32
        )

        yield [
            sample
        ]


# ============================================================
# 16. Convert Model to Fully Quantized INT8 TFLite
# ============================================================

converter = tf.lite.TFLiteConverter.from_keras_model(
    model
)

# Enable post-training quantization
converter.optimizations = [
    tf.lite.Optimize.DEFAULT
]

# Calibration data
converter.representative_dataset = (
    representative_dataset
)

# Force integer-only TensorFlow Lite operators
converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]

# Force INT8 input and output tensors
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

tflite_int8_model = converter.convert()

with open(
    "ldr_ann_model_int8.tflite",
    "wb"
) as f:

    f.write(
        tflite_int8_model
    )

print(
    "ldr_ann_model_int8.tflite"
)


# ============================================================
# 17. Create INT8 TFLite Interpreter
# ============================================================

interpreter = tf.lite.Interpreter(
    model_content=tflite_int8_model
)

interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print(
    "\nINT8 TensorFlow Lite Model:"
)

print(
    "Input details:"
)

print(
    input_details[0]
)

print(
    "\nOutput details:"
)

print(
    output_details[0]
)


# ============================================================
# 18. Read INT8 Quantization Parameters
# ============================================================
#
# TensorFlow Lite quantization formula:
#
#     q = round(x / scale) + zero_point
#
# Dequantization:
#
#     x = (q - zero_point) * scale
#
# ============================================================

input_scale, input_zero_point = (
    input_details[0]["quantization"]
)

output_scale, output_zero_point = (
    output_details[0]["quantization"]
)

print(
    "\n================================"
)

print(
    "INT8 Quantization Parameters"
)

print(
    "================================"
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


# ============================================================
# 19. Helper — Float32 to INT8
# ============================================================

def quantize_input(
    x_float
):

    q = np.round(
        x_float
        /
        input_scale
        +
        input_zero_point
    )

    q = np.clip(
        q,
        -128,
        127
    )

    return q.astype(
        np.int8
    )


# ============================================================
# 20. Helper — INT8 Output to Float32
# ============================================================

def dequantize_output(
    q_output
):

    return (
        q_output.astype(
            np.float32
        )
        -
        output_zero_point
    ) * output_scale


# ============================================================
# 21. Evaluate INT8 Model
# ============================================================

int8_predicted_classes = []

int8_probabilities = []

for sample in X_test:

    # Original normalized Float32 input
    x_float = sample.reshape(
        1,
        1
    ).astype(
        np.float32
    )

    # Convert to INT8 using TFLite scale and zero point
    x_int8 = quantize_input(
        x_float
    )

    interpreter.set_tensor(
        input_details[0]["index"],
        x_int8
    )

    interpreter.invoke()

    output_int8 = interpreter.get_tensor(
        output_details[0]["index"]
    )[0]

    # Convert output back to Float32 for readable probabilities
    output_float = dequantize_output(
        output_int8
    )

    int8_probabilities.append(
        output_float
    )

    int8_predicted_classes.append(
        np.argmax(
            output_float
        )
    )

int8_probabilities = np.array(
    int8_probabilities
)

int8_predicted_classes = np.array(
    int8_predicted_classes
)

int8_accuracy = np.mean(
    int8_predicted_classes
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
    "Float32 Test Accuracy:",
    test_accuracy
)

print(
    "Accuracy Difference:",
    int8_accuracy
    -
    test_accuracy
)

print(
    "================================"
)


# ============================================================
# 22. Display Sample INT8 Predictions
# ============================================================

print(
    "\nSample INT8 Predictions:"
)

for i in range(
    min(
        10,
        len(X_test)
    )
):

    x_float = X_test[
        i:i + 1
    ]

    x_int8 = quantize_input(
        x_float
    )

    print(
        f"Norm ADC = "
        f"{X_test[i][0]:.4f}"
        f" | INT8 Input = "
        f"{int(x_int8[0][0])}"
        f" | Actual = "
        f"{CLASS_NAMES[y_test[i]]}"
        f" | Predicted = "
        f"{CLASS_NAMES[int8_predicted_classes[i]]}"
        f" | Dequantized Output = "
        f"{int8_probabilities[i]}"
    )


# ============================================================
# 23. Test Selected ADC Values with INT8 Model
# ============================================================

test_adc = np.array(
    [
        300,
        500,
        800,
        1200,
        1600,
        2000,
        2500,
        3000,
        3400,
        3800
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

print(
    "\nSelected ADC Tests — INT8:"
)

for i in range(
    len(test_adc)
):

    x_float = test_input[
        i:i + 1
    ].astype(
        np.float32
    )

    x_int8 = quantize_input(
        x_float
    )

    interpreter.set_tensor(
        input_details[0]["index"],
        x_int8
    )

    interpreter.invoke()

    output_int8 = interpreter.get_tensor(
        output_details[0]["index"]
    )[0]

    output_float = dequantize_output(
        output_int8
    )

    predicted_index = np.argmax(
        output_float
    )

    print(
        f"ADC = "
        f"{int(test_adc[i]):4d}"
        f" | Norm = "
        f"{x_float[0,0]:.4f}"
        f" | INT8 = "
        f"{int(x_int8[0,0]):4d}"
        f" | Prediction = "
        f"{CLASS_NAMES[predicted_index]}"
        f" | Output INT8 = "
        f"{output_int8}"
        f" | Output Float = "
        f"{output_float}"
    )


# ============================================================
# 24. Export INT8 TFLite Model as Arduino C Header
# ============================================================

def export_tflite_as_c_array(
    model_bytes,
    output_file,
    array_name
):

    with open(
        output_file,
        "w"
    ) as f:

        guard = (
            array_name.upper()
            +
            "_H"
        )

        f.write(
            f"#ifndef {guard}\n"
        )

        f.write(
            f"#define {guard}\n\n"
        )

        f.write(
            "#include <stdint.h>\n\n"
        )

        f.write(
            "alignas(16) const unsigned char "
            f"{array_name}[] = {{\n"
        )

        for i, byte in enumerate(
            model_bytes
        ):

            if i % 12 == 0:
                f.write(
                    "  "
                )

            f.write(
                f"0x{byte:02X}"
            )

            if i != len(
                model_bytes
            ) - 1:

                f.write(
                    ", "
                )

            if (
                i % 12 == 11
                or
                i == len(
                    model_bytes
                ) - 1
            ):

                f.write(
                    "\n"
                )

        f.write(
            "};\n\n"
        )

        f.write(
            "const unsigned int "
            f"{array_name}_len = "
            f"{len(model_bytes)};\n\n"
        )

        f.write(
            f"#endif  // {guard}\n"
        )


export_tflite_as_c_array(
    tflite_int8_model,
    "ldr_ann_model_int8.h",
    "ldr_ann_model_int8_tflite"
)

print(
    "\nSaved Arduino INT8 model header:"
)

print(
    "ldr_ann_model_int8.h"
)


# ============================================================
# 25. Save Quantization Parameters for Arduino Reference
# ============================================================
#
# Note:
# A TFLM/EloquentTinyML runtime normally reads these parameters
# directly from the model tensors. This header is useful for
# teaching or for manual INT8 input/output handling.
# ============================================================

with open(
    "ldr_int8_quantization.h",
    "w"
) as f:

    f.write(
        "#ifndef LDR_INT8_QUANTIZATION_H\n"
    )

    f.write(
        "#define LDR_INT8_QUANTIZATION_H\n\n"
    )

    f.write(
        f"const float LDR_INPUT_SCALE = "
        f"{input_scale:.12g}f;\n"
    )

    f.write(
        f"const int LDR_INPUT_ZERO_POINT = "
        f"{int(input_zero_point)};\n\n"
    )

    f.write(
        f"const float LDR_OUTPUT_SCALE = "
        f"{output_scale:.12g}f;\n"
    )

    f.write(
        f"const int LDR_OUTPUT_ZERO_POINT = "
        f"{int(output_zero_point)};\n\n"
    )

    f.write(
        "#endif\n"
    )

print(
    "ldr_int8_quantization.h"
)


# ============================================================
# 26. Model Size Comparison
# ============================================================

# Create a Float32 TFLite model only for size comparison.

float_converter = (
    tf.lite.TFLiteConverter
    .from_keras_model(
        model
    )
)

tflite_float_model = (
    float_converter.convert()
)

with open(
    "ldr_ann_model_float32.tflite",
    "wb"
) as f:

    f.write(
        tflite_float_model
    )

float_size = len(
    tflite_float_model
)

int8_size = len(
    tflite_int8_model
)

print(
    "\n================================"
)

print(
    "Model Size Comparison"
)

print(
    "================================"
)

print(
    "Float32 TFLite:",
    float_size,
    "bytes"
)

print(
    "INT8 TFLite:",
    int8_size,
    "bytes"
)

if float_size > 0:

    reduction = (
        1.0
        -
        int8_size
        /
        float_size
    ) * 100.0

    print(
        "Size reduction:",
        f"{reduction:.2f}%"
    )


# ============================================================
# 27. Final Summary
# ============================================================

print(
    "\n================================"
)

print(
    "Generated Files"
)

print(
    "================================"
)

print(
    "1. ldr_ann_model.keras"
)

print(
    "2. ldr_ann_model_float32.tflite"
)

print(
    "3. ldr_ann_model_int8.tflite"
)

print(
    "4. ldr_ann_model_int8.h"
)

print(
    "5. ldr_int8_quantization.h"
)

print(
    "\nINT8 class mapping:"
)

print(
    "0 = Dark"
)

print(
    "1 = Normal"
)

print(
    "2 = Bright"
)

print(
    "\nArduino/TFLM preprocessing:"
)

print(
    "1. Read ADC."
)

print(
    "2. Normalize ADC / 4095.0."
)

print(
    "3. Quantize using:"
)

print(
    "   q = round(x / input_scale) "
    "+ input_zero_point"
)

print(
    "4. Clip q to [-128, 127]."
)

print(
    "5. Run INT8 inference."
)
