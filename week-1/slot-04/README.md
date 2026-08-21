
# 📘 Theory: Synchronous Communication, Master–Slave Operation, Addressing, and Timing

## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Embedded systems frequently contain multiple devices that must exchange information. A microcontroller may communicate with sensors, displays, memories, ADCs, DACs, motor controllers, and other processors.

For example:

```text
Temperature Sensor ──┐
                     │
IMU Sensor ──────────┼──► ESP32
                     │
Display ─────────────┤
                     │
External Memory ─────┘
```

To exchange information reliably, devices must follow a defined **communication protocol**.

One important class is **synchronous communication**, where data transfer is coordinated using a clock signal.

The fundamental relationship is

$$
\boxed{
\text{Clock}+
\text{Data}+
\text{Protocol}=
\text{Synchronous Communication}
}
$$

Common synchronous interfaces used with the ESP32 include:

- **I²C (Inter-Integrated Circuit)**
- **SPI (Serial Peripheral Interface)**

These protocols differ in wiring, addressing, speed, device selection, and timing, but both use a clock to coordinate data transfer.

---

# 🔄 2. Serial Communication

In serial communication, bits are transmitted sequentially over one or more signal lines.

For an 8-bit value such as

$$
D=10110101_2,
$$

the bits are transmitted sequentially:

```text
Time ─────────────────────────►

1 → 0 → 1 → 1 → 0 → 1 → 0 → 1
```

This differs from parallel communication, where several bits are transferred simultaneously.

---

# 📊 3. Serial vs. Parallel Communication

| Characteristic | Serial | Parallel |
|---|---|---|
| Data transmission | Bit-by-bit | Multiple bits simultaneously |
| Number of wires | Low | High |
| PCB complexity | Lower | Higher |
| Long-distance operation | Generally easier | More difficult |
| Typical MCU interfaces | I²C, SPI, UART | Memory/data buses |
| Pin requirements | Low | High |

Modern embedded systems therefore make extensive use of serial communication.

---

# ⏱️ 4. Synchronous Communication

In synchronous communication, the transmitter and receiver coordinate data transfer using a shared clock.

A simplified interface is

```text
Controller                         Peripheral
┌───────────┐                     ┌───────────┐
│           │──── Clock ─────────►│           │
│           │                     │           │
│           │──── Data ──────────►│           │
│           │                     │           │
└───────────┘                     └───────────┘
```

The clock determines when data should be transmitted or sampled.

Therefore,

$$
\boxed{
\text{Clock Edge}
\rightarrow
\text{Data Sampling}
}
$$

---

# 🆚 5. Synchronous vs. Asynchronous Communication

### Synchronous

A clock signal accompanies or coordinates the data.

Examples:

- I²C
- SPI

### Asynchronous

No shared clock line is required. Timing information is embedded in the communication format.

A common example is:

- UART

| Feature | Synchronous | Asynchronous |
|---|---|---|
| Shared clock | Yes | No |
| Timing source | Clock | Baud-rate agreement |
| Examples | I²C, SPI | UART |
| Hardware lines | Clock + data | TX/RX |
| Synchronization | Explicit | Start/stop framing |
| Typical throughput | Often high | Protocol-dependent |

---

# 🧠 6. Controller–Peripheral Operation

Traditionally, synchronous serial systems are often described using **master–slave** terminology. Modern technical documentation increasingly uses **controller–peripheral** or **controller–target** terminology.

In this chapter:

- **Controller** = device that initiates and coordinates communication.
- **Peripheral** = device that responds to the controller.

In many ESP32 applications:

$$
\boxed{
\text{ESP32}=\text{Controller}
}
$$

while sensors and displays operate as peripherals.

```text
                 ESP32
              Controller
                  │
        ┌─────────┼─────────┐
        ▼         ▼         ▼
     Sensor      IMU      Display
   Peripheral Peripheral Peripheral
```

---

# ⚙️ 7. Responsibilities of the Controller

The controller typically performs tasks such as:

- initiating communication,
- generating the clock,
- selecting a peripheral,
- sending commands,
- sending addresses,
- transmitting data,
- requesting data,
- terminating communication.

Conceptually:

$$
\boxed{
\text{Controller}
\rightarrow
\text{Initiate}
\rightarrow
\text{Clock}
\rightarrow
\text{Transfer}
\rightarrow
\text{Terminate}
}
$$

---

# 📥 8. Responsibilities of a Peripheral

A peripheral normally:

