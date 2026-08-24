# 📘 Theory: MCU Architecture, Memory Mapping, Register-Level Programming, and HAL Basics
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

A **Microcontroller Unit (MCU)** is a compact computing system designed to perform dedicated embedded-control tasks. Unlike a general-purpose computer, an MCU integrates the processor, memory, communication interfaces, timers, and input/output peripherals into a single integrated circuit.

The ESP32 family provides a useful case study because it combines conventional MCU functions with wireless communication and hardware peripherals suitable for **IoT, embedded systems, cyber-physical systems, robotics, and edge intelligence**.

A simplified MCU can be represented as:

```text
              Microcontroller Unit
                      │
        ┌─────────────┼─────────────┐
        ▼             ▼             ▼
       CPU          Memory      Peripherals
        │             │             │
        │        ┌────┴────┐    ┌───┴─────────┐
        │        ▼         ▼    ▼             ▼
        │       RAM      Flash GPIO       Timers/ADC
        │
        └──────────── System Bus ──────────────┘
```

Conceptually:

$$
\boxed{
\text{MCU System} =
\text{CPU}
+
\text{Memory}
+
\text{Peripherals}
+
\text{Software}
}
$$

This manuscript introduces four fundamental topics:

1. **MCU architecture**
2. **Memory mapping**
3. **Register-level programming**
4. **Hardware Abstraction Layer (HAL)**

---

## 🎯 2. Learning Objectives

After studying this theory, students should be able to:

- explain the basic architecture of an MCU;
- identify the major components of the ESP32;
- distinguish CPU, memory, and peripheral functions;
- explain memory-mapped I/O;
- describe the role of peripheral registers;
- perform basic register-level operations;
- apply bitwise operators to hardware registers;
- distinguish register-level programming from HAL programming;
- explain the advantages and disadvantages of hardware abstraction;
- understand how Arduino and ESP-IDF APIs relate to lower-level hardware;
- select an appropriate programming abstraction for an embedded application.

---

## 🧠 3. What Is a Microcontroller?

A microcontroller is essentially a small computer implemented on a single chip.

Typical MCU components include:

```text
┌──────────────────────────────────────┐
│                 MCU                  │
│                                      │
│  ┌────────┐       ┌──────────────┐   │
│  │  CPU   │       │    Memory    │   │
│  └───┬────┘       └──────┬───────┘   │
│      │                   │           │
│  ────┴──── System Bus ───┴────────   │
│      │                   │           │
│  ┌───▼────┐       ┌──────▼───────┐   │
│  │  GPIO  │       │   Timers     │   │
│  └────────┘       └──────────────┘   │
│                                      │
│  ┌────────┐       ┌──────────────┐   │
│  │  ADC   │       │ UART/I²C/SPI │   │
│  └────────┘       └──────────────┘   │
└──────────────────────────────────────┘
```

The CPU executes instructions, while peripherals interact with the physical environment.

---

## ⚙️ 4. ESP32 as a Microcontroller Platform

The ESP32 family is designed for embedded and connected applications. Depending on the particular ESP32 variant, features may include one or more processor cores, SRAM, external flash interface, GPIO, ADC, PWM-capable peripherals, hardware timers, watchdog timers, UART, SPI, I²C, Wi-Fi, Bluetooth/BLE, DMA, interrupt controllers, and hardware security mechanisms.

> Exact CPU architecture, peripheral availability, register definitions, and memory layout vary across ESP32-family devices. Always use the documentation for the specific target device.

---

## 🧮 5. Central Processing Unit

The **CPU** executes program instructions.

A simplified instruction cycle is:

$$
\boxed{
\text{Fetch}
\rightarrow
\text{Decode}
\rightarrow
\text{Execute}
}
$$

The processor repeatedly fetches an instruction from memory, decodes it, executes the required operation, updates registers or memory, and proceeds to the next instruction.

Example:

```c
c = a + b;
```

At the processor level, this involves operations such as loading operands, adding them, and storing the result.

---

## 🧱 6. CPU Registers

CPU registers are small, fast storage locations located within the processor. They may hold operands, addresses, intermediate values, and program state.

CPU registers should not be confused with **peripheral control registers**, which configure hardware modules such as GPIO, timers, UART, and ADC.

---

## 🚌 7. System Bus and Interconnect

