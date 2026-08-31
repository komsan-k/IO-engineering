# LAB 1 — Bare-Metal GPIO Programming: LED Blinking and Push-Button Input

## 1. Objective

The objective of this lab is to:

1. Set up the embedded development environment for the ESP32.
2. Understand the basic architecture of a microcontroller-based embedded system.
3. Identify GPIO pins, registers, input/output direction, and digital logic levels.
4. Understand the difference between **high-level GPIO APIs** and **register-level programming**.
5. Configure a GPIO pin as a digital output using hardware registers.
6. Create a **bare-metal LED blinking program** without using `digitalWrite()`.
7. Configure a GPIO pin as a digital input.
8. Read a push button directly from a GPIO input register.
9. Control an LED according to the button state.
10. Understand memory-mapped I/O and bit manipulation.
11. Use bit masks to set, clear, and test individual register bits.
12. Observe the effect of switch bouncing.
13. Implement a simple software debounce mechanism.
14. Build a foundation for interrupts, timers, ADC, PWM, and RTOS programming.

---

## 2. Background

A microcontroller communicates with external hardware through **peripherals** such as:

```text
GPIO
ADC
PWM
UART
I²C
SPI
Timers
Interrupt Controllers
```

In this lab, the focus is on the simplest peripheral:

```text
GPIO
```

or:

```text
General-Purpose Input/Output
```

A GPIO pin can usually be configured as either:

```text
GPIO
 ├── Input
 │      ↓
 │   Read Signal
 │
 └── Output
        ↓
     Drive Signal
```

Typical applications include:

```text
Output → LED
Input  → Push Button
```

---

## 3. Embedded-System Architecture

The basic system used in this lab is:

```text
             ESP32
        ┌──────────────┐
        │              │
Button ─→ GPIO Input   │
        │              │
        │     CPU      │
        │              │
LED    ←─ GPIO Output  │
        │              │
        └──────────────┘
```

The program performs:

```text
Read Button
     ↓
Process State
     ↓
Control LED
```

---

## 4. Development Environment

Recommended hardware:

- ESP32 development board
- USB cable
- LED
- 220–330 $\Omega$ resistor
- Push button
- 10 k$\Omega$ resistor
- Breadboard
- Jumper wires

Recommended software:

- Arduino IDE, or
- Visual Studio Code + PlatformIO
- ESP32 Arduino Core
- Serial Monitor

For this lab, the Arduino framework may be used for program startup and serial debugging, while GPIO access is performed directly through ESP32 hardware registers.

This gives the programming model:

```text
Arduino Build Environment
          +
Register-Level GPIO Access
          ↓
Bare-Metal GPIO Experiment
```

---

## 5. Development Environment Setup

### Step 1 — Install Arduino IDE

Install the current Arduino IDE.

Launch:

```text
Arduino IDE
```

### Step 2 — Install ESP32 Board Support

Open:

```text
File
 ↓
Preferences
```

Add the ESP32 board package URL to the **Additional Boards Manager URLs** field if required by your installation.

Then open:

```text
Tools
 ↓
Board
 ↓
Boards Manager
```

Search for:

```text
esp32
```

and install the ESP32 board package.

### Step 3 — Select the Board

For a typical ESP32 development board:

```text
Tools
 ↓
Board
 ↓
ESP32 Arduino
 ↓
ESP32 Dev Module
```

The exact board selection may differ depending on the hardware used.

### Step 4 — Select the Serial Port

Connect the ESP32 through USB.

Select:

```text
Tools
 ↓
Port
 ↓
COMx
```

where `COMx` is the port assigned to the ESP32.

### Step 5 — Test the Toolchain

Compile and upload a simple program:

```cpp
void setup() {
}

void loop() {
}
```

Successful compilation and uploading confirm that the development environment is ready.

---

## 6. High-Level vs. Register-Level Programming

A conventional Arduino LED program uses:

