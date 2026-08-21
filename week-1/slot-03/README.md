
# 📘 Theory: ADC Operation, Resolution, Sampling, and PWM Duty-Cycle Control

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Microcontrollers operate primarily with **digital data**, while many physical quantities in the real world are analog. Temperature, light intensity, pressure, sound, battery voltage, and sensor outputs may vary continuously with time.

The ESP32 provides hardware peripherals that allow the digital processor to interact with these signals. Two important mechanisms are:

- **Analog-to-Digital Conversion (ADC)** — converts an analog voltage into a digital value.
- **Pulse-Width Modulation (PWM)** — generates a digital pulse waveform whose duty cycle can be controlled by software.

The overall relationship can be represented as

$$
\boxed{
\text{Analog World}
\rightarrow
\text{ADC}
\rightarrow
\text{ESP32 Processing}
\rightarrow
\text{PWM}
\rightarrow
\text{Physical Output}
}
$$

For example:

```text
Light Sensor
     │
     ▼
Analog Voltage
     │
     ▼
    ADC
     │
     ▼
   ESP32
     │
     ▼
PWM Generator
     │
     ▼
LED Brightness
```

This chapter introduces **ADC operation, ADC resolution, quantization, sampling, sampling rate, PWM generation, PWM frequency, and duty-cycle control** using the ESP32 as the MCU case study.

---

# 🔌 2. Analog and Digital Signals

## 2.1 Analog Signal

An analog signal can take a continuous range of values.

Examples include:

- temperature,
- light intensity,
- microphone voltage,
- pressure,
- battery voltage,
- potentiometer position.

Conceptually:

```text
Voltage
  ▲
  │        ╭────╮
  │      ╭─╯    ╰─╮
  │    ╭─╯        ╰─╮
  │  ╭─╯            ╰─
  └────────────────────► Time
```

An analog signal may be represented as

$$
v(t).
$$

---

## 2.2 Digital Signal

A digital signal has discrete states.

For binary digital logic:

$$
x\in\{0,1\}.
$$

Conceptually:

```text
HIGH ─────┐      ┌────────
          │      │
LOW       └──────┘
```

The ESP32 CPU processes information digitally, so analog sensor signals must first be converted into numerical values.

---

# 🧠 3. What Is an ADC?

An **Analog-to-Digital Converter (ADC)** converts an analog input voltage into a digital number.

The conversion process can be represented as

$$
\boxed{
\text{Analog Voltage}
\rightarrow
\text{Sampling}
\rightarrow
\text{Quantization}
\rightarrow
\text{Digital Code}
}
$$

For example:

```text
Potentiometer
     │
     ▼
0–3.3 V
     │
     ▼
ESP32 ADC
     │
     ▼
Digital Value
     │
     ▼
Application
```

The CPU can then use the digital value for calculations, control decisions, communication, or data logging.

---

# ⚙️ 4. ADC Operation

A simplified ADC operation consists of three important processes:

### Step 1 — Sampling

The ADC measures the analog input at a particular instant.

### Step 2 — Quantization

The measured voltage is assigned to one of a finite number of discrete levels.

### Step 3 — Encoding

The quantized level is represented as a binary number.

Thus,

$$
\boxed{
V_{IN}
\rightarrow
\text{Sample}
\rightarrow
\text{Quantize}
\rightarrow
D
}
$$

where:

- $V_{IN}$ = analog input voltage,
- $D$ = digital output code.

---

# 🔢 5. ADC Resolution

ADC **resolution** determines how many discrete digital levels can represent the analog input range.

For an $N$-bit ADC,

$$
L=2^N
$$

where $L$ is the number of quantization levels.

Examples:

| ADC Resolution | Number of Levels |
|---:|---:|
| 8 bit | 256 |
| 10 bit | 1,024 |
| 12 bit | 4,096 |
| 16 bit | 65,536 |

For a nominal 12-bit conversion,

$$
2^{12}=4096.
$$

The digital code therefore spans nominally

$$
0\text{ to }4095.
$$

---

# 📏 6. ADC Voltage Resolution

Suppose the ADC input range is represented by