The CPU communicates with memory and peripherals through the MCU's internal interconnect.

```text
             CPU
              │
              ▼
       System Interconnect
        /       |       \
       ▼        ▼        ▼
   Memory      GPIO     UART/ADC/Timer
```

A simplified abstraction is:

```text
Address → Which resource?
Data    → What value?
Control → Read or write?
```

---

## 💾 8. MCU Memory

Embedded systems use different memory types for different purposes.

| Memory | Typical Purpose |
|---|---|
| **Flash** | Program and persistent data |
| **SRAM** | Runtime variables, stack, heap |
| **ROM** | Fixed system routines/boot functions |
| **Registers** | CPU/peripheral control and state |

---

## 💽 9. Flash Memory

Flash is **non-volatile**, meaning its contents remain when power is removed. It commonly stores application firmware, constants, filesystem data, configuration data, and OTA firmware partitions.

$$
\text{Power Off}
\Rightarrow
\text{Flash Contents Retained}
$$

---

## 🧠 10. SRAM

SRAM is normally used for runtime information such as global variables, static variables, stack, heap, and runtime buffers.

$$
\text{Power Off}
\Rightarrow
\text{Runtime SRAM Contents Lost}
$$

---

## 📚 11. Stack and Heap

The **stack** is commonly used for local variables, function-call information, and saved execution context. The **heap** supports dynamic memory allocation.

Example:

```c
int *buffer = malloc(100 * sizeof(int));
```

---

## 🗺️ 12. What Is Memory Mapping?

A processor accesses memory locations through numerical addresses. **Memory mapping** defines how the MCU's address space corresponds to memory and hardware resources.

```text
Address          Resource
--------------------------------
0x........       Program/Data
0x........       SRAM
0x........       Peripheral
0x........       System Resource
```

---

## 🧭 13. Address Space

If a processor supports an address width of $n$ bits, the theoretical number of distinct addresses is:

$$
2^n
$$

For a 32-bit address space:

$$
2^{32}=4,294,967,296
$$

distinct byte addresses, corresponding to 4 GiB of theoretical address space.

This does **not** mean that an MCU physically contains 4 GiB of RAM.

---

## 🔌 14. Memory-Mapped I/O

In **memory-mapped I/O**, peripheral registers are assigned addresses within the processor's address space.

```text
CPU
 │
 ▼
Address 0xXXXXXXXX
 │
 ▼
GPIO Register
 │
 ▼
Physical Pin
```

Thus:

$$
\boxed{
\text{Writing a Memory-Mapped Register}
\rightarrow
\text{Changing Hardware State}
}
$$

---

## ⚙️ 15. Peripheral Registers

A peripheral typically exposes several registers, for example:

```text
GPIO Peripheral
     │
     ├── ENABLE Register
     ├── OUTPUT Register
     ├── INPUT Register
     ├── SET Register
     └── CLEAR Register
```

---

## 🔢 16. Register Structure

A 32-bit peripheral register may contain individual control bits. For example, if bit 2 is set:

$$
R=0000\ldots0100_2
$$

then:

$$
R=2^2=4
$$

or:

```text
0x00000004
```

---

## 🔧 17. Register-Level Programming

**Register-level programming** directly reads and writes hardware control registers.

```c
REG |= (1U << 5);
```

This sets bit 5 of `REG`.

$$
R_{\text{new}} =
R_{\text{old}}
\operatorname{OR}
(1 << 5)
$$

---

## 🧮 18. Bitwise Operators

| Operator | Meaning | Example |
|---|---|---|
| `&` | AND | `A & B` |
| `\|` | OR | `A \| B` |
| `^` | XOR | `A ^ B` |
| `~` | NOT | `~A` |
| `<<` | Left shift | `1 << n` |
| `>>` | Right shift | `A >> n` |

---

## 🟢 19. Setting a Bit

```c
REG |= (1U << n);
```

For $n=3$:

```text
1          = 00000001
1 << 3     = 00001000
```

---

## 🔴 20. Clearing a Bit

```c
REG &= ~(1U << n);
```

For bit 3:

```text
1 << 3       00001000
~(1 << 3)    11110111
```

---

## 🔄 21. Toggling a Bit

```c
REG ^= (1U << n);
```

This changes:

```text
0 → 1
1 → 0
```

