#!/usr/bin/env python3
"""Train MPU6050_Synthetic_1000.csv and generate an ESP32 TFLM model.h.

Install (Python 3.10-3.12 suggested):
    python -m pip install tensorflow numpy
Run:
    python train_mpu6050.py --csv MPU6050_Synthetic_1000.csv --out trained_model
Validate input and sequence splits without TensorFlow:
    python train_mpu6050.py --csv MPU6050_Synthetic_1000.csv --check-data

Outputs: model.h, model_int8.tflite, model.keras, training_report.json,
         split_assignments.csv.
Six instantaneous inputs, in order: acc_x_g, acc_y_g, acc_z_g,
gyro_x_dps, gyro_y_dps, gyro_z_dps. Labels 0/1/2: stationary/moving/shaking.
The supplied data are SYNTHETIC. Test scores measure simulated-data performance.
This is a per-reading baseline, not a temporal motion recognition model.
Use independent real recordings and window features for practical validation.

Arduino usage after allocating TFLM tensors:
    const tflite::Model* model = tflite::GetModel(model_data);
    float raw[6] = {ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps};
    mpu6050_prepare_input(raw, interpreter.input(0)->data.int8);
    // Call interpreter.Invoke() and check kTfLiteOk.
    // Find the maximum of interpreter.output(0)->data.int8[0..2].
    // Convert a score using mpu6050_output_value(value).
The header standardizes and quantizes inputs. Do not normalize them twice.
Required TFLM operators: FullyConnected and Softmax (ReLU is fused).
Verify operator support and tensor arena allocation on your chosen TFLM port.
"""

import argparse
import csv
import json
from pathlib import Path
import numpy as np

FEATURES = ['acc_x_g', 'acc_y_g', 'acc_z_g',
            'gyro_x_dps', 'gyro_y_dps', 'gyro_z_dps']
CLASSES = ['stationary', 'moving', 'shaking']


def load_and_split(path, seed):
    with path.open(newline='', encoding='utf-8-sig') as f:
        reader = csv.DictReader(f)
        required = set(FEATURES + ['sequence_id', 'label'])
        missing = required - set(reader.fieldnames or [])
        if missing:
            raise ValueError(f'Missing columns: {sorted(missing)}')
        records = list(reader)
    if not records:
        raise ValueError('Empty CSV')
    x = np.asarray([[float(r[k]) for k in FEATURES] for r in records], dtype=np.float32)
    if not np.isfinite(x).all():
        raise ValueError('Sensor inputs contain NaN or infinity')
    if any(r['label'] not in CLASSES for r in records):
        raise ValueError('Unrecognized label')
    y = np.asarray([CLASSES.index(r['label']) for r in records], dtype=np.int32)
    groups = np.asarray([r['sequence_id'] for r in records])
    if any(not str(g).strip() for g in groups):
        raise ValueError('Empty sequence_id')
    assignment = {}
    rng = np.random.default_rng(seed)
    for label_id in range(3):
        ids = np.unique(groups[y == label_id])
        if len(ids) < 5:
            raise ValueError('Each class needs at least five independent sequences')
        rng.shuffle(ids)
        n_test = max(1, int(round(len(ids) * .2)))
        n_val = max(1, int(round(len(ids) * .2)))
        for i, group in enumerate(ids):
            if np.unique(y[groups == group]).size != 1:
                raise ValueError(f'Sequence {group} has mixed labels')
            assignment[str(group)] = ('test' if i < n_test else
                                      'validation' if i < n_test + n_val else 'train')
    splits = {name: np.flatnonzero([assignment[str(g)] == name for g in groups])
              for name in ['train', 'validation', 'test']}
    for name, indices in splits.items():
        if len(np.unique(y[indices])) != 3:
            raise ValueError(f'{name} lacks a class')
    return x, y, splits, assignment


