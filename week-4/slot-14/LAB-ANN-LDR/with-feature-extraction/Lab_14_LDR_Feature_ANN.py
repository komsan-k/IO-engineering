"""
LAB 14 — ANN Light Classification with LDR Feature Extraction
Dark / Normal / Bright
Plain NumPy implementation (no TensorFlow)

Pipeline:
Raw LDR CSV
    -> fixed windows
    -> feature extraction
       [mean, std, min, max, range]
    -> normalization
    -> ANN 5-4-3
    -> training
    -> test evaluation
    -> confusion matrix
    -> export weights/biases
    -> export Arduino-ready header

Expected raw CSV format:
timestamp,light_value,label
1000,425,dark
1100,438,dark
...
"""

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# ============================================================
# 1. Configuration
# ============================================================

RAW_CSV = "light_dataset.csv"
FEATURE_CSV = "ldr_feature_dataset.csv"

WINDOW_SIZE = 20

INPUT_SIZE = 5
HIDDEN_SIZE = 4
OUTPUT_SIZE = 3

LEARNING_RATE = 0.30
EPOCHS = 5000

TRAIN_RATIO = 0.80

RANDOM_SEED = 42

CLASS_NAMES = [
    "dark",
    "normal",
    "bright"
]

LABEL_MAP = {
    "dark": 0,
    "normal": 1,
    "bright": 2
}

FEATURE_NAMES = [
    "mean",
    "std",
    "min",
    "max",
    "range"
]

ADC_MAX = 4095.0


# ============================================================
# 2. Optional Mock LDR Dataset
# ============================================================

def create_mock_dataset(
    filename,
    samples_per_class=600
):
    """
    Create a mock LDR dataset when RAW_CSV does not exist.

    Dark   ~ 500
    Normal ~ 1700
    Bright ~ 3300
    """

    rng = np.random.default_rng(
        RANDOM_SEED
    )

    dark = rng.normal(
        loc=500,
        scale=160,
        size=samples_per_class
    )

    normal = rng.normal(
        loc=1700,
        scale=260,
        size=samples_per_class
    )

    bright = rng.normal(
        loc=3300,
        scale=220,
        size=samples_per_class
    )

    dark = np.clip(
        dark,
        0,
        4095
    )

    normal = np.clip(
        normal,
        0,
        4095
    )

    bright = np.clip(
        bright,
        0,
        4095
    )

    values = np.concatenate(
        [
            dark,
            normal,
            bright
        ]
    ).astype(
        np.float32
    )

    labels = (
        ["dark"] * samples_per_class
        +
        ["normal"] * samples_per_class
        +
        ["bright"] * samples_per_class
    )

    timestamps = np.arange(
        len(values)
    ) * 100

    df = pd.DataFrame(
        {
            "timestamp": timestamps,
            "light_value": values,
            "label": labels
        }
    )

    df.to_csv(
        filename,
        index=False
    )

    print(
        f"Mock dataset created: {filename}"
    )


# ============================================================
# 3. Load Raw Dataset
# ============================================================

def load_raw_dataset(
    filename
):
    if not os.path.exists(
        filename
    ):
        print(
            f"{filename} not found."
        )

        print(
            "Creating mock LDR dataset..."
        )

        create_mock_dataset(
            filename
        )

    df = pd.read_csv(
        filename
    )

    required_columns = {
        "light_value",
        "label"
    }

    if not required_columns.issubset(
        df.columns
    ):
        raise ValueError(
            "CSV must contain at least: "
            "light_value,label"
        )

    df["label"] = (
        df["label"]
        .astype(str)
        .str.strip()
        .str.lower()
    )

    df = df[
        df["label"].isin(
            CLASS_NAMES
        )
    ].copy()

    df["light_value"] = (
        pd.to_numeric(
            df["light_value"],
            errors="coerce"
        )
    )

    df = df.dropna(
        subset=[
            "light_value",
            "label"
        ]
    )

    df["light_value"] = (
        df["light_value"]
        .clip(
            0,
            4095
        )
        .astype(
            np.float32
        )
    )

    print(
        "\nRaw dataset class counts:"
    )

    print(
        df["label"].value_counts()
    )

    return df


# ============================================================
# 4. Feature Extraction
# ============================================================