$$
V_{FS}.
$$

The approximate voltage represented by one least significant bit (LSB) is

$$
V_{LSB}
\approx
\frac{V_{FS}}{2^N}.
$$

For example, if a simplified ideal 12-bit ADC model uses

$$
V_{FS}=3.3~V,
$$

then

$$
V_{LSB}=
\frac{3.3}{4096}.
$$

Therefore,

$$
V_{LSB}
\approx
0.000806~V
$$

or

$$
\boxed{
V_{LSB}\approx0.806~mV
}
$$

in this idealized example.

In practice, the usable ADC input range, attenuation configuration, calibration, and non-idealities of a particular ESP32 variant must be considered rather than assuming that every ADC configuration maps ideally to exactly 0–3.3 V.

---

# 🔢 7. Analog Voltage to Digital Code

For an ideal ADC, an approximate relationship is

$$
D
\approx
\frac{V_{IN}}{V_{FS}}
(2^N-1).
$$

For a 12-bit ADC,

$$
D
\approx
\frac{V_{IN}}{V_{FS}}
4095.
$$

Suppose

$$
V_{IN}=1.65~V
$$

and an idealized full-scale value is

$$
V_{FS}=3.3~V.
$$

Then

$$
D
\approx
\frac{1.65}{3.3}
\times4095
$$

$$
D\approx2047.5.
$$

Thus, approximately,

$$
\boxed{D\approx2048}
$$

which corresponds to about half of the digital range.

---

# 🔄 8. Digital Code to Approximate Voltage

The reverse conversion can be estimated by

$$
V_{IN}
\approx
\frac{D}{2^N-1}
V_{FS}.
$$

For a 12-bit ADC:

$$
V_{IN}
\approx
\frac{D}{4095}
V_{FS}.
$$

If

$$
D=1024
$$

and

$$
V_{FS}=3.3~V,
$$

then

$$
V_{IN}
\approx
\frac{1024}{4095}
(3.3)
$$

$$
V_{IN}\approx0.825~V.
$$

Again, this is an ideal conversion model. Calibration may be required for accurate ESP32 voltage measurement.

---

# 📊 9. ADC Resolution Comparison

Suppose the same 3.3 V idealized input range is used.

| Resolution | Levels | Approx. LSB |
|---:|---:|---:|
| 8 bit | 256 | 12.89 mV |
| 10 bit | 1,024 | 3.22 mV |
| 12 bit | 4,096 | 0.806 mV |

Increasing the number of bits produces smaller quantization steps:

$$
\boxed{
\text{Higher Resolution}
\Rightarrow
\text{Smaller Quantization Step}
}
$$

---

# 🪜 10. Quantization

An ADC cannot represent every possible analog voltage exactly.

Instead, the continuous analog range is divided into discrete intervals.

```text
Digital
Code
 ▲
 │                ┌────
 │           ┌────┘
 │      ┌────┘
 │ ┌────┘
 └──────────────────────► Analog Voltage
```

This staircase behavior is called **quantization**.

The difference between the actual analog value and its quantized representation is called **quantization error**.

For an ideal uniform ADC, it is commonly bounded approximately by

$$
-\frac{1}{2}LSB
\leq
e_q
\leq
+\frac{1}{2}LSB.
$$

---

# 📉 11. ADC Accuracy vs. Resolution

**Resolution** and **accuracy** are not the same.

Resolution describes the number of available digital levels:

$$
\text{Resolution}\rightarrow2^N.
$$

Accuracy describes how closely the measured result corresponds to the actual physical voltage.

ADC accuracy can be affected by:

- reference variation,
- electrical noise,
- ADC nonlinearity,
- offset error,
- gain error,
- source impedance,
- temperature,
- calibration,
- power-supply noise.

Therefore,

$$
\boxed{
\text{High Resolution}
\neq
\text{Automatically High Accuracy}
}
$$

---

# 🧪 12. ESP32 ADC Reading with Arduino

A basic ADC reading can be obtained using:

```cpp
#define ADC_PIN 34

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    int adcValue = analogRead(ADC_PIN);

    Serial.println(adcValue);

    delay(500);
}
```

