#include <Arduino.h>
#include <math.h>

// =====================================================
// LDR Configuration
// =====================================================

#define LDR_PIN 36

#define LED_DARK   16
#define LED_NORMAL 17
#define LED_BRIGHT 18


// =====================================================
// ANN Architecture
// =====================================================

#define INPUT_SIZE  1
#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 3


// =====================================================
// Quantization Parameters
// =====================================================
//
// Replace these values with those printed by Python.
//
// INPUT_SCALE = 1 / 127
// HIDDEN_SCALE = 1 / 127
//
// W1_SCALE
// W2_SCALE
//
// B1_SCALE = INPUT_SCALE × W1_SCALE
// B2_SCALE = HIDDEN_SCALE × W2_SCALE
// =====================================================

const float INPUT_SCALE =
  1.0f / 127.0f;

const float HIDDEN_SCALE =
  1.0f / 127.0f;


// -----------------------------------------------------
// EXAMPLE ONLY
// Replace with your Python result
// -----------------------------------------------------

const float W1_SCALE =
  0.0250f;

const float W2_SCALE =
  0.0300f;


// Bias scales

const float B1_SCALE =
  INPUT_SCALE * W1_SCALE;

const float B2_SCALE =
  HIDDEN_SCALE * W2_SCALE;


// =====================================================
// INT8 ANN Parameters
// =====================================================
//
// W1 shape = 1 × 4
// =====================================================

const int8_t W1_INT8[HIDDEN_SIZE] =
{
   35,
   72,
  -58,
  110
};


// =====================================================
// Bias Layer 1
// Stored as INT32
// =====================================================

const int32_t B1_INT32[HIDDEN_SIZE] =
{
   -50,
   120,
  -180,
    30
};


// =====================================================
// W2 shape = 4 × 3
// =====================================================

const int8_t W2_INT8[HIDDEN_SIZE][OUTPUT_SIZE] =
{
  {  90, -45, -80 },
  {  60,  85, -70 },
  { -75, 100,  40 },
  {-100, -35, 110 }
};


// =====================================================
// Bias Layer 2
// =====================================================

const int32_t B2_INT32[OUTPUT_SIZE] =
{
  100,
  -50,
   70
};


// =====================================================
// Class Names
// =====================================================

const char* classNames[OUTPUT_SIZE] =
{
  "Dark",
  "Normal",
  "Bright"
};


// =====================================================
// Sigmoid
// =====================================================

float sigmoid(
  float x
)
{
  return
    1.0f /
    (
      1.0f +
      expf(
        -x
      )
    );
}


// =====================================================
// Quantize Input
// =====================================================
//
// normalized input:
//
// 0.0 ... 1.0
//
// becomes approximately:
//
// 0 ... 127
// =====================================================

int8_t quantizeInput(
  float value
)
{
  int32_t q =
    (int32_t)roundf(
      value /
      INPUT_SCALE
    );


  if (
    q > 127
  )
  {
    q = 127;
  }


  if (
    q < 0
  )
  {
    q = 0;
  }


  return
    (int8_t)q;
}


// =====================================================
// Quantize Hidden Activation
// =====================================================
//
// sigmoid output:
//
// 0.0 ... 1.0
//
// becomes:
//
// 0 ... 127
// =====================================================

int8_t quantizeHidden(
  float value
)
{
  int32_t q =
    (int32_t)roundf(
      value /
      HIDDEN_SCALE
    );


  if (
    q > 127
  )
  {
    q = 127;
  }


  if (
    q < 0
  )
  {
    q = 0;
  }


  return
    (int8_t)q;
}


// =====================================================
// INT8 ANN Prediction
// =====================================================

