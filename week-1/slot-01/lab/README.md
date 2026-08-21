# 📘 Theory: MCU Architecture, Memory Mapping, Register-Level Programming, and HAL Basics

## ESP32 MCU Case

## 1. Introduction

A **microcontroller unit (MCU)** is a compact computing system that integrates a processor core, memory, digital and analog peripherals, communication interfaces, timers, and control logic on a single chip. Unlike a general-purpose computer, an MCU is designed to interact directly with physical devices such as sensors, actuators, switches, displays, motors, and communication modules.

The ESP32 family is a useful case study because it combines:

* a programmable CPU,
* internal SRAM and ROM,
* external flash support,
* GPIO,
* timers,
* ADC/DAC capabilities on selected devices,
* PWM,
* UART,
* SPI,
* I²C,
* Wi-Fi,
* Bluetooth,
* interrupt controllers,
* low-power subsystems.

The basic embedded-system model is

$$
\boxed{
\text{CPU}
+
\text{Memory}
+
\text{Peripherals}
+
\text{Software}
===============

\text{MCU System}
}
$$

---

# 2. MCU Architecture

## 2.1 Basic MCU Structure

A typical MCU can be represented as

```text
                 ┌─────────────────┐
                 │      CPU        │
                 │  ALU + Control  │
                 └────────┬────────┘
                          │
                    System Bus
                          │
        ┌─────────────────┼─────────────────┐
        ▼                 ▼                 ▼
      Memory          Peripherals       Interrupts
        │                 │                 │
   Flash / SRAM       GPIO / UART      IRQ Controller
                      SPI / I2C
                      ADC / Timer
```

The CPU executes instructions, memory stores code and data, and peripherals interface the MCU with the outside world.

---

## 2.2 CPU Core

The processor executes the instruction cycle

$$
\boxed{
\text{Fetch}
\rightarrow
\text{Decode}
\rightarrow
\text{Execute}
}
$$

During execution, the CPU performs operations such as:

* arithmetic,
* logical operations,
* data movement,
* branch operations,
* memory accesses,
* peripheral accesses.

For example,

```c
c = a + b;
```

may eventually produce processor instructions that:

1. load `a`,
2. load `b`,
3. add them,
4. store the result.

---

# 3. ESP32 MCU Architecture

The original ESP32 commonly uses dual Xtensa LX6 CPU cores, while newer ESP32 variants may use Xtensa or RISC-V cores depending on the device.

Therefore, when designing for the ESP32 family, the exact architecture should always be checked for the selected chip.

A simplified ESP32 architecture is

```text
                    ┌──────────────────┐
                    │   CPU Core(s)    │
                    └────────┬─────────┘
                             │
                       System Bus
                             │
          ┌──────────────────┼──────────────────┐
          ▼                  ▼                  ▼
       Memory              GPIO            Communication
   ROM / SRAM / Flash       │              UART / SPI / I2C
                            │
          ┌─────────────────┼──────────────────┐
          ▼                 ▼                  ▼
        Timers             ADC               PWM
                                               │
                                               ▼
                                           Actuators
```

Wireless subsystems add

```text
Wi-Fi
Bluetooth
```

to the embedded platform.

---

# 4. Harvard and Von Neumann Concepts

MCU architectures are often explained using two classical memory organizations.

## 4.1 Von Neumann Architecture

Instructions and data share a common memory and bus.

```text
CPU
 │
 ▼
Common Bus
 │
 ▼
Instructions + Data
```

---

## 4.2 Harvard Architecture

Instructions and data use separate paths.

```text
             CPU
          /       \
         ▼         ▼
Instruction     Data
 Memory        Memory
```

Many modern MCUs use a modified Harvard architecture internally to improve performance while still presenting a unified software address space.

---

# 5. MCU Memory Types

An MCU commonly contains several forms of memory.

| Memory Type                | Purpose                                             |
| -------------------------- | --------------------------------------------------- |
| **ROM**                    | Boot code, fixed functions                          |
| **Flash**                  | Program storage and persistent data                 |
| **SRAM**                   | Runtime variables and stack                         |
| **Registers**              | Fast CPU and peripheral state                       |
| **RTC Memory**             | Low-power data retention on supported ESP32 devices |
| **External Flash / PSRAM** | Additional program or data storage                  |

---

# 6. Program Memory

Program instructions are commonly stored in nonvolatile flash.

For example:

```text
Power OFF
   │
   ▼
Program remains stored
   │
   ▼
Power ON
   │
   ▼
Bootloader starts
   │
   ▼
Application runs
```