def extract_features_from_window(
    window
):
    """
    Extract:
    mean
    standard deviation
    minimum
    maximum
    range
    """

    window = np.asarray(
        window,
        dtype=np.float32
    )

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
        max_value
        -
        min_value
    )

    return np.array(
        [
            mean_value,
            std_value,
            min_value,
            max_value,
            range_value
        ],
        dtype=np.float32
    )


def create_feature_dataset(
    raw_df,
    window_size=WINDOW_SIZE
):
    """
    Create non-overlapping windows separately
    for each class.
    """

    feature_rows = []

    for label in CLASS_NAMES:

        values = (
            raw_df[
                raw_df["label"] == label
            ]["light_value"]
            .values
            .astype(
                np.float32
            )
        )

        number_of_windows = (
            len(values)
            //
            window_size
        )

        for window_id in range(
            number_of_windows
        ):

            start = (
                window_id
                *
                window_size
            )

            end = (
                start
                +
                window_size
            )

            window = values[
                start:end
            ]

            features = (
                extract_features_from_window(
                    window
                )
            )

            feature_rows.append(
                {
                    "mean": features[0],
                    "std": features[1],
                    "min": features[2],
                    "max": features[3],
                    "range": features[4],
                    "label": label
                }
            )

    feature_df = pd.DataFrame(
        feature_rows
    )

    feature_df.to_csv(
        FEATURE_CSV,
        index=False
    )

    print(
        f"\nFeature dataset saved: {FEATURE_CSV}"
    )

    print(
        "\nFeature dataset class counts:"
    )

    print(
        feature_df["label"].value_counts()
    )

    print(
        "\nFirst five feature rows:"
    )

    print(
        feature_df.head()
    )

    return feature_df


# ============================================================
# 5. Prepare ANN Data
# ============================================================

def prepare_ann_data(
    feature_df
):
    X = (
        feature_df[
            FEATURE_NAMES
        ]
        .values
        .astype(
            np.float32
        )
    )

    # Normalize every feature to ADC scale
    X = (
        X
        /
        ADC_MAX
    ).astype(
        np.float32
    )

    y = (
        feature_df[
            "label"
        ]
        .map(
            LABEL_MAP
        )
        .values
        .astype(
            np.int64
        )
    )

    Y = np.eye(
        OUTPUT_SIZE,
        dtype=np.float32
    )[
        y
    ]

    return X, y, Y


# ============================================================
# 6. Stratified Train/Test Split Without sklearn
# ============================================================

def stratified_split(
    X,
    y,
    Y,
    train_ratio=TRAIN_RATIO
):
    rng = np.random.default_rng(
        RANDOM_SEED
    )

    train_indices = []
    test_indices = []

    for class_id in range(
        OUTPUT_SIZE
    ):

        idx = np.where(
            y == class_id
        )[0]

        rng.shuffle(
            idx
        )

        split = int(
            train_ratio
            *
            len(idx)
        )

        train_indices.extend(
            idx[:split]
        )

        test_indices.extend(
            idx[split:]
        )

    train_indices = np.array(
        train_indices
    )

    test_indices = np.array(
        test_indices
    )

    rng.shuffle(
        train_indices
    )

    rng.shuffle(
        test_indices
    )

    return (
        X[train_indices],
        X[test_indices],
        y[train_indices],
        y[test_indices],
        Y[train_indices],
        Y[test_indices]
    )


# ============================================================
# 7. Activation Functions
# ============================================================

def sigmoid(
    z
):
    z = np.clip(
        z,
        -20.0,
        20.0
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
    ).astype(
        np.float32
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
    ).astype(
        np.float32
    )


# ============================================================
# 8. ANN Initialization
# ============================================================

def initialize_ann():
    rng = np.random.default_rng(
        RANDOM_SEED
    )

    W1 = (
        rng.normal(
            0.0,
            0.5,
            size=(
                INPUT_SIZE,
                HIDDEN_SIZE
            )
        )
        .astype(
            np.float32
        )
    )

    b1 = np.zeros(
        (
            1,
            HIDDEN_SIZE
        ),
        dtype=np.float32
    )

    W2 = (
        rng.normal(
            0.0,
            0.5,
            size=(
                HIDDEN_SIZE,
                OUTPUT_SIZE
            )
        )
        .astype(
            np.float32
        )
    )

    b2 = np.zeros(
        (
            1,
            OUTPUT_SIZE
        ),
        dtype=np.float32
    )

    return W1, b1, W2, b2


