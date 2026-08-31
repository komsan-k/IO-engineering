# LAB 3 — ADC Sensor Acquisition and PWM Control of LED Brightness or Servo Position

## 1. Objective

The objective of this lab is to:

1. Understand the operation of the ESP32 **Analog-to-Digital Converter (ADC)**.
2. Read analog sensor values using an ADC input.
3. Convert raw ADC readings into normalized values or approximate voltage.
4. Understand ADC resolution and quantization.
5. Observe how sensor values vary with physical input.
6. Understand the principle of **Pulse-Width Modulation (PWM)**.
7. Generate a PWM signal using the ESP32.
8. Control LED brightness using PWM duty cycle.
9. Map ADC sensor values to LED brightness.
10. Optionally control a servo motor using a PWM-compatible control signal.
11. Understand the relationship among ADC input, scaling, mapping, and actuator output.
12. Build a simple **sensor-to-actuator embedded control system**.

---

## 2. Background

Many physical sensors produce an analog voltage rather than a digital HIGH or LOW signal.

Examples include:

```text
Potentiometer
Light Sensor
Temperature Sensor
Pressure Sensor
Analog Joystick
Force Sensor
```

The microcontroller converts the analog voltage into a digital number using an ADC.

The basic data path is:

```text
Physical Quantity
      ↓
Sensor
      ↓
Analog Voltage
      ↓
ADC
      ↓
Digital Value
      ↓
CPU
```

The resulting digital value can then be used to control an actuator such as:

```text
LED
Servo Motor
DC Motor
Buzzer
```

---

## 3. System Architecture

The complete system developed in this lab is:

```text
Sensor
  ↓
Analog Voltage
  ↓
ESP32 ADC
  ↓
Digital Sensor Value
  ↓
Scaling / Mapping
  ↓
PWM Output
  ↓
LED or Servo
```

This represents a basic embedded control loop:

$$
\boxed{
\text{Sense}
\rightarrow
\text{Process}
\rightarrow
\text{Actuate}
}
$$

---

## 4. Hardware Requirements

Use:

- ESP32 development board
- USB cable
- Breadboard
- Jumper wires
- Potentiometer or analog sensor
- LED
- 220–330 $\Omega$ resistor
- Optional SG90 servo motor
- Optional external 5-V supply for the servo

Suggested GPIO assignments:

```text
Analog Sensor → GPIO34
LED PWM       → GPIO25
Servo Signal  → GPIO26
```

The exact pins may differ depending on the ESP32 board.

---

## 5. ADC Concept

An ADC converts an analog input voltage into a digital value.

Conceptually:

```text
Analog Voltage
      ↓
ADC
      ↓
Digital Number
```

For an ideal $N$-bit ADC:

$$
2^N
$$

possible digital levels are available.

For a nominal 12-bit ADC:

$$
2^{12}=4096
$$

levels.

Therefore, the digital output range is commonly represented as:

$$
0 \leq x_{\text{ADC}} \leq 4095.
$$

---

## 6. ADC Quantization

The ADC divides the input-voltage range into discrete levels.

Conceptually:

```text
Analog Input
     ↓
Continuous Voltage
     ↓
ADC Quantization
     ↓
Discrete Digital Levels
```

For an ideal ADC with reference voltage $V_{\text{ref}}$, the approximate voltage resolution is:

$$
\Delta V
= \frac{V_{\text{ref}}}{2^N}.
$$

For:

$$
V_{\text{ref}}=3.3~\text{V}
$$

and:

$$
N=12,
$$

the idealized step size is:

$$
\Delta V
= \frac{3.3}{4096}
\approx
0.806~\text{mV}.
$$

For ESP32 devices, actual ADC behavior is not perfectly ideal, so this calculation is mainly useful as a conceptual approximation.

---

## 7. Potentiometer Circuit

A potentiometer can be used as a controllable analog voltage source.

Connect:

```text
3.3 V
  │
  ↓
Potentiometer
  │
  ├────────→ GPIO34
  │
  ↓
 GND
```

The middle terminal, or **wiper**, is connected to the ADC input.

Rotating the potentiometer changes the voltage applied to GPIO34.

---

