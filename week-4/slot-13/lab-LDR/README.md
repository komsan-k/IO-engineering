# LAB 13 — LDR Light Data Collection & Classification

## 1. Objective

This lab modifies the original sensor-data and dataset-preparation workflow into a simple **LDR light-classification activity**. Students will:

1. Interface an LDR with an ESP32 ADC input.
2. Read ambient-light values.
3. Define three classes: **Dark, Normal, Bright**.
4. Select thresholds from measured data.
5. Perform real-time classification.
6. Collect labeled samples.
7. Export the dataset in CSV format.
8. Inspect class balance and data quality.

---

## 2. System Architecture

```text
Ambient Light
      ↓
     LDR
      ↓
Voltage Divider
      ↓
ESP32 ADC
      ↓
Light Value
      ↓
Classification
      ↓
Dark / Normal / Bright
```

For dataset preparation:

```text
LDR
 ↓
ESP32
 ↓
Sampling
 ↓
Timestamp
 ↓
Light Label
 ↓
CSV Dataset
 ↓
Train / Validation / Test
```

---

## 3. Hardware

- ESP32 development board
- LDR
- 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cable

### Example connection

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

> Depending on the voltage-divider orientation, ADC values may increase or decrease as illumination increases. Verify the behavior experimentally before defining thresholds.

---

## 4. Experiment 1 — Read the LDR

```cpp
#include <Arduino.h>

#define LDR_PIN 36

void setup()
{
  Serial.begin(115200);

  pinMode(
    LDR_PIN,
    INPUT
  );
}

void loop()
{
  int lightValue =
    analogRead(
      LDR_PIN
    );

  Serial.println(
    lightValue
  );

  delay(500);
}
```

A typical 12-bit ESP32 ADC value is:

```text
0 to 4095
```

---

## 5. Experiment 2 — Observe Three Light Conditions

Measure the LDR under:

```text
Dark
Normal
Bright
```

Example measurements:

| Condition | Example ADC Values |
|---|---|
| Dark | 350, 420, 470 |
| Normal | 1250, 1380, 1500 |
| Bright | 2800, 3100, 3450 |

These values are only examples. Use measurements from your own circuit.

---

## 6. Define Thresholds

Suppose the measured values suggest:

```text
Dark    : ADC < 1000
Normal  : 1000 ≤ ADC < 2500
Bright  : ADC ≥ 2500
```

Then:

```cpp
String classifyLight(
  int value
)
{
  if (value < 1000)
  {
    return "dark";
  }
  else if (value < 2500)
  {
    return "normal";
  }
  else
  {
    return "bright";
  }
}
```

Mathematically:

```text
x < T1          -> Dark
T1 <= x < T2    -> Normal
x >= T2         -> Bright
```

with:

```text
T1 = 1000
T2 = 2500
```

---

## 7. Complete Real-Time Classification Code

```cpp
#include <Arduino.h>

#define LDR_PIN 36

String classifyLight(
  int value
)
{
  if (value < 1000)
  {
    return "dark";
  }
  else if (value < 2500)
  {
    return "normal";
  }
  else
  {
    return "bright";
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(
    LDR_PIN,
    INPUT
  );
}

void loop()
{
  int lightValue =
    analogRead(
      LDR_PIN
    );

  String lightClass =
    classifyLight(
      lightValue
    );

  Serial.print(
    "Light Value: "
  );

  Serial.print(
    lightValue
  );

  Serial.print(
    " -> "
  );

  Serial.println(
    lightClass
  );

  delay(500);
}
```

### Expected output

```text
Light Value: 420 -> dark
Light Value: 1375 -> normal
Light Value: 3150 -> bright
```

---

## 8. Sampling Rate

For dataset collection, use:

```cpp
#define SAMPLE_INTERVAL_MS 100
```

This gives approximately:

```text
10 samples/second
```

because:

```text
100 ms = 0.1 s
1 / 0.1 = 10 Hz
```

---

## 9. Timestamped Dataset

Each sample should contain:

```text
timestamp,light_value,label
```

