/*
  LAB 14 — LDR Feature Extraction + ANN Classification
  ESP32 Arduino
  Classes: Dark / Normal / Bright

  ANN architecture:
    5 inputs -> 4 hidden neurons -> 3 outputs

  Features:
    1. Mean
    2. Standard deviation
    3. Minimum
    4. Maximum
    5. Range

  This sketch expects the Python training script to generate:
    ldr_ann_weights.h

  Put ldr_ann_weights.h in the same Arduino sketch folder.
*/

#include <Arduino.h>
#include <math.h>

#include "ldr_ann_weights.h"

// ============================================================
// 1. Hardware Configuration
// ============================================================

#define LDR_PIN 36

// Optional class indicator LEDs
#define LED_DARK   16
#define LED_NORMAL 17
#define LED_BRIGHT 18


// ============================================================
// 2. Sampling Configuration
// ============================================================

#define WINDOW_SIZE 20

#define SAMPLE_INTERVAL_MS 100

#define NUM_FEATURES 5

#define NUM_CLASSES 3


// ============================================================
// 3. Class Names
// ============================================================

const char* classNames[NUM_CLASSES] =
{
  "Dark",
  "Normal",
  "Bright"
};


// ============================================================
// 4. Buffers
// ============================================================

float ldrWindow[WINDOW_SIZE];

float features[NUM_FEATURES];

float normalizedFeatures[NUM_FEATURES];

float outputScores[NUM_CLASSES];


// ============================================================
// 5. Sigmoid Activation
// ============================================================

float sigmoid(
  float x
)
{
  // Prevent excessive expf() values
  if (
    x > 20.0f
  )
  {
    x = 20.0f;
  }

  if (
    x < -20.0f
  )
  {
    x = -20.0f;
  }

  return
    1.0f /
    (
      1.0f +
      expf(
        -x
      )
    );
}


// ============================================================
// 6. Read One LDR Sample
// ============================================================

float readLDR()
{
  return
    (float)analogRead(
      LDR_PIN
    );
}


// ============================================================
// 7. Collect One Window
// ============================================================

void collectWindow()
{
  for (
    int i = 0;
    i < WINDOW_SIZE;
    i++
  )
  {
    ldrWindow[i] =
      readLDR();

    delay(
      SAMPLE_INTERVAL_MS
    );
  }
}


// ============================================================
// 8. Feature Extraction
// ============================================================
//
// Feature order MUST match Python:
//
// features[0] = mean
// features[1] = standard deviation
// features[2] = minimum
// features[3] = maximum
// features[4] = range
//
// ============================================================

void extractFeatures(
  const float samples[],
  int sampleCount,
  float featureVector[]
)
{
  float sum =
    0.0f;

  float minValue =
    samples[0];

  float maxValue =
    samples[0];


  // ----------------------------------------------------------
  // Mean + Min + Max
  // ----------------------------------------------------------

  for (
    int i = 0;
    i < sampleCount;
    i++
  )
  {
    float value =
      samples[i];

    sum +=
      value;


    if (
      value < minValue
    )
    {
      minValue =
        value;
    }


    if (
      value > maxValue
    )
    {
      maxValue =
        value;
    }
  }


  float meanValue =
    sum /
    sampleCount;


  // ----------------------------------------------------------
  // Standard Deviation
  // ----------------------------------------------------------

  float variance =
    0.0f;


  for (
    int i = 0;
    i < sampleCount;
    i++
  )
  {
    float difference =
      samples[i]
      -
      meanValue;

    variance +=
      difference
      *
      difference;
  }


  variance /=
    sampleCount;


  float stdValue =
    sqrtf(
      variance
    );


  // ----------------------------------------------------------
  // Range
  // ----------------------------------------------------------

  float rangeValue =
    maxValue
    -
    minValue;


  // ----------------------------------------------------------
  // Store Features
  // ----------------------------------------------------------

  featureVector[0] =
    meanValue;

  featureVector[1] =
    stdValue;

  featureVector[2] =
    minValue;

  featureVector[3] =
    maxValue;

  featureVector[4] =
    rangeValue;
}


// ============================================================
// 9. Normalize Features
// ============================================================
//
// Python training used:
//
//     X = X / 4095.0
//
// The same preprocessing MUST be used on ESP32.
//
// ============================================================