- waits for communication,
- detects whether it is selected,
- receives commands,
- receives or transmits data,
- responds according to the protocol.

For example:

```text
ESP32 Controller
       │
       │ Request temperature
       ▼
Temperature Sensor
       │
       │ Temperature data
       ▼
ESP32 Controller
```

---

# 🔢 9. Device Addressing

When multiple devices share a communication bus, the controller needs a method for selecting the desired device.

One solution is **addressing**.

Conceptually:

```text
               Shared Bus
                   │
       ┌───────────┼───────────┐
       ▼           ▼           ▼
    Device A    Device B    Device C
   Address 1   Address 2   Address 3
```

The controller sends an address:

$$
A_{device}
$$

and only the matching device responds.

---

# 🔌 10. I²C Communication

I²C is a synchronous serial communication protocol commonly used for connecting relatively low-speed peripherals.

A standard I²C bus uses two main signals:

| Signal | Meaning |
|---|---|
| SDA | Serial Data |
| SCL | Serial Clock |

The basic structure is

```text
                ESP32
              Controller
             ┌─────────┐
SDA ─────────┤         ├──────── SDA
SCL ─────────┤         ├──────── SCL
             └─────────┘
                  │
       ┌──────────┼──────────┐
       ▼          ▼          ▼
     Sensor      IMU       Display
```

Multiple peripherals can share the same SDA and SCL lines.

---

# 🔧 11. I²C Pull-Up Resistors

I²C commonly uses open-drain/open-collector signaling.

Therefore, SDA and SCL require pull-up resistors.

```text
             VCC
              │
             RPU
              │
SDA ──────────┼──────────── Devices

             VCC
              │
             RPU
              │
SCL ──────────┼──────────── Devices
```

When no device pulls a line LOW, the pull-up resistor causes it to become HIGH.

Therefore,

$$
\boxed{
\text{Released Bus Line}\rightarrow HIGH
}
$$

and

$$
\boxed{
\text{Device Pulls Line}\rightarrow LOW
}
$$

---

# 🏷️ 12. I²C Addressing

Each I²C peripheral normally has an address.

A common addressing format uses **7-bit addresses**.

Conceptually:

```text
Address
A6 A5 A4 A3 A2 A1 A0
```

The controller uses the address to identify the desired peripheral.

For example:

```text
ESP32
  │
  ├── 0x3C → OLED Display
  │
  ├── 0x68 → IMU
  │
  └── 0x76 → Environmental Sensor
```

These are illustrative/common examples; actual addresses depend on the device and its configuration.

---

# 📖 13. I²C Read/Write Direction

After addressing a device, the communication indicates whether the controller intends to:

- write data, or
- read data.

Conceptually:

$$
\boxed{
\text{Address}
+
\text{R/W Direction}
}
$$

The resulting transaction determines the direction of the following data transfer.

---

# ▶️ 14. I²C START Condition

An I²C transaction begins with a **START condition**.

Conceptually, while SCL is HIGH:

$$
SDA: HIGH\rightarrow LOW
$$

represents START.

```text
SCL ───────────── HIGH ─────────

SDA ────────┐
            └──────── LOW
            ↑
          START
```

Thus,

$$
\boxed{
START:
SDA\downarrow
\text{ while SCL is HIGH}
}
$$

---

# ⏹️ 15. I²C STOP Condition

The transaction normally ends with a **STOP condition**.

While SCL is HIGH:

$$
SDA: LOW\rightarrow HIGH
$$

represents STOP.

```text
SCL ───────────── HIGH ─────────

SDA _________┌──────── HIGH
             ↑
            STOP
```

Therefore,

$$
\boxed{
STOP:
SDA\uparrow
\text{ while SCL is HIGH}
}
$$

---

# 🔄 16. Basic I²C Transaction

A simplified I²C write operation is

```text
START
  │
  ▼
Address + Write
  │
  ▼
ACK
  │
  ▼
Register Address
  │
  ▼
ACK
  │
  ▼
Data
  │
  ▼
ACK
  │
  ▼
STOP
```

Mathematically:

$$
\boxed{
S
\rightarrow
A
\rightarrow
W
\rightarrow
ACK
\rightarrow
D
\rightarrow
ACK
\rightarrow
P
}
$$

where:

- $S$ = START,
- $A$ = address,
- $W$ = write direction,
- $D$ = data,
- $P$ = STOP.

---

# ✅ 17. ACK and NACK