## 8. Experiment 1 — Read Raw ADC Values

Define:

```cpp
#define SENSOR_PIN 34
```

Read the ADC:

```cpp
int sensorValue =
    analogRead(
      SENSOR_PIN
    );
```

Display the result:

```cpp
Serial.println(
  sensorValue
);
```

---

## 9. Complete ADC Reading Program

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34

void setup() {

  Serial.begin(115200);

  pinMode(
    SENSOR_PIN,
    INPUT
  );
}

void loop() {

  int sensorValue =
      analogRead(
        SENSOR_PIN
      );

  Serial.print(
    "ADC = "
  );

  Serial.println(
    sensorValue
  );

  delay(200);
}
```

---

## 10. Expected Result

Rotate the potentiometer.

The Serial Monitor should show values that vary approximately from:

```text
0
↓
1000
↓
2000
↓
3000
↓
4095
```

A typical result may look like:

```text
ADC = 423
ADC = 1157
ADC = 2058
ADC = 3174
ADC = 4012
```

---

## 11. Experiment 2 — Convert ADC Value to Voltage

A simplified voltage estimate is:

$$
V_{\text{in}}
= \frac{x_{\text{ADC}}}{4095}
V_{\text{ref}}.
$$

Using:

$$
V_{\text{ref}}=3.3~\text{V},
$$

the program becomes:

```cpp
float voltage =
    sensorValue
    * 3.3
    / 4095.0;
```

Display:

```cpp
Serial.print(
  "Voltage = "
);

Serial.print(
  voltage,
  3
);

Serial.println(
  " V"
);
```

---

## 12. Complete ADC Voltage Program

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34

void setup() {

  Serial.begin(115200);
}

void loop() {

  int raw =
      analogRead(
        SENSOR_PIN
      );

  float voltage =
      raw
      * 3.3
      / 4095.0;

  Serial.print(
    "Raw = "
  );

  Serial.print(
    raw
  );

  Serial.print(
    " | Voltage ≈ "
  );

  Serial.print(
    voltage,
    3
  );

  Serial.println(
    " V"
  );

  delay(200);
}
```

The voltage calculation is an approximation for teaching purposes. For accurate voltage measurement, ADC calibration should be considered.

---

## 13. Experiment 3 — Normalize Sensor Value

A sensor value can be normalized to:

$$
0 \leq x_n \leq 1.
$$

Use:

$$
x_n
= \frac{x_{\text{ADC}}}{4095}.
$$

For example:

```cpp
float normalized =
    raw / 4095.0;
```

Possible values are:

```text
ADC = 0     → Normalized = 0.0
ADC ≈ 2048  → Normalized ≈ 0.5
ADC = 4095  → Normalized = 1.0
```

---

## 14. PWM Concept

Pulse-Width Modulation controls the fraction of time that a digital output remains HIGH.

A PWM waveform is:

```text
HIGH ────────┐      ┌────────
             │      │
LOW          └──────┘
```

The duty cycle is:

$$
D
= \frac{T_{\text{ON}}}{T}
\times100\%.
$$

where:

- $T_{\text{ON}}$ is the HIGH time.
- $T$ is the complete PWM period.

---

## 15. Duty-Cycle Examples

### 25% Duty Cycle

```text
HIGH ──┐
       │
LOW    └──────
```

$$
D=25\%.
$$

### 50% Duty Cycle

```text
HIGH ─────┐
          │
LOW       └─────
```

$$
D=50\%.
$$

### 75% Duty Cycle

```text
HIGH ────────┐
             │
LOW          └──
```

$$
D=75\%.
$$

For an LED, increasing duty cycle generally increases perceived brightness.

---

## 16. PWM Frequency

PWM frequency is:

$$
f_{\text{PWM}}
= \frac{1}{T_{\text{PWM}}}.
$$

For LED brightness control, the PWM frequency should generally be high enough that the human eye does not perceive obvious flicker.

The ESP32 provides hardware PWM peripherals, commonly exposed through the **LEDC** functionality in the Arduino environment.

---

## 17. Experiment 4 — PWM LED Brightness

Connect:

```text
GPIO25
  │
  ↓
220–330 Ω
  │
 LED
  │
  ↓
 GND
```