Example:

```text
1000,425,dark
1100,438,dark
1200,1320,normal
1300,3185,bright
```

---

## 10. Complete Dataset Collection Code

Change `lightLabel` before collecting each class.

```cpp
#include <Arduino.h>

#define LDR_PIN 36
#define SAMPLE_INTERVAL_MS 100

const char* lightLabel =
  "dark";

void setup()
{
  Serial.begin(115200);

  pinMode(
    LDR_PIN,
    INPUT
  );

  Serial.println(
    "timestamp,light_value,label"
  );

  delay(1000);
}

void loop()
{
  int lightValue =
    analogRead(
      LDR_PIN
    );

  unsigned long timestamp =
    millis();

  Serial.print(timestamp);
  Serial.print(",");

  Serial.print(lightValue);
  Serial.print(",");

  Serial.println(lightLabel);

  delay(
    SAMPLE_INTERVAL_MS
  );
}
```

For the next class, change:

```cpp
const char* lightLabel = "normal";
```

and then:

```cpp
const char* lightLabel = "bright";
```

---

## 11. Class Definitions

| Class ID | Condition | Label |
|---:|---|---|
| 0 | Dark | `dark` |
| 1 | Normal | `normal` |
| 2 | Bright | `bright` |

Use the same definitions consistently throughout data collection.

---

## 12. Dataset Size

For a simple introductory dataset, collect:

```text
Dark   = 500 samples
Normal = 500 samples
Bright = 500 samples
```

Total:

```text
1500 samples
```

A balanced dataset reduces class bias.

---

## 13. CSV Format

Example:

```csv
timestamp,light_value,label
1000,425,dark
1100,440,dark
1200,1325,normal
1300,1430,normal
1400,3100,bright
1500,3250,bright
```

---

## 14. Dataset Structure

```text
light_dataset/
│
├── dark/
│   ├── dark_01.csv
│   └── ...
│
├── normal/
│   ├── normal_01.csv
│   └── ...
│
└── bright/
    ├── bright_01.csv
    └── ...
```

---

## 15. Training, Validation, and Test Sets

A simple division is:

```text
Training   = 70%
Validation = 15%
Testing    = 15%
```

For 1500 samples:

```text
Training   ≈ 1050
Validation ≈ 225
Testing    ≈ 225
```

Avoid putting nearly identical consecutive samples from the same recording into both training and testing.

---

## 16. Experiment 3 — Visualize LDR Data

Use Serial Plotter or another plotting tool.

```text
ADC Value
   ▲
4095|             Bright
2500|---------------------
    |             Normal
1000|---------------------
    |             Dark
   0+--------------------► Time
```

Use the plot to verify whether the three classes are clearly separated.

---

## 17. Experiment 4 — Moving Average

A moving average can reduce noise.

```cpp
#define NUM_SAMPLES 10

int readAverageLight()
{
  long sum = 0;

  for (
    int i = 0;
    i < NUM_SAMPLES;
    i++
  )
  {
    sum += analogRead(
      LDR_PIN
    );

    delay(10);
  }

  return
    sum / NUM_SAMPLES;
}
```

Use:

```cpp
int lightValue =
  readAverageLight();
```

instead of a single `analogRead()`.

---

## 18. Complete Moving-Average Classifier

```cpp
#include <Arduino.h>

#define LDR_PIN 36
#define NUM_SAMPLES 10

int readAverageLight()
{
  long sum = 0;

  for (
    int i = 0;
    i < NUM_SAMPLES;
    i++
  )
  {
    sum += analogRead(
      LDR_PIN
    );

    delay(10);
  }

  return
    sum / NUM_SAMPLES;
}

String classifyLight(
  int value
)
{
  if (value < 1000)
  {
    return "dark";
  }
  else if (value < 2500)
  {
    return "normal";
  }
  else
  {
    return "bright";
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(
    LDR_PIN,
    INPUT
  );
}

void loop()
{
  int lightValue =
    readAverageLight();

  String lightClass =
    classifyLight(
      lightValue
    );

  Serial.print(lightValue);
  Serial.print(",");
  Serial.println(lightClass);

  delay(500);
}
```