I²C uses acknowledgment bits to indicate whether a transmitted byte has been accepted.

### ACK

Acknowledgment indicates successful reception.

### NACK

Not acknowledgment indicates that the receiver did not acknowledge the byte or that a read sequence is being terminated, depending on transaction context.

Conceptually:

```text
Byte Transfer
     │
     ▼
ACK/NACK Bit
   ┌─┴─┐
  ACK NACK
```

---

# ⏱️ 18. I²C Timing

The SCL signal determines the timing of data transfer.

```text
SCL  ┌───┐   ┌───┐   ┌───┐
─────┘   └───┘   └───┘   └────

SDA  ──D7────D6────D5────D4────
```

Data must satisfy timing requirements relative to the clock.

Important concepts include:

- setup time,
- hold time,
- clock period,
- rise time,
- fall time.

---

# 📐 19. Clock Period and Frequency

Clock frequency is related to period by

$$
f_{CLK}=\frac{1}{T_{CLK}}.
$$

If

$$
f_{CLK}=100~kHz,
$$

then

$$
T_{CLK}=
\frac{1}{100000}=
10~\mu s.
$$

Therefore,

$$
\boxed{
100~kHz\Rightarrow10~\mu s\text{ clock period}
}
$$

---

# ⌛ 20. Approximate I²C Byte Transfer Time

An I²C byte normally involves:

- 8 data bits,
- 1 ACK/NACK bit.

Therefore, approximately

$$
N_{clock}=9
$$

clock periods are required per transferred byte, excluding START, STOP, addressing overhead, and other protocol effects.

At

$$
f_{SCL}=100~kHz,
$$

the approximate time is

$$
T_{byte}=
\frac{9}{100000}
$$

$$
T_{byte}=90~\mu s.
$$

---

# 💻 21. ESP32 I²C with Arduino

The Arduino environment commonly uses the `Wire` library.

```cpp
#include <Wire.h>

void setup()
{
    Wire.begin();
}

void loop()
{
}
```

A controller may transmit to a peripheral using a structure such as:

```cpp
Wire.beginTransmission(deviceAddress);

Wire.write(data);

Wire.endTransmission();
```

Conceptually:

```text
beginTransmission()
        │
        ▼
Device Address
        │
        ▼
Write Data
        │
        ▼
endTransmission()
```

---

# 🔍 22. I²C Address Scanner

A useful ESP32 experiment is scanning the I²C bus for responding addresses.

Conceptually:

```text
Address 0x01
     │
     ▼
Respond?
 ┌───┴───┐
No      Yes
│        │
▼        ▼
Next    Display
Address Address
```

The scanner tests candidate addresses and reports devices that acknowledge.

This is useful for:

- verifying wiring,
- finding device addresses,
- debugging I²C systems.

---

# ⚡ 23. SPI Communication

**SPI** is another synchronous serial communication protocol widely used with microcontrollers.

A typical SPI system uses:

| Signal | Function |
|---|---|
| SCLK | Serial Clock |
| MOSI | Controller Out / Peripheral In |
| MISO | Controller In / Peripheral Out |
| CS/SS | Chip Select |

Modern documentation may use alternative names such as **SDO/SDI**, **COPI/CIPO**, or device-specific labels.

---

# 🏗️ 24. SPI Architecture

A basic SPI connection is

```text
ESP32 Controller                 SPI Peripheral
┌──────────────┐                ┌──────────────┐
│              │── SCLK ───────►│              │
│              │── MOSI ───────►│              │
│              │◄─ MISO ────────│              │
│              │── CS ─────────►│              │
└──────────────┘                └──────────────┘
```

Unlike I²C, SPI normally uses a dedicated **chip-select signal** to select a peripheral.

---

# 🎯 25. SPI Device Selection

Suppose three SPI devices share the same clock and data lines.

```text
                  ESP32
                 Controller
                    │
          ┌─────────┼─────────┐
          │         │         │
         CS1       CS2       CS3
          │         │         │
          ▼         ▼         ▼
       Device 1  Device 2  Device 3
```

To communicate with Device 2, for example:

```text
CS1 = HIGH
CS2 = LOW
CS3 = HIGH
```

Thus,

$$
\boxed{
CS=LOW
\Rightarrow
\text{Peripheral Selected}
}
$$

for many SPI devices, although device datasheets should always be checked.

---

# 🏷️ 26. SPI Addressing

SPI itself does not normally provide a standardized bus address comparable to I²C device addressing.