This makes flash suitable for firmware storage.

---

# 7. SRAM

SRAM stores temporary runtime information.

Examples include:

```c
int counter;
float temperature;
char buffer[128];
```

These values normally exist while the MCU is powered.

SRAM is used for:

* local variables,
* global variables,
* stack,
* heap,
* communication buffers.

---

# 8. Stack and Heap

## 8.1 Stack

The stack stores items such as:

* function parameters,
* local variables,
* return addresses,
* saved registers.

Conceptually:

```text
Function Call
    │
    ▼
Stack Frame Created
    │
    ▼
Function Executes
    │
    ▼
Stack Frame Removed
```

---

## 8.2 Heap

The heap is used for dynamic memory allocation.

For example:

```c
ptr = malloc(100);
```

allocates memory during runtime.

Embedded systems should use dynamic allocation carefully because fragmentation and unpredictable memory usage can reduce reliability.

---

# 9. Memory Mapping

## 9.1 Concept

In an MCU, memory and peripheral hardware are assigned specific addresses.

This is called a **memory map**.

The CPU may see the system as

```text
Address Space
     │
     ├── Program Memory
     ├── SRAM
     ├── Peripheral Registers
     ├── ROM
     └── External Memory
```

Therefore,

$$
\boxed{
\text{Address}
\rightarrow
\text{Specific Hardware Resource}
}
$$

---

# 10. Memory-Mapped I/O

A peripheral register can be accessed as if it were a memory location.

For example, conceptually,

```text
0x....0000 → GPIO control register
0x....0004 → GPIO output register
0x....0008 → GPIO input register
```

The exact addresses depend on the ESP32 device.

The CPU can therefore perform something conceptually similar to

$$
\text{Memory Write}
\rightarrow
\text{GPIO Register}
\rightarrow
\text{Physical Pin}
$$

---

# 11. Peripheral Registers

A hardware peripheral is controlled by registers.

Examples:

* configuration register,
* status register,
* data register,
* interrupt-enable register.

A GPIO peripheral might conceptually contain

| Register      | Function                   |
| ------------- | -------------------------- |
| `GPIO_ENABLE` | Configure output direction |
| `GPIO_OUT`    | Set output value           |
| `GPIO_IN`     | Read input value           |
| `GPIO_SET`    | Set selected bits          |
| `GPIO_CLEAR`  | Clear selected bits        |

The exact names vary by architecture and software layer.

---

# 12. Register Bit Fields

One register often contains multiple control fields.

For example, imagine an 8-bit register

```text
Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1   Bit 0
 EN      MODE    IRQ     ---      ---     CFG1    CFG0    STATUS
```

Different bits control different hardware functions.

Register programming therefore requires operations such as:

* set bit,
* clear bit,
* toggle bit,
* test bit.

---

# 13. Bitwise Operations

Embedded programming heavily uses bitwise operators.

## Set a Bit

```c
reg |= (1 << n);
```

This forces bit (n) to `1`.

---

## Clear a Bit

```c
reg &= ~(1 << n);
```

This forces bit (n) to `0`.

---

## Toggle a Bit

```c
reg ^= (1 << n);
```

This reverses bit (n).

---

## Test a Bit

```c
if (reg & (1 << n)) {
    // bit is set
}
```

---

# 14. Register-Level Programming

Register-level programming accesses hardware registers directly.

Conceptually:

```c
REGISTER = VALUE;
```

or

```c
REGISTER |= MASK;
```

This gives very precise hardware control.

The basic flow is

```text
C Code
  │
  ▼
Register Address
  │
  ▼
Peripheral Register
  │
  ▼
Hardware Behavior
```

---

# 15. Advantages of Register-Level Programming

Register-level programming provides:

* precise control,
* low overhead,
* high performance,
* detailed understanding of hardware,
* deterministic peripheral configuration.

It is useful for:

* device-driver development,
* high-performance code,
* custom timing,
* low-level debugging.

---

# 16. Disadvantages of Register-Level Programming

Direct register access also has disadvantages:

* device-specific code,
* reduced portability,
* more difficult debugging,
* higher risk of configuration errors,
* dependency on datasheets and technical reference manuals.

Therefore,

$$
\boxed{
\text{Low-Level Control}
\leftrightarrow
\text{Software Complexity}
}
$$

---

# 17. ESP32 Register-Level Concept

An ESP32 GPIO output may conceptually be controlled through dedicated GPIO registers.

The programming model is