int predictANN_INT8(
  int adcValue,
  float outputScores[]
)
{
  // ---------------------------------------------------
  // 1. Normalize LDR ADC
  //
  // 0 ... 4095
  //       ↓
  // 0.0 ... 1.0
  // ---------------------------------------------------

  float xFloat =
    adcValue /
    4095.0f;


  // ---------------------------------------------------
  // 2. Quantize Input
  // ---------------------------------------------------

  int8_t xInt8 =
    quantizeInput(
      xFloat
    );


  // ---------------------------------------------------
  // 3. Hidden Layer
  //
  // INT8 × INT8
  //       ↓
  // INT32 Accumulator
  // ---------------------------------------------------

  float hiddenFloat[
    HIDDEN_SIZE
  ];


  int8_t hiddenInt8[
    HIDDEN_SIZE
  ];


  for (
    int j = 0;
    j < HIDDEN_SIZE;
    j++
  )
  {
    int32_t accumulator =
      (
        (int32_t)xInt8
        *
        (int32_t)W1_INT8[j]
      )
      +
      B1_INT32[j];


    // Convert INT32 accumulator
    // back to approximate real value

    float z =
      accumulator
      *
      B1_SCALE;


    // Sigmoid activation

    hiddenFloat[j] =
      sigmoid(
        z
      );


    // Quantize hidden activation

    hiddenInt8[j] =
      quantizeHidden(
        hiddenFloat[j]
      );
  }


  // ---------------------------------------------------
  // 4. Output Layer
  //
  // INT8 × INT8
  //       ↓
  // INT32
  // ---------------------------------------------------

  for (
    int k = 0;
    k < OUTPUT_SIZE;
    k++
  )
  {
    int32_t accumulator =
      B2_INT32[k];


    for (
      int j = 0;
      j < HIDDEN_SIZE;
      j++
    )
    {
      accumulator +=
        (
          (int32_t)hiddenInt8[j]
          *
          (int32_t)W2_INT8[j][k]
        );
    }


    // Convert back to float

    float z =
      accumulator
      *
      B2_SCALE;


    // Educational output activation

    outputScores[k] =
      sigmoid(
        z
      );
  }


  // ---------------------------------------------------
  // 5. Argmax
  // ---------------------------------------------------

  int bestClass =
    0;


  float bestScore =
    outputScores[0];


  for (
    int k = 1;
    k < OUTPUT_SIZE;
    k++
  )
  {
    if (
      outputScores[k]
      >
      bestScore
    )
    {
      bestScore =
        outputScores[k];

      bestClass =
        k;
    }
  }


  return
    bestClass;
}


// =====================================================
// LED Output
// =====================================================

void updateLEDs(
  int predictedClass
)
{
  digitalWrite(
    LED_DARK,
    predictedClass == 0
  );


  digitalWrite(
    LED_NORMAL,
    predictedClass == 1
  );


  digitalWrite(
    LED_BRIGHT,
    predictedClass == 2
  );
}


// =====================================================
// Setup
// =====================================================

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
    "ESP32 INT8 ANN LDR Classifier"
  );

  Serial.println(
    "Architecture: 1-4-3"
  );

  Serial.println(
    "Classes: Dark / Normal / Bright"
  );
}


// =====================================================
// Loop
// =====================================================

void loop()
{
  // ---------------------------------------------------
  // Read LDR
  // ---------------------------------------------------

  int lightValue =
    analogRead(
      LDR_PIN
    );


  // ---------------------------------------------------
  // ANN Scores
  // ---------------------------------------------------

  float scores[
    OUTPUT_SIZE
  ];


  // ---------------------------------------------------
  // Measure Inference Time
  // ---------------------------------------------------

  unsigned long startTime =
    micros();


  int predictedClass =
    predictANN_INT8(
      lightValue,
      scores
    );


  unsigned long endTime =
    micros();


  unsigned long latency =
    endTime -
    startTime;


  // ---------------------------------------------------
  // LED Indicators
  // ---------------------------------------------------

  updateLEDs(
    predictedClass
  );


  // ---------------------------------------------------
  // Display Results
  // ---------------------------------------------------

  Serial.print(
    "ADC = "
  );

  Serial.print(
    lightValue
  );


  Serial.print(
    " | Norm = "
  );

  Serial.print(
    lightValue /
    4095.0f,
    3
  );


  Serial.print(
    " | Dark = "
  );

  Serial.print(
    scores[0],
    4
  );


  Serial.print(
    " | Normal = "
  );

  Serial.print(
    scores[1],
    4
  );


  Serial.print(
    " | Bright = "
  );

  Serial.print(
    scores[2],
    4
  );


  Serial.print(
    " | Prediction = "
  );

  Serial.print(
    classNames[
      predictedClass
    ]
  );


  Serial.print(
    " | Inference = "
  );

  Serial.print(
    latency
  );


  Serial.println(
    " us"
  );


  delay(
    500
  );
}