Instead, peripheral selection is typically performed using separate CS lines.

Therefore:

### I²C

$$
\boxed{
\text{Device Selection}=
\text{Bus Address}
}
$$

### SPI

$$
\boxed{
\text{Device Selection}=
\text{Chip Select}
}
$$

---

# 🔄 27. Full-Duplex SPI

SPI can support simultaneous transmission and reception.

During each clock cycle:

```text
Controller ── MOSI ──► Peripheral

Controller ◄─ MISO ─── Peripheral
```

Therefore,

$$
\boxed{
\text{Transmit}
+
\text{Receive}=
\text{Simultaneous}
}
$$

This is called **full-duplex communication**.

---

# ⏱️ 28. SPI Clock Timing

SPI uses a clock signal:

```text
SCLK
     ┌───┐   ┌───┐   ┌───┐
─────┘   └───┘   └───┘   └────
```

Data transfer is synchronized with particular clock edges.

Two important parameters are:

- **Clock Polarity (CPOL)**
- **Clock Phase (CPHA)**

Together, these define the SPI mode.

---

# 🔢 29. SPI Modes

The four common SPI modes are:

| SPI Mode | CPOL | CPHA |
|---:|:---:|:---:|
| Mode 0 | 0 | 0 |
| Mode 1 | 0 | 1 |
| Mode 2 | 1 | 0 |
| Mode 3 | 1 | 1 |

The controller and peripheral must use compatible timing.

Therefore,

$$
\boxed{
\text{Correct SPI Mode}
\Rightarrow
\text{Correct Data Sampling}
}
$$

---

# 🧠 30. Clock Polarity

Clock polarity determines the idle state of the clock.

### CPOL = 0

Clock is normally LOW when idle.

```text
Idle     Active       Idle

____ ┌─┐ ┌─┐ ┌─┐ ______
```

### CPOL = 1

Clock is normally HIGH when idle.

```text
──── ┐ └─┘ └─┘ └─┘ ─────
```

---

# 🔄 31. Clock Phase

Clock phase determines which clock transition is used for sampling relative to the leading and trailing edges.

Conceptually:

```text
Clock Edge
    │
    ├── Shift Data
    │
    └── Sample Data
```

The exact relationship depends on the selected SPI mode.

Incorrect CPOL/CPHA settings can cause corrupted communication.

---

# 🚀 32. SPI Transfer Rate

If the SPI clock is

$$
f_{SPI}=10~MHz,
$$

then the clock period is

$$
T_{SPI}=
\frac{1}{10\times10^6}
$$

$$
T_{SPI}=100~ns.
$$

An 8-bit transfer requires approximately eight clock cycles:

$$
T_{8bit}=
8T_{SPI}.
$$

Therefore,

$$
T_{8bit}=
800~ns
$$

before considering software, chip-select, and transaction overhead.

---

# 🆚 33. I²C vs. SPI

| Feature | I²C | SPI |
|---|---|---|
| Communication | Synchronous | Synchronous |
| Basic signal lines | 2 | Usually 4+ |
| Clock | SCL | SCLK |
| Data | SDA | MOSI + MISO |
| Device selection | Address | CS/SS |
| Multiple devices | Easy | Requires additional CS lines |
| Duplex | Half-duplex-like shared data line | Full-duplex capable |
| Pull-up resistors | Required | Normally not |
| Typical speed | Moderate | Often higher |
| Wiring complexity | Low | Higher |
| Typical applications | Sensors, RTCs, displays | Displays, ADCs, memory, high-speed sensors |

---

# 📡 34. Example ESP32 Sensor System

Consider an ESP32 system containing:

- MPU6050 IMU,
- OLED display,
- SPI flash memory.

A possible communication architecture is

```text
                 ESP32
                   │
        ┌──────────┴──────────┐
        │                     │
       I²C                   SPI
        │                     │
   ┌────┴────┐                ▼
   ▼         ▼            Flash Memory
MPU6050    OLED
```

The I²C devices share SDA and SCL, while the SPI memory uses its SPI signals.

---

# 🔢 35. Address Conflict

Two I²C devices on the same bus generally cannot simultaneously use the same fixed address unless additional hardware or configuration resolves the conflict.

Suppose:

```text
Device A → 0x68
Device B → 0x68
```

The controller cannot uniquely identify them through that shared address alone.

Possible solutions include:

- changing a configurable device address,
- using an I²C multiplexer,
- placing devices on different I²C buses,
- controlling device enable lines where supported.