Define:

```cpp
#define LED_PIN 25
```

The conceptual setup is:

```text
Configure PWM Pin
       ↓
Set PWM Frequency
       ↓
Set Resolution
       ↓
Write Duty Cycle
```

---

## 18. PWM Resolution

Suppose an 8-bit PWM resolution is used.

The number of levels is:

$$
2^8=256.
$$

Therefore:

$$
0 \leq D_{\text{value}} \leq 255.
$$

Examples:

| PWM Value | Approx. Duty Cycle |
|---:|---:|
| 0 | 0% |
| 64 | 25% |
| 128 | 50% |
| 191 | 75% |
| 255 | 100% |

The approximate duty cycle is:

$$
D
= \frac{D_{\text{value}}}{255}
\times100\%.
$$

---

## 19. Experiment 5 — Fixed LED Brightness Levels

Test:

```text
0%
25%
50%
75%
100%
```

Possible 8-bit PWM values are:

```text
0
64
128
191
255
```

Observe the LED brightness for each value.

| PWM Value | Duty Cycle | Observed Brightness |
|---:|---:|---|
| 0 | 0% | |
| 64 | 25% | |
| 128 | 50% | |
| 191 | 75% | |
| 255 | 100% | |

---

## 20. Experiment 6 — Map ADC Input to PWM Output

The main objective is to connect the sensor to the actuator.

Required behavior:

```text
Low Sensor Value
      ↓
Low PWM Duty Cycle
      ↓
Dim LED

High Sensor Value
      ↓
High PWM Duty Cycle
      ↓
Bright LED
```

The conversion is:

$$
x_{\mathrm{ADC}}
\in[0,4095]
$$

mapped to:

$$
x_{\mathrm{PWM}}
\in[0,255].
$$

---

## 21. ADC-to-PWM Mapping

Mathematically:

$$
x_{\mathrm{PWM}}
= \frac{255}{4095}
x_{\mathrm{ADC}}.
$$

In Arduino:

```cpp
int pwmValue =
    map(
      sensorValue,
      0,
      4095,
      0,
      255
    );
```

---

## 22. Sensor-to-LED Architecture

```text
Potentiometer
      ↓
Analog Voltage
      ↓
ADC
      ↓
0–4095
      ↓
Mapping
      ↓
0–255
      ↓
PWM
      ↓
LED Brightness
```

The system can be represented as:

$$
\boxed{
x_{\mathrm{ADC}}
\rightarrow
x_{\mathrm{PWM}}
\rightarrow
\text{LED Brightness}
}
$$

---

## 23. Complete Sensor-Controlled LED Program

```cpp
#include <Arduino.h>

#define SENSOR_PIN 34
#define LED_PIN    25

const int pwmFrequency = 5000;
const int pwmResolution = 8;

void setup() {

  Serial.begin(115200);

  pinMode(
    SENSOR_PIN,
    INPUT
  );

  ledcAttach(
    LED_PIN,
    pwmFrequency,
    pwmResolution
  );
}

void loop() {

  int sensorValue =
      analogRead(
        SENSOR_PIN
      );

  int pwmValue =
      map(
        sensorValue,
        0,
        4095,
        0,
        255
      );

  ledcWrite(
    LED_PIN,
    pwmValue
  );

  Serial.print(
    "ADC = "
  );

  Serial.print(
    sensorValue
  );

  Serial.print(
    " | PWM = "
  );

  Serial.println(
    pwmValue
  );

  delay(50);
}
```

The exact PWM API may vary depending on the installed ESP32 Arduino Core version.

---

## 24. Expected Behavior

Rotate the potentiometer.

The result should approximately follow:

```text
ADC ≈ 0
   ↓
PWM ≈ 0
   ↓
LED OFF

ADC ≈ 2048
   ↓
PWM ≈ 128
   ↓
LED Medium Brightness

ADC ≈ 4095
   ↓
PWM ≈ 255
   ↓
LED Maximum Brightness
```

---

## 25. Experiment 7 — Measure Sensor-to-Output Relationship

Record at least five operating points.