Conceptually:

```text
Analog Sensor
     │
     ▼
ADC GPIO
     │
     ▼
analogRead()
     │
     ▼
Digital Value
     │
     ▼
Serial Monitor
```

The exact ADC-capable GPIOs depend on the ESP32 family and development board.

---

# 🎚️ 13. Potentiometer as an ADC Input

A potentiometer provides a simple adjustable analog voltage.

```text
3.3 V
  │
  │
 ┌┴┐
 │ │ Potentiometer
 │◄├────────► ADC GPIO
 │ │
 └┬┘
  │
 GND
```

Rotating the potentiometer changes the ADC input voltage.

Conceptually:

```text
Minimum → ADC ≈ 0

Middle  → ADC ≈ Mid-scale

Maximum → ADC ≈ Full-scale
```

subject to the actual configured ADC range.

---

# ⏱️ 14. Sampling

An analog signal is continuous:

$$
x(t).
$$

The ADC measures it at discrete times:

$$
x[n]=x(nT_s)
$$

where:

- $T_s$ = sampling period,
- $f_s$ = sampling frequency.

The relationship is

$$
f_s=\frac{1}{T_s}.
$$

---

# 📈 15. Sampling Example

Suppose the ADC reads a sensor every

$$
T_s=10~ms.
$$

Then

$$
T_s=0.01~s
$$

and

$$
f_s =
\frac{1}{0.01}=
100~Hz.
$$

Therefore,

$$
\boxed{
f_s=100~samples/s
}
$$

---

# 🧠 16. Why Sampling Rate Matters

If sampling is too slow, rapid changes in the input signal may not be represented correctly.

Consider:

```text
Analog Signal
      ╭──╮    ╭──╮
     ╱    ╲  ╱    ╲
────╯      ╰╯      ╰────

Samples
  •       •       •
```

Too few samples can lose important information.

A higher sampling rate provides more temporal information:

```text
• • • • • • • • • • • •
```

However, higher sampling rates also increase:

- CPU workload,
- memory requirements,
- communication bandwidth,
- storage requirements,
- power consumption.

---

# 📐 17. Nyquist Sampling Principle

For a band-limited signal with highest frequency component

$$
f_{max},
$$

the theoretical sampling requirement is

$$
f_s>2f_{max}.
$$

This is associated with the **Nyquist sampling criterion**.

For example, if

$$
f_{max}=1~kHz,
$$

then theoretically

$$
f_s>2~kHz.
$$

Practical systems normally use additional margin and appropriate analog filtering.

---

# ⚠️ 18. Aliasing

When a signal is sampled too slowly, higher-frequency components may appear as incorrect lower-frequency components.

This phenomenon is called **aliasing**.

Conceptually:

$$
\boxed{
\text{Insufficient Sampling Rate}
\rightarrow
\text{Aliasing}
\rightarrow
\text{Incorrect Digital Signal}
}
$$

An **anti-aliasing filter** is often placed before an ADC:

```text
Analog
Signal
  │
  ▼
Low-Pass
Filter
  │
  ▼
 ADC
  │
  ▼
Digital
Samples
```

---

# 🔄 19. ADC Sampling Loop

A simple Arduino implementation might be:

```cpp
#define ADC_PIN 34

void loop()
{
    int sample = analogRead(ADC_PIN);

    Serial.println(sample);

    delay(10);
}
```

The approximate software sampling period is

$$
T_s\approx10~ms + T_{\text{processing}},
$$

so `delay(10)` alone does not guarantee a precise 100 Hz sampling frequency.

For more accurate periodic sampling, hardware timers or dedicated peripheral mechanisms are preferable.

---

# 💡 20. What Is PWM?

**Pulse-Width Modulation (PWM)** controls the proportion of time that a digital output remains HIGH during each waveform period.

A PWM waveform alternates between

$$
HIGH
$$

and

$$
LOW.
$$

Example:

```text
HIGH ┌────┐      ┌────┐
     │    │      │    │
LOW ─┘    └──────┘    └──────
```

The important parameters are:

- frequency,
- period,
- duty cycle.

---