```cpp
pinMode(2, OUTPUT);

digitalWrite(2, HIGH);
```

This is a **high-level API**.

Conceptually:

```text
Application
     ↓
digitalWrite()
     ↓
Arduino HAL
     ↓
ESP32 GPIO Driver
     ↓
GPIO Register
     ↓
Hardware
```

In bare-metal programming, the program accesses the register more directly:

```text
Application
     ↓
GPIO Register
     ↓
Hardware
```

This provides more direct understanding of how MCU peripherals operate.

---

## 7. Memory-Mapped I/O

Microcontroller peripherals are controlled through registers located at specific memory addresses.

Conceptually:

```text
CPU
 ↓
Memory Bus
 ↓
Peripheral Register
 ↓
GPIO Hardware
```

For example:

```text
Write Register
      ↓
GPIO Output Changes
      ↓
LED Changes State
```

This mechanism is called **memory-mapped I/O**.

---

## 8. GPIO Register Model

GPIO operation typically involves registers controlling:

```text
Direction
   ↓
Input / Output

Output Value
   ↓
HIGH / LOW

Input Value
   ↓
Read External Signal
```

The conceptual register architecture is:

```text
CPU
 │
 ├── GPIO_ENABLE Register
 │        ↓
 │   Input / Output
 │
 ├── GPIO_OUT Register
 │        ↓
 │    Output Value
 │
 └── GPIO_IN Register
          ↓
      Input Value
```

---

## 9. Bit Manipulation

Registers usually contain many control bits.

Suppose:

```text
GPIO2 → Bit 2
GPIO4 → Bit 4
```

A bit mask for GPIO2 is:

```cpp
1UL << 2
```

which corresponds conceptually to:

```text
00000000 00000000 00000000 00000100
```

For GPIO4:

```cpp
1UL << 4
```

which corresponds to:

```text
00000000 00000000 00000000 00010000
```

---

## 10. Basic Bit Operations

### Set a Bit

Mathematically:

$$
R_{\text{new}}
= R_{\text{old}}
\;|\;
(1\ll n)
$$

In C/C++:

```cpp
reg |= (1UL << n);
```

### Clear a Bit

$$
R_{\text{new}}
= R_{\text{old}}
\;\&\;
\sim(1\ll n)
$$

In C/C++:

```cpp
reg &= ~(1UL << n);
```

### Test a Bit

```cpp
if (reg & (1UL << n)) {
    // Bit is HIGH
}
```

These operations are fundamental to embedded register programming.

---

## 11. Circuit Configuration

For this lab, use:

```text
LED    → GPIO 2
Button → GPIO 4
```

The exact GPIO pins may be changed according to the ESP32 board being used.

### LED Connection

```text
GPIO2
  │
  ↓
220–330 Ω
  │
  ↓
 LED
  │
  ↓
 GND
```

### Push-Button Connection

Use an external pull-up resistor:

```text
3.3 V
  │
  ↓
10 kΩ
  │
  ├────────→ GPIO4
  │
Button
  │
  ↓
 GND
```

Therefore:

```text
Button Released → GPIO4 = HIGH
Button Pressed  → GPIO4 = LOW
```

This is called an **active-low** button configuration.

---

## 12. Experiment 1 — Register-Level LED Output

Define:

```cpp
#define LED_PIN 2
```

For a classic ESP32, GPIO control registers can be accessed through the ESP32 SoC register definitions.

Include:

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"
```

Configure GPIO2 as output:

```cpp
REG_WRITE(
    GPIO_ENABLE_W1TS_REG,
    (1UL << LED_PIN)
);
```

`W1TS` means:

```text
Write 1 To Set
```

Writing a `1` to bit 2 enables GPIO2 as an output without modifying unrelated GPIO bits.

---

## 13. Turn the LED ON

Use:

```cpp
REG_WRITE(
    GPIO_OUT_W1TS_REG,
    (1UL << LED_PIN)
);
```

Conceptually:

```text
CPU
 ↓