| ADC Value | Approx. Voltage | PWM Value | Duty Cycle | LED Brightness |
|---:|---:|---:|---:|---|
| | | | | |
| | | | | |
| | | | | |
| | | | | |
| | | | | |

Calculate:

$$
D
= \frac{x_{\mathrm{PWM}}}{255}
\times100\%.
$$

---

## 26. Experiment 8 — Reverse the Control Response

Modify the system so that:

```text
Low Sensor Value
      ↓
Bright LED

High Sensor Value
      ↓
Dim LED
```

Use:

$$
x_{\mathrm{PWM}}
= 255-
\frac{255}{4095}x_{\mathrm{ADC}}.
$$

In code:

```cpp
int pwmValue =
    map(
      sensorValue,
      0,
      4095,
      255,
      0
    );
```

---

## 27. Experiment 9 — Threshold Control

Instead of continuous brightness control, divide the ADC range into regions.

For example:

$$
L(x)=
\begin{cases}
0, & x<1000,\\
128, & 1000\le x<3000,\\
255, & x\ge3000.
\end{cases}
$$

This gives:

```text
Low Sensor Level
      ↓
LED OFF

Medium Sensor Level
      ↓
LED Medium

High Sensor Level
      ↓
LED Bright
```

---

## 28. Sensor Classification Program

```cpp
if (
  sensorValue < 1000
) {

  pwmValue = 0;
}
else if (
  sensorValue < 3000
) {

  pwmValue = 128;
}
else {

  pwmValue = 255;
}
```

This introduces a simple form of **rule-based embedded control**.

---

## 29. Experiment 10 — Sensor Noise

Keep the potentiometer approximately fixed and observe several ADC readings.

You may see:

```text
2042
2047
2045
2051
2046
2049
```

even though the physical input appears unchanged.

Possible causes include:

- ADC quantization
- Electrical noise
- Power-supply noise
- Sensor noise
- Wiring effects

---

## 30. Moving-Average Filtering

A simple moving average can reduce sensor fluctuations.

For $N$ samples:

$$
\bar{x}
= \frac{1}{N}
\sum_{i=1}^{N}x_i.
$$

For example, with:

$$
N=5,
$$

use five ADC samples and calculate their average before updating the PWM output.

---

## 31. Experiment 11 — ADC Averaging

Example:

```cpp
int readAverageADC() {

  const int N = 5;

  long sum = 0;

  for (
    int i = 0;
    i < N;
    i++
  ) {

    sum +=
        analogRead(
          SENSOR_PIN
        );
  }

  return
      sum / N;
}
```

Then:

```cpp
int sensorValue =
    readAverageADC();
```

Compare the raw and averaged signals.

---

## 32. Optional Part — Servo Motor Control

A servo motor can also be controlled from a sensor value.

The architecture becomes:

```text
Potentiometer
      ↓
ADC
      ↓
0–4095
      ↓
Map
      ↓
0°–180°
      ↓
Servo Position
```

---

## 33. Servo Motor Principle

A hobby servo is commonly controlled using repetitive control pulses.

A typical educational model is:

```text
≈ 1.0 ms → Near 0°
≈ 1.5 ms → Near 90°
≈ 2.0 ms → Near 180°
```

with a repetition period around:

```text
20 ms
```

or approximately:

$$
50~\text{Hz}.
$$

Exact pulse-width limits vary by servo model.

---

## 34. Servo Wiring

For an SG90-style servo:

```text
Servo Red    → External 5 V
Servo Brown  → GND
Servo Orange → ESP32 GPIO26
```

Important:

```text
ESP32 GND
   │
   └──── Shared Ground ──── Servo Power GND
```

For reliable operation, avoid powering a servo directly from an ESP32 GPIO pin. A suitable external supply is preferred.

---

## 35. ADC-to-Servo Mapping

Map:

$$
0\leq x_{\mathrm{ADC}}\leq4095
$$

to:

$$
0^\circ\leq\theta\leq180^\circ.
$$

Use:

$$
\theta
= \frac{180}{4095}
x_{\mathrm{ADC}}.
$$

or:

```cpp
int angle =
    map(
      sensorValue,
      0,
      4095,
      0,
      180
    );
```

---

## 36. Experiment 12 — Potentiometer-Controlled Servo

