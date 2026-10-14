# LAB — TinyML Light Classification using Edge Impulse
## Dark, Normal, and Bright Classification with ESP32 + LDR

## 1. Objective

The objective of this lab is to:

1. Read ambient-light data from an LDR using the ESP32 ADC.
2. Collect labeled light data for three classes:
   - **Bright**
   - **Normal**
   - **Dark**
3. Upload the sensor data to **Edge Impulse Studio**.
4. Build a TinyML classification model.
5. Train and evaluate the model using training and test data.
6. Examine the confusion matrix and classification accuracy.
7. Build a quantized INT8 deployment.
8. Export the trained model as an Arduino library.
9. Deploy the TinyML model on the ESP32.
10. Perform real-time light classification.

---

## 2. Learning Outcomes

After completing this lab, students should be able to:

- explain the basic TinyML workflow;
- collect sensor data from an ESP32;
- prepare labeled datasets;
- use Edge Impulse Studio;
- train a three-class classifier;
- evaluate a trained model;
- explain INT8 quantization;
- deploy an Edge Impulse model as an Arduino library;
- perform inference directly on an ESP32.

---

## 3. System Architecture

```text
Ambient Light
     ↓
    LDR
     ↓
ESP32 ADC
     ↓
Data Collection
     ↓
Edge Impulse
     ↓
Model Training
     ↓
INT8 Quantization
     ↓
Arduino Library
     ↓
ESP32
     ↓
TinyML Inference
     ↓
Bright / Normal / Dark
```

---

## 4. Hardware Requirements

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable
- Optional LEDs

Suggested LED outputs:

```text
GPIO 16 → Bright
GPIO 17 → Normal
GPIO 18 → Dark
```

---

## 5. Software Requirements

- Arduino IDE
- Edge Impulse account
- Edge Impulse Studio
- Edge Impulse CLI, optional
- USB serial driver for ESP32

---

## 6. LDR Circuit

Use a voltage divider:

```text
3.3 V
  |
 LDR
  |
  +------ GPIO 36
  |
10 kΩ
  |
 GND
```

For this lab, assume:

```text
Bright : 0–99
Normal : 100–499
Dark   : 500–1000
```

These ranges are teaching examples and should be calibrated for the actual circuit.

---

## 7. Arduino Data-Collection Code

```cpp
#include <Arduino.h>

#define LDR_PIN 36

String classifyLight(int value)
{
  if (value < 100)
  {
    return "bright";
  }
  else if (value >= 100 && value < 500)
  {
    return "normal";
  }
  else
  {
    return "dark";
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);
  Serial.println("light_value,label");
}

void loop()
{
  int value = analogRead(LDR_PIN);
  String label = classifyLight(value);

  Serial.print(value);
  Serial.print(",");
  Serial.println(label);

  delay(100);
}
```

Expected output:

```text
light_value,label
42,bright
68,bright
217,normal
384,normal
650,dark
812,dark
```

---

## 8. Dataset Collection

Collect approximately:

```text
Bright = 200 samples
Normal = 200 samples
Dark   = 200 samples
```

Total:

```text
600 samples
```

Example:

```csv
light_value,label
38,bright
72,bright
215,normal
425,normal
645,dark
830,dark
```

---

## 9. Create an Edge Impulse Project

Suggested project name:

```text
ESP32-LDR-Light-Classification
```

Workflow:

```text
Data Acquisition
      ↓
Create Impulse
      ↓
Generate Features
      ↓
Train Model
      ↓
Model Testing
      ↓
Deployment
```

---

## 10. Upload the Dataset

Use:

```text
light_value
```

as the input column and:

```text
label
```

as the class.

Classes:

```text
bright
normal
dark
```

---

## 11. Training and Testing Split

Suggested split:

```text
Training = 80%
Testing  = 20%
```

For 600 samples:

```text
Training ≈ 480 samples
Testing  ≈ 120 samples
```

---

## 12. Create the Impulse

A basic architecture is:

```text
LDR ADC
   ↓
Normalization / Processing
   ↓
Neural Network
   ↓
Bright
Normal
Dark
```

---

## 13. Optional Feature Extraction