---

## 🔍 22. Reading a Bit

```c
if (REG & (1U << n)) {
    // Bit is set
}
```

---

## 🎭 23. Bit Masks

A **bit mask** selects particular bits in a register.

```text
Register = 10110110
Mask     = 00001111
Result   = 00000110
```

---

## 💡 24. Example: LED Control Concept

An LED connected to a GPIO pin normally requires:

1. configuring the GPIO as an output;
2. setting the output state.

Conceptually:

```c
GPIO_ENABLE |= LED_MASK;
GPIO_OUT    |= LED_MASK;
```

The exact register names depend on the target MCU.

---

## 🔘 25. Example: Push Button Input

A button can be read through a GPIO input register.

```c
if (GPIO_INPUT & BUTTON_MASK) {
    // Button active
}
```

Pull-up/pull-down configuration and active-high/active-low behavior must also be considered.

---

## ⚠️ 26. Why Direct Register Programming Is Difficult

Direct register programming requires knowledge of register addresses, widths, bit positions, reset values, read/write behavior, timing requirements, peripheral dependencies, and device-specific restrictions.

This motivates the use of a **Hardware Abstraction Layer**.

---

## 🧱 27. Hardware Abstraction Layer

A **Hardware Abstraction Layer (HAL)** hides many hardware-specific details.

Instead of:

```c
REG |= (1U << PIN);
```

the programmer may use:

```c
gpio_set_level(pin, 1);
```

or:

```cpp
digitalWrite(pin, HIGH);
```

The conceptual hierarchy is:

```text
Application
     │
     ▼
High-Level API / HAL
     │
     ▼
Driver
     │
     ▼
Peripheral Registers
     │
     ▼
Hardware
```

---

## 🧠 28. Why Use HAL?

HAL programming provides:

- easier development;
- improved readability;
- reduced dependence on register layouts;
- fewer hardware-specific errors;
- better maintainability;
- better portability.

---

## ⚖️ 29. Register Level vs. HAL

| Feature | Register Level | HAL |
|---|---|---|
| Hardware control | Very high | High |
| Abstraction | Low | High |
| Ease of learning | Difficult | Easier |
| Portability | Low | Better |
| Code readability | Lower | Higher |
| Hardware knowledge required | High | Moderate |
| Development speed | Slower | Faster |
| Fine optimization potential | High | Depends on API |

---

## 🏗️ 30. Software Abstraction Layers

```text
┌─────────────────────────────┐
│      User Application       │
├─────────────────────────────┤
│ Arduino API / ESP-IDF API   │
├─────────────────────────────┤
│ Drivers / HAL / LL Layers   │
├─────────────────────────────┤
│ Peripheral Registers        │
├─────────────────────────────┤
│ ESP32 Hardware              │
└─────────────────────────────┘
```

---

## 🔵 31. Arduino Abstraction

Arduino provides high-level functions such as:

```cpp
pinMode()
digitalWrite()
digitalRead()
analogRead()
delay()
Serial.begin()
```

Example:

```cpp
pinMode(2, OUTPUT);
digitalWrite(2, HIGH);
```

---

## 🟢 32. ESP-IDF

**ESP-IDF** is Espressif's official development framework for ESP32-family devices.

Example:

```c
gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_NUM_2, 1);
```

ESP-IDF provides APIs for GPIO, timers, UART, SPI, I²C, Wi-Fi, BLE, FreeRTOS, networking, OTA, and security.

---

## 🔽 33. Low-Level Access

Low-level access may be required for precise timing, specialized peripherals, performance optimization, custom drivers, hardware debugging, and learning MCU architecture.

A good embedded engineer should understand the lower layers even when primarily using high-level APIs.

---

## 🔄 34. Read-Modify-Write

A common operation is:

```c
REG |= (1U << 5);
```

Conceptually:

$$
R_{\text{new}} =
R_{\text{old}}
\lor
M
$$

where:

$$
M=1\ll5
$$

Some hardware registers have special semantics, so read-modify-write is not always safe.

---

## ⚡ 35. Atomic Register Operations

In concurrent or interrupt-driven systems, read-modify-write sequences can create race conditions. Some peripherals therefore provide dedicated `SET` and `CLEAR` registers or other atomic mechanisms.

---

## ⏱️ 36. Volatile Keyword