```text
CPU
 │
 ▼
GPIO Register
 │
 ▼
GPIO Peripheral
 │
 ▼
Physical Pin
 │
 ▼
LED
```

A direct register-level implementation typically requires knowledge of:

* peripheral base addresses,
* register offsets,
* bit definitions.

For portable application development, ESP-IDF generally exposes higher-level APIs instead.

---

# 18. Volatile Keyword

Hardware registers can change independently of normal program flow.

Therefore, low-level register pointers are commonly declared as `volatile`.

Conceptually:

```c
volatile uint32_t *reg;
```

`volatile` tells the compiler that the value may change unexpectedly and should not be optimized away based on ordinary variable assumptions.

---

# 19. Why `volatile` Matters

Suppose the CPU repeatedly checks a status register:

```c
while ((*status_reg & READY_BIT) == 0) {
}
```

The hardware may set `READY_BIT` later.

Without appropriate volatile semantics, a compiler could incorrectly assume that the value never changes.

Thus,

$$
\boxed{
\text{Hardware Register}
\Rightarrow
\text{Volatile Access}
}
$$

is an important low-level programming concept.

---

# 20. Hardware Abstraction Layer

A **Hardware Abstraction Layer (HAL)** provides software functions that hide low-level register details.

Instead of writing

```text
Set register X
Clear bit Y
Configure register Z
```

the application can use

```c
gpio_set_level(pin, 1);
```

or an equivalent API.

The architecture becomes

```text
Application
    │
    ▼
HAL / Driver API
    │
    ▼
Registers
    │
    ▼
Peripheral
    │
    ▼
Hardware
```

---

# 21. HAL Advantages

HAL-based development provides:

* improved readability,
* faster development,
* reduced hardware-specific code,
* easier maintenance,
* better portability,
* safer peripheral configuration.

The trade-off is that abstraction can add some overhead or hide lower-level details.

---

# 22. ESP32 Software Stack

A simplified ESP32 software stack can be represented as

```text
Application Code
      │
      ▼
ESP-IDF APIs
      │
      ▼
Drivers / HAL
      │
      ▼
Low-Level Layer
      │
      ▼
Peripheral Registers
      │
      ▼
ESP32 Hardware
```

ESP-IDF provides APIs for:

* GPIO,
* UART,
* SPI,
* I²C,
* timers,
* Wi-Fi,
* Bluetooth,
* FreeRTOS integration.

---

# 23. Register-Level versus HAL Programming

| Feature              | Register-Level        | HAL / Driver API        |
| -------------------- | --------------------- | ----------------------- |
| Control              | Very high             | High                    |
| Portability          | Low                   | Higher                  |
| Complexity           | High                  | Lower                   |
| Development speed    | Slower                | Faster                  |
| Hardware knowledge   | Extensive             | Moderate                |
| Performance overhead | Minimal               | Usually small           |
| Readability          | Lower                 | Higher                  |
| Best use             | Drivers, optimization | Application development |

---

# 24. ESP32 GPIO Example Using HAL

A typical ESP-IDF-style GPIO program concept is

```c
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2

void app_main(void)
{
    gpio_reset_pin(LED_PIN);

    gpio_set_direction(
        LED_PIN,
        GPIO_MODE_OUTPUT
    );

    while (1)
    {
        gpio_set_level(
            LED_PIN,
            1
        );

        gpio_set_level(
            LED_PIN,
            0
        );
    }
}
```

The API hides the internal register-level implementation.

---

# 25. GPIO Configuration Flow

A GPIO output normally requires several configuration steps.

```text
Select GPIO Pin
      │
      ▼
Configure Pin Function
      │
      ▼
Configure Direction
      │
      ▼
Set Output Level
```

In a HAL-based environment, these steps are handled by driver functions.

---

# 26. GPIO Input Example

Conceptually:

```c
gpio_set_direction(
    BUTTON_PIN,
    GPIO_MODE_INPUT
);

int value =
    gpio_get_level(
        BUTTON_PIN
    );
```

The data flow is

```text
Push Button
    │
    ▼
GPIO Input Circuit
    │
    ▼
GPIO Register
    │
    ▼
Driver API
    │
    ▼
Application Variable
```

---

# 27. Pull-Up and Pull-Down Resistors

Digital inputs should not normally be left floating.

An input can be configured with:

* pull-up,
* pull-down,
* external resistor.

For a pull-up configuration:

```text
VCC
 │
 R
 │
 ├──── GPIO
 │
Switch
 │
GND
```