Set GPIO2 Output Bit
 ↓
GPIO2 = HIGH
 ↓
LED ON
```

---

## 14. Turn the LED OFF

Use:

```cpp
REG_WRITE(
    GPIO_OUT_W1TC_REG,
    (1UL << LED_PIN)
);
```

`W1TC` means:

```text
Write 1 To Clear
```

Conceptually:

```text
CPU
 ↓
Clear GPIO2 Output Bit
 ↓
GPIO2 = LOW
 ↓
LED OFF
```

---

## 15. Experiment 2 — Bare-Metal LED Blink

Create a simple busy-wait delay:

```cpp
void busyDelay() {

  for (
    volatile uint32_t i = 0;
    i < 3000000;
    i++
  ) {
  }
}
```

Then alternate the output.

### Complete LED Blink Program

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#define LED_PIN 2

void busyDelay() {

  for (
    volatile uint32_t i = 0;
    i < 3000000;
    i++
  ) {
  }
}

void setup() {

  REG_WRITE(
    GPIO_ENABLE_W1TS_REG,
    (1UL << LED_PIN)
  );
}

void loop() {

  REG_WRITE(
    GPIO_OUT_W1TS_REG,
    (1UL << LED_PIN)
  );

  busyDelay();

  REG_WRITE(
    GPIO_OUT_W1TC_REG,
    (1UL << LED_PIN)
  );

  busyDelay();
}
```

Notice that the program does **not** use:

```cpp
pinMode()
digitalWrite()
delay()
```

for GPIO control and timing.

---

## 16. Expected Result

The LED should repeatedly change:

```text
ON
 ↓
OFF
 ↓
ON
 ↓
OFF
 ↓
...
```

The basic signal is approximately:

```text
HIGH ──────┐      ┌──────
           │      │
LOW        └──────┘
```

One complete ON/OFF cycle defines a period:

$$
T=T_{\text{ON}}+T_{\text{OFF}}.
$$

The blink frequency is:

$$
f=\frac{1}{T}.
$$

Because this experiment uses a software busy loop, the timing is approximate rather than accurately controlled.

---

## 17. Experiment 3 — Modify the Blink Rate

Change:

```cpp
3000000
```

to values such as:

```text
500000
1000000
3000000
6000000
```

Record the observed blink speed.

| Loop Count | Observed Blink Rate |
|---:|---|
| 500000 | |
| 1000000 | |
| 3000000 | |
| 6000000 | |

Determine the relationship between loop count and approximate delay.

---

## 18. Why Busy-Wait Delays Are Inefficient

During:

```cpp
for (...) {
}
```

the CPU repeatedly executes instructions without performing useful application work.

Conceptually:

```text
CPU
 ↓
Waiting
 ↓
Waiting
 ↓
Waiting
 ↓
Waiting
```

Therefore:

```text
Busy-Wait Delay
      ↓
CPU Occupied
```

Later labs will replace this approach using:

```text
Hardware Timers
Interrupts
FreeRTOS Delays
Software Timers
```

---

## 19. Experiment 4 — Read a Push Button

Define:

```cpp
#define BUTTON_PIN 4
```

Read the GPIO input register:

```cpp
uint32_t gpioState =
    REG_READ(
      GPIO_IN_REG
    );
```

Then test the bit corresponding to GPIO4:

```cpp
bool buttonState =
    gpioState
    &
    (1UL << BUTTON_PIN);
```

---

## 20. Understanding the Input Register

Suppose:

```text
GPIO4 = HIGH
```

then bit 4 of `GPIO_IN_REG` is:

```text
1
```

Therefore:

```cpp
gpioState &
(1UL << BUTTON_PIN)
```

returns a nonzero value.

If:

```text
GPIO4 = LOW
```

the expression evaluates to zero.

---

## 21. Active-Low Button Logic

With the external pull-up resistor:

```text
Released
   ↓
GPIO4 = HIGH
   ↓
Logic 1
```

and:

```text
Pressed
   ↓
GPIO4 Connected to GND
   ↓
GPIO4 = LOW
   ↓
Logic 0
```

Therefore:

$$
B=
\begin{cases}
0,&\text{button pressed},\\
1,&\text{button released}.
\end{cases}
$$

---

## 22. Experiment 5 — Display Button State

Use Serial only for debugging.

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#define BUTTON_PIN 4

void setup() {

  Serial.begin(115200);
}

void loop() {

  uint32_t input =
      REG_READ(
        GPIO_IN_REG
      );

  bool state =
      input
      &
      (1UL << BUTTON_PIN);

  if (state == 0) {

    Serial.println(
      "Button PRESSED"
    );
  }
  else {

    Serial.println(
      "Button RELEASED"
    );
  }

  delay(200);
}
```

In this experiment, `delay()` is only used to slow Serial output. GPIO input itself is still accessed directly from the register.

---

## 23. Experiment 6 — Control LED with Push Button

The required behavior is:

```text
Button Pressed
      ↓
LED ON

Button Released
      ↓
LED OFF
```

Since the button is active-low:

```text
GPIO4 = 0 → LED ON
GPIO4 = 1 → LED OFF
```

### Complete Bare-Metal Button-Controlled LED Program

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#define LED_PIN     2
#define BUTTON_PIN  4

void setup() {

  REG_WRITE(
    GPIO_ENABLE_W1TS_REG,
    (1UL << LED_PIN)
  );
}

void loop() {

  uint32_t inputState =
      REG_READ(
        GPIO_IN_REG
      );

  bool buttonReleased =
      inputState
      &
      (1UL << BUTTON_PIN);

  if (!buttonReleased) {

    REG_WRITE(
      GPIO_OUT_W1TS_REG,
      (1UL << LED_PIN)
    );
  }
  else {

    REG_WRITE(
      GPIO_OUT_W1TC_REG,
      (1UL << LED_PIN)
    );
  }
}
```

---

## 24. System Operation

The complete data path is:

```text
Push Button
     ↓
GPIO4
     ↓
GPIO_IN_REG
     ↓
CPU Reads Bit 4
     ↓
Decision
     ↓
GPIO_OUT Register
     ↓
GPIO2
     ↓
LED
```

This represents the basic structure of an embedded control system:

$$
\boxed{
\text{Input}
\rightarrow
\text{Processing}
\rightarrow
\text{Output}
}
$$

---

## 25. Experiment 7 — Reverse the LED Logic

Modify the system so that:

```text
Button Released → LED ON
Button Pressed  → LED OFF
```

The control law becomes:

$$
L=
\begin{cases}
0,&B=0,\\
1,&B=1.
\end{cases}
$$

where:

- $B$ is the button logic state.
- $L$ is the LED output state.

---

## 26. Experiment 8 — Toggle LED on Each Button Press

Instead of keeping the LED on only while the button is pressed, make each button press toggle the LED.

Required sequence:

```text
Initial
LED OFF

Press 1
   ↓
LED ON

Press 2
   ↓
LED OFF

Press 3
   ↓
LED ON
```

This requires storing the previous state.

Conceptually:

```text
Read Current Button
        ↓
Compare with Previous Button
        ↓
Detect New Press
        ↓
Toggle LED State
```

---

## 27. Edge Detection

A new button press occurs when:

```text
Previous = HIGH
Current  = LOW
```

This transition is:

```text
1 → 0
```

or a **falling edge**.

---

## 28. Experiment 9 — Button Toggle Program

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#define LED_PIN     2
#define BUTTON_PIN  4

bool ledState = false;
bool previousButton = true;

void setup() {

  REG_WRITE(
    GPIO_ENABLE_W1TS_REG,
    (1UL << LED_PIN)
  );
}