def write_header(path, model_bytes, mean, std, inp, out):
    def floats(values):
        return ', '.join(f'{float(v):.9e}f' for v in values)
    input_scale, input_zero = inp['quantization']
    output_scale, output_zero = out['quantization']
    lines = [
        '// Generated from synthetic MPU6050 data. Real-sensor validation required.',
        '// Features: ' + ', '.join(FEATURES),
        '// Input: standardized six features, int8. Output: three int8 class scores.',
        '// Classes: 0=stationary, 1=moving, 2=shaking.',
        '#pragma once', '#include <stdint.h>', '#include <math.h>', '',
        'alignas(16) const unsigned char model_data[] = {',
    ]
    for i in range(0, len(model_bytes), 12):
        lines.append('    ' + ', '.join(f'0x{b:02X}' for b in model_bytes[i:i+12]) + ',')
    lines += [
        '};', 'const unsigned int model_data_len = sizeof(model_data);', '',
        'const float mpu6050_mean[6] = {' + floats(mean) + '};',
        'const float mpu6050_std[6] = {' + floats(std) + '};',
        f'const float mpu6050_input_scale = {input_scale:.9e}f;',
        f'const int mpu6050_input_zero_point = {input_zero};',
        f'const float mpu6050_output_scale = {output_scale:.9e}f;',
        f'const int mpu6050_output_zero_point = {output_zero};',
        'const char* const mpu6050_labels[3] = {"stationary", "moving", "shaking"};', '',
        '// raw: acceleration in g, angular velocity in degrees/second.',
        'inline void mpu6050_prepare_input(const float raw[6], int8_t input[6]) {',
        '    for (int i = 0; i < 6; ++i) {',
        '        float z = (raw[i] - mpu6050_mean[i]) / mpu6050_std[i];',
        '        float q = roundf(z / mpu6050_input_scale) + mpu6050_input_zero_point;',
        '        q = fmaxf(-128.0f, fminf(127.0f, q));',
        '        input[i] = static_cast<int8_t>(q);',
        '    }', '}', '',
        'inline float mpu6050_output_value(int8_t value) {',
        '    return (static_cast<int>(value) - mpu6050_output_zero_point)',
        '           * mpu6050_output_scale;', '}', '',
    ]
    path.write_text('\n'.join(lines), encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--csv', type=Path, default=Path('MPU6050_Synthetic_1000.csv'))
    parser.add_argument('--out', type=Path, default=Path('trained_model'))
    parser.add_argument('--epochs', type=int, default=150)
    parser.add_argument('--seed', type=int, default=42)
    parser.add_argument('--check-data', action='store_true')
    args = parser.parse_args()
    if args.epochs < 1:
        parser.error('--epochs must be positive')
    x, y, split, assignment = load_and_split(args.csv, args.seed)
    print('Rows:', len(x), 'Class counts:', dict(zip(CLASSES, np.bincount(y).tolist())))
    print('Sequence-disjoint split:', {k: len(v) for k, v in split.items()})
    if args.check_data:
        print('Input validation passed. TensorFlow was not imported.')
        return
    try:
        import tensorflow as tf
    except ImportError as exc:
        raise SystemExit('Install dependencies: python -m pip install tensorflow numpy') from exc
    tf.keras.utils.set_random_seed(args.seed)
    train, val, test = (split[k] for k in ['train', 'validation', 'test'])
    mean = x[train].mean(axis=0)
    std = np.maximum(x[train].std(axis=0), 1e-6)
    z = ((x - mean) / std).astype(np.float32)
    model = tf.keras.Sequential([
        tf.keras.Input(shape=(6,), name='sensor_features'),
        tf.keras.layers.Dense(24, activation='relu'),
        tf.keras.layers.Dense(16, activation='relu'),
        tf.keras.layers.Dense(3, activation='softmax', name='class_scores'),
    ])
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=.001),
                  loss='sparse_categorical_crossentropy', metrics=['accuracy'])
    history = model.fit(z[train], y[train], validation_data=(z[val], y[val]),
                        epochs=args.epochs, batch_size=32, verbose=2,
                        callbacks=[tf.keras.callbacks.EarlyStopping(
                            monitor='val_loss', patience=20, restore_best_weights=True)])
    args.out.mkdir(parents=True, exist_ok=True)
    model.save(args.out / 'model.keras')
    float_probs = model.predict(z[test], verbose=0)
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    def representative_data():
        # Only training data calibrate INT8 ranges. Keep all three classes.
        for row in z[train]:
            yield [row.reshape(1, 6)]
    converter.representative_dataset = representative_data
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    model_bytes = converter.convert()
    (args.out / 'model_int8.tflite').write_bytes(model_bytes)
    interpreter = tf.lite.Interpreter(model_content=model_bytes)
    interpreter.allocate_tensors()
    inp, out = interpreter.get_input_details()[0], interpreter.get_output_details()[0]
    if inp['dtype'] != np.int8 or out['dtype'] != np.int8:
        raise RuntimeError('Conversion did not produce INT8 input/output')
    input_scale, input_zero = inp['quantization']
    if input_scale <= 0 or out['quantization'][0] <= 0:
        raise RuntimeError('Invalid quantization parameters')
    predictions = []
    for row in z[test]:
        q = np.clip(np.round(row / input_scale) + input_zero, -128, 127).astype(np.int8)
        interpreter.set_tensor(inp['index'], q.reshape(1, 6))
        interpreter.invoke()
        predictions.append(int(np.argmax(interpreter.get_tensor(out['index'])[0])))
    predictions = np.asarray(predictions)
    confusion = np.zeros((3, 3), dtype=int)
    np.add.at(confusion, (y[test], predictions), 1)
    write_header(args.out / 'model.h', model_bytes, mean, std, inp, out)
    report = {
        'data_source': 'Synthetic MPU6050 demonstration data; not real sensor validation',
        'tensorflow_version': tf.__version__, 'seed': args.seed,
        'features': FEATURES, 'classes': CLASSES, 'row_count': len(x),
        'split_method': 'Per-class shuffled sequences, approximately 60/20/20',
        'split_rows': {k: len(v) for k, v in split.items()},
        'sequence_assignments': assignment,
        'normalization_train_mean': mean.tolist(), 'normalization_train_std': std.tolist(),
        'epochs_run': len(history.history['loss']),
        'keras_test_accuracy': float(np.mean(np.argmax(float_probs, axis=1) == y[test])),
        'int8_test_accuracy': float(np.mean(predictions == y[test])),
        'confusion_matrix_true_rows_predicted_columns': confusion.tolist(),
        'model_bytes': len(model_bytes),
        'input_quantization': [float(input_scale), int(input_zero)],
        'output_quantization': [float(out['quantization'][0]), int(out['quantization'][1])],
        'history': {k: [float(v) for v in values] for k, values in history.history.items()},
        'limitations': ['Per-reading model does not learn temporal motion patterns.',
                       'Synthetic classes may be unrealistically separable.',
                       'Desktop TFLite verification does not validate ESP32 TFLM compatibility.',
                       'Tensor arena size must be measured on the selected ESP32 runtime.'],
    }
    (args.out / 'training_report.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    with (args.out / 'split_assignments.csv').open('w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['sequence_id', 'split'])
        writer.writerows(sorted(assignment.items()))
    print('INT8 test accuracy:', report['int8_test_accuracy'])
    print('Confusion matrix (true rows, predicted columns):\n', confusion)
    print('Generated:', args.out.resolve() / 'model.h')


if __name__ == '__main__':
    main()
