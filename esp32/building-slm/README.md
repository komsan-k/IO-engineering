# Building an SLM for an MCU

A **Small Language Model (SLM) for an MCU** is a highly compressed language-processing model designed to perform inference within limited Flash, RAM, power, and processing capacity.

For an ESP32-class MCU, the most practical SLM is usually a model for:

- Intent classification
- Keyword spotting
- Command extraction
- Small-vocabulary text classification
- Sensor-query interpretation
- Template-based response generation

A general conversational model is usually too large for an MCU. TensorFlow Lite for Microcontrollers is specifically designed to execute compact machine-learning models on microcontrollers and other memory-constrained devices.

## 1. Select the SLM Type

| Model type | MCU feasibility | Recommended purpose |
|---|---:|---|
| Keyword/rule model | Excellent | Simple commands |
| Neural intent classifier | Excellent | Natural-language device control |
| Character-level RNN/GRU | Moderate | Short constrained text generation |
| Tiny Transformer | Difficult | Experimental language generation |
| General conversational SLM | Impractical | Use an edge computer |

For a first implementation, use an **Int8 neural intent classifier**.

## 2. Recommended Architecture

```mermaid
flowchart LR
    A["Text or Voice Input"] --> B["Preprocessing"]
    B --> C["Tokenizer"]
    C --> D["Int8 SLM on MCU"]
    D --> E["Intent and Parameters"]
    E --> F["Safety Validation"]
    F --> G["MCU Control"]
    G --> H["Sensors and Actuators"]
    H --> I["Response Template"]
```

## 3. Hardware Platform

A suitable experimental platform is:

- ESP32-S3 with 8 MB PSRAM
- External Flash or microSD, if required
- USB serial interface
- Environmental sensor
- LED, relay, servo, or motor
- Optional microphone for voice commands

Other possible targets include STM32H7, ESP32-P4, Arduino Portenta H7, and higher-memory Cortex-M7 MCUs.

## 4. Define the Application

Do not begin by training a general chatbot. Define a small set of supported functions:

```text
LIGHT_ON
LIGHT_OFF
FAN_ON
FAN_OFF
READ_TEMPERATURE
READ_HUMIDITY
SET_FAN_SPEED
UNKNOWN
```

A useful model should produce structured results:

```json
{
  "intent": "SET_FAN_SPEED",
  "value": 60,
  "confidence": 0.94
}
```

## 5. Prepare the Dataset

```csv
text,intent,value
turn on the light,LIGHT_ON,
switch the lamp off,LIGHT_OFF,
start the fan,FAN_ON,
set the fan to 60 percent,SET_FAN_SPEED,60
what is the temperature,READ_TEMPERATURE,
measure the humidity,READ_HUMIDITY,
```

Include different sentence structures, abbreviations, typing errors, and expressions for each intent.

A reasonable laboratory dataset could contain:

- 7–10 intent classes
- 100–500 examples per class
- Separate training, validation, and test sets
- An `UNKNOWN` class containing unsupported commands

## 6. Use a Small Tokenizer

Large subword tokenizers consume too much memory. Suitable MCU representations include:

- Keyword vectors
- Bag-of-Words
- Feature hashing
- Character-level tokens
- A vocabulary of 128–1,000 words
- A fixed input length of 16–32 tokens

Example:

```text
Vocabulary = [turn, on, off, light, fan, read, temperature]

"turn on light"
→ [1, 1, 0, 1, 0, 0, 0]
```

The tokenizer implemented on the MCU must produce exactly the same representation used during training.

## 7. Build a Compact Model

```python
import tensorflow as tf

VOCAB_SIZE = 256
NUM_CLASSES = 8

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(VOCAB_SIZE,)),
    tf.keras.layers.Dense(32, activation="relu"),
    tf.keras.layers.Dense(16, activation="relu"),
    tf.keras.layers.Dense(NUM_CLASSES, activation="softmax")
])

model.compile(
    optimizer="adam",
    loss="sparse_categorical_crossentropy",
    metrics=["accuracy"]
)

model.fit(
    x_train,
    y_train,
    validation_data=(x_validation, y_validation),
    epochs=50,
    batch_size=32
)
```