void loop() {

  uint32_t input =
      REG_READ(
        GPIO_IN_REG
      );

  bool currentButton =
      input
      &
      (1UL << BUTTON_PIN);

  if (
    previousButton == true &&
    currentButton == false
  ) {

    ledState =
        !ledState;

    if (ledState) {

      REG_WRITE(
        GPIO_OUT_W1TS_REG,
        (1UL << LED_PIN)
      );
    }
    else {

      REG_WRITE(
        GPIO_OUT_W1TC_REG,
        (1UL << LED_PIN)
      );
    }
  }

  previousButton =
      currentButton;
}
```

Test the program several times.

You may observe that one physical press occasionally creates multiple LED toggles.

This is caused by **button bouncing**.

---

## 29. Mechanical Switch Bounce

A mechanical button does not always change instantly from:

```text
HIGH → LOW
```

Instead, the signal may look like:

```text
HIGH ─────┐
          └─┐ ┌─┐
            └─┘ └──── LOW
```

The MCU may interpret this as several transitions.

Instead of:

```text
1 → 0
```

the actual signal may temporarily be:

```text
1 → 0 → 1 → 0 → 1 → 0
```

This is called **switch bounce**.

---

## 30. Experiment 10 — Simple Software Debounce

One simple teaching method is to wait briefly after detecting a button press.

For example:

```cpp
void debounceDelay() {

  for (
    volatile uint32_t i = 0;
    i < 300000;
    i++
  ) {
  }
}
```

Then after detecting a falling edge:

```cpp
if (
  previousButton &&
  !currentButton
) {

  debounceDelay();

  // Verify the button again.
}
```

A better algorithm is:

```text
Detect Possible Press
       ↓
Wait Short Interval
       ↓
Read Button Again
       ↓
Still Pressed?
   ┌───┴───┐
  Yes      No
   ↓        ↓
Accept    Ignore
```

---

## 31. Experiment 11 — Debounced LED Toggle

```cpp
#include <Arduino.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"

#define LED_PIN     2
#define BUTTON_PIN  4

bool ledState = false;
bool previousButton = true;

bool readButton() {

  uint32_t input =
      REG_READ(
        GPIO_IN_REG
      );

  return (
    input
    &
    (1UL << BUTTON_PIN)
  );
}

void debounceDelay() {

  for (
    volatile uint32_t i = 0;
    i < 300000;
    i++
  ) {
  }
}

void setLED(bool state) {

  if (state) {

    REG_WRITE(
      GPIO_OUT_W1TS_REG,
      (1UL << LED_PIN)
    );
  }
  else {

    REG_WRITE(
      GPIO_OUT_W1TC_REG,
      (1UL << LED_PIN)
    );
  }
}

void setup() {

  REG_WRITE(
    GPIO_ENABLE_W1TS_REG,
    (1UL << LED_PIN)
  );

  setLED(false);
}

void loop() {

  bool currentButton =
      readButton();

  if (
    previousButton &&
    !currentButton
  ) {

    debounceDelay();

    if (
      !readButton()
    ) {

      ledState =
          !ledState;

      setLED(
        ledState
      );

      while (
        !readButton()
      ) {
      }
    }
  }

  previousButton =
      readButton();
}
```

---

## 32. Register-Level Program Structure

The complete bare-metal GPIO process can be summarized as:

```text
Configure Hardware
      ↓
Set GPIO Direction
      ↓
Read Input Register
      ↓
Perform Decision
      ↓
Write Output Register
      ↓
Repeat
```

This is the foundation of many embedded applications.

---

## 33. Experiment 12 — Measure Response Time

The system response time can be approximated as:

$$
T_{\text{response}}
= t_{\text{LED}}
- t_{\text{button}}.
$$

For a continuously polling program:

```text
Read Button
     ↓
Process
     ↓
Set LED
```

the response is typically fast, but the CPU continuously checks the button.

This is called **polling**.

---

## 34. Polling

The current implementation uses:

```text
while(1)
   ↓