The input normally reads `1` and becomes `0` when the switch is pressed.

---

# 28. Peripheral Multiplexing

One ESP32 pin may support several functions.

For example, a physical pin may be usable as:

* GPIO,
* UART,
* SPI,
* PWM,
* ADC,

depending on the selected chip and pin-matrix capabilities.

Thus,

$$
\boxed{
\text{Physical Pin}
\neq
\text{Single Fixed Function}
}
$$

on many modern MCUs.

---

# 29. Interrupts

Polling repeatedly checks hardware status:

```text
Check Input
   │
   ▼
Check Again
   │
   ▼
Check Again
```

Interrupts allow hardware to notify the CPU.

```text
Peripheral Event
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

ISR means **Interrupt Service Routine**.

---

# 30. Polling versus Interrupts

| Feature                    | Polling         | Interrupt      |
| -------------------------- | --------------- | -------------- |
| CPU repeatedly checks      | Yes             | No             |
| Response efficiency        | Lower           | Higher         |
| Simplicity                 | High            | Moderate       |
| Real-time response         | Depends on loop | Usually better |
| CPU can perform other work | Limited         | Yes            |

---

# 31. ESP32 GPIO Interrupt Concept

A GPIO can generate an interrupt when an event occurs such as:

* rising edge,
* falling edge,
* any edge,
* selected logic level.

The processing flow is

```text
Button Press
    │
    ▼
GPIO Edge
    │
    ▼
Interrupt Request
    │
    ▼
CPU ISR
    │
    ▼
Application Response
```

---

# 32. Timers

Timers are hardware counters driven by a clock source.

A timer can be used for:

* delays,
* periodic interrupts,
* frequency measurement,
* event timing,
* PWM generation.

Conceptually,

$$
COUNT[n+1]=COUNT[n]+1.
$$

When the timer reaches a programmed value,

$$
COUNT=COMPARE,
$$

an event can be generated.

---

# 33. UART Peripheral

UART provides asynchronous serial communication.

The typical frame is

```text
Start | Data Bits | Stop
```

A common configuration is

```text
115200 baud
8 data bits
No parity
1 stop bit
```

or

```text
115200-8-N-1
```

The architecture is

```text
ESP32
  │
  ▼
UART Peripheral
  │
  ▼
TX / RX Pins
  │
  ▼
PC or External MCU
```

---

# 34. SPI Peripheral

SPI is a synchronous serial protocol.

Typical signals are:

* SCLK,
* MOSI,
* MISO,
* CS.

The master controls communication timing.

```text
Master
 │
 ├── SCLK
 ├── MOSI
 ├── MISO
 └── CS
      │
      ▼
    Slave
```

SPI is commonly used for:

* displays,
* ADCs,
* flash memory,
* sensors.

---

# 35. I²C Peripheral

I²C typically uses two wires:

* SDA,
* SCL.

Multiple devices can share the same bus using addresses.

```text
MCU
 │
 ├── SDA ──────────┬── Sensor 1
 └── SCL ──────────┼── Sensor 2
                   └── Sensor 3
```

This is widely used for sensors and low-speed peripherals.

---

# 36. MCU Register-Level Development Flow

A low-level workflow is

```text
Read Datasheet
     │
     ▼
Read Technical Reference Manual
     │
     ▼
Find Peripheral Base Address
     │
     ▼
Find Register Offset
     │
     ▼
Configure Bit Fields
     │
     ▼
Test Peripheral
```

This process requires detailed understanding of the hardware.

---

# 37. HAL-Based Development Flow

A HAL-based workflow is simpler:

```text
Select Peripheral
      │
      ▼
Call Driver Configuration API
      │
      ▼
Call Read/Write API
      │
      ▼
Test Application
```

This is more appropriate for many application-level projects.

---

# 38. Example: LED Control at Three Abstraction Levels

## Level 1 — Hardware Concept

```text
GPIO Output Register
        │
        ▼
     GPIO Pin
        │
        ▼
       LED
```

## Level 2 — Register-Level Concept

```c
GPIO_REGISTER |= LED_MASK;
```

## Level 3 — HAL API

```c
gpio_set_level(LED_PIN, 1);
```

All three ultimately affect the same hardware.

---

# 39. Example: Button-to-LED System

A simple embedded application is

```text
Button
   │
   ▼
GPIO Input
   │
   ▼
CPU
   │
   ▼
GPIO Output
   │
   ▼
LED
```

The algorithm is

```text
Read Button
    │
    ▼
If Pressed?
 ┌──┴──┐
