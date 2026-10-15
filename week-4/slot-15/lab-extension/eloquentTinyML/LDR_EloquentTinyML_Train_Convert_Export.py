"""
Complete LDR TinyML pipeline for the EloquentTinyML lab.

ANN: 1 -> 4 -> 3
Classes: 0=dark, 1=normal, 2=bright
Input normalization: light_value / 1000.0

Outputs:
- ldr_model.keras
- ldr_model.tflite
- ldr_model.h
- training_history.csv

Expected CSV columns:
light_value,label
"""

from pathlib import Path
import sys
import numpy as np
import pandas as pd
import tensorflow as tf

CSV_FILE = Path("light_dataset.csv")
KERAS_FILE = Path("ldr_model.keras")
TFLITE_FILE = Path("ldr_model.tflite")
HEADER_FILE = Path("ldr_model.h")
HISTORY_FILE = Path("training_history.csv")

ADC_MAX = 1000.0
HIDDEN_SIZE = 4
OUTPUT_SIZE = 3
EPOCHS = 100
BATCH_SIZE = 16
LEARNING_RATE = 0.01
RANDOM_SEED = 42

CLASS_NAMES = ["dark", "normal", "bright"]
LABEL_MAP = {"dark": 0, "normal": 1, "bright": 2}

np.random.seed(RANDOM_SEED)
tf.random.set_seed(RANDOM_SEED)


def create_mock_dataset(csv_path: Path):
    """Create 600 balanced teaching samples if CSV is absent."""
    rng = np.random.default_rng(RANDOM_SEED)
    bright = rng.integers(0, 100, 200)
    normal = rng.integers(100, 500, 200)
    dark = rng.integers(500, 1001, 200)

    df = pd.DataFrame({
        "light_value": np.concatenate([bright, normal, dark]),
        "label": ["bright"] * 200 + ["normal"] * 200 + ["dark"] * 200,
    })
    df = df.sample(frac=1.0, random_state=RANDOM_SEED).reset_index(drop=True)
    df.to_csv(csv_path, index=False)
    print(f"[INFO] Created mock dataset: {csv_path}")
    return df


def load_dataset():
    if CSV_FILE.exists():
        print(f"[INFO] Loading dataset: {CSV_FILE}")
        df = pd.read_csv(CSV_FILE)
    else:
        print(f"[WARNING] {CSV_FILE} not found; creating a mock dataset.")
        df = create_mock_dataset(CSV_FILE)

    required = {"light_value", "label"}
    missing = required - set(df.columns)
    if missing:
        raise ValueError(f"Missing columns: {sorted(missing)}")

    df["light_value"] = pd.to_numeric(df["light_value"], errors="coerce")
    df["label"] = df["label"].astype(str).str.strip().str.lower()
    df = df.dropna(subset=["light_value", "label"]).reset_index(drop=True)

    unknown = sorted(set(df["label"]) - set(LABEL_MAP))
    if unknown:
        raise ValueError(f"Unknown labels: {unknown}")

    print("\nDataset class counts:")
    print(df["label"].value_counts())
    print("Total samples:", len(df))
    return df


def stratified_split(df):
    """70/15/15 split independently within each class."""
    train_parts, val_parts, test_parts = [], [], []

    for label in CLASS_NAMES:
        part = (
            df[df["label"] == label]
            .sample(frac=1.0, random_state=RANDOM_SEED)
            .reset_index(drop=True)
        )
        n = len(part)
        train_end = int(0.70 * n)
        val_end = int(0.85 * n)
        train_parts.append(part.iloc[:train_end])
        val_parts.append(part.iloc[train_end:val_end])
        test_parts.append(part.iloc[val_end:])

    train_df = pd.concat(train_parts).sample(frac=1.0, random_state=RANDOM_SEED).reset_index(drop=True)
    val_df = pd.concat(val_parts).sample(frac=1.0, random_state=RANDOM_SEED + 1).reset_index(drop=True)
    test_df = pd.concat(test_parts).sample(frac=1.0, random_state=RANDOM_SEED + 2).reset_index(drop=True)
    return train_df, val_df, test_df


def dataframe_to_xy(df):
    X = (df["light_value"].to_numpy(dtype=np.float32) / ADC_MAX).reshape(-1, 1)
    y = df["label"].map(LABEL_MAP).to_numpy(dtype=np.int32)
    return X, y


def build_model():
    model = tf.keras.Sequential([
        tf.keras.layers.Input(shape=(1,), name="ldr_input"),
        tf.keras.layers.Dense(HIDDEN_SIZE, activation="sigmoid", name="hidden"),
        tf.keras.layers.Dense(OUTPUT_SIZE, activation="softmax", name="class_output"),
    ])

    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE),
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )
    return model