Read Button
   ↓
Read Button
   ↓
Read Button
   ↓
Read Button
```

Advantages:

- Simple
- Easy to understand
- Fast response for small applications

Disadvantages:

- CPU repeatedly checks the input.
- CPU time is wasted when nothing changes.
- Becomes inefficient as system complexity increases.

The next stage is:

```text
Polling
   ↓
Interrupts
```

---

## 35. Experiment 13 — Multiple Output Patterns

Extend the LED control program.

### Mode 1

```text
Button Released → LED OFF
Button Pressed  → LED ON
```

### Mode 2

```text
Button Released → LED ON
Button Pressed  → LED OFF
```

### Mode 3

```text
Each Press → Toggle LED
```

Compare the software logic required for each mode.

---

## 36. Experiment 14 — Two LEDs

Add another LED.

For example:

```text
LED 1 → GPIO2
LED 2 → GPIO5
```

Implement:

```text
Button Released
      ↓
LED1 ON
LED2 OFF

Button Pressed
      ↓
LED1 OFF
LED2 ON
```

Use bit masks for both outputs.

Conceptually:

```text
GPIO Register

Bit 5      Bit 2
 ↓          ↓
LED2       LED1
```

---

## 37. Bit-Mask Exercise

For:

```text
GPIO2
```

the mask is:

$$
M_2=1\ll2.
$$

For:

```text
GPIO5
```

the mask is:

$$
M_5=1\ll5.
$$

For both:

$$
M=M_2|M_5.
$$

Therefore:

```cpp
uint32_t mask =
    (1UL << 2)
    |
    (1UL << 5);
```

---

## 38. Experiment 15 — Compare Bare-Metal and Arduino API

Implement the same button-controlled LED using Arduino APIs:

```cpp
#define LED_PIN 2
#define BUTTON_PIN 4

void setup() {

  pinMode(
    LED_PIN,
    OUTPUT
  );

  pinMode(
    BUTTON_PIN,
    INPUT
  );
}

void loop() {

  if (
    digitalRead(
      BUTTON_PIN
    ) == LOW
  ) {

    digitalWrite(
      LED_PIN,
      HIGH
    );
  }
  else {

    digitalWrite(
      LED_PIN,
      LOW
    );
  }
}
```

Compare this with the register-level implementation.

---

## 39. Arduino API vs. Bare-Metal GPIO

| Feature | Arduino API | Register-Level |
|---|---|---|
| Ease of programming | High | Lower |
| Hardware visibility | Low | High |
| Portability | Higher | Lower |
| Register knowledge required | No | Yes |
| Learning MCU architecture | Limited | Strong |
| Control over hardware | Abstracted | Direct |
| Code readability | High | More technical |

The two approaches are not competitors. High-level APIs improve productivity, while register-level programming improves understanding and allows more direct hardware control.

---

## 40. Common Errors

### Wrong GPIO Pin

The selected board may use a different built-in LED pin.

Always verify the development-board schematic or pinout.

### Missing Current-Limiting Resistor

Do not connect a conventional LED directly between a GPIO and ground without an appropriate current-limiting resistor.

Use approximately:

```text
220–330 Ω
```

for the lab circuit.

### Floating Button Input

Without a pull-up or pull-down resistor:

```text
Button Released
       ↓
Undefined Voltage
       ↓