void normalizeFeatures(
  const float inputFeatures[],
  float outputFeatures[]
)
{
  for (
    int i = 0;
    i < NUM_FEATURES;
    i++
  )
  {
    outputFeatures[i] =
      inputFeatures[i]
      /
      4095.0f;
  }
}


// ============================================================
// 10. ANN Forward Propagation
// ============================================================
//
// Expected arrays from ldr_ann_weights.h:
//
// W1[5][4]
// b1[4]
//
// W2[4][3]
// b2[3]
//
// ============================================================

int predictANN(
  const float inputVector[],
  float scores[]
)
{
  float hidden[
    ANN_HIDDEN_SIZE
  ];


  // ----------------------------------------------------------
  // Hidden Layer
  // ----------------------------------------------------------

  for (
    int j = 0;
    j < ANN_HIDDEN_SIZE;
    j++
  )
  {
    float z =
      b1[j];


    for (
      int i = 0;
      i < ANN_INPUT_SIZE;
      i++
    )
    {
      z +=
        inputVector[i]
        *
        W1[i][j];
    }


    hidden[j] =
      sigmoid(
        z
      );
  }


  // ----------------------------------------------------------
  // Output Layer
  // ----------------------------------------------------------
  //
  // Python training uses softmax for training.
  // For classification, argmax of the logits is sufficient.
  //
  // We therefore calculate raw output logits here.
  //
  // ----------------------------------------------------------

  for (
    int k = 0;
    k < ANN_OUTPUT_SIZE;
    k++
  )
  {
    float z =
      b2[k];


    for (
      int j = 0;
      j < ANN_HIDDEN_SIZE;
      j++
    )
    {
      z +=
        hidden[j]
        *
        W2[j][k];
    }


    scores[k] =
      z;
  }


  // ----------------------------------------------------------
  // Argmax
  // ----------------------------------------------------------

  int bestClass =
    0;

  float bestScore =
    scores[0];


  for (
    int k = 1;
    k < ANN_OUTPUT_SIZE;
    k++
  )
  {
    if (
      scores[k] >
      bestScore
    )
    {
      bestScore =
        scores[k];

      bestClass =
        k;
    }
  }


  return
    bestClass;
}


// ============================================================
// 11. Softmax for Display Only
// ============================================================

void softmaxForDisplay(
  const float logits[],
  float probabilities[]
)
{
  float maxLogit =
    logits[0];


  for (
    int i = 1;
    i < NUM_CLASSES;
    i++
  )
  {
    if (
      logits[i] >
      maxLogit
    )
    {
      maxLogit =
        logits[i];
    }
  }


  float sumExp =
    0.0f;


  for (
    int i = 0;
    i < NUM_CLASSES;
    i++
  )
  {
    probabilities[i] =
      expf(
        logits[i]
        -
        maxLogit
      );

    sumExp +=
      probabilities[i];
  }


  if (
    sumExp <= 0.0f
  )
  {
    return;
  }


  for (
    int i = 0;
    i < NUM_CLASSES;
    i++
  )
  {
    probabilities[i] /=
      sumExp;
  }
}


// ============================================================
// 12. Update Indicator LEDs
// ============================================================

void updateLEDs(
  int predictedClass
)
{
  digitalWrite(
    LED_DARK,
    predictedClass == 0
      ? HIGH
      : LOW
  );


  digitalWrite(
    LED_NORMAL,
    predictedClass == 1
      ? HIGH
      : LOW
  );


  digitalWrite(
    LED_BRIGHT,
    predictedClass == 2
      ? HIGH
      : LOW
  );
}


// ============================================================
// 13. Display Raw Window
// ============================================================

void printWindow()
{
  Serial.print(
    "Window: "
  );


  for (
    int i = 0;
    i < WINDOW_SIZE;
    i++
  )
  {
    Serial.print(
      ldrWindow[i],
      0
    );


    if (
      i <
      WINDOW_SIZE - 1
    )
    {
      Serial.print(
        ", "
      );
    }
  }


  Serial.println();
}


// ============================================================
// 14. Display Extracted Features
// ============================================================