Hardware registers can change independently of normal program flow. C/C++ therefore commonly uses `volatile` for memory-mapped hardware access.

```c
volatile uint32_t *reg;
```

`volatile` does **not** provide thread safety, atomicity, or synchronization.

---

## 🧮 37. Generic Memory-Mapped Register Access

```c
#define REG_ADDR 0xXXXXXXXX

volatile uint32_t *reg =
    (volatile uint32_t *)REG_ADDR;

*reg |= (1U << 3);
```

In real ESP32 applications, official headers and documented register macros should be used.

---

## 📖 38. Datasheets and Technical Reference Manuals

Important documents include:

### Datasheet

- electrical characteristics;
- pin functions;
- package information;
- major peripherals;
- operating limits.

### Technical Reference Manual

- peripheral architecture;
- register definitions;
- register fields;
- timing;
- interrupts;
- hardware behavior.

### API Documentation

- functions;
- parameters;
- return values;
- configuration structures;
- driver usage.

---

## 🧭 39. Understanding a Register Description

| Bits | Name | Access | Description |
|---|---|---|---|
| 31:8 | Reserved | — | Reserved |
| 7 | ENABLE | R/W | Enable peripheral |
| 6:4 | MODE | R/W | Operating mode |
| 3:1 | Reserved | — | Reserved |
| 0 | START | R/W | Start operation |

---

## 🧮 40. Multi-Bit Register Fields

To replace a multi-bit field:

```c
REG = (REG & ~FIELD_MASK) |
      ((value << FIELD_SHIFT) & FIELD_MASK);
```

This clears the old field, shifts and masks the new value, and inserts it into the register.

---

## 🧩 41. GPIO Example Using Arduino

```cpp
const int LED_PIN = 2;

void setup() {
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(500);

    digitalWrite(LED_PIN, LOW);
    delay(500);
}
```

---

## ⚙️ 42. GPIO Example Using ESP-IDF

```c
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2

void app_main(void)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

---

## 🔍 43. Comparing the Programming Levels

The same operation, **Turn LED ON**, can be expressed as:

### Application Level

```cpp
LED_ON();
```

### Arduino API

```cpp
digitalWrite(LED_PIN, HIGH);
```

### ESP-IDF Driver

```c
gpio_set_level(LED_PIN, 1);
```

### Physical Result

```text
GPIO Voltage Changes
        │
        ▼
      LED ON
```

---

## 🔘 44. Button-to-LED Example

```cpp
const int BUTTON = 4;
const int LED = 2;

void setup() {
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
}

void loop() {
    int state = digitalRead(BUTTON);

    if (state == LOW) {
        digitalWrite(LED, HIGH);
    } else {
        digitalWrite(LED, LOW);
    }
}
```

---

## 🧠 45. Polling

Polling repeatedly checks an input:

```text
Read Button
    │
    ▼
Check State
    │
    ▼
Update LED
    │
    └──────► Repeat
```

Polling is simple but can waste processor time if events are infrequent.

---

## 🚨 46. Interrupt Connection

```text
External Event
      │
      ▼
GPIO Peripheral
      │
      ▼
Interrupt Flag
      │
      ▼
Interrupt Controller
      │
      ▼
CPU
      │
      ▼
ISR
```

HAL/driver functions usually configure the underlying registers on behalf of the programmer.

---

## ⏲️ 47. Timer Connection

```text
Clock
  │
  ▼
Prescaler
  │
  ▼
Counter
  │
  ▼
Compare / Alarm
  │
  ▼
Interrupt
```

Registers may control prescaler, counter value, alarm value, enable state, and interrupts.

---

## 📡 48. Communication Peripheral Registers

Communication interfaces such as UART, SPI, and I²C expose registers for configuration, data, status, and interrupts.

Instead of configuring UART registers directly, an application may use:

```cpp
Serial.begin(115200);
```

---

## 🧱 49. HAL Design Concept

```text
Application
     │
     ▼
HAL Function
     │
     ▼
Device Driver
     │
     ▼
Register Operations
     │
     ▼
Hardware
```

Example:

```c
HAL_LED_Write(1);
```

---

## 🧩 50. Creating a Simple Custom HAL

```cpp
void LED_Init() {
    pinMode(2, OUTPUT);
}