Random HIGH / LOW
```

This is called a **floating input**.

Use an external resistor in this register-level lab.

### Using a 5-V Signal

ESP32 GPIO uses 3.3-V logic.

Do not intentionally apply 5 V directly to a GPIO pin.

### Wrong Bit Position

For GPIO4:

```cpp
1UL << 4
```

not:

```cpp
1UL << 3
```

### Confusing W1TS and W1TC

```text
W1TS → Write 1 to Set
W1TC → Write 1 to Clear
```

These registers are useful because they allow specific GPIO bits to be modified without changing unrelated output bits.

---

## 41. Experimental Summary

### Experiment 1 — Configure GPIO Output

Configure GPIO2 as an output using a GPIO enable register.

### Experiment 2 — Blink LED

Use direct register writes to turn the LED on and off.

### Experiment 3 — Change Blink Rate

Modify the busy-loop delay.

### Experiment 4 — Read Input Register

Read GPIO4 directly from `GPIO_IN_REG`.

### Experiment 5 — Display Button State

Observe HIGH and LOW states through Serial Monitor.

### Experiment 6 — Button-Controlled LED

Control GPIO2 according to GPIO4.

### Experiment 7 — Reverse Logic

Reverse the LED control behavior.

### Experiment 8 — Toggle LED

Toggle the LED after every button press.

### Experiment 9 — Observe Switch Bounce

Investigate multiple transitions from a physical button.

### Experiment 10 — Debounce

Introduce a software debounce delay.

### Experiment 11 — Reliable Toggle

Implement debounced state-based LED control.

### Experiment 12 — Response Time

Study input-to-output response.

### Experiment 13 — Control Modes

Implement multiple button/LED behaviors.

### Experiment 14 — Multiple LEDs

Control multiple output bits.

### Experiment 15 — API Comparison

Compare register-level programming with Arduino APIs.

---

## 42. Exercises

### Exercise 1 — LED Blink

Create a register-level program that blinks an LED.

The program must not use:

```cpp
digitalWrite()
```

for LED control.

### Exercise 2 — Adjustable Blink Rate

Create three blink modes:

```text
Slow
Medium
Fast
```

using different busy-wait counts.

Record the approximate LED periods.

| Mode | Delay Count | Measured Period |
|---|---:|---:|
| Slow | | |
| Medium | | |
| Fast | | |

### Exercise 3 — Push-Button Input

Read the button directly from:

```cpp
GPIO_IN_REG
```

Display:

```text
PRESSED
```

or:

```text
RELEASED
```

through Serial Monitor.

### Exercise 4 — Button-Controlled LED

Implement:

$$
L=
\begin{cases}
1,&B=0,\\
0,&B=1.
\end{cases}
$$

where:

- $B$ is the active-low button input.
- $L$ is the LED output.

### Exercise 5 — LED Toggle

Each new button press must toggle:

```text
OFF → ON
ON  → OFF
```

### Exercise 6 — Debounce

Modify the toggle program so that one physical button press produces only one state change.

### Exercise 7 — Two LEDs

Connect two LEDs and implement:

```text
Button Released:
LED1 = ON
LED2 = OFF

Button Pressed:
LED1 = OFF
LED2 = ON
```

Use register-level GPIO operations.

### Exercise 8 — Analyze Registers

For each of the following, explain its purpose:

```cpp
GPIO_ENABLE_W1TS_REG
GPIO_OUT_W1TS_REG
GPIO_OUT_W1TC_REG
GPIO_IN_REG
```

---

## 43. Checkpoint Questions

1. What does GPIO stand for?
2. What is the difference between a GPIO input and output?
3. What is memory-mapped I/O?
4. What is a peripheral register?
5. Why are registers divided into individual bits?
6. What does `1UL << n` represent?
7. How can a bit be set using a bit mask?
8. How can a bit be cleared?
9. How can a bit be tested?
10. What does `W1TS` mean?
11. What does `W1TC` mean?
12. Which register is used in this lab to read GPIO input states?
13. Why is an LED resistor required?
14. Why does a button input need a pull-up or pull-down resistor?
15. What is a floating input?
16. What is an active-low button?
17. What logic level is obtained when the button is released in this circuit?
18. What logic level is obtained when the button is pressed?
19. What is polling?
20. What is a busy-wait delay?
21. Why is busy waiting inefficient?
22. What is switch bounce?
23. What is software debouncing?
24. What is falling-edge detection?
25. Why must the previous button state be stored for toggle detection?
26. What is the difference between `digitalWrite()` and direct register access?
27. What is an advantage of register-level programming?
28. What is a disadvantage of register-level programming?
29. Why should the GPIO pinout be checked for each ESP32 board?
30. Why are direct register examples generally MCU-specific?

---

## 44. Expected Results

Students should successfully create the following sequence:

```text
Development Environment
        ↓