# ⏱️ 21. PWM Period and Frequency

PWM frequency is related to the waveform period by

$$
f_{PWM}=
\frac{1}{T_{PWM}}.
$$

For example, if

$$
T_{PWM}=1~ms,
$$

then

$$
f_{PWM}=
\frac{1}{0.001}=
1000~Hz.
$$

Therefore,

$$
\boxed{
f_{PWM}=1~kHz
}
$$

---

# 📊 22. PWM Duty Cycle

Duty cycle represents the percentage of one PWM period for which the signal is HIGH.

$$
D =
\frac{T_{ON}}{T_{PWM}}
\times100\%.
$$

Since

$$
T_{PWM}=T_{ON}+T_{OFF},
$$

we can also write

$$
D =
\frac{T_{ON}}
{T_{ON}+T_{OFF}}
\times100\%.
$$

---

# 🔢 23. PWM Duty-Cycle Examples

### 0% Duty Cycle

```text
LOW  ─────────────────────
```

$$
D=0\%
$$

### 25% Duty Cycle

```text
HIGH ┌─┐       ┌─┐
     │ │       │ │
LOW ─┘ └───────┘ └───────
```

$$
D=25\%
$$

### 50% Duty Cycle

```text
HIGH ┌────┐    ┌────┐
     │    │    │    │
LOW ─┘    └────┘    └────
```

$$
D=50\%
$$

### 75% Duty Cycle

```text
HIGH ┌──────┐  ┌──────┐
     │      │  │      │
LOW ─┘      └──┘      └──
```

$$
D=75\%
$$

### 100% Duty Cycle

```text
HIGH ─────────────────────
```

$$
D=100\%
$$

---

# ⚡ 24. Average PWM Output

For an ideal PWM waveform switching between 0 and $V_{HIGH}$, the average value is

$$
V_{AVG} =
D V_{HIGH}
$$

where $D$ is expressed from 0 to 1.

If

$$
V_{HIGH}=3.3~V
$$

and

$$
D=0.5,
$$

then

$$
V_{AVG} =
0.5(3.3) =
1.65~V.
$$

Thus,

$$
\boxed{
V_{AVG}=1.65~V
}
$$

This does not mean that the GPIO directly outputs a constant 1.65 V; it rapidly switches between LOW and HIGH. The average effect depends on the load and filtering.

---

# 💡 25. PWM for LED Brightness

PWM can control perceived LED brightness.

Conceptually:

$$
\boxed{
\text{Higher Duty Cycle}
\Rightarrow
\text{Higher Average LED Current}
\Rightarrow
\text{Brighter LED}
}
$$

Example:

| Duty Cycle | Approximate LED Effect |
|---:|---|
| 0% | OFF |
| 25% | Dim |
| 50% | Medium |
| 75% | Bright |
| 100% | Maximum |

The perceived brightness of the human eye is not perfectly linear with duty cycle.

---

# ⚙️ 26. ESP32 PWM Hardware

ESP32-family devices provide hardware peripherals capable of generating PWM signals. A commonly used PWM peripheral in ESP32 Arduino applications is the **LEDC** peripheral.

Conceptually:

```text
ESP32 Clock
     │
     ▼
PWM Peripheral
     │
 ┌───┴────┐
 │        │
 ▼        ▼
Frequency Duty Cycle
 │        │
 └───┬────┘
     ▼
 GPIO
     │
     ▼
LED / Actuator
```

Using hardware PWM means the CPU does not need to manually toggle the pin continuously.

---

# 🔢 27. PWM Resolution

PWM resolution determines the number of available duty-cycle steps.

For $N$-bit PWM:

$$
L=2^N.
$$

Examples:

| PWM Resolution | Duty Levels |
|---:|---:|
| 8 bit | 256 |
| 10 bit | 1,024 |
| 12 bit | 4,096 |

For 8-bit PWM, a common numerical duty range is

$$
0\text{ to }255.
$$

An approximate percentage mapping is

$$
D(\%) =
\frac{\text{PWM Code}}
{2^N-1}
\times100.
$$

---

# 🧮 28. PWM Code Example