This is technically a compact language-understanding model rather than a generative LLM.

## 8. Quantize the Model

Int8 quantization reduces model storage and generally lowers inference memory and computation requirements.

```python
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]

def representative_dataset():
    for sample in x_train[:200]:
        yield [sample.reshape(1, -1).astype("float32")]

converter.representative_dataset = representative_dataset
converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

quantized_model = converter.convert()

with open("mcu_slm_int8.tflite", "wb") as file:
    file.write(quantized_model)
```

## 9. Convert the Model for Firmware

Convert the model into a C array:

```bash
xxd -i mcu_slm_int8.tflite > mcu_slm_int8.h
```

Include the header in the MCU project:

```cpp
#include "mcu_slm_int8.h"
```

Alternatively, supported platforms can load the model from Flash or microSD.

## 10. Perform MCU Inference

```cpp
String command = receiveCommand();

normalizeText(command);
createInputVector(command, inputTensor);

if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Inference failed");
    return;
}

int predictedIntent = findMaximumOutput(outputTensor);
float confidence = getConfidence(outputTensor);

if (confidence < 0.80) {
    predictedIntent = UNKNOWN;
}

executeValidatedIntent(predictedIntent);
```

A confidence threshold prevents uncertain commands from controlling hardware.

## 11. Generate Responses

Use deterministic response templates:

```cpp
String generateResponse(int intent, float temperature) {
    switch (intent) {
        case LIGHT_ON:
            return "The light has been turned on.";

        case READ_TEMPERATURE:
            return "The temperature is " +
                   String(temperature, 1) +
                   " degrees Celsius.";

        default:
            return "The command was not recognized.";
    }
}
```

This provides language-like interaction without running an expensive generative decoder.

## 12. Memory Estimation

Model-weight storage can be estimated as:

$$
M_{\mathrm{weights}} = \frac{Pq}{8},
$$

where $P$ is the number of parameters and $q$ is the number of bits per weight.

For a 500,000-parameter Int8 model:

$$
M_{\mathrm{weights}}
=
\frac{500{,}000 \times 8}{8}
=
500{,}000\ \text{bytes}
\approx
488\ \text{KiB}.
$$

Additional memory is required for:

- Tensor arena
- Intermediate activations
- Tokenizer vocabulary
- Input and output buffers
- Firmware and communication stacks

Therefore, parameter size alone does not determine whether the model fits.

## 13. Evaluation

| Category | Metrics |
|---|---|
| Model quality | Accuracy, precision, recall, and F1-score |
| Unknown detection | False-acceptance and false-rejection rates |
| Memory | Flash, static RAM, and peak tensor-arena use |
| Performance | Inference latency and commands per second |
| Energy | Energy consumed per inference |
| Reliability | Performance under noise and typing variations |
| Safety | Invalid commands rejected before actuation |

## 14. Hybrid Alternative

If unrestricted conversation is required, run the SLM on a Raspberry Pi or local computer and use the MCU for deterministic control:

```mermaid
flowchart LR
    A["User"] --> B["Local SLM"]
    B --> C["Structured JSON"]
    C --> D["MQTT or HTTP"]
    D --> E["ESP32"]
    E --> F["Safety Rules"]
    F --> G["Sensors and Actuators"]
```

The MCU should validate every generated command before executing it.

## Recommended Research Direction

A suitable project title is:

> **Development of a Quantized Small Language Model for Natural-Language Control of MCU-Based Intelligent Objects**

The research can compare:

- Keyword-based control
- Neural intent classification
- Character-level SLM
- Tiny Transformer
- Hybrid edge-SLM architecture

The principal contribution would be the joint optimization of **language accuracy, model size, inference latency, energy consumption, and safe physical control**.

## References

1. TensorFlow, “TensorFlow Lite for Microcontrollers.” Available: [https://github.com/tensorflow/tflite-micro](https://github.com/tensorflow/tflite-micro)
2. Espressif Systems, “ESP-VISION: AI Inference.” Available: [https://docs.espressif.com/projects/esp-vision/en/latest/esp32p4/concepts/ai-inference.html](https://docs.espressif.com/projects/esp-vision/en/latest/esp32p4/concepts/ai-inference.html)