void LED_On() {
    digitalWrite(2, HIGH);
}

void LED_Off() {
    digitalWrite(2, LOW);
}
```

---

## 🏗️ 51. Benefits of Modular HAL Design

A HAL improves:

$$
\boxed{
\text{Maintainability}
+
\text{Portability}
+
\text{Readability}
}
$$

---

## 🔗 52. MCU Programming Stack

```text
┌─────────────────────────────┐
│        Application          │
├─────────────────────────────┤
│ Algorithms / State Machine  │
├─────────────────────────────┤
│ Middleware / RTOS           │
├─────────────────────────────┤
│ Drivers / HAL               │
├─────────────────────────────┤
│ Register Definitions        │
├─────────────────────────────┤
│ MCU Hardware                │
└─────────────────────────────┘
```

---

## 🤖 53. Connection to IoT Systems

```text
Sensor
  │
  ▼
ADC / I²C
  │
  ▼
ESP32 Peripheral
  │
  ▼
Driver / HAL
  │
  ▼
Application
  │
  ▼
Wi-Fi
  │
  ▼
MQTT
  │
  ▼
Cloud
```

---

## 🧠 54. Connection to TinyML

```text
Sensor
   │
   ▼
Peripheral
   │
   ▼
HAL / Driver
   │
   ▼
Data Buffer
   │
   ▼
TinyML Model
   │
   ▼
Prediction
   │
   ▼
GPIO / Network
```

$$
\boxed{
\text{TinyML System} =
\text{MCU}
+
\text{Memory}
+
\text{Sensors}
+
\text{Inference Software}
}
$$

---

## ⚡ 55. Performance Considerations

A simplified timing model is:

$$
T_{\text{operation}} =
T_{\text{software}}
+
T_{\text{driver}}
+
T_{\text{hardware}}
$$

Register-level optimization becomes important for extremely tight timing, high interrupt rates, specialized hardware behavior, or highly optimized drivers.

---

## 🛡️ 56. Safety of Register-Level Programming

Incorrect register manipulation can cause unexpected peripheral behavior, crashes, watchdog resets, communication failure, GPIO conflicts, and system instability.

> Never write arbitrary values to undocumented or reserved registers.

---

## 🧪 57. Practical Exercise 1 — Bit Operations

Given:

```text
REG = 00110100
```

Set bit 1.

Result:

```text
00110110
```

---

## 🧪 58. Practical Exercise 2 — Clear a Bit

Given:

```text
REG = 00110110
```

Clear bit 2.

Result:

```text
00110010
```

---

## 🧪 59. Practical Exercise 3 — Read a Bit

Given:

```text
REG = 10100100
```

Test bit 5 with:

```text
Mask = 00100000
```

The result is nonzero, therefore:

$$
\boxed{\text{Bit 5}=1}
$$

---

## 🧪 60. Practical Exercise 4 — HAL Design

Create:

```c
LED_Init();
LED_On();
LED_Off();
LED_Toggle();
```

Then write an application that uses only these functions.

---

## 🧪 61. Practical Exercise 5 — Compare APIs

Implement LED blinking using:

- Arduino `digitalWrite()`;
- ESP-IDF `gpio_set_level()`.

Compare readability, setup complexity, portability, and hardware knowledge required.

---

## 📊 62. Comparison Exercise

| Characteristic | Register Level | ESP-IDF | Arduino |
|---|---:|---:|---:|
| Ease of use | | | |
| Hardware control | | | |
| Portability | | | |
| Readability | | | |
| Learning difficulty | | | |
| Development speed | | | |

---

## 💬 63. Discussion Questions

1. What is a microcontroller?
2. How does an MCU differ from a general-purpose computer?
3. What are the major components of an MCU?
4. What is the role of the CPU?
5. What is SRAM?
6. What is flash memory?
7. What is the difference between volatile and non-volatile memory?
8. What are the stack and heap?
9. What is an address space?
10. What is memory mapping?
11. What is memory-mapped I/O?
12. What is a peripheral register?
13. Why are GPIO registers needed?
14. What is register-level programming?
15. What is a bit mask?
16. How do you set a register bit?
17. How do you clear a register bit?
18. How do you toggle a bit?
19. How do you test a bit?
20. What is a read-modify-write operation?
21. Why can read-modify-write create problems in concurrent systems?
22. What does `volatile` mean in embedded C?
23. Why is `volatile` not a replacement for synchronization?
24. What is a HAL?
25. Why is HAL useful?
26. What is the difference between HAL and register-level programming?
27. What is ESP-IDF?
28. What role does the Arduino core play on ESP32?
29. When might low-level programming be preferred?
30. Why should reserved register bits be handled carefully?
31. What information is found in a datasheet?
32. What information is found in a technical reference manual?
33. How do MCU peripherals support IoT systems?
34. How does TinyML depend on MCU architecture?
35. Why should embedded engineers understand both high- and low-level programming?

---

## 🧾 64. Expected Learning Outcomes

After completing this topic, students should be able to:

- describe the architecture of a microcontroller;
- identify CPU, memory, buses, and peripherals;
- explain ESP32 as an embedded computing platform;
- distinguish SRAM, flash, and ROM conceptually;
- explain stack and heap memory;
- describe an MCU address space;
- explain memory-mapped I/O;
- describe peripheral registers;
- manipulate register bits using masks;
- use AND, OR, XOR, NOT, and shift operators;
- explain read-modify-write operations;
- explain the role of `volatile`;
- distinguish register-level and HAL programming;
- use Arduino APIs for hardware control;
- recognize ESP-IDF driver-based programming;
- design a simple custom HAL;
- understand the relationship between application software and physical hardware;
- use MCU documentation when developing low-level embedded applications.

---

## 🔑 65. Key Concepts

### MCU Architecture

$$
\boxed{
\text{MCU} =
\text{CPU}
+
\text{Memory}
+
\text{Peripherals}
}
$$

### Embedded System

$$
\boxed{
\text{Embedded System} =
\text{MCU}
+
\text{Hardware}
+
\text{Software}
}
$$

### Address Space

$$
\boxed{
N_{\text{addresses}}=2^n
}
$$

### Memory-Mapped I/O

$$
\boxed{
\text{Memory Address}
\leftrightarrow
\text{Hardware Register}
}
$$

### Set Bit

```c
REG |= (1U << n);
```

### Clear Bit

```c
REG &= ~(1U << n);
```

### Toggle Bit

```c
REG ^= (1U << n);
```

### Read Bit

```c
REG & (1U << n)
```

### Hardware Abstraction

$$
\boxed{
\text{Application}
\rightarrow
\text{HAL/Driver}
\rightarrow
\text{Registers}
\rightarrow
\text{Hardware}
}
$$

---

## 🎯 66. Summary

Understanding MCU architecture is fundamental to embedded-system engineering.

The processor communicates with hardware peripherals through the MCU's internal architecture and associated control registers.

The basic relationship is:

```text
Application
     │
     ▼