Using a compatible servo library:

```cpp
#include <ESP32Servo.h>

#define SENSOR_PIN 34
#define SERVO_PIN  26

Servo myServo;

void setup() {

  Serial.begin(115200);

  myServo.attach(
    SERVO_PIN
  );
}

void loop() {

  int sensorValue =
      analogRead(
        SENSOR_PIN
      );

  int angle =
      map(
        sensorValue,
        0,
        4095,
        0,
        180
      );

  myServo.write(
    angle
  );

  Serial.print(
    "ADC = "
  );

  Serial.print(
    sensorValue
  );

  Serial.print(
    " | Angle = "
  );

  Serial.println(
    angle
  );

  delay(20);
}
```

The exact usable angle range and pulse width should be adjusted to match the servo.

---

## 37. Expected Servo Behavior

```text
Potentiometer Minimum
       ↓
ADC ≈ 0
       ↓
Servo ≈ 0°

Potentiometer Middle
       ↓
ADC ≈ 2048
       ↓
Servo ≈ 90°

Potentiometer Maximum
       ↓
ADC ≈ 4095
       ↓
Servo ≈ 180°
```

---

## 38. Experiment 13 — Limit Servo Motion

For mechanical safety, limit the operating range.

For example:

```text
20° ≤ Servo Angle ≤ 160°
```

Map:

```cpp
int angle =
    map(
      sensorValue,
      0,
      4095,
      20,
      160
    );
```

This can reduce the risk of driving the servo into mechanical end stops.

---

## 39. Experiment 14 — Compare LED and Servo Control

Use the same sensor reading for two actuators.

```text
                  ┌→ PWM LED Brightness
Sensor → ADC ─────┤
                  └→ Servo Position
```

For example:

```cpp
int pwmValue =
    map(
      sensorValue,
      0,
      4095,
      0,
      255
    );

int angle =
    map(
      sensorValue,
      0,
      4095,
      0,
      180
    );
```

This demonstrates that the same sensor information can control multiple output variables.

---

## 40. Input-Output Mathematical Model

For LED control:

$$
u_{\mathrm{LED}}
= \frac{255}{4095}
x_{\mathrm{ADC}}.
$$

For servo control:

$$
\theta
= \frac{180}{4095}
x_{\mathrm{ADC}}.
$$

The general mapping between two ranges is:

$$
y
= y_{\min}
+
\frac{x-x_{\min}}
{x_{\max}-x_{\min}}
\left(
y_{\max}-y_{\min}
\right).
$$

This equation is fundamental for sensor-to-actuator mapping.

---

## 41. Experiment 15 — Response-Time Observation

The approximate sensor-to-output response time is:

$$
T_{\mathrm{response}}
= t_{\mathrm{output}}
- t_{\mathrm{input}}.
$$

The total response can include:

$$
T_{\mathrm{response}}
= T_{\mathrm{ADC}}
+
T_{\mathrm{processing}}
+
T_{\mathrm{PWM/actuator}}.
$$

Observe whether increasing software delays makes the system feel less responsive.

---

## 42. ADC Sampling Rate

If the program reads the ADC every:

```text
100 ms
```

then the approximate sampling rate is:

$$
f_s
= \frac{1}{0.1}
= 10~\text{Hz}.
$$

If the sampling interval is:

```text
20 ms
```

then:

$$
f_s
= \frac{1}{0.02}
= 50~\text{Hz}.
$$

The sampling rate determines how quickly the application can respond to changes in sensor input.

---

## 43. Common Errors

### Using the Wrong ADC Pin

ADC capability varies among ESP32 families and boards.

Always verify the specific board pinout.

### Applying Excessive Voltage

Do not intentionally apply a voltage higher than the permitted GPIO/ADC input range.

For this lab, the potentiometer should operate between:

```text
0 V and 3.3 V
```

### Floating Analog Input

An unconnected ADC pin may produce unstable values.

Always connect the input to a defined source.

### Expecting Ideal ADC Accuracy

The expression:

$$
V=
\frac{x}{4095}
\times3.3
$$

is a useful approximation but does not provide laboratory-grade voltage accuracy without calibration.

### Confusing PWM with Analog Voltage