Instead of using one ADC reading, collect a short window and calculate:

```text
Mean
Standard Deviation
Minimum
Maximum
```

The TinyML input becomes:

```text
[mean, std, min, max]
```

Architecture:

```text
20 LDR Samples
      ↓
Feature Extraction
      ↓
Mean
Std
Min
Max
      ↓
Neural Network
      ↓
Bright / Normal / Dark
```

---

## 14. Example Feature Extraction

For:

```text
120
125
118
130
122
```

the extracted features are approximately:

```text
Mean = 123
Std  = 4.1
Min  = 118
Max  = 130
```

Input vector:

```text
[123, 4.1, 118, 130]
```

---

## 15. Neural Network

For four features:

```text
4 Inputs
   ↓
8 Hidden Neurons
   ↓
3 Outputs
```

Outputs:

```text
Output 0 → Bright
Output 1 → Normal
Output 2 → Dark
```

---

## 16. Training

Observe:

```text
Training Loss
Training Accuracy
Validation Accuracy
Confusion Matrix
```

---

## 17. Confusion Matrix

| Actual \ Predicted | Bright | Normal | Dark |
|---|---:|---:|---:|
| Bright | 38 | 2 | 0 |
| Normal | 1 | 37 | 2 |
| Dark | 0 | 1 | 39 |

Accuracy:

```text
Accuracy = Correct Predictions / Total Predictions × 100%
```

---

## 18. Model Testing

Record:

```text
Accuracy
RAM Usage
Flash Usage
Inference Time
```

---

## 19. Float32 vs INT8

```text
Float32 → 32 bits per parameter
INT8    → 8 bits per parameter
```

INT8 models are generally more suitable for resource-constrained microcontrollers.

---

## 20. Deploy the Model

Navigate to:

```text
Deployment
```

Select:

```text
Arduino Library
```

Then build and download the library.

---

## 21. Install the Library in Arduino IDE

```text
Sketch
 ↓
Include Library
 ↓
Add .ZIP Library
```

---

## 22. Edge Impulse Inference Structure

```text
Sensor Data
    ↓
Input Buffer
    ↓
Signal
    ↓
run_classifier()
    ↓
Classification Result
```

---

## 23. Example Arduino Inference Code

```cpp
#include <Arduino.h>
#include <your_project_inferencing.h>

#define LDR_PIN 36

void setup()
{
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);
}

void loop()
{
  float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

  int adcValue = analogRead(LDR_PIN);

  features[0] = (float)adcValue;

  signal_t signal;

  numpy::signal_from_buffer(
    features,
    EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE,
    &signal
  );

  ei_impulse_result_t result = {0};

  EI_IMPULSE_ERROR status =
    run_classifier(
      &signal,
      &result,
      false
    );

  if (status != EI_IMPULSE_OK)
  {
    Serial.println("Inference failed");
    return;
  }

  for (size_t i = 0;
       i < EI_CLASSIFIER_LABEL_COUNT;
       i++)
  {
    Serial.print(result.classification[i].label);
    Serial.print(": ");
    Serial.println(
      result.classification[i].value,
      4
    );
  }

  delay(500);
}
```

---

## 24. Feature-Based Inference

For a feature model, first collect a window:

```cpp
#define WINDOW_SIZE 20

float samples[WINDOW_SIZE];
```

---

## 25. Feature Extraction Code

```cpp
void extractFeatures(
  float samples[],
  int n,
  float features[]
)
{
  float sum = 0.0f;
  float minValue = samples[0];
  float maxValue = samples[0];

  for (int i = 0; i < n; i++)
  {
    sum += samples[i];

    if (samples[i] < minValue)
      minValue = samples[i];

    if (samples[i] > maxValue)
      maxValue = samples[i];
  }

  float mean = sum / n;

  float variance = 0.0f;

  for (int i = 0; i < n; i++)
  {
    float d = samples[i] - mean;
    variance += d * d;
  }

  variance /= n;

  float stdValue = sqrtf(variance);

  features[0] = mean;
  features[1] = stdValue;
  features[2] = minValue;
  features[3] = maxValue;
}
```

---