For an 8-bit PWM,

$$
2^8-1=255.
$$

For approximately 50% duty cycle:

$$
PWM
\approx
0.5\times255
$$

$$
PWM\approx128.
$$

Therefore,

$$
\boxed{
PWM\ Code\approx128
}
$$

corresponds to approximately 50% duty cycle.

---

# 💻 29. ESP32 PWM Concept in Arduino

The exact LEDC API can differ between Arduino-ESP32 core versions, so students should check the API corresponding to their installed version.

A conceptual structure is:

```cpp
#define LED_PIN 2

void setup()
{
    // Configure PWM frequency
    // Configure PWM resolution
    // Attach PWM to LED_PIN
}

void loop()
{
    // Set PWM duty cycle
}
```

The essential hardware relationship is

$$
\boxed{
\text{Duty Code}
\rightarrow
\text{PWM Peripheral}
\rightarrow
\text{GPIO}
\rightarrow
\text{Load}
}
$$

---

# 🌗 30. LED Fade with PWM

An LED can be gradually brightened by increasing the duty-cycle code.

Conceptually:

```cpp
for (int duty = 0; duty <= 255; duty++)
{
    // Set PWM duty
    delay(10);
}
```

The sequence is

```text
Duty = 0
   │
   ▼
Duty = 1
   │
   ▼
Duty = 2
   │
   ▼
  ...
   │
   ▼
Duty = 255
```

which produces a gradual brightness increase.

---

# 🔄 31. ADC-to-PWM Control

One of the most useful introductory MCU applications combines ADC and PWM.

For example:

```text
Potentiometer
     │
     ▼
Analog Voltage
     │
     ▼
ESP32 ADC
     │
     ▼
Digital ADC Value
     │
     ▼
Scaling
     │
     ▼
PWM Duty Cycle
     │
     ▼
LED Brightness
```

The ADC acts as the **input**, while PWM acts as the **output**.

---

# 🧮 32. ADC-to-PWM Mapping

Suppose a nominal 12-bit ADC produces

$$
ADC\in[0,4095]
$$

and an 8-bit PWM uses

$$
PWM\in[0,255].
$$

A simple mapping is

$$
PWM=
ADC
\frac{255}{4095}.
$$

For example, if

$$
ADC=2048,
$$

then

$$
PWM=
2048
\frac{255}{4095}
$$

$$
PWM\approx128.
$$

Therefore,

$$
\boxed{
ADC\approx50\%
\Rightarrow
PWM\approx50\%
}
$$

---

# 💻 33. ADC-to-PWM Program Structure

A conceptual ESP32 Arduino program is:

```cpp
#define ADC_PIN 34
#define LED_PIN 2

void setup()
{
    Serial.begin(115200);

    // Configure ADC if required
    // Configure PWM peripheral
}

void loop()
{
    int adcValue =
        analogRead(ADC_PIN);

    int pwmValue =
        map(
            adcValue,
            0,
            4095,
            0,
            255
        );

    // Write pwmValue to PWM hardware

    Serial.print("ADC = ");
    Serial.print(adcValue);

    Serial.print("  PWM = ");
    Serial.println(pwmValue);

    delay(20);
}
```

This demonstrates the fundamental embedded-control relationship

$$
\boxed{
\text{Sense}
\rightarrow
\text{Convert}
\rightarrow
\text{Process}
\rightarrow
\text{Control}
}
$$

---

# 🎛️ 34. Closed-Loop Perspective

ADC and PWM are also fundamental components of feedback-control systems.

```text
          ┌─────────────────────┐
          │                     │
          ▼                     │
      ADC Input                 │
          │                     │
          ▼                     │
      ESP32 MCU                 │
          │                     │
          ▼                     │
     Control Algorithm          │
          │                     │
          ▼                     │
         PWM                    │
          │                     │
          ▼                     │
       Actuator                 │
          │                     │
          ▼                     │
       Physical System ─────────┘
```

The MCU can measure the system using the ADC and modify an actuator using PWM.

---

# 🔧 35. PWM Applications

PWM is widely used for:

- LED brightness control,
- DC motor speed control,
- power conversion,
- heater control,
- buzzer/tone generation,
- waveform generation,
- actuator control.

Some devices, such as hobby servos, use pulse-width-based command signals with specific timing requirements rather than generic duty-cycle power control.

---

# ⚠️ 36. PWM Is Not a True DAC

A PWM output is fundamentally digital:

$$
V_{OUT}\in\{0,V_{HIGH}\}.
$$

It does not directly generate every intermediate voltage.

For example, 50% duty cycle produces:

```text
3.3 V ┌────┐    ┌────┐
      │    │    │    │
0 V ──┘    └────┘    └────
```

rather than a constant 1.65 V.

However, an appropriate low-pass filter can convert the PWM waveform into a smoother average voltage.

---

# 🔧 37. PWM Low-Pass Filtering

A basic RC low-pass filter can be represented as:

```text
PWM ─── R ───┬──── VOUT
             │
             C
             │
            GND
```

The filter reduces high-frequency switching components.

Conceptually:

$$
\boxed{
\text{PWM}
\rightarrow
\text{Low-Pass Filter}
\rightarrow
\text{Approximate Analog Voltage}
}
$$

---

# 📊 38. ADC and PWM Comparison

| Characteristic | ADC | PWM |
|---|---|---|
| Main direction | Analog → Digital | Digital pulses → controlled average effect |
| Primary role | Measurement | Control |
| Important parameter | Resolution | Duty cycle / frequency |
| Input/output | Analog input | Digital output |
| Typical device | Sensor | LED / motor driver |
| Example Arduino concept | `analogRead()` | LEDC/PWM API |

Thus,

$$
\boxed{
ADC=\text{Input Interface}
}
$$

and

$$
\boxed{
PWM=\text{Output-Control Interface}
}
$$

---

# ⏱️ 39. Sampling Frequency vs. PWM Frequency

These frequencies describe different processes.

### ADC Sampling Frequency

$$
f_s =
\text{number of ADC samples per second}
$$

### PWM Frequency

$$
f_{PWM} =
\text{number of PWM periods per second}
$$

They should not be confused.

For example:

```text
ADC Sampling = 100 Hz

PWM Output   = 5 kHz
```

can coexist in the same application.

---

# 🧠 40. ADC–Processing–PWM Architecture

A common embedded-system structure is

```text
Physical
Quantity
   │
   ▼
Sensor
   │
   ▼
Analog Voltage
   │
   ▼
  ADC
   │
   ▼
Digital Data
   │
   ▼
Algorithm
   │
   ▼
PWM Duty
   │
   ▼
Actuator
```

Mathematically:

$$
\boxed{
x(t)
\rightarrow
ADC
\rightarrow
x[n]
\rightarrow
f(x[n])
\rightarrow
PWM
}
$$

where $f(\cdot)$ represents the application or control algorithm.

---

# 🔬 41. Example: Smart Light Controller

Consider a light-dependent sensor connected to the ADC and an LED controlled by PWM.

```text
Ambient Light
      │
      ▼
Light Sensor
      │
      ▼
Analog Voltage
      │
      ▼
     ADC
      │
      ▼
    ESP32
      │
      ▼
Control Mapping
      │
      ▼
     PWM
      │
      ▼
LED Brightness
```

A control rule might be

$$
D_{PWM} =
1-
\frac{ADC}{ADC_{MAX}}.
$$

Therefore:

- bright environment → lower LED duty cycle,
- dark environment → higher LED duty cycle.

---

# 📉 42. ADC Noise and Averaging

Real ADC measurements may fluctuate because of noise.

For example:

```text
2048
2052
2045
2050
2047
```

One simple method for reducing random variation is averaging.

For $N$ samples:

$$
\bar{x} =
\frac{1}{N}
\sum_{i=1}^{N}x_i.
$$

Example:

```cpp
long sum = 0;

for (int i = 0; i < 10; i++)
{
    sum += analogRead(ADC_PIN);
}

int average = sum / 10;
```

Averaging can reduce random noise but also introduces additional processing and delay.

---

# ⚡ 43. Sampling and Real-Time Processing

An embedded application often performs the following sequence:

```text
Timer Event
    │
    ▼
Sample ADC
    │
    ▼
Process Sample
    │
    ▼
Calculate PWM
    │
    ▼
Update Output
    │
    ▼
Wait for Next Sample
```

For a sampling period $T_s$, the computation should normally finish before the next required sample:

$$
T_{ADC}
+
T_{process}
+
T_{output}
<
T_s.
$$

This introduces an important real-time design constraint.

---

# 🧮 44. Example Design Calculation

Suppose a control application requires:

$$
f_s=100~Hz.
$$

Then

$$
T_s =
\frac{1}{100} =
0.01~s =
10~ms.
$$

The ADC measurement, processing, and PWM update should therefore complete within the available sampling interval.

Conceptually:

```text
|<---------- 10 ms ---------->|

ADC → Process → PWM → Idle/Other Work
```

---

# 🆚 45. ADC, DAC, and PWM

| Feature | ADC | DAC | PWM |
|---|---|---|---|
| Conversion | Analog → Digital | Digital → Analog | Digital → Pulse waveform |
| Input | Voltage | Digital code | Duty/frequency parameters |
| Output | Digital code | Analog voltage/current | HIGH/LOW pulses |
| Main purpose | Measurement | Analog generation | Power/control modulation |
| Filtering required | Sometimes at input | Usually no | Often for analog-like output |

Not all ESP32 variants provide the same DAC hardware, so board-specific documentation should be checked.

---

# ⚙️ 46. Register-Level Perspective

At the register level, ADC and PWM peripherals are controlled through memory-mapped registers.

Conceptually:

```text
Application
    │
    ▼
Arduino API / HAL
    │
    ▼
Peripheral Driver
    │
    ▼
Memory-Mapped Registers
    │
 ┌──┴─────┐
 ▼        ▼
ADC      PWM
```

The software abstraction can therefore be represented as

$$
\boxed{
\text{Application}
\rightarrow
\text{HAL}
\rightarrow
\text{Registers}
\rightarrow
\text{Peripheral Hardware}
}
$$

---

# 🧰 47. Arduino API vs. Register-Level Control

| Characteristic | Arduino/HAL | Register-Level |
|---|---|---|
| Ease of programming | High | Low |
| Development time | Short | Longer |
| Hardware knowledge | Moderate | High |
| Portability | Higher | Lower |
| Fine timing control | Moderate | High |
| Peripheral control | Convenient | Detailed |
| Typical use | Applications / education | Drivers / optimization |

For introductory ADC and PWM experiments, the HAL/API approach is generally preferred.

---

# 💬 48. Discussion Questions

1. What is an ADC?
2. Why does an MCU need an ADC?
3. What is ADC resolution?
4. How many levels are available in a 12-bit ADC?
5. What is an LSB?
6. What is quantization?
7. What is quantization error?
8. What is the difference between resolution and accuracy?
9. What is sampling?
10. What is sampling frequency?
11. What is the Nyquist sampling principle?
12. What is aliasing?
13. Why might an anti-aliasing filter be required?
14. What is PWM?
15. What is PWM frequency?
16. What is duty cycle?
17. How does PWM control LED brightness?
18. Is PWM the same as a true analog voltage?
19. What is the difference between ADC sampling frequency and PWM frequency?
20. How can ADC and PWM be combined in an embedded control system?

---

# 🧠 49. Exercises

### Exercise 1 — ADC Resolution

Calculate the number of quantization levels for:

- 8-bit ADC,
- 10-bit ADC,
- 12-bit ADC,
- 16-bit ADC.

Use

$$
L=2^N.
$$

---

### Exercise 2 — ADC Voltage

Assuming an idealized 12-bit, 3.3 V mapping, calculate the approximate digital code for

$$
V_{IN}=2.0~V.
$$

Use

$$
D =
\frac{V_{IN}}{V_{FS}}
(2^N-1).
$$

---

### Exercise 3 — Sampling

Calculate the sampling frequency for:

$$
T_s=5~ms.
$$

Use

$$
f_s=\frac{1}{T_s}.
$$

---