Compile & Upload
        ↓
Configure GPIO
        ↓
Blink LED
        ↓
Read Button
        ↓
Control LED
        ↓
Detect Button Edge
        ↓
Debounce Button
```

The fundamental MCU control loop is:

```text
Input
  ↓
Read Register
  ↓
CPU Processing
  ↓
Write Register
  ↓
Output
```

or:

$$
\boxed{
\text{Embedded Control}
=
\text{Input}
+
\text{Processing}
+
\text{Output}
}
$$

---

## 45. Result Table

Complete the following table.

| Experiment | Input | Output | GPIO Technique | Result |
|---|---|---|---|---|
| LED Blink | None | LED | Register Write | |
| Button Read | Button | Serial | Register Read | |
| Button LED | Button | LED | Read + Write | |
| Toggle | Button | LED | Edge Detection | |
| Debounce | Button | LED | State + Delay | |
| Two LEDs | Button | 2 LEDs | Bit Masks | |

---

## 46. Discussion

Students should explain how the CPU controls GPIO pins using memory-mapped registers rather than relying only on high-level library functions.

The LED experiment demonstrates:

```text
CPU
 ↓
GPIO Output Register
 ↓
GPIO Pin
 ↓
LED
```

while the push-button experiment demonstrates:

```text
Button
 ↓
GPIO Pin
 ↓
GPIO Input Register
 ↓
CPU
```

Combining them gives:

```text
Button
   ↓
Input Register
   ↓
CPU Decision
   ↓
Output Register
   ↓
LED
```

The experiment demonstrates that high-level functions such as:

```cpp
digitalRead()
digitalWrite()
```

ultimately control the same underlying MCU hardware registers.

Register-level programming therefore provides an important understanding of the connection between:

```text
Software
   ↓
Registers
   ↓
Digital Hardware
```

---

## 47. Submission

Submit:

- Screenshot showing the development environment.
- Source code for register-level LED blinking.
- LED blink-rate measurements.
- Source code for button reading.
- Serial Monitor screenshot showing button states.
- Source code for button-controlled LED.
- Source code for LED toggle operation.
- Debounced button implementation.
- Two-LED extension.
- Completed experimental tables.
- Answers to checkpoint questions.
- Brief discussion comparing Arduino API and register-level GPIO programming.

---

## 48. Conclusion

In this lab, the ESP32 development environment is configured and used to implement fundamental GPIO operations through direct register access. An LED is first configured as an output and controlled using register-level set and clear operations. A push button is then connected as a digital input and read through the GPIO input register.

The lab progresses from simple output control to a complete input-processing-output application:

```text
LED Output
    ↓
Button Input
    ↓
Input + Output
    ↓
Edge Detection
    ↓
Debouncing
```

Students also learn the fundamental relationship:

$$
\boxed{
\text{MCU Peripheral Control}
= \text{Memory-Mapped Registers}
+
\text{Bit Manipulation}
}
$$

The learning sequence for the embedded-system laboratories can therefore begin with:

```text
LAB 1
Development Environment
      ↓
GPIO Registers
      ↓
Bare-Metal LED & Button
      ↓
LAB 2
Hardware Interrupts
      ↓
Timers
      ↓
Event-Driven Programming
      ↓
LAB 3
ADC & PWM
      ↓
Analog Interfacing
      ↓
Actuator Control
      ↓
LAB 4
I²C & SPI
      ↓
Digital Sensor Interfacing
```

This lab establishes the **register-level programming foundation** needed for subsequent work with interrupts, timers, ADC, PWM, communication peripherals, and FreeRTOS.