void printFeatures()
{
  Serial.println(
    "Raw Features:"
  );


  Serial.print(
    "  Mean  : "
  );

  Serial.println(
    features[0],
    2
  );


  Serial.print(
    "  Std   : "
  );

  Serial.println(
    features[1],
    2
  );


  Serial.print(
    "  Min   : "
  );

  Serial.println(
    features[2],
    2
  );


  Serial.print(
    "  Max   : "
  );

  Serial.println(
    features[3],
    2
  );


  Serial.print(
    "  Range : "
  );

  Serial.println(
    features[4],
    2
  );


  Serial.println(
    "Normalized Features:"
  );


  for (
    int i = 0;
    i < NUM_FEATURES;
    i++
  )
  {
    Serial.print(
      "  x"
    );

    Serial.print(
      i + 1
    );

    Serial.print(
      " = "
    );

    Serial.println(
      normalizedFeatures[i],
      5
    );
  }
}


// ============================================================
// 15. Setup
// ============================================================

void setup()
{
  Serial.begin(
    115200
  );


  pinMode(
    LDR_PIN,
    INPUT
  );


  pinMode(
    LED_DARK,
    OUTPUT
  );


  pinMode(
    LED_NORMAL,
    OUTPUT
  );


  pinMode(
    LED_BRIGHT,
    OUTPUT
  );


  digitalWrite(
    LED_DARK,
    LOW
  );


  digitalWrite(
    LED_NORMAL,
    LOW
  );


  digitalWrite(
    LED_BRIGHT,
    LOW
  );


  Serial.println();

  Serial.println(
    "========================================"
  );

  Serial.println(
    "ESP32 LDR Feature ANN Classifier"
  );

  Serial.println(
    "Architecture: 5 -> 4 -> 3"
  );

  Serial.println(
    "Features: Mean, Std, Min, Max, Range"
  );

  Serial.println(
    "Classes: Dark, Normal, Bright"
  );

  Serial.println(
    "========================================"
  );

  Serial.println();
}


// ============================================================
// 16. Main Loop
// ============================================================

void loop()
{
  Serial.println(
    "Collecting LDR window..."
  );


  // ----------------------------------------------------------
  // 1. Collect Window
  // ----------------------------------------------------------

  collectWindow();


  // ----------------------------------------------------------
  // 2. Feature Extraction
  // ----------------------------------------------------------

  extractFeatures(
    ldrWindow,
    WINDOW_SIZE,
    features
  );


  // ----------------------------------------------------------
  // 3. Normalize Features
  // ----------------------------------------------------------

  normalizeFeatures(
    features,
    normalizedFeatures
  );


  // ----------------------------------------------------------
  // 4. ANN Inference
  // ----------------------------------------------------------

  unsigned long startTime =
    micros();


  int predictedClass =
    predictANN(
      normalizedFeatures,
      outputScores
    );


  unsigned long endTime =
    micros();


  unsigned long inferenceTime =
    endTime
    -
    startTime;


  // ----------------------------------------------------------
  // 5. Softmax Probabilities
  // ----------------------------------------------------------

  float probabilities[
    NUM_CLASSES
  ];


  softmaxForDisplay(
    outputScores,
    probabilities
  );


  // ----------------------------------------------------------
  // 6. Output LEDs
  // ----------------------------------------------------------

  updateLEDs(
    predictedClass
  );


  // ----------------------------------------------------------
  // 7. Serial Output
  // ----------------------------------------------------------

  Serial.println();

  printWindow();

  Serial.println();

  printFeatures();

  Serial.println();


  Serial.print(
    "Dark probability   : "
  );

  Serial.println(
    probabilities[0],
    4
  );


  Serial.print(
    "Normal probability : "
  );

  Serial.println(
    probabilities[1],
    4
  );


  Serial.print(
    "Bright probability : "
  );

  Serial.println(
    probabilities[2],
    4
  );


  Serial.println();


  Serial.print(
    "Prediction: "
  );

  Serial.println(
    classNames[
      predictedClass
    ]
  );


  Serial.print(
    "Inference time: "
  );

  Serial.print(
    inferenceTime
  );

  Serial.println(
    " us"
  );


  Serial.println(
    "----------------------------------------"
  );

  Serial.println();
}