A PWM pin normally switches digitally between HIGH and LOW.

It does not directly generate a continuously variable analog voltage.

The average behavior is controlled through duty cycle.

### Servo Power Problems

Servo motors can draw much more current than an LED.

Symptoms of inadequate power can include:

```text
ESP32 Reset
Servo Jitter
Unstable Movement
Communication Failure
```

Use an appropriate external servo supply and common ground.

---

## 44. Experimental Summary

### Experiment 1 — ADC Reading

Read raw sensor values.

### Experiment 2 — Voltage Conversion

Estimate analog input voltage.

### Experiment 3 — Normalization

Convert ADC readings to $0$–$1$.

### Experiment 4 — PWM Generation

Configure PWM output.

### Experiment 5 — LED Brightness Levels

Test several duty cycles.

### Experiment 6 — ADC-to-PWM Control

Control LED brightness using the sensor.

### Experiment 7 — Input-Output Measurement

Record ADC, voltage, PWM, and brightness.

### Experiment 8 — Reverse Mapping

Invert the sensor-to-LED relationship.

### Experiment 9 — Threshold Control

Implement LOW, MEDIUM, and HIGH output levels.

### Experiment 10 — Sensor Noise

Observe ADC variations.

### Experiment 11 — Moving Average

Filter sensor readings.

### Experiment 12 — Servo Control

Map ADC input to servo angle.

### Experiment 13 — Servo Limiting

Restrict the mechanical range.

### Experiment 14 — Dual Actuation

Control LED and servo from one sensor.

### Experiment 15 — Response Analysis

Study sampling and system responsiveness.

---

## 45. Exercises

### Exercise 1 — Raw ADC

Read the potentiometer and display:

```text
ADC = xxxx
```

through the Serial Monitor.

### Exercise 2 — ADC Voltage

Display:

```text
ADC = xxxx | Voltage = x.xxx V
```

### Exercise 3 — LED Brightness

Set LED brightness to:

```text
0%
25%
50%
75%
100%
```

using PWM.

### Exercise 4 — Sensor-Controlled LED

Map:

$$
0\text{–}4095
\rightarrow
0\text{–}255.
$$

### Exercise 5 — Reverse LED Response

Make:

```text
Low ADC → Bright LED
High ADC → Dim LED
```

### Exercise 6 — Three-Level Controller

Implement:

$$
PWM=
\begin{cases}
0,&ADC<1000,\\
128,&1000\leq ADC<3000,\\
255,&ADC\geq3000.
\end{cases}
$$

### Exercise 7 — Moving Average

Average five ADC samples before updating the PWM value.

### Exercise 8 — Servo Control

Map:

$$
0\text{–}4095
\rightarrow
0^\circ\text{–}180^\circ.
$$

### Exercise 9 — Safe Servo Range

Modify the servo system to operate only from:

```text
20° to 160°
```

### Exercise 10 — Sampling Rate

Test sensor update intervals of:

```text
20 ms
100 ms
500 ms
```

and discuss the effect on system response.

---

## 46. Checkpoint Questions

1. What does ADC stand for?
2. Why is an ADC required for analog sensors?
3. What is ADC resolution?
4. How many ideal levels are available from a 12-bit ADC?
5. What is the commonly used raw range for a 12-bit ADC?
6. What is quantization?
7. What is ADC voltage resolution?
8. Why is the calculated ESP32 input voltage only approximate?
9. What is a potentiometer?
10. What is PWM?
11. What is PWM duty cycle?
12. How is duty cycle calculated?
13. What does 0% duty cycle represent?
14. What does 50% duty cycle represent?
15. What does 100% duty cycle represent?
16. Why can PWM control perceived LED brightness?
17. What is PWM resolution?
18. How many PWM levels are available with 8-bit resolution?
19. How can a 12-bit ADC value be mapped to an 8-bit PWM value?
20. What PWM value corresponds approximately to 50% for an 8-bit output?
21. What is the difference between PWM and a true analog voltage?
22. Why may ADC values fluctuate even with a constant input?
23. What is a moving-average filter?
24. How does increasing $N$ in a moving average affect noise and responsiveness?
25. How can ADC input control a servo?
26. What is the approximate center pulse width of a typical hobby servo?
27. Why should a servo generally use an appropriate external supply?
28. Why must the ESP32 and external servo supply share a common ground?
29. What is sensor-to-actuator mapping?
30. How does sampling interval affect control-system responsiveness?