---

## 19. Dataset Quality Inspection

Check for:

- missing measurements
- incorrect labels
- ADC saturation
- unstable illumination
- strong class overlap
- duplicate samples
- class imbalance
- sensor noise

Suggested table:

| Class | Samples | Minimum | Maximum | Mean | Valid |
|---|---:|---:|---:|---:|---|
| Dark |  |  |  |  |  |
| Normal |  |  |  |  |  |
| Bright |  |  |  |  |  |

---

## 20. Rule-Based vs Machine-Learning Classification

### Rule-Based

```text
LDR Value
   ↓
Thresholds
   ↓
Dark / Normal / Bright
```

### Machine Learning

```text
LDR Measurements
      ↓
Dataset
      ↓
Training
      ↓
Classifier
      ↓
Dark / Normal / Bright
```

For a single LDR with clearly separated classes, threshold classification may already work very well. The dataset collected here can later be used to compare threshold-based and machine-learning approaches.

---

## 21. Experiments

### Experiment 1
Read raw LDR values.

### Experiment 2
Measure Dark, Normal, and Bright conditions.

### Experiment 3
Define thresholds `T1` and `T2`.

### Experiment 4
Perform real-time classification.

### Experiment 5
Collect CSV data.

### Experiment 6
Balance the three classes.

### Experiment 7
Plot light values.

### Experiment 8
Compare raw ADC values with moving-average values.

---

## 22. Exercises

### Exercise 1 — Basic Logger

```text
LDR
 ↓
ESP32
 ↓
ADC Value
 ↓
Serial Monitor
```

### Exercise 2 — Threshold Classifier

Classify:

```text
Dark
Normal
Bright
```

### Exercise 3 — CSV Dataset

Generate:

```text
timestamp,light_value,label
```

### Exercise 4 — Dataset Statistics

Calculate:

```text
Minimum
Maximum
Mean
Standard Deviation
```

for each class.

### Exercise 5 — Compare Thresholds

Test different values of:

```text
T1
T2
```

and identify the best separation.

---

## 23. Checkpoint Questions

1. What is an LDR?
2. Why is a resistor used with the LDR?
3. What does the ESP32 ADC measure?
4. What is the typical 12-bit ADC range?
5. Why should the ADC response be checked experimentally?
6. What are the three classes in this lab?
7. What is a classification threshold?
8. Why should the dataset be balanced?
9. Why is a fixed sampling interval useful?
10. What is the purpose of timestamping?
11. Why should training and testing data be separated?
12. What is data leakage?
13. Why can a moving average improve stability?
14. What is the difference between rule-based and ML classification?
15. How could this work be extended with other sensors?

---

## 24. Expected Results

Expected real-time output:

```text
420 -> dark
1450 -> normal
3120 -> bright
```

Expected dataset format:

```text
timestamp,light_value,label
```

The final dataset should contain approximately balanced measurements for:

```text
Dark
Normal
Bright
```

---

## 25. Submission

Submit:

- ESP32 LDR source code
- circuit diagram or photo
- raw ADC measurements
- selected thresholds
- real-time classification output
- CSV dataset
- dataset statistics
- light-level plot
- short discussion of classification quality

Suggested summary:

| Property | Value |
|---|---|
| Sensor | LDR |
| ADC pin | GPIO 36 |
| ADC resolution | 12 bit |
| Classes | Dark, Normal, Bright |
| Sampling interval | 100 ms |
| Sampling frequency | ~10 Hz |
| File format | CSV |

---

## 26. Conclusion

This lab uses an LDR and ESP32 to create a simple **three-class light-level classification system**.

```text
LDR
 ↓
ESP32 ADC
 ↓
Light Value
 ↓
Dark / Normal / Bright
```

Students first inspect real sensor values, then define thresholds, perform real-time classification, and prepare a labeled CSV dataset. The same dataset can later support a machine-learning classifier and a comparison between data-driven and threshold-based approaches.