---

# ⏳ 36. Setup Time

**Setup time** is the minimum time data must be stable before the relevant sampling clock edge.

Conceptually:

```text
Data ───── VALID ─────────
              │
        <---->│
        Setup │
              ▼
Clock _______/‾‾‾
```

We can represent it as

$$
T_{setup}.
$$

The protocol/device timing requirement must satisfy

$$
T_{available}\geq T_{setup(min)}.
$$

---

# ⏱️ 37. Hold Time

**Hold time** is the time that data must remain stable after the relevant clock edge.

```text
             Clock Edge
                 │
                 ▼
Clock _________/‾‾‾

Data ───────── VALID ────
                 <---->
                  Hold
```

Represented as

$$
T_{hold}.
$$

Correct communication requires the device timing specifications to be satisfied.

---

# 📊 38. Timing Margin

Suppose the available setup time is

$$
T_{available}=50~ns
$$

while the peripheral requires

$$
T_{setup}=20~ns.
$$

The timing margin is

$$
T_{margin}=
T_{available}-

T_{setup}.
$$

Therefore,

$$
T_{margin}=50-20=30~ns.
$$

A positive timing margin indicates that the requirement is met.

---

# ⚠️ 39. Timing Violations

If data changes too close to a sampling edge, the receiving device may read the wrong value.

Conceptually:

$$
\boxed{
\text{Timing Violation}
\rightarrow
\text{Incorrect Sampling}
\rightarrow
\text{Data Error}
}
$$

Possible causes include:

- excessive clock frequency,
- long wiring,
- high bus capacitance,
- incorrect pull-up resistors,
- signal integrity problems,
- wrong SPI mode,
- software configuration errors.

---

# 📏 40. Physical Bus Considerations

Communication performance depends not only on software but also on electrical characteristics.

Important factors include:

- cable length,
- PCB trace length,
- bus capacitance,
- pull-up resistance,
- supply voltage,
- electromagnetic noise,
- grounding,
- clock frequency.

Therefore,

$$
\boxed{
\text{Reliable Communication}=
\text{Correct Protocol}
+
\text{Correct Timing}
+
\text{Correct Hardware}
}
$$

---

# 🔧 41. Pull-Up Resistance and I²C Rise Time

I²C line rise time depends partly on pull-up resistance and bus capacitance.

A simplified RC relationship is

$$
\tau=RC.
$$

If resistance or capacitance becomes too large, the signal rises more slowly.

Conceptually:

```text
Ideal:

____|‾‾‾‾‾

Large RC:

____/‾‾‾‾‾
```

At higher communication speeds, slow rise times can become a significant problem.

---

# 🧰 42. HAL Perspective

Using Arduino or ESP-IDF, programmers usually access I²C and SPI through software libraries rather than manipulating peripheral registers directly.

Conceptually:

```text
Application
     │
     ▼
Arduino API / ESP-IDF Driver
     │
     ▼
HAL / Peripheral Driver
     │
     ▼
Memory-Mapped Registers
     │
     ▼
I²C / SPI Hardware
     │
     ▼
External Device
```

Therefore,

$$
\boxed{
\text{Application}
\rightarrow
\text{HAL}
\rightarrow
\text{Peripheral}
\rightarrow
\text{Communication Bus}
}
$$

---

# 💻 43. ESP32 I²C Software Structure

A typical high-level I²C application follows:

```text
Initialize I²C
      │
      ▼
Select Address
      │
      ▼
Send Register/Command
      │
      ▼
Read/Write Data
      │
      ▼
Process Result
```

In Arduino:

```cpp
#include <Wire.h>

void setup()
{
    Wire.begin();
}

void loop()
{
    // Communicate with I²C peripheral
}
```

---

# 💻 44. ESP32 SPI Software Structure

A typical high-level SPI application follows:

```text
Initialize SPI
      │
      ▼
Configure Frequency
      │
      ▼
Configure SPI Mode
      │
      ▼
Select Peripheral
      │
      ▼
Transfer Data
      │
      ▼
Deselect Peripheral
```

A conceptual Arduino structure is:

```cpp
#include <SPI.h>

void setup()
{
    SPI.begin();
}

void loop()
{
    // Select peripheral
    // Transfer data
    // Deselect peripheral
}
```

---

# 🔄 45. Communication Transaction Model

Most synchronous communication can be understood using a general transaction model:

$$
\boxed{
\text{Initialize}
\rightarrow
\text{Select}
\rightarrow
\text{Synchronize}
\rightarrow
\text{Transfer}
\rightarrow
\text{Terminate}
}
$$

For I²C:

```text
START
  ↓
Address
  ↓
Transfer
  ↓
STOP
```

For SPI:

```text
CS Active
   ↓
Clock + Transfer
   ↓
CS Inactive
```

---

# 🧠 46. Protocol Selection

Selecting a protocol depends on the application.

### Choose I²C when:

- many devices share the same bus,
- GPIO pins are limited,
- moderate data rates are sufficient,
- sensors and low-speed peripherals are used.

### Choose SPI when:

- high data rate is important,
- full-duplex transfer is useful,
- additional GPIO pins are available,
- displays, memories, ADCs, or high-speed sensors are used.

Thus,

$$
\boxed{
\text{Protocol Selection}=
f(
\text{Speed},
\text{Pins},
\text{Devices},
\text{Distance},
\text{Complexity}
)
}
$$

---

# 🆚 47. I²C, SPI, and UART Overview

| Feature | I²C | SPI | UART |
|---|---|---|---|
| Type | Synchronous | Synchronous | Asynchronous |
| Clock line | Yes | Yes | No |
| Basic data lines | 1 bidirectional | 2 directional | TX + RX |
| Device addressing | Yes | Usually no | Usually no |
| Device selection | Address | Chip Select | Connection/protocol |
| Duplex | Shared data line | Full duplex capable | Full duplex capable |
| Typical application | Sensors | High-speed peripherals | PC/module communication |

---

# 🔬 48. Example: ESP32 + MPU6050

The MPU6050 is commonly interfaced with an ESP32 using I²C.

A simplified system is:

```text
ESP32                      MPU6050
─────                      ───────

3.3V  ───────────────────► VCC
GND   ───────────────────► GND
SDA   ◄──────────────────► SDA
SCL   ───────────────────► SCL
```

The communication sequence may be represented as

```text
ESP32
  │
  ▼
Send MPU6050 Address
  │
  ▼
Select Register
  │
  ▼
Read Sensor Data
  │
  ▼
Process Accelerometer/
Gyroscope Values
```

---

# 📦 49. Register Address vs. Device Address

An important distinction is between a **device address** and an internal **register address**.

Suppose an I²C sensor has:

```text
Device Address = 0x68
```

Inside the sensor are registers:

```text
Register 0x3B → Accelerometer X High
Register 0x3C → Accelerometer X Low
Register 0x43 → Gyroscope X High
...
```

Thus:

$$
\boxed{
\text{Device Address}
\rightarrow
\text{Select Device}
}
$$

while

$$
\boxed{
\text{Register Address}
\rightarrow
\text{Select Data Inside Device}
}
$$

---

# 🔄 50. Typical Sensor Read Sequence

A common register-based sensor transaction can be represented as:

```text
Controller
    │
    ▼
Select Device
    │
    ▼
Send Register Address
    │
    ▼
Request Data
    │
    ▼
Peripheral Sends Data
    │
    ▼
Controller Processes Data
```

Mathematically:

$$
\boxed{
A_D
\rightarrow
A_R
\rightarrow
R
\rightarrow
D
}
$$

where:

- $A_D$ = device address,
- $A_R$ = register address,
- $R$ = read operation,
- $D$ = returned data.

---

# ⚡ 51. Communication Errors

Common communication problems include:

| Problem | Possible Cause |
|---|---|
| Device not detected | Wrong address |
| No I²C response | Missing pull-ups |
| Incorrect values | Wrong register |
| Random errors | Noise |
| SPI data corrupted | Wrong CPOL/CPHA |
| Communication fails at high speed | Timing/signal-integrity problem |
| Wrong device responds | Address/CS configuration problem |
| Bus locked | Device or transaction problem |

A systematic debugging process should check:

$$
\boxed{
\text{Wiring}
\rightarrow
\text{Voltage}
\rightarrow
\text{Address}
\rightarrow
\text{Protocol}
\rightarrow
\text{Timing}
\rightarrow
\text{Software}
}
$$

---

# 🔍 52. Logic Analyzer

A **logic analyzer** is extremely useful for debugging digital communication.

It can display:

```text
SCL  ─┐ ┌─┐ ┌─┐ ┌─┐ ┌─
      └─┘ └─┘ └─┘ └─┘

SDA  ───1──0──1──1────

CS   ──────── LOW ─────
```

