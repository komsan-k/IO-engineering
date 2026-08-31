# LAB 4 — I²C Sensor Interfacing, Register Access, and UART Serial Output

## 1. Objective

The objective of this lab is to:

1. Understand the basic operation of the **I²C communication protocol**.
2. Identify the roles of SDA, SCL, controller, and peripheral devices.
3. Connect an I²C sensor to the ESP32.
4. Detect and verify the sensor I²C address.
5. Understand the concept of sensor registers.
6. Write configuration data to an I²C register.
7. Read raw sensor data directly from device registers.
8. Combine multiple register bytes into a complete sensor value.
9. Convert raw sensor data into meaningful physical units.
10. Display formatted sensor data through the ESP32 UART Serial Monitor.
11. Handle basic I²C communication errors.
12. Build a complete digital-sensor data-acquisition system.

---

## 2. Background

Digital sensors often communicate with a microcontroller through serial communication protocols rather than analog voltages.

Common protocols include:

```text
UART
I²C
SPI
```

In this lab, the focus is on:

```text
I²C
```

or **Inter-Integrated Circuit**.

I²C allows multiple peripheral devices to communicate using only two signal lines:

```text
SDA → Serial Data
SCL → Serial Clock
```

The basic system is:

```text
I²C Sensor
    ↓
SDA / SCL
    ↓
ESP32
    ↓
Register Processing
    ↓
UART
    ↓
Serial Monitor
```

The complete data path is:

$$
\boxed{
\text{Sensor}
\rightarrow
\text{I}^2\text{C}
\rightarrow
\text{ESP32}
\rightarrow
\text{UART}
\rightarrow
\text{User}
}
$$

---

## 3. I²C Architecture

A basic I²C system consists of a controller and one or more peripheral devices.

```text
                 I²C Bus
             SDA         SCL
              │           │
      ┌───────┴───────────┴───────┐
      │                           │
    ESP32                      Sensor
 Controller                  Peripheral
```

The controller normally:

- Generates the clock.
- Initiates communication.
- Selects a device using its address.
- Reads or writes data.

The sensor responds when its I²C address is selected.

---

## 4. I²C Addressing

Each I²C device normally has an address.

For example:

```text
Device A → 0x48
Device B → 0x68
Device C → 0x76
```

The address allows multiple devices to share the same SDA and SCL wires.

Conceptually:

```text
ESP32
  │
  ├── Address 0x48 → Sensor A
  ├── Address 0x68 → Sensor B
  └── Address 0x76 → Sensor C
```

A common I²C address uses 7 bits:

$$
0 \leq A_{\mathrm{I2C}} \leq 127.
$$

Addresses are usually written in hexadecimal notation.

---

## 5. Hardware Requirements

Use:

- ESP32 development board
- USB cable
- Breadboard
- Jumper wires
- One I²C digital sensor
- Suitable pull-up resistors if they are not already provided on the sensor module

For a classic ESP32 development board, typical I²C pins are:

```text
SDA → GPIO21
SCL → GPIO22
```

Pin assignments can differ among ESP32 variants, so verify the actual board pinout.

---

## 6. Sensor Connection

A typical four-wire I²C sensor connection is:

```text
ESP32              I²C Sensor

3.3 V  ─────────── VCC
GND    ─────────── GND
GPIO21 ─────────── SDA
GPIO22 ─────────── SCL
```

Architecture:

```text
          ESP32
      ┌───────────┐
GPIO21│ SDA       │──────── SDA
GPIO22│ SCL       │──────── SCL
      │           │        Sensor
      └───────────┘
```

Many I²C modules include pull-up resistors on SDA and SCL. If they are absent, suitable external pull-up resistors may be required.

---

## 7. Software Requirements

Use:

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- `Wire.h`
- Serial Monitor

Include:

```cpp
#include <Wire.h>
```

Initialize UART:

```cpp
Serial.begin(115200);
```

Initialize I²C:

```cpp
Wire.begin(21, 22);
```

where:

```text
21 → SDA
22 → SCL
```

---

## 8. Experiment 1 — Initialize the I²C Bus

```cpp
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

void setup() {

  Serial.begin(115200);

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Serial.println(
    "I2C initialized"
  );
}

void loop() {
}
```

Expected Serial output:

```text
I2C initialized
```

This confirms only that the ESP32 I²C interface has been initialized. It does not yet confirm that a sensor is connected correctly.

---

## 9. Experiment 2 — I²C Address Scanner

Before reading sensor registers, determine the device address.

```cpp
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

void setup() {

  Serial.begin(115200);

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Serial.println(
    "I2C Scanner"
  );
}

void loop() {

  int devices = 0;

  for (
    uint8_t address = 1;
    address < 127;
    address++
  ) {

    Wire.beginTransmission(
      address
    );

    uint8_t error =
        Wire.endTransmission();

    if (error == 0) {

      Serial.print(
        "Device found at 0x"
      );

      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(
        address,
        HEX
      );

      devices++;
    }
  }

  Serial.print(
    "Devices found: "
  );

  Serial.println(
    devices
  );

  delay(5000);
}
```

---

## 10. Expected Scanner Result

A typical output might be:

```text
I2C Scanner
Device found at 0x48
Devices found: 1
```

The exact address depends on the sensor.

Record:

| Device | Detected Address |
|---|---|
| Sensor | |

If no device is found, check:

- Power
- Ground
- SDA
- SCL
- Pull-up resistors
- Sensor voltage
- Correct GPIO pins

---

## 11. Sensor Register Concept

Most digital sensors organize their internal data using registers.

Conceptually:

```text
Sensor
 ├── Register 0x00 → Measurement MSB
 ├── Register 0x01 → Measurement LSB
 ├── Register 0x02 → Configuration
 └── Register 0x03 → Device Status
```

A register is a small storage location inside the peripheral.

To access sensor information:

```text
Select Sensor
     ↓
Select Register
     ↓
Read Register Data
```

This is called **register-level sensor communication**.

---

## 12. I²C Register Read Sequence

A common register-read transaction follows:

```text
START
  ↓
Sensor Address + WRITE
  ↓
Register Address
  ↓
Repeated START
  ↓
Sensor Address + READ
  ↓
Receive Data
  ↓
STOP
```

Conceptually:

```text
ESP32                           Sensor

  │──── Device Address ─────────→│
  │──── Register Address ────────→│
  │                              │
  │←──────── Register Data ──────│
```

---

## 13. Experiment 3 — Read One Register

Create a general register-read function:

```cpp
uint8_t readRegister(
  uint8_t deviceAddress,
  uint8_t registerAddress
) {

  Wire.beginTransmission(
    deviceAddress
  );

  Wire.write(
    registerAddress
  );

  if (
    Wire.endTransmission(false)
    != 0
  ) {

    return 0;
  }

  Wire.requestFrom(
    deviceAddress,
    (uint8_t)1
  );

  if (
    Wire.available()
  ) {

    return Wire.read();
  }

  return 0;
}
```

Usage:

```cpp
uint8_t value =
    readRegister(
      SENSOR_ADDRESS,
      REGISTER_ADDRESS
    );
```

---

## 14. Display Register Data

Display the register in both decimal and hexadecimal formats:

```cpp
Serial.print(
  "Register value = "
);

Serial.print(
  value
);

Serial.print(
  " (0x"
);

Serial.print(
  value,
  HEX
);

Serial.println(
  ")"
);
```

Example:

```text
Register value = 104 (0x68)
```

Hexadecimal notation is commonly used when working with device registers because register maps are normally documented in hexadecimal.

---

## 15. Experiment 4 — Write to a Sensor Register

Some sensors require configuration before measurements can be obtained.

A general register-write function is:

```cpp
bool writeRegister(
  uint8_t deviceAddress,
  uint8_t registerAddress,
  uint8_t value
) {

  Wire.beginTransmission(
    deviceAddress
  );

  Wire.write(
    registerAddress
  );

  Wire.write(
    value
  );

  return (
    Wire.endTransmission()
    == 0
  );
}
```

The sequence is:

```text
START
  ↓
Device Address
  ↓
Register Address
  ↓
Configuration Value
  ↓
STOP
```

---

## 16. Experiment 5 — Read Multiple Bytes

Many sensor measurements use more than one register.

For example:

```text
Register 0x00 → MSB
Register 0x01 → LSB
```

The two bytes may represent one 16-bit measurement.

Read two bytes:

```cpp
Wire.requestFrom(
  SENSOR_ADDRESS,
  (uint8_t)2
);

uint8_t msb =
    Wire.read();

uint8_t lsb =
    Wire.read();
```

Combine them:

```cpp
uint16_t rawValue =
    (
      ((uint16_t)msb << 8)
      |
      lsb
    );
```

Mathematically:

$$
x_{\text{raw}}
= 256x_{\text{MSB}}
+
x_{\text{LSB}}.
$$

---

## 17. Understanding MSB and LSB

For a 16-bit value:

```text
MSB               LSB
xxxxxxxx           yyyyyyyy
   │                  │
   └──────┬───────────┘
          ↓
      16-bit Value
```

The combination is:

$$
x
= (x_{\text{MSB}}\ll8)
|
x_{\text{LSB}}.
$$

For example:

```text
MSB = 0x12
LSB = 0x34
```

then:

$$
x=0x1234.
$$

In decimal:

$$
0x1234=4660.
$$

---

## 18. Signed Sensor Data

Some sensors produce signed measurements.

For a 16-bit signed quantity:

```cpp
int16_t rawValue =
    (int16_t)(
      ((uint16_t)msb << 8)
      |
      lsb
    );
```

The range is:

$$
-32768
\leq
x
\leq
32767.
$$

Correct interpretation of signed and unsigned register data is essential when reading accelerometers, temperature sensors, and similar devices.

---

## 19. Experiment 6 — Convert Raw Data to Physical Units

A sensor datasheet typically provides a conversion relationship.

The general model is:

$$
y
= Sx_{\text{raw}}
+
b,
$$

where:

- $x_{\text{raw}}$ is the register value.
- $S$ is the sensor scaling factor.
- $b$ is an optional offset.
- $y$ is the physical measurement.

For example:

$$
T
= S_T x_{\text{raw}}.
$$

The exact scale factor must come from the sensor datasheet.

Do not assume that every sensor uses the same conversion formula.

---

## 20. Example Sensor Data Flow

Suppose two registers contain a temperature measurement:

```text
Temperature MSB
      +
Temperature LSB
      ↓
Combine Bytes
      ↓
Raw Temperature
      ↓
Apply Scale Factor
      ↓
Temperature in °C
```

The processing sequence is:

$$
\boxed{
\text{Registers}
\rightarrow
\text{Raw Data}
\rightarrow
\text{Scaling}
\rightarrow
\text{Physical Unit}
}
$$

---

## 21. Experiment 7 — Formatted UART Output

Rather than displaying only the raw number:

```text
2345
```

format the output:

```text
Sensor Address : 0x48
Raw Value      : 2345
Temperature    : 27.35 °C
```

Example:

```cpp
Serial.print(
  "Raw = "
);

Serial.print(
  rawValue
);

Serial.print(
  " | Temperature = "
);

Serial.print(
  temperature,
  2
);

Serial.println(
  " °C"
);
```

Formatted output makes sensor data easier to understand and analyze.

---

## 22. UART Serial Communication

UART stands for:

**Universal Asynchronous Receiver/Transmitter**

In this lab:

```text
I²C Sensor
    ↓
ESP32
    ↓
UART
    ↓
USB Serial Interface
    ↓
Computer
    ↓
Serial Monitor
```

Initialize UART using:

```cpp
Serial.begin(115200);
```

The value:

```text
115200
```

is the baud rate.

---

## 23. UART Frame Concept

A simplified UART frame contains:

```text
Start Bit
   ↓
Data Bits
   ↓
Optional Parity
   ↓
Stop Bit
```

A common configuration is:

```text
8N1
```

meaning:

```text
8 data bits
No parity
1 stop bit
```

---

## 24. Experiment 8 — Periodic Sensor Reading

Read and display the sensor at a fixed interval.

Example:

```cpp
void loop() {

  readSensor();

  delay(1000);
}
```

This gives approximately:

$$
T_s=1~\text{s}
$$

and:

$$
f_s
= \frac{1}{T_s}
= 1~\text{Hz}.
$$

For a 100-ms interval:

$$
f_s
= \frac{1}{0.1}
= 10~\text{Hz}.
$$

---

## 25. Non-Blocking Periodic Reading

A better embedded programming method avoids unnecessarily blocking the CPU.