def convert_to_tflite(model, output_path: Path):
    """Convert trained Keras model to Float32 TensorFlow Lite."""
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()
    output_path.write_bytes(tflite_model)
    print(f"[INFO] Saved TFLite model: {output_path}")
    return tflite_model


def evaluate_tflite(tflite_path: Path, X_test, y_test):
    interpreter = tf.lite.Interpreter(model_path=str(tflite_path))
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print("\nTFLite tensor information")
    print("Input shape :", input_details[0]["shape"])
    print("Input dtype :", input_details[0]["dtype"])
    print("Output shape:", output_details[0]["shape"])
    print("Output dtype:", output_details[0]["dtype"])

    preds = []
    for sample in X_test:
        x = sample.reshape(1, 1).astype(np.float32)
        interpreter.set_tensor(input_details[0]["index"], x)
        interpreter.invoke()
        y_out = interpreter.get_tensor(output_details[0]["index"])[0]
        preds.append(int(np.argmax(y_out)))

    preds = np.asarray(preds, dtype=np.int32)
    accuracy = float(np.mean(preds == y_test))
    print(f"TFLite test accuracy: {accuracy * 100:.2f}%")
    return accuracy


def export_c_array(tflite_path: Path, header_path: Path, array_name="ldr_model_tflite"):
    """Export .tflite bytes to an Arduino-ready C header."""
    data = tflite_path.read_bytes()
    guard = "LDR_MODEL_H"

    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <Arduino.h>",
        "",
        f"alignas(16) const unsigned char {array_name}[] = {{",
    ]

    for i in range(0, len(data), 12):
        chunk = data[i:i + 12]
        row = ", ".join(f"0x{b:02X}" for b in chunk)
        if i + 12 < len(data):
            row += ","
        lines.append("  " + row)

    lines += [
        "};",
        "",
        f"const unsigned int {array_name}_len = {len(data)};",
        "",
        f"#endif  // {guard}",
        "",
    ]

    header_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"[INFO] Saved Arduino header: {header_path}")
    print(f"[INFO] Model size: {len(data)} bytes")


def test_selected_values(model):
    values = np.array([25, 75, 100, 250, 499, 500, 650, 800, 950, 1000], dtype=np.float32)
    X_demo = (values / ADC_MAX).reshape(-1, 1)
    probs = model.predict(X_demo, verbose=0)
    preds = np.argmax(probs, axis=1)

    print("\nSelected ADC test values")
    print("------------------------")
    for adc, p, pred in zip(values, probs, preds):
        print(
            f"ADC={int(adc):4d}  normalized={adc/ADC_MAX:.3f}  "
            f"dark={p[0]:.4f}  normal={p[1]:.4f}  bright={p[2]:.4f}  "
            f"=> {CLASS_NAMES[pred]}"
        )


def main():
    print("========================================")
    print("LDR TinyML Training Pipeline")
    print("TensorFlow -> TFLite -> Arduino C Array")
    print("========================================")
    print("TensorFlow version:", tf.__version__)

    df = load_dataset()
    train_df, val_df, test_df = stratified_split(df)

    X_train, y_train = dataframe_to_xy(train_df)
    X_val, y_val = dataframe_to_xy(val_df)
    X_test, y_test = dataframe_to_xy(test_df)

    print("\nDataset split")
    print("Training  :", len(X_train))
    print("Validation:", len(X_val))
    print("Testing   :", len(X_test))

    model = build_model()
    print()
    model.summary()

    history = model.fit(
        X_train,
        y_train,
        validation_data=(X_val, y_val),
        epochs=EPOCHS,
        batch_size=BATCH_SIZE,
        verbose=1,
    )

    pd.DataFrame(history.history).to_csv(HISTORY_FILE, index=False)
    print(f"[INFO] Saved training history: {HISTORY_FILE}")

    test_loss, test_accuracy = model.evaluate(X_test, y_test, verbose=0)
    print(f"\nKeras test loss    : {test_loss:.6f}")
    print(f"Keras test accuracy: {test_accuracy * 100:.2f}%")

    model.save(KERAS_FILE)
    print(f"[INFO] Saved Keras model: {KERAS_FILE}")

    test_selected_values(model)
    convert_to_tflite(model, TFLITE_FILE)
    evaluate_tflite(TFLITE_FILE, X_test, y_test)
    export_c_array(TFLITE_FILE, HEADER_FILE)

    print("\nGenerated files")
    print("---------------")
    print(KERAS_FILE)
    print(TFLITE_FILE)
    print(HEADER_FILE)
    print(HISTORY_FILE)

    print("\nClass order used by Arduino:")
    for i, name in enumerate(CLASS_NAMES):
        print(f"  {i} -> {name}")


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print("\n[ERROR]", exc)
        sys.exit(1)