Many logic analyzers can automatically decode:

- I²C addresses,
- I²C data,
- ACK/NACK,
- SPI data,
- UART frames.

This allows students to observe the actual protocol rather than only the software output.

---

# 🧪 53. Timing Measurement

Suppose a logic analyzer shows an SPI clock period of

$$
T=0.5~\mu s.
$$

The frequency is

$$
f=\frac{1}{T}.
$$

Therefore,

$$
f=
\frac{1}{0.5\times10^{-6}}=
2~MHz.
$$

Thus,

$$
\boxed{
f_{SPI}=2~MHz
}
$$

---

# 📊 54. Communication Performance

The theoretical bit rate is not equal to the final useful application-data rate.

Protocol overhead may include:

- addresses,
- commands,
- register numbers,
- acknowledgments,
- START/STOP conditions,
- chip-select timing,
- software processing.

Therefore,

$$
\boxed{
R_{\text{useful}}
<
R_{\text{raw}}
}
$$

in most practical systems.

---

# 🧮 55. Simple Efficiency Model

Suppose a transaction transfers

$$
N_D
$$

useful data bits and requires

$$
N_T
$$

total transmitted clocked bits.

The protocol efficiency can be approximated by

$$
\eta
=
\frac{N_D}{N_T}
\times100\%.
$$

For example, if 64 useful bits require 80 total transferred bits,

$$
\eta =
\frac{64}{80}
\times100 =
80\%.
$$

---

# 🏗️ 56. Complete ESP32 Communication Architecture

A more complete embedded system can be represented as:

```text
                     ESP32
                       │
        ┌──────────────┼──────────────┐
        │              │              │
       I²C            SPI            UART
        │              │              │
   ┌────┴────┐    ┌────┴────┐         ▼
   ▼         ▼    ▼         ▼      Computer
 Sensor    OLED  Flash    Display
```

This demonstrates how different communication protocols can coexist within one MCU system.

---

# 🧠 57. Layered View of Communication

Communication can be understood through several layers:

```text
Application
    │
    ▼
Device Driver
    │
    ▼
HAL / API
    │
    ▼
Peripheral Registers
    │
    ▼
I²C / SPI Controller
    │
    ▼
Electrical Signals
    │
    ▼
External Device
```

Therefore,

$$
\boxed{
\text{Software}
\rightarrow
\text{Protocol}
\rightarrow
\text{Timing}
\rightarrow
\text{Electrical Signals}
}
$$

All layers must operate correctly for reliable communication.

---

# 💬 58. Discussion Questions

1. What is synchronous communication?
2. How does synchronous communication differ from asynchronous communication?
3. What is the purpose of a clock signal?
4. What is meant by controller–peripheral operation?
5. What responsibilities does the controller have?
6. Why is device addressing required?
7. What are SDA and SCL?
8. Why does I²C require pull-up resistors?
9. What is an I²C START condition?
10. What is an I²C STOP condition?
11. What are ACK and NACK?
12. What is the difference between a device address and a register address?
13. What are MOSI and MISO?
14. What is the purpose of SPI chip select?
15. Why does SPI generally not require device addresses?
16. What are CPOL and CPHA?
17. How many standard SPI modes exist?
18. What are setup and hold times?
19. What causes communication timing violations?
20. When would SPI be preferred over I²C?

---

# 🧠 59. Exercises

### Exercise 1 — Clock Period

Calculate the clock period for an I²C clock frequency of

$$
f_{SCL}=400~kHz.
$$

Use

$$
T=\frac{1}{f}.
$$

---

### Exercise 2 — I²C Transfer

Assume that one byte plus its acknowledgment requires nine clock cycles.

Calculate the approximate transfer time at

$$
f_{SCL}=100~kHz.
$$

---

### Exercise 3 — SPI Transfer

Calculate the ideal time required to transfer 16 bits using

$$
f_{SPI}=8~MHz.
$$

---

### Exercise 4 — Address Identification

An ESP32 has the following I²C devices:

| Device | Address |
|---|---|
| OLED | 0x3C |
| MPU6050 | 0x68 |
| Sensor | 0x76 |

Explain how the ESP32 selects only the MPU6050.

---

### Exercise 5 — SPI Selection

Three SPI peripherals use:

```text
CS1
CS2
CS3
```

Write the CS states required to select Device 3 assuming active-LOW chip selects.

---

### Exercise 6 — Timing Margin