---

## 47. Expected Results

Students should successfully implement:

```text
Analog Sensor
      ↓
ADC
      ↓
Digital Measurement
```

and:

```text
PWM
 ↓
LED Brightness
```

The two functions are then combined:

```text
Sensor
   ↓
ADC
   ↓
Scaling
   ↓
PWM
   ↓
LED
```

or optionally:

```text
Sensor
   ↓
ADC
   ↓
Scaling
   ↓
Servo Command
   ↓
Servo Position
```

The core relationship is:

$$
\boxed{
\text{Sensor Measurement}
+
\text{Signal Processing}
+
\text{Actuator Control}
}
$$

---

## 48. Result Tables

### ADC Measurements

| Test | ADC Value | Approx. Voltage | Normalized Value |
|---:|---:|---:|---:|
| 1 | | | |
| 2 | | | |
| 3 | | | |
| 4 | | | |
| 5 | | | |

### PWM Measurements

| ADC Value | PWM Value | Duty Cycle | LED Observation |
|---:|---:|---:|---|
| | | | |
| | | | |
| | | | |
| | | | |
| | | | |

### Servo Measurements

| ADC Value | Calculated Angle | Observed Servo Position |
|---:|---:|---|
| 0 | 0° | |
| 1024 | $\approx45^\circ$ | |
| 2048 | $\approx90^\circ$ | |
| 3072 | $\approx135^\circ$ | |
| 4095 | 180° | |

---

## 49. Discussion

Students should explain how ADC and PWM perform complementary roles in an embedded system.

ADC provides the path:

```text
Physical Quantity
      ↓
Analog Signal
      ↓
Digital Information
```

while PWM provides the path:

```text
Digital Control Value
      ↓
PWM Signal
      ↓
Physical Output
```

Together:

```text
Physical Input
      ↓
ADC
      ↓
Digital Processing
      ↓
PWM
      ↓
Physical Output
```

The overall embedded-system concept is therefore:

$$
\boxed{
\text{Physical World}
\rightarrow
\text{ADC}
\rightarrow
\text{Computation}
\rightarrow
\text{PWM}
\rightarrow
\text{Physical World}
}
$$

Students should also discuss ADC resolution, measurement noise, filtering, PWM resolution, actuator response, and the trade-off between smoothness and responsiveness.

---

## 50. Submission

Submit:

- ADC sensor-reading source code.
- Serial Monitor screenshot showing raw ADC values.
- ADC voltage-conversion results.
- PWM LED-brightness source code.
- Duty-cycle observations.
- Sensor-controlled LED program.
- Completed ADC-to-PWM result table.
- Moving-average implementation.
- Optional servo-control program.
- Servo angle measurements, if performed.
- Answers to checkpoint questions.
- Brief discussion of ADC resolution, PWM duty cycle, noise, filtering, and sensor-to-actuator mapping.

---

## 51. Conclusion

In this lab, an analog sensor is connected to the ESP32 ADC and converted into a digital measurement. The ADC data is then scaled and used to control an actuator through PWM.

For LED control:

$$
x_{\mathrm{ADC}}
\rightarrow
x_{\mathrm{PWM}}
\rightarrow
\text{Brightness}.
$$

For servo control:

$$
x_{\mathrm{ADC}}
\rightarrow
\theta
\rightarrow
\text{Servo Position}.
$$

The progression of the MCU laboratories is:

```text
LAB 1
GPIO Input / Output
      ↓
Polling
      ↓
LAB 2
Interrupts
      ↓
Timers
      ↓
LAB 3
ADC
      ↓
PWM
      ↓
Sensor-to-Actuator Control
```

The key principle is:

$$
\boxed{
\text{Embedded Control}
= \text{Sensing}
+
\text{Computation}
+
\text{Actuation}
}
$$

This lab prepares students for the next stage, where **I²C and SPI communication** can be used to interface digital sensors and peripheral devices.
