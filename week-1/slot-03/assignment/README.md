# Assignment — LDR and LED Driver for Automatic Brightness Control

## Objective

Develop reusable **LDR** and **LED** drivers for the ESP32 and use them to create an automatic LED brightness-control system.

The system should follow:

$$
\boxed{
\text{LDR}
\rightarrow
\text{Light Measurement}
\rightarrow
\text{ESP32}
\rightarrow
\text{PWM}
\rightarrow
\text{LED Brightness}
}
$$

---

## LDR Driver

Create an LDR driver using two files:

```text
LDR.h
LDR.cpp
```

The driver must provide the following API:

```cpp
class LDR {
public:
    LDR(int pin);

    int readRAW();
    float readNormalized();
    float readLUX();
};
```

### API Requirements

| Function | Description | Expected Range |
|---|---|---|
| `readRAW()` | Read the raw ADC value | 0–4095 |
| `readNormalized()` | Convert ADC reading to normalized light level | 0.0–1.0 |
| `readLUX()` | Estimate illumination level | lux |


---

## LED Driver

Create an LED driver using:

```text
LED.h
LED.cpp
```

The LED driver should provide a simple brightness-control API:

```cpp
class LED {
public:
    LED(int pin);

    void on();
    void off();
    void setBrightness(int brightness);
};
```

The brightness should use the range:

```text
0   -> LED OFF
255 -> Maximum brightness
```

---

## Main Program

Create:

```text
main.ino
```

The program must create LDR and LED objects:

```cpp
LDR ldr(36);
LED led(2);
```

Read the sensor using the three APIs:

```cpp
int raw = ldr.readRAW();

float normalized = ldr.readNormalized();

float lux = ldr.readLUX();
```

Display the results on the Serial Monitor:

```text
RAW        : 2530
Normalized : 0.618
Lux        : 320.5 lx
LED PWM    : 97
```
```cpp
#include "LDR.h"
#include "LED.h"

// --------------------------------------------------
// Pin Configuration
// --------------------------------------------------
#define LDR_PIN 34
#define LED_PIN 2

// --------------------------------------------------
// Create Driver Objects
// --------------------------------------------------
LDR ldr(LDR_PIN);
LED led(LED_PIN);

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" LDR Automatic LED Brightness Control");
  Serial.println("====================================");
  Serial.println();
}

// --------------------------------------------------
// Main Loop
// --------------------------------------------------
void loop() {

  // -----------------------------------------------
  // 1. Read raw ADC value
  // -----------------------------------------------
  int raw = ldr.readRAW();

  // -----------------------------------------------
  // 2. Read normalized light level
  // Range: 0.0 - 1.0
  // -----------------------------------------------
  float normalized = ldr.readNormalized();

  // -----------------------------------------------
  // 3. Estimate illumination in lux
  // -----------------------------------------------
  float lux = ldr.readLUX();

  // -----------------------------------------------
  // 4. Calculate LED brightness
  //
  // Dark   -> LED bright
  // Bright -> LED dim
  //
  // normalized = 0.0 -> PWM = 255
  // normalized = 1.0 -> PWM = 0
  // -----------------------------------------------
  int brightness = (int)(255.0 * (1.0 - normalized));

  // Limit PWM value to valid range
  brightness = constrain(brightness, 0, 255);

  // -----------------------------------------------
  // 5. Control LED using LED Driver
  // -----------------------------------------------
  led.setBrightness(brightness);

  // -----------------------------------------------
  // 6. Display results
  // -----------------------------------------------
  Serial.println("------------------------------------");

  Serial.print("RAW        : ");
  Serial.println(raw);

  Serial.print("Normalized : ");
  Serial.println(normalized, 3);

  Serial.print("Lux        : ");
  Serial.print(lux, 2);
  Serial.println(" lx");

  Serial.print("LED PWM    : ");
  Serial.println(brightness);

  // -----------------------------------------------
  // 7. Display lighting condition
  // -----------------------------------------------
  Serial.print("Condition  : ");

  if (normalized < 0.25) {
    Serial.println("Very Dark");
  }
  else if (normalized < 0.50) {
    Serial.println("Dark");
  }
  else if (normalized < 0.75) {
    Serial.println("Bright");
  }
  else {
    Serial.println("Very Bright");
  }

  // Sampling interval
  delay(500);
}
```

---

## Automatic LED Brightness Control

Use the LDR reading to automatically control LED brightness.

The required behavior is:

```text
Dark Environment   -> LED Bright
Bright Environment -> LED Dim
```

Therefore, the LED brightness can be determined from:

$$
B_{\text{LED}}
= 255(1-L_{\text{norm}})
$$

where $L_{\text{norm}}$ is between 0 and 1.

For example:

| Normalized Light | Environment | LED Brightness |
|---:|---|---:|
| 0.0 | Very dark | 255 |
| 0.25 | Dark | 191 |
| 0.50 | Medium | 128 |
| 0.75 | Bright | 64 |
| 1.0 | Very bright | 0 |

Use the LED API to apply the calculated brightness:

```cpp
led.setBrightness(brightness);
```

---

## Program Structure

Your final project should have the following structure:

```text
LDR_LED_Control/
│
├── LDR.h
├── LDR.cpp
├── LED.h
├── LED.cpp
└── main.ino
```

The main program should not directly perform ADC or PWM operations. These hardware operations should be encapsulated inside the corresponding drivers.

---

## Expected System Operation

```text
Read LDR
   |
   v
readRAW()
   |
   +----> readNormalized()
   |
   +----> readLUX()
   |
   v
Calculate LED Brightness
   |
   v
setBrightness()
   |
   v
LED PWM Output
```

---

## Assignment Tasks

1. Build the LDR voltage-divider circuit and LED circuit.
2. Implement the `LDR` driver.
3. Implement `readRAW()`, `readNormalized()`, and `readLUX()`.
4. Implement the `LED` driver.
5. Implement `on()`, `off()`, and `setBrightness()`.
6. Write the main program using the two driver objects.
7. Display RAW, normalized, lux, and PWM values on the Serial Monitor.
8. Demonstrate automatic LED brightness control under different lighting conditions.