A peripheral requires

$$
T_{setup}=15~ns.
$$

The controller provides

$$
T_{available}=40~ns.
$$

Calculate the timing margin.

---

# 🔬 60. Practical Analysis Exercise

Consider:

```text
ESP32
  │
  ├── I²C → MPU6050
  │
  ├── I²C → OLED
  │
  └── SPI → External Memory
```

Students should identify:

- number of communication buses,
- shared signals,
- device-selection method,
- required addresses,
- chip-select signals,
- clock signals,
- data direction.

Complete the table:

| Device | Protocol | Selection | Clock | Data |
|---|---|---|---|---|
| MPU6050 | | | | |
| OLED | | | | |
| Memory | | | | |

---

# 🚀 61. Advanced Exercise: ESP32 Multi-Peripheral System

Design an ESP32 system containing:

- MPU6050 using I²C,
- OLED using I²C,
- external memory using SPI,
- Serial Monitor using UART.

The architecture should follow:

```text
                     ESP32
                       │
        ┌──────────────┼──────────────┐
        │              │              │
       I²C            SPI            UART
        │              │              │
   ┌────┴────┐         ▼              ▼
   ▼         ▼       Memory       Computer
MPU6050     OLED
```

Students should determine:

1. Required ESP32 GPIO signals.
2. I²C addresses.
3. SPI chip-select signal.
4. Appropriate clock frequencies.
5. Data direction for each interface.
6. Expected transaction sequence.
7. Possible communication conflicts.

---

# 🧾 62. Expected Learning Outcomes

After studying this theory, students should be able to:

- Explain synchronous serial communication.
- Distinguish synchronous and asynchronous communication.
- Explain controller–peripheral operation.
- Describe the roles of controller and peripheral devices.
- Explain device addressing.
- Describe I²C architecture.
- Explain SDA and SCL operation.
- Explain I²C START, STOP, ACK, and NACK.
- Distinguish device addresses from internal register addresses.
- Describe SPI architecture.
- Explain SCLK, MOSI, MISO, and CS.
- Explain SPI chip-select operation.
- Describe CPOL, CPHA, and SPI modes.
- Calculate clock periods and approximate transfer times.
- Explain setup and hold timing.
- Compare I²C and SPI.
- Select an appropriate communication protocol for an ESP32 application.
- Use communication timing concepts to diagnose embedded-system problems.

---

# 📘 63. References

1. Espressif Systems, *ESP32 Series Datasheet*.
2. Espressif Systems, *ESP32 Technical Reference Manual*.
3. Espressif Systems, *ESP-IDF Programming Guide — I2C*.
4. Espressif Systems, *ESP-IDF Programming Guide — SPI Master Driver*.
5. Espressif Systems, *Arduino Core for ESP32 Documentation*.
6. NXP Semiconductors, *I²C-bus Specification and User Manual*.
7. Arduino, *Wire Library Documentation*.
8. Arduino, *SPI Library Documentation*.
9. J. W. Valvano, *Embedded Systems: Real-Time Interfacing to ARM Cortex-M Microcontrollers*.
10. M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly.

---

# 🔑 64. Key Concepts

### Synchronous Communication

$$
\boxed{
\text{Clock}
+
\text{Data} =
\text{Synchronized Transfer}
}
$$

### Controller–Peripheral Operation

$$
\boxed{
\text{Controller}
\rightarrow
\text{Initiates and Coordinates Communication}
}
$$

### I²C Device Selection

$$
\boxed{
\text{START}
\rightarrow
\text{Address}
\rightarrow
\text{Read/Write}
\rightarrow
\text{Data}
\rightarrow
\text{STOP}
}
$$

### SPI Device Selection

$$
\boxed{
\text{Chip Select}
\rightarrow
\text{Clocked Data Transfer}
\rightarrow
\text{Chip Deselect}
}
$$

### Timing

$$
\boxed{
f_{CLK}=\frac{1}{T_{CLK}}
}
$$

### Reliable Communication

$$
\boxed{
\text{Correct Addressing}
+
\text{Correct Timing}
+
\text{Correct Protocol}
+
\text{Correct Wiring} =
\text{Reliable Data Transfer}
}
$$

For the ESP32, **I²C provides a simple addressed two-wire bus for connecting multiple peripherals, while SPI provides a higher-speed clocked interface using dedicated chip-select signals. Understanding addressing and timing is essential for designing reliable embedded communication systems.**