### Exercise 4 — PWM Duty Cycle

Calculate the duty cycle if

$$
T_{ON}=3~ms
$$

and

$$
T_{PWM}=10~ms.
$$

---

### Exercise 5 — PWM Code

For 8-bit PWM, calculate the approximate PWM code corresponding to:

- 25%,
- 50%,
- 75%,
- 100%.

---

### Exercise 6 — ADC-to-PWM Mapping

Map

$$
ADC=3000
$$

from the nominal range

$$
0\text{--}4095
$$

to an 8-bit PWM range

$$
0\text{--}255.
$$

---

# 🚀 50. Advanced Exercise: ESP32 Analog Control System

Design an ESP32 application using:

- one potentiometer,
- one ADC input,
- one PWM output,
- one LED,
- Serial Monitor.

The system should operate as:

```text
Potentiometer
     │
     ▼
ADC Input
     │
     ▼
ESP32
     │
     ├────► Serial Monitor
     │
     ▼
ADC-to-PWM Mapping
     │
     ▼
PWM Output
     │
     ▼
LED Brightness
```

The desired relationship is

$$
\boxed{
\text{Potentiometer Position}
\rightarrow
\text{ADC Value}
\rightarrow
\text{PWM Duty Cycle}
\rightarrow
\text{LED Brightness}
}
$$

Students should measure and record:

| Potentiometer | ADC Value | PWM Value | Duty Cycle | LED |
|---|---:|---:|---:|---|
| Minimum | | | | |
| 25% | | | | |
| 50% | | | | |
| 75% | | | | |
| Maximum | | | | |

---

# 🧾 51. Expected Learning Outcomes

After studying this theory, students should be able to:

- Explain analog-to-digital conversion.
- Describe ADC sampling and quantization.
- Calculate ADC resolution and quantization levels.
- Convert between ideal ADC codes and voltages.
- Explain the difference between ADC resolution and accuracy.
- Calculate sampling frequency and period.
- Explain the Nyquist sampling principle and aliasing.
- Explain PWM generation.
- Calculate PWM period, frequency, and duty cycle.
- Explain PWM resolution.
- Control output power or LED brightness through PWM.
- Map ADC input values to PWM output values.
- Explain the role of ADC and PWM in embedded control systems.
- Relate Arduino/HAL functions to ESP32 hardware peripherals.

---

# 📘 52. References

1. Espressif Systems, *ESP32 Series Datasheet*.
2. Espressif Systems, *ESP32 Technical Reference Manual*.
3. Espressif Systems, *ESP-IDF Programming Guide — Analog to Digital Converter (ADC)*.
4. Espressif Systems, *ESP-IDF Programming Guide — LED Control (LEDC)*.
5. Espressif Systems, *Arduino Core for ESP32 Documentation*.
6. Arduino, *Arduino Language Reference*.
7. J. W. Valvano, *Embedded Systems: Real-Time Interfacing to ARM Cortex-M Microcontrollers*.
8. M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly.

---

# 🔑 53. Key Concepts

### ADC

$$
\boxed{
\text{Analog Signal}
\rightarrow
\text{Sampling}
\rightarrow
\text{Quantization}
\rightarrow
\text{Digital Data}
}
$$

### ADC Resolution

$$
\boxed{
L=2^N
}
$$

### Sampling

$$
\boxed{
f_s=\frac{1}{T_s}
}
$$

### PWM

$$
\boxed{
\text{Digital Output}
\rightarrow
\text{Pulse Width}
\rightarrow
\text{Average Output Effect}
}
$$

### PWM Duty Cycle

$$
\boxed{
D=
\frac{T_{ON}}{T_{PWM}}
\times100\%
}
$$

### ADC-to-PWM Embedded Control

$$
\boxed{
\text{Sensor}
\rightarrow
\text{ADC}
\rightarrow
\text{ESP32}
\rightarrow
\text{PWM}
\rightarrow
\text{Actuator}
}
$$

Together, ADC and PWM provide two of the most important interfaces between an MCU and the physical world: **ADC allows the ESP32 to sense analog quantities, while PWM allows it to control output devices with variable intensity or power.**