# ============================================================
# 9. Forward Propagation
# ============================================================

def forward_pass(
    X,
    W1,
    b1,
    W2,
    b2
):
    Z1 = (
        X @ W1
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

    return Z1, A1, Z2, A2


# ============================================================
# 10. Train ANN
# ============================================================

def train_ann(
    X_train,
    y_train,
    Y_train
):
    W1, b1, W2, b2 = (
        initialize_ann()
    )

    loss_history = []
    accuracy_history = []

    for epoch in range(
        EPOCHS
    ):

        # ----------------------------
        # Forward
        # ----------------------------

        Z1, A1, Z2, A2 = (
            forward_pass(
                X_train,
                W1,
                b1,
                W2,
                b2
            )
        )

        # ----------------------------
        # Cross-Entropy Loss
        # ----------------------------

        loss = -np.mean(
            np.sum(
                Y_train
                *
                np.log(
                    A2
                    +
                    1e-8
                ),
                axis=1
            )
        )

        # ----------------------------
        # Accuracy
        # ----------------------------

        train_pred = np.argmax(
            A2,
            axis=1
        )

        train_accuracy = np.mean(
            train_pred
            ==
            y_train
        )

        loss_history.append(
            loss
        )

        accuracy_history.append(
            train_accuracy
        )

        # ----------------------------
        # Backpropagation
        # ----------------------------

        dZ2 = (
            A2
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

        # ----------------------------
        # Gradient Descent
        # ----------------------------

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

        if epoch % 500 == 0:

            print(
                f"Epoch {epoch:4d}"
                f" | Loss = {loss:.6f}"
                f" | Accuracy = {train_accuracy:.4f}"
            )

    return (
        W1,
        b1,
        W2,
        b2,
        np.array(
            loss_history
        ),
        np.array(
            accuracy_history
        )
    )


# ============================================================
# 11. Prediction
# ============================================================

def predict(
    X,
    W1,
    b1,
    W2,
    b2
):
    _, _, _, probabilities = (
        forward_pass(
            X,
            W1,
            b1,
            W2,
            b2
        )
    )

    prediction = np.argmax(
        probabilities,
        axis=1
    )

    return prediction, probabilities


# ============================================================
# 12. Confusion Matrix Without sklearn
# ============================================================

def confusion_matrix_plain(
    actual,
    predicted,
    num_classes=OUTPUT_SIZE
):
    cm = np.zeros(
        (
            num_classes,
            num_classes
        ),
        dtype=np.int32
    )

    for a, p in zip(
        actual,
        predicted
    ):
        cm[
            a,
            p
        ] += 1

    return cm


# ============================================================
# 13. Evaluate Model
# ============================================================

def evaluate_model(
    X_test,
    y_test,
    W1,
    b1,
    W2,
    b2
):
    predictions, probabilities = (
        predict(
            X_test,
            W1,
            b1,
            W2,
            b2
        )
    )

    accuracy = np.mean(
        predictions
        ==
        y_test
    )

    cm = confusion_matrix_plain(
        y_test,
        predictions
    )

    print(
        "\n=============================="
    )

    print(
        "Test Accuracy:",
        f"{accuracy * 100:.2f}%"
    )

    print(
        "=============================="
    )

    print(
        "\nConfusion Matrix:"
    )

    print(
        cm
    )

    return (
        accuracy,
        cm,
        predictions,
        probabilities
    )


# ============================================================
# 14. Plot Training History
# ============================================================

def plot_training_history(
    loss_history,
    accuracy_history
):
    plt.figure()

    plt.plot(
        loss_history
    )

    plt.xlabel(
        "Epoch"
    )

    plt.ylabel(
        "Loss"
    )

    plt.title(
        "ANN Training Loss"
    )

    plt.tight_layout()

    plt.savefig(
        "training_loss.png",
        dpi=150
    )

    plt.close()


    plt.figure()

    plt.plot(
        accuracy_history
    )

    plt.xlabel(
        "Epoch"
    )

    plt.ylabel(
        "Accuracy"
    )

    plt.title(
        "ANN Training Accuracy"
    )

    plt.tight_layout()

    plt.savefig(
        "training_accuracy.png",
        dpi=150
    )

    plt.close()


# ============================================================
# 15. Plot Feature Distributions
# ============================================================

def plot_feature_distributions(
    feature_df
):
    for feature in FEATURE_NAMES:

        plt.figure()

        for label in CLASS_NAMES:

            values = feature_df[
                feature_df["label"]
                ==
                label
            ][
                feature
            ].values

            plt.hist(
                values,
                bins=15,
                alpha=0.5,
                label=label
            )

        plt.xlabel(
            feature
        )

        plt.ylabel(
            "Count"
        )

        plt.title(
            f"{feature} distribution"
        )

        plt.legend()

        plt.tight_layout()

        plt.savefig(
            f"feature_{feature}.png",
            dpi=150
        )

        plt.close()


# ============================================================
# 16. Export Parameters
# ============================================================

def export_parameters(
    W1,
    b1,
    W2,
    b2
):
    np.savetxt(
        "W1.csv",
        W1,
        delimiter=","
    )

    np.savetxt(
        "b1.csv",
        b1,
        delimiter=","
    )

    np.savetxt(
        "W2.csv",
        W2,
        delimiter=","
    )

    np.savetxt(
        "b2.csv",
        b2,
        delimiter=","
    )

    print(
        "\nSaved parameter files:"
    )

    print(
        "W1.csv"
    )

    print(
        "b1.csv"
    )

    print(
        "W2.csv"
    )

    print(
        "b2.csv"
    )


# ============================================================
# 17. Export Arduino Header
# ============================================================

def array_to_cpp_1d(
    name,
    values
):
    values = np.asarray(
        values
    ).flatten()

    body = ", ".join(
        f"{float(v):.8f}f"
        for v in values
    )

    return (
        f"const float {name}[{len(values)}] = "
        "{"
        f"{body}"
        "};"
    )


def array_to_cpp_2d(
    name,
    matrix
):
    matrix = np.asarray(
        matrix
    )

    rows = []

    for row in matrix:

        body = ", ".join(
            f"{float(v):.8f}f"
            for v in row
        )

        rows.append(
            "  {"
            f"{body}"
            "}"
        )

    return (
        f"const float {name}"
        f"[{matrix.shape[0]}]"
        f"[{matrix.shape[1]}] = "
        "{\n"
        +
        ",\n".join(
            rows
        )
        +
        "\n};"
    )


def export_arduino_header(
    W1,
    b1,
    W2,
    b2
):
    header = []

    header.append(
        "#ifndef LDR_ANN_WEIGHTS_H"
    )

    header.append(
        "#define LDR_ANN_WEIGHTS_H"
    )

    header.append(
        ""
    )

    header.append(
        "#define ANN_INPUT_SIZE 5"
    )

    header.append(
        "#define ANN_HIDDEN_SIZE 4"
    )

    header.append(
        "#define ANN_OUTPUT_SIZE 3"
    )

    header.append(
        ""
    )

    header.append(
        array_to_cpp_2d(
            "W1",
            W1
        )
    )

    header.append(
        ""
    )

    header.append(
        array_to_cpp_1d(
            "b1",
            b1
        )
    )

    header.append(
        ""
    )

    header.append(
        array_to_cpp_2d(
            "W2",
            W2
        )
    )

    header.append(
        ""
    )

    header.append(
        array_to_cpp_1d(
            "b2",
            b2
        )
    )

    header.append(
        ""
    )

    header.append(
        "#endif"
    )

    with open(
        "ldr_ann_weights.h",
        "w",
        encoding="utf-8"
    ) as f:

        f.write(
            "\n".join(
                header
            )
        )

    print(
        "\nArduino header saved:"
    )

    print(
        "ldr_ann_weights.h"
    )


# ============================================================
# 18. Test Selected Feature Vectors
# ============================================================

def test_example_windows(
    W1,
    b1,
    W2,
    b2
):
    example_windows = {
        "dark": np.array(
            [
                420, 430, 410, 440, 425,
                415, 435, 422, 428, 418,
                430, 424, 419, 433, 421,
                427, 416, 438, 423, 429
            ],
            dtype=np.float32
        ),

        "normal": np.array(
            [
                1350, 1420, 1380, 1450, 1400,
                1375, 1430, 1395, 1410, 1360,
                1440, 1385, 1425, 1405, 1390,
                1435, 1370, 1415, 1380, 1420
            ],
            dtype=np.float32
        ),

        "bright": np.array(
            [
                3050, 3180, 3120, 3250, 3200,
                3090, 3220, 3150, 3280, 3170,
                3100, 3240, 3190, 3260, 3130,
                3210, 3160, 3270, 3110, 3230
            ],
            dtype=np.float32
        )
    }

    print(
        "\nExample Window Predictions:"
    )

    for expected, window in (
        example_windows.items()
    ):

        features = (
            extract_features_from_window(
                window
            )
        )

        normalized = (
            features
            /
            ADC_MAX
        ).reshape(
            1,
            -1
        )

        pred, probs = predict(
            normalized,
            W1,
            b1,
            W2,
            b2
        )

        predicted_name = (
            CLASS_NAMES[
                pred[0]
            ]
        )

        print(
            f"\nExpected: {expected}"
        )

        print(
            "Features:",
            np.round(
                features,
                3
            )
        )

        print(
            "Prediction:",
            predicted_name
        )

        print(
            "Probabilities:",
            np.round(
                probs[0],
                4
            )
        )


# ============================================================
# 19. Main
# ============================================================

def main():

    print(
        "LAB 14 — LDR Feature ANN"
    )

    print(
        "Architecture: 5-4-3"
    )

    print(
        "Classes: Dark / Normal / Bright"
    )

    # ----------------------------
    # Raw data
    # ----------------------------

    raw_df = load_raw_dataset(
        RAW_CSV
    )

    # ----------------------------
    # Feature extraction
    # ----------------------------

    feature_df = (
        create_feature_dataset(
            raw_df,
            WINDOW_SIZE
        )
    )

    # ----------------------------
    # Visualization
    # ----------------------------

    plot_feature_distributions(
        feature_df
    )

    # ----------------------------
    # ANN data
    # ----------------------------

    X, y, Y = prepare_ann_data(
        feature_df
    )

    (
        X_train,
        X_test,
        y_train,
        y_test,
        Y_train,
        Y_test
    ) = stratified_split(
        X,
        y,
        Y
    )

    print(
        "\nTraining windows:",
        len(
            X_train
        )
    )

    print(
        "Testing windows:",
        len(
            X_test
        )
    )

    # ----------------------------
    # Training
    # ----------------------------

    (
        W1,
        b1,
        W2,
        b2,
        loss_history,
        accuracy_history
    ) = train_ann(
        X_train,
        y_train,
        Y_train
    )

    # ----------------------------
    # Test
    # ----------------------------

    (
        test_accuracy,
        cm,
        predictions,
        probabilities
    ) = evaluate_model(
        X_test,
        y_test,
        W1,
        b1,
        W2,
        b2
    )

    # ----------------------------
    # Save plots
    # ----------------------------

    plot_training_history(
        loss_history,
        accuracy_history
    )

    # ----------------------------
    # Export
    # ----------------------------

    export_parameters(
        W1,
        b1,
        W2,
        b2
    )

    export_arduino_header(
        W1,
        b1,
        W2,
        b2
    )

    # ----------------------------
    # Print model
    # ----------------------------

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

    # ----------------------------
    # Test example windows
    # ----------------------------

    test_example_windows(
        W1,
        b1,
        W2,
        b2
    )

    print(
        "\nGenerated outputs:"
    )

    print(
        "- ldr_feature_dataset.csv"
    )

    print(
        "- W1.csv"
    )

    print(
        "- b1.csv"
    )

    print(
        "- W2.csv"
    )

    print(
        "- b2.csv"
    )

    print(
        "- ldr_ann_weights.h"
    )

    print(
        "- training_loss.png"
    )

    print(
        "- training_accuracy.png"
    )

    print(
        "- feature_mean.png"
    )

    print(
        "- feature_std.png"
    )

    print(
        "- feature_min.png"
    )

    print(
        "- feature_max.png"
    )

    print(
        "- feature_range.png"
    )


if __name__ == "__main__":
    main()
