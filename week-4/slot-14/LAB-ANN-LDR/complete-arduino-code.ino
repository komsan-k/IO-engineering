#include <Arduino.h>
#include <math.h>

// =====================================================
// LDR Configuration
// =====================================================

#define LDR_PIN 36

// Optional LEDs
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
// Replace these parameters with the trained Float32
// parameters printed by the Python code
// =====================================================

// W1 shape = 1 x 4
float W1[HIDDEN_SIZE] =
{
  1.0f,
  2.0f,
  3.0f,
  4.0f
};


// b1 shape = 4
float b1[HIDDEN_SIZE] =
{
  0.0f,
  0.0f,
  0.0f,
  0.0f
};


// W2 shape = 4 x 3
float W2[HIDDEN_SIZE][OUTPUT_SIZE] =
{
  { 1.0f,  0.0f, -1.0f },
  { 0.5f,  0.5f, -0.5f },
  {-0.5f,  1.0f,  0.5f },
  {-1.0f,  0.0f,  1.0f }
};


// b2 shape = 3
float b2[OUTPUT_SIZE] =
{
  0.0f,
  0.0f,
  0.0f
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
// Sigmoid Activation
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
// ANN Float32 Inference
// =====================================================

int predictANN(
  int adcValue,
  float outputScores[]
)
{
  // ---------------------------------
  // Normalize LDR ADC
  // 0 ... 4095 -> 0.0 ... 1.0
  // ---------------------------------

  float x =
    adcValue / 4095.0f;


  // ---------------------------------
  // Hidden Layer
  // ---------------------------------

  float hidden[
    HIDDEN_SIZE
  ];


  for (
    int j = 0;
    j < HIDDEN_SIZE;
    j++
  )
  {
    float z =
      x * W1[j]
      +
      b1[j];


    hidden[j] =
      sigmoid(
        z
      );
  }


  // ---------------------------------
  // Output Layer
  // ---------------------------------

  for (
    int k = 0;
    k < OUTPUT_SIZE;
    k++
  )
  {
    float z =
      b2[k];


    for (
      int j = 0;
      j < HIDDEN_SIZE;
      j++
    )
    {
      z +=
        hidden[j]
        *
        W2[j][k];
    }


    outputScores[k] =
      sigmoid(
        z
      );
  }


  // ---------------------------------
  // Argmax
  // ---------------------------------

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
    "ESP32 ANN LDR Classifier"
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
  // ---------------------------------
  // Read LDR
  // ---------------------------------

  int lightValue =
    analogRead(
      LDR_PIN
    );


  // ---------------------------------
  // ANN Output
  // ---------------------------------

  float scores[
    OUTPUT_SIZE
  ];


  // ---------------------------------
  // Measure Inference Time
  // ---------------------------------

  unsigned long startTime =
    micros();


  int predictedClass =
    predictANN(
      lightValue,
      scores
    );


  unsigned long endTime =
    micros();


  unsigned long inferenceTime =
    endTime - startTime;


  // ---------------------------------
  // Update LEDs
  // ---------------------------------

  updateLEDs(
    predictedClass
  );


  // ---------------------------------
  // Display Results
  // ---------------------------------

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
    lightValue / 4095.0f,
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
    inferenceTime
  );


  Serial.println(
    " us"
  );


  delay(
    500
  );
}