Use:

```cpp
unsigned long previousTime = 0;

const unsigned long interval = 1000;

void loop() {

  unsigned long currentTime =
      millis();

  if (
    currentTime
    - previousTime
    >= interval
  ) {

    previousTime =
        currentTime;

    readSensor();
  }
}
```

This allows other application code to execute between sensor readings.

---

## 26. Experiment 9 — Read and Display Multiple Registers

Read several registers from the sensor.

Example output:

```text
--------------------------------
I2C Sensor Data
Address   : 0x48
Register0 : 0x1A
Register1 : 0x40
Raw Data  : 6720
Value     : 26.25
--------------------------------
```

This demonstrates the complete register-processing sequence.

---

## 27. Experiment 10 — Communication Error Detection

Check:

```cpp
uint8_t status =
    Wire.endTransmission();
```

Interpret:

```cpp
if (status == 0) {

  Serial.println(
    "I2C communication OK"
  );
}
else {

  Serial.print(
    "I2C error: "
  );

  Serial.println(
    status
  );
}
```

The program should not silently assume that every transaction succeeds.

---

## 28. Experiment 11 — Sensor Disconnect Test

While the program is running:

1. Record normal operation.
2. Disconnect the sensor.
3. Observe communication errors.
4. Reconnect the sensor.
5. Observe whether normal communication resumes.

Expected behavior:

```text
Sensor Connected
      ↓
Valid Data

Sensor Disconnected
      ↓
I2C Error

Sensor Reconnected
      ↓
Valid Data
```

This introduces the concept of **fault-aware embedded systems**.

---

## 29. Experiment 12 — Compare Raw and Converted Data

Record several measurements.

| Sample | MSB | LSB | Raw Value | Converted Value | Unit |
|---:|---:|---:|---:|---:|---|
| 1 | | | | | |
| 2 | | | | | |
| 3 | | | | | |
| 4 | | | | | |
| 5 | | | | | |

Students should explain how the register bytes are transformed into a physical measurement.

---

## 30. Complete Program Structure

The complete application can be organized as:

```cpp
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define SENSOR_ADDRESS 0x48

uint8_t readRegister(
  uint8_t reg
) {

  Wire.beginTransmission(
    SENSOR_ADDRESS
  );

  Wire.write(
    reg
  );

  if (
    Wire.endTransmission(false)
    != 0
  ) {

    return 0;
  }

  Wire.requestFrom(
    SENSOR_ADDRESS,
    (uint8_t)1
  );

  if (
    Wire.available()
  ) {

    return Wire.read();
  }

  return 0;
}

void setup() {

  Serial.begin(115200);

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Serial.println(
    "I2C Sensor Interface"
  );
}

void loop() {

  uint8_t value =
      readRegister(
        0x00
      );

  Serial.print(
    "Register 0x00 = 0x"
  );

  Serial.println(
    value,
    HEX
  );

  delay(1000);
}
```

The actual device address, registers, and conversion formula must be changed to match the selected sensor datasheet.

---

## 31. Recommended Program Architecture

A cleaner embedded design separates functions:

```text
setup()
   ↓
Initialize UART
   ↓
Initialize I²C
   ↓
Configure Sensor

loop()
   ↓
Read Registers
   ↓
Convert Data
   ↓
Check Errors
   ↓
Format Results
   ↓
UART Output
```

Recommended functions:

```cpp
initializeSensor();

readRegister();

writeRegister();

readSensor();

convertData();

printSensorData();
```

This makes the program easier to test and maintain.

---

## 32. I²C vs. UART

| Feature | I²C | UART |
|---|---|---|
| Typical role in this lab | Sensor communication | PC output |
| Clock | Synchronous | Asynchronous |
| Main signal lines | SDA, SCL | TX, RX |
| Device addressing | Yes | No bus address |
| Multiple devices on same bus | Yes | Normally point-to-point |
| Register-based sensors | Common | Less common |
| Used for Serial Monitor | No | Yes |

The lab uses both protocols:

```text
Sensor
  ↓
I²C
  ↓
ESP32
  ↓
UART
  ↓
Computer
```

---

## 33. Common Errors

### Wrong I²C Address

If the address is incorrect:

```text
ESP32
  ↓
Wrong Address
  ↓
No Sensor Response
```