Yes   No
 │     │
 ▼     ▼
LED ON LED OFF
```

---

# 40. Example HAL Implementation

```c
#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_0
#define LED_PIN    GPIO_NUM_2

void app_main(void)
{
    gpio_reset_pin(BUTTON_PIN);
    gpio_reset_pin(LED_PIN);

    gpio_set_direction(
        BUTTON_PIN,
        GPIO_MODE_INPUT
    );

    gpio_set_direction(
        LED_PIN,
        GPIO_MODE_OUTPUT
    );

    while (1)
    {
        int button =
            gpio_get_level(
                BUTTON_PIN
            );

        gpio_set_level(
            LED_PIN,
            button
        );
    }
}
```

The exact active level depends on the circuit and board configuration.

---

# 41. Read-Modify-Write Concept

Peripheral control often requires changing one field without modifying other register bits.

Suppose

```text
Register = 10110010
```

and only bit 2 should be set.

A read-modify-write operation is

```text
Read Register
      │
      ▼
Modify Selected Bit
      │
      ▼
Write Register Back
```

In C:

```c
reg |= (1U << 2);
```

---

# 42. Masks

A **bit mask** selects specific bits.

For example,

```c
#define BIT3 (1U << 3)
```

gives

```text
00001000
```

A multi-bit field may use

```c
#define FIELD_MASK (0x3U << 4)
```

which targets bits 5:4.

Masks are fundamental in register-level embedded programming.

---

# 43. Setting a Multi-Bit Field

Suppose a two-bit mode field occupies bits 5:4.

First clear the old value:

```c
reg &= ~(0x3U << 4);
```

Then write the new value:

```c
reg |= ((mode & 0x3U) << 4);
```

This preserves unrelated register bits.

---

# 44. Atomic Set/Clear Registers

Many MCU peripherals provide dedicated set and clear registers.

Instead of:

```text
Read
Modify
Write
```

software may perform:

```text
Write mask to SET register
```

or

```text
Write mask to CLEAR register
```

This can reduce race conditions and improve efficiency.

The exact capabilities depend on the selected ESP32 device.

---

# 45. Concurrency and Shared Registers

In embedded systems, registers or variables may be accessed by:

* main code,
* interrupts,
* RTOS tasks,
* hardware.

Therefore, designers must consider:

* race conditions,
* atomic operations,
* critical sections,
* synchronization.

This becomes especially important when using FreeRTOS on ESP32.

---

# 46. ESP32 and FreeRTOS

ESP-IDF commonly uses FreeRTOS concepts.

The application may contain multiple tasks:

```text
Task 1 → Read Sensor
Task 2 → Update Display
Task 3 → Send Wi-Fi Data
Task 4 → Control Actuator
```

The scheduler shares CPU time among these tasks.

This adds an additional software layer above the MCU peripherals.

---

# 47. Embedded Software Layer Model

A complete ESP32 software stack can be represented as

```text
Application
     │
     ▼
FreeRTOS Tasks
     │
     ▼
ESP-IDF Drivers
     │
     ▼
HAL / Low-Level Drivers
     │
     ▼
Peripheral Registers
     │
     ▼
ESP32 Hardware
```

Each layer provides a different level of abstraction.

---

# 48. When to Use Register-Level Programming

Register-level programming is useful when:

* learning MCU hardware,
* developing drivers,
* optimizing execution time,
* implementing unusual peripheral behavior,
* debugging low-level issues,
* minimizing overhead.

---

# 49. When to Use HAL APIs

HAL or driver APIs are usually preferable when:

* developing applications quickly,
* working with complex peripherals,
* improving portability,
* maintaining large projects,
* reducing programming errors,
* using Wi-Fi, Bluetooth, networking, or RTOS services.

---

# 50. Recommended Learning Progression

A useful learning path is

```text
Digital Logic
     │
     ▼
CPU Architecture
     │
     ▼
Memory Map
     │
     ▼
Registers
     │
     ▼
GPIO Register Control
     │
     ▼
HAL / Driver APIs
     │
     ▼
Interrupts
     │
     ▼
Timers / UART / SPI / I2C
     │
     ▼
RTOS Applications
     │
     ▼