## 26. Classification Result

Example:

```text
bright: 0.012
normal: 0.936
dark:   0.052
```

Prediction:

```text
Normal
```

---

## 27. Optional LED Output

```cpp
#define LED_BRIGHT 16
#define LED_NORMAL 17
#define LED_DARK   18
```

---

## 28. Experiment 1 — Collect LDR Data

| Class | Minimum | Maximum | Samples |
|---|---:|---:|---:|
| Bright |  |  | 200 |
| Normal |  |  | 200 |
| Dark |  |  | 200 |

---

## 29. Experiment 2 — Train Edge Impulse Model

| Metric | Result |
|---|---:|
| Training accuracy | |
| Validation accuracy | |
| Test accuracy | |
| Loss | |

---

## 30. Experiment 3 — Confusion Matrix

| Actual \ Predicted | Bright | Normal | Dark |
|---|---:|---:|---:|
| Bright | | | |
| Normal | | | |
| Dark | | | |

---

## 31. Experiment 4 — Float32 vs INT8

| Model | Accuracy | Flash | RAM | Inference Time |
|---|---:|---:|---:|---:|
| Float32 | | | | |
| INT8 | | | | |

---

## 32. Experiment 5 — Real-Time Classification

Test at least:

```text
10 Bright conditions
10 Normal conditions
10 Dark conditions
```

---

## 33. Experiment 6 — Threshold vs TinyML

Threshold rule:

```cpp
if (value < 100)
    bright;
else if (value >= 100 && value < 500)
    normal;
else
    dark;
```

Compare:

| Method | Accuracy | Training Required | Complexity |
|---|---:|---|---|
| Threshold | | No | Low |
| Edge Impulse ANN | | Yes | Medium |

---

## 34. Experiment 7 — Add Noise

Introduce:

```text
Sensor noise
Temporary shadow
Light flicker
Small movement
```

Compare classification stability.

---

## 35. Experiment 8 — Feature-Based TinyML

### Model A

```text
Raw ADC
   ↓
TinyML
```

### Model B

```text
20 ADC Samples
      ↓
Mean / Std / Min / Max
      ↓
TinyML
```

| Model | Inputs | Accuracy | Stability |
|---|---:|---:|---|
| Raw LDR | 1 | | |
| Feature LDR | 4 | | |

---

## 36. Checkpoint Questions

1. What is Edge Impulse?
2. Why are labels required during model training?
3. Why should the training dataset be balanced?
4. What is the difference between training and testing data?
5. What is an impulse in Edge Impulse?
6. What does a confusion matrix show?
7. Why is INT8 useful for microcontrollers?
8. What is the difference between threshold classification and TinyML classification?
9. Why must preprocessing on the ESP32 match preprocessing used during training?
10. Why can window-based feature extraction improve sensor classification?

---

## 37. Assignment

Develop an ESP32 TinyML light classifier that outputs:

```text
Bright
Normal
Dark
```

The system must include:

```text
LDR
 ↓
ESP32
 ↓
Data Collection
 ↓
Edge Impulse Training
 ↓
INT8 Model
 ↓
ESP32 Deployment
 ↓
Real-Time Classification
```

Students should test at least 30 new samples.

---

## 38. Submission

Submit:

- Arduino data-collection code
- LDR dataset
- Screenshot of Edge Impulse dataset
- Impulse configuration
- Neural-network configuration
- Training results
- Confusion matrix
- Test accuracy
- Float32/INT8 comparison
- Exported Arduino library
- Final ESP32 inference code
- Serial Monitor output
- Short discussion

---

## 39. Expected Result

Example:

```text
ADC: 48
Prediction: Bright
Confidence: 0.97

ADC: 276
Prediction: Normal
Confidence: 0.94

ADC: 728
Prediction: Dark
Confidence: 0.98
```

---

## 40. Conclusion

This lab introduces an end-to-end TinyML workflow:

```text
Sensor
 ↓
Dataset
 ↓
Training
 ↓
Testing
 ↓
Quantization
 ↓
Deployment
 ↓
Inference
```

It demonstrates the progression from fixed threshold rules to learned models deployed directly on an ESP32.
