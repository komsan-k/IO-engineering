# ESP32 pthread --- Two LED Blinking Tasks with Period Parameters

## Objective

Create two `pthread` tasks on the **ESP32** to blink two LEDs
independently.\
The **GPIO pin** and **blinking period** are passed to each thread as
parameters.

## Task Configuration

  Thread           LED GPIO    Period
  -------------- ---------- ---------
  LED Thread 1       GPIO 2    500 ms
  LED Thread 2      GPIO 12   1000 ms

------------------------------------------------------------------------

## Complete Arduino Code

``` cpp
#include <Arduino.h>
#include <pthread.h>

// Structure for passing parameters
typedef struct {
  int pin;
  int period;
} LEDParams;

// LED parameters
LEDParams led1 = {2, 500};
LEDParams led2 = {12, 1000};

// -----------------------------------------
// LED Blinking Thread
// -----------------------------------------
void *blinkLED(void *arg)
{
  // Convert parameter to LEDParams
  LEDParams *led = (LEDParams *)arg;

  pinMode(led->pin, OUTPUT);

  while (1)
  {
    // LED ON
    digitalWrite(led->pin, HIGH);

    Serial.print("GPIO ");
    Serial.print(led->pin);
    Serial.println(" : ON");

    delay(led->period);

    // LED OFF
    digitalWrite(led->pin, LOW);

    Serial.print("GPIO ");
    Serial.print(led->pin);
    Serial.println(" : OFF");

    delay(led->period);
  }

  return NULL;
}

// -----------------------------------------
// Arduino Setup
// -----------------------------------------
void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println("ESP32 pthread - Two LED Tasks");

  // Thread variables
  pthread_t threadLED1;
  pthread_t threadLED2;

  // Create LED 1 thread
  pthread_create(
    &threadLED1,
    NULL,
    blinkLED,
    &led1
  );

  // Create LED 2 thread
  pthread_create(
    &threadLED2,
    NULL,
    blinkLED,
    &led2
  );
}

// -----------------------------------------
// Arduino Main Loop
// -----------------------------------------
void loop()
{
  // LED threads operate independently
  delay(1000);
}
```

------------------------------------------------------------------------

## Passing Parameters

The GPIO pin and blinking period are stored in a structure:

``` cpp
typedef struct {
  int pin;
  int period;
} LEDParams;
```

The parameters for the two LEDs are:

``` cpp
LEDParams led1 = {2, 500};
LEDParams led2 = {12, 1000};
```

Therefore:

``` text
LED 1 → GPIO 2  → 500 ms
LED 2 → GPIO 12 → 1000 ms
```

The parameter structure is passed through the fourth argument of
`pthread_create()`:

``` cpp
pthread_create(&threadLED1, NULL, blinkLED, &led1);
pthread_create(&threadLED2, NULL, blinkLED, &led2);
```

Inside the thread, the parameter is recovered using:

``` cpp
LEDParams *led = (LEDParams *)arg;
```

The thread can then access:

``` cpp
led->pin
led->period
```

------------------------------------------------------------------------

## Program Architecture

``` text
                    ESP32
                      |
             pthread_create()
                      |
          +-----------+-----------+
          |                       |
          v                       v
    LED Thread 1              LED Thread 2
      GPIO 2                    GPIO 12
      500 ms                    1000 ms
          |                       |
          v                       v
        LED 1                   LED 2
```

------------------------------------------------------------------------

## Key Concepts

This example demonstrates:

-   `pthread_create()`
-   Multiple concurrent threads
-   Passing parameters to a thread
-   Structure-based task parameters
-   Independent LED control
-   Different blinking periods
-   ESP32 GPIO control

> **Note:** In this program, `period` is the delay for each ON and OFF
> state. Therefore, a `500 ms` parameter produces a complete ON--OFF
> cycle of approximately `1000 ms`.