API / HAL
     │
     ▼
Driver
     │
     ▼
Memory-Mapped Registers
     │
     ▼
MCU Peripheral
     │
     ▼
Physical Hardware
```

**Memory mapping** allows hardware resources to appear within the processor's address space.

**Register-level programming** provides detailed control by directly manipulating peripheral configuration and state.

**HAL programming** provides a higher-level interface that improves readability, maintainability, and development speed.

For ESP32 development:

$$
\boxed{
\text{Arduino}
\rightarrow
\text{ESP-IDF}
\rightarrow
\text{HAL/Low-Level Interfaces}
\rightarrow
\text{Registers}
\rightarrow
\text{Hardware}
}
$$

A strong embedded-systems engineer should understand all these levels, even when most application development is performed using higher-level frameworks.

---

## 📘 References

1. Espressif Systems, *ESP32 Series Datasheets*.  
2. Espressif Systems, *ESP32 Series Technical Reference Manuals*.  
3. Espressif Systems, *ESP-IDF Programming Guide*.  
4. Espressif Systems, *ESP-IDF GPIO Driver Documentation*.  
5. Arduino, *Arduino Language Reference*.  
6. J. Yiu, *The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors*, Newnes.  
7. J. W. Valvano, *Embedded Systems: Introduction to ARM Cortex-M Microcontrollers*.  
8. M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly Media.  
9. P. Marwedel, *Embedded System Design*, Springer.  
10. J.-P. Vasseur and A. Dunkels, *Interconnecting Smart Objects with IP: The Next Internet*, Morgan Kaufmann.

---