IoT Systems
```

---

# 51. Discussion Questions

1. What is an MCU?
2. What are the main components of an MCU?
3. What is the role of the CPU?
4. What is the difference between flash and SRAM?
5. What is a memory map?
6. What is memory-mapped I/O?
7. What is a peripheral register?
8. Why are bit masks important?
9. Why is `volatile` commonly used with hardware registers?
10. What is register-level programming?
11. What is a HAL?
12. What are the advantages of using a HAL?
13. What is the difference between polling and interrupts?
14. Why might an input require pull-up or pull-down resistors?
15. What is the difference between UART, SPI, and I²C?

---

# 52. Practical Exercises

## Exercise 1 — Memory Map

Draw a conceptual MCU memory map containing:

* ROM,
* Flash,
* SRAM,
* GPIO registers,
* UART registers.

---

## Exercise 2 — Bit Manipulation

Given

```text
register = 00101010
```

perform:

1. set bit 0,
2. clear bit 3,
3. toggle bit 5.

---

## Exercise 3 — GPIO HAL

Write an ESP-IDF program that:

* configures one LED output,
* toggles it every second.

---

## Exercise 4 — Button Input

Read a push button and control an LED.

---

## Exercise 5 — Interrupt

Configure a button to generate a GPIO interrupt.

---

## Exercise 6 — UART

Transmit

```text
Hello ESP32
```

through a UART interface.

---

# 53. Advanced Exercise — Register versus HAL Comparison

Implement the same GPIO function using:

### Method A

Register-level programming.

### Method B

ESP-IDF GPIO API.

Compare:

| Characteristic     | Register-Level | HAL |
| ------------------ | -------------- | --- |
| Code length        |                |     |
| Readability        |                |     |
| Portability        |                |     |
| Execution overhead |                |     |
| Debug complexity   |                |     |

Discuss the trade-off.

---

# 54. Advanced Exercise — Simple Embedded System

Build the architecture

```text
Button
  │
  ▼
GPIO Input
  │
  ▼
ESP32 CPU
  │
  ├────► LED
  │
  └────► UART
              │
              ▼
          PC Terminal
```

The software should:

1. read the button,
2. update the LED,
3. send the button state through UART.

This integrates:

* GPIO,
* memory-mapped peripherals,
* software APIs,
* embedded communication.

---

# 55. Extension to IoT Systems

The MCU architecture studied here provides the foundation for larger connected systems.

For example:

```text
Sensor
  │
  ▼
GPIO / ADC / I2C
  │
  ▼
ESP32
  │
  ▼
Wi-Fi
  │
  ▼
MQTT Broker
  │
  ▼
Cloud / Dashboard
```

The low-level concepts remain important because every high-level IoT operation ultimately depends on MCU peripherals and memory-mapped hardware.

---

# 56. Expected Learning Outcomes

After studying this material, students will be able to:

* Explain the basic architecture of a microcontroller.
* Identify CPU, memory, bus, and peripheral functions.
* Explain ESP32 as an MCU platform.
* Distinguish ROM, flash, SRAM, stack, and heap.
* Explain memory mapping and memory-mapped I/O.
* Describe peripheral registers and bit fields.
* Perform bit masking and register manipulation.
* Explain the role of `volatile`.
* Distinguish register-level programming from HAL-based programming.
* Use basic ESP-IDF GPIO APIs.
* Explain interrupt, timer, UART, SPI, and I²C concepts.
* Understand how low-level MCU architecture supports embedded and IoT systems.

---

# 📘 References

1. Espressif Systems, *ESP32 Technical Reference Manual*.
2. Espressif Systems, *ESP-IDF Programming Guide*.
3. J. Yiu, *The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors*, Newnes.
4. J. Catsoulis, *Designing Embedded Hardware*, O'Reilly.
5. M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly.
6. D. E. Simon, *An Embedded Software Primer*, Addison-Wesley.
7. FreeRTOS Documentation, *Kernel and Task Management Concepts*.

---

## 🔑 Key Concept

The fundamental MCU architecture is

$$
\boxed{
\text{CPU}
+
\text{Memory}
+
\text{Peripherals}
+
\text{System Bus}
}
$$

Memory-mapped I/O provides the connection

$$
\boxed{
\text{CPU Address}
\rightarrow
\text{Peripheral Register}
\rightarrow
\text{Physical Hardware}
}
$$

while software abstraction can be viewed as

$$
\boxed{
\text{Application}
\rightarrow
\text{HAL / Driver}
\rightarrow
\text{Registers}
\rightarrow
\text{ESP32 Hardware}
}
$$

This theory provides the foundation for later work involving **GPIO, interrupts, timers, UART, SPI, I²C, FreeRTOS, Wi-Fi, MQTT, sensor interfacing, embedded intelligence, and IoT systems**.
