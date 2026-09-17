# Assignment — LDR and LED Driver for Automatic Brightness Control

## 1. Objective

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

## 2. Hardware

- ESP32 development board
- LDR (Light Dependent Resistor)
- 10 kΩ resistor
- LED
- 220–330 Ω resistor
- Breadboard
- Jumper wires

---

## 3. LDR Driver

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

The normalized value can be calculated as:

$$
L_{\text{norm}} =
\frac{\text{ADC}_{\text{raw}}}{4095}
$$

> **Note:** `readLUX()` should use a suitable calibration or LDR resistance-to-lux model. The calculated lux value is an estimate unless the LDR has been calibrated against a reference lux meter.

---

## 4. LED Driver

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

## 5. Main Program

Create:

```text
main.ino
```

The program must create LDR and LED objects:

```cpp
LDR ldr(34);
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

---

## 6. Automatic LED Brightness Control

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

## 7. Program Structure

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

## 8. Expected System Operation

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

## 9. Assignment Tasks

1. Build the LDR voltage-divider circuit and LED circuit.
2. Implement the `LDR` driver.
3. Implement `readRAW()`, `readNormalized()`, and `readLUX()`.
4. Implement the `LED` driver.
5. Implement `on()`, `off()`, and `setBrightness()`.
6. Write the main program using the two driver objects.
7. Display RAW, normalized, lux, and PWM values on the Serial Monitor.
8. Demonstrate automatic LED brightness control under different lighting conditions.