Run the I²C scanner first.

### SDA and SCL Reversed

Incorrect wiring can prevent communication.

Verify:

```text
SDA → SDA
SCL → SCL
```

### Missing Common Ground

The ESP32 and sensor must share a common ground reference.

### Wrong Supply Voltage

Verify whether the sensor module is designed for:

```text
3.3 V
```

or another permitted supply voltage.

### Missing Pull-Up Resistors

I²C SDA and SCL are typically open-drain signals and require pull-ups.

Many breakout boards already include them.

### Wrong Register Address

The I²C device address and internal register address are different concepts.

```text
Device Address
      ↓
Select Sensor

Register Address
      ↓
Select Data Inside Sensor
```

### Incorrect Byte Order

Be careful whether the sensor stores data as:

```text
MSB first
```

or:

```text
LSB first
```

### Incorrect Signedness

A signed sensor register interpreted as unsigned can produce incorrect results.

---

## 34. Experimental Summary

### Experiment 1 — Initialize I²C

Configure SDA and SCL.

### Experiment 2 — Scan the Bus

Identify the sensor address.

### Experiment 3 — Read a Register

Access a sensor register directly.

### Experiment 4 — Write a Register

Configure the sensor.

### Experiment 5 — Read Multiple Bytes

Acquire MSB and LSB data.

### Experiment 6 — Convert Sensor Data

Transform raw values into physical units.

### Experiment 7 — UART Formatting

Display readable sensor results.

### Experiment 8 — Periodic Acquisition

Read sensor data at a controlled rate.

### Experiment 9 — Multiple Registers

Display multiple sensor parameters.

### Experiment 10 — Error Detection

Detect unsuccessful I²C transactions.

### Experiment 11 — Disconnect Test

Observe system behavior during sensor failure.

### Experiment 12 — Data Analysis

Compare register data, raw data, and converted measurements.

---

## 35. Exercises

### Exercise 1 — I²C Scanner

Create a program that scans:

$$
0x01
$$

through:

$$
0x7E
$$

and displays all responding devices.

### Exercise 2 — Register Reading

Read one known register from the selected sensor and display:

```text
Register 0xXX = 0xYY
```

### Exercise 3 — Two-Byte Measurement

Read two consecutive registers and combine them using:

$$
x
= (x_{\text{MSB}}\ll8)
|
x_{\text{LSB}}.
$$

### Exercise 4 — Physical Unit Conversion

Use the sensor datasheet to determine the correct conversion equation.

Display both:

```text
Raw Value
```

and:

```text
Converted Value
```

### Exercise 5 — Formatted Output

Create output such as:

```text
Sensor: I2C Device
Address: 0x48
Raw: 12345
Value: 24.76 °C
```

### Exercise 6 — Sampling Rate

Test acquisition intervals of:

```text
100 ms
500 ms
1000 ms
```

Calculate the corresponding sampling rates.

### Exercise 7 — Error Handling

Display:

```text
Sensor OK
```

or:

```text
Sensor Communication Error
```

according to the I²C transaction result.

### Exercise 8 — Disconnect Test

Disconnect the sensor during operation and describe how the system behaves.

---

## 36. Checkpoint Questions

1. What does I²C stand for?
2. What are the two main I²C signal lines?
3. What is the purpose of SDA?
4. What is the purpose of SCL?
5. Which device normally generates the I²C clock?
6. What is an I²C device address?
7. Why can several sensors use the same I²C bus?
8. What is the difference between an I²C device address and a register address?
9. Why is an I²C scanner useful?
10. What is a sensor register?
11. What is the purpose of `Wire.beginTransmission()`?
12. What is the purpose of `Wire.write()` when reading a register?
13. What does `Wire.requestFrom()` do?
14. Why may `endTransmission(false)` be used before a register read?
15. What do MSB and LSB mean?
16. How are two 8-bit values combined into a 16-bit value?
17. Why must byte order be checked?
18. What is the difference between signed and unsigned data?
19. Why is a sensor scaling factor required?
20. Where should the correct sensor conversion equation be obtained?
21. What does UART stand for?
22. Why is UART useful in this lab?
23. What does a baud rate of 115200 represent?
24. What is an 8N1 UART configuration?
25. Why should sensor results be formatted before transmission?
26. Why are I²C pull-up resistors required?
27. What happens if the I²C device address is incorrect?
28. Why should I²C transaction errors be checked?
29. What is the advantage of non-blocking periodic sensor acquisition?
30. How do I²C and UART work together in this experiment?

---

## 37. Expected Results

Students should successfully implement:

```text
I²C Sensor
     ↓
Device Address Detection
     ↓
Register Selection
     ↓
Register Reading
     ↓
Raw Data
     ↓
Conversion
     ↓
Formatted UART Output
```

The resulting embedded-system architecture is:

```text
Physical Measurement
        ↓
Digital Sensor
        ↓
I²C Bus
        ↓
ESP32
        ↓
Data Processing
        ↓
UART
        ↓
Serial Monitor
```

The key relationship is:

$$
\boxed{
\text{Digital Sensor Interface}
= \text{I}^2\text{C Communication}
+
\text{Register Processing}
+
\text{UART Reporting}
}
$$

---

## 38. Result Tables

### I²C Detection

| Test | Sensor Connected | Detected Address | Status |
|---:|---|---|---|
| 1 | Yes | | |
| 2 | No | | |
| 3 | Yes | | |

### Register Measurement

| Sample | MSB | LSB | Raw Value | Converted Value |
|---:|---:|---:|---:|---:|
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |
| 4 | | | | |
| 5 | | | | |

### Sampling Rate

| Sampling Interval | Sampling Rate | Observation |
|---:|---:|---|
| 100 ms | 10 Hz | |
| 500 ms | 2 Hz | |
| 1000 ms | 1 Hz | |

---

## 39. Discussion

Students should explain how I²C enables the ESP32 to communicate with a digital sensor using only SDA and SCL while distinguishing between the sensor's **device address** and its internal **register addresses**.

The communication path is:

```text
ESP32
  ↓
Select Device Address
  ↓
Select Register Address
  ↓
Read Register Bytes
  ↓
Combine Data
  ↓
Apply Conversion
```

The resulting information is then transmitted to the computer:

```text
Converted Sensor Data
        ↓
UART
        ↓
Serial Monitor
```

The complete processing model is:

$$
\boxed{
\text{Register Data}
\rightarrow
\text{Raw Measurement}
\rightarrow
\text{Physical Quantity}
\rightarrow
\text{UART Output}
}
$$

Students should also discuss communication errors, byte order, signed data, sampling rate, and why datasheet information is necessary when implementing register-level sensor drivers.

---

## 40. Submission

Submit:

- Photograph or diagram of the sensor wiring.
- I²C scanner source code.
- Detected sensor address.
- Register-read source code.
- Register-write source code if required by the sensor.
- Raw MSB and LSB measurements.
- Raw-to-physical-unit conversion calculation.
- Serial Monitor screenshot showing formatted results.
- Sampling-rate measurements.
- Sensor disconnect/error-handling results.
- Completed result tables.
- Answers to checkpoint questions.
- Brief discussion of I²C addressing, sensor registers, data conversion, and UART output.

---

## 41. Conclusion

In this lab, an I²C digital sensor is interfaced with the ESP32 through the SDA and SCL communication lines. The sensor address is first identified using an I²C scanner. Internal sensor registers are then accessed directly to obtain raw measurement data.

For multi-byte measurements:

$$
x_{\text{raw}}
= (x_{\text{MSB}}\ll8)
|
x_{\text{LSB}}.
$$

The raw register data is converted into a meaningful physical quantity using the conversion relationship specified by the sensor datasheet. The final information is formatted and transmitted to the computer using UART.

The learning progression becomes:

```text
LAB 1
GPIO Input / Output
        ↓
LAB 2
Interrupts & Timers
        ↓
LAB 3
ADC & PWM
        ↓
LAB 4
I²C Communication
        ↓
Register Access
        ↓
Digital Sensor Data
        ↓
UART Reporting
```

The central principle of this lab is:

$$
\boxed{
\text{Sensor Communication}
= \text{Addressing}
+
\text{Register Access}
+
\text{Data Conversion}
+
\text{Serial Reporting}
}
$$

This establishes the foundation for interfacing more complex digital sensors and for integrating sensor acquisition with **FreeRTOS, networking, MQTT, BLE, and IoT applications**.
