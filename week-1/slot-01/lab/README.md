# 🔬 Lab 1: ESP32 Bare-Metal GPIO Programming

## Development Environment Setup, LED Blinking, and Push-Button Input

---

## 🧩 1. Objective

This laboratory introduces basic **bare-metal embedded programming** using the ESP32 microcontroller. Students will configure the development environment and implement simple GPIO operations for an LED and push button.

After completing this laboratory, students will be able to:

-  Set up an ESP32 development environment.
-  Understand the basic workflow of MCU firmware development.
-  Configure a GPIO pin as a digital output.
-  Blink an LED using software-controlled timing.
-  Configure a GPIO pin as a digital input.
-  Read the state of a push button.
-  Control an LED according to the push-button state.
-  Understand the relationship between **GPIO registers, low-level drivers, and physical pins**.
-  Compare polling-based input with basic event-driven embedded-system concepts.

---

## ⚙️ 2. Equipment and Tools

| Tool / ResourceDescription  |                                           |
| --------------------------- | ----------------------------------------- |
| **ESP32 Development Board** | Target MCU platform                       |
| **USB Cable**               | Programming and serial communication      |
| **LED**                     | Digital output device                     |
| **220–330 Ω Resistor**      | LED current-limiting resistor             |
| **Push Button**             | Digital input device                      |
| **10 kΩ Resistor**          | Optional external pull-up/pull-down       |
| **Breadboard**              | Circuit construction                      |
| **Jumper Wires**            | Electrical connections                    |
| **VS Code / Terminal**      | Source-code development                   |
| **ESP-IDF**                 | ESP32 development framework and toolchain |
| **Serial Monitor**          | Debug and status output                   |

---

# 🧠 3. Background Theory

## 3.1 Bare-Metal Embedded Programming

Bare-metal programming refers to software that interacts closely with the microcontroller hardware without relying on a large operating-system environment.

The basic relationship is

Application→MCU Hardware​

At a low level, the CPU controls peripherals through hardware registers:

CPU→Peripheral Registers→GPIO Hardware​

In this laboratory, ESP-IDF may provide startup, toolchain, and low-level support while the application remains a simple MCU GPIO program without application-level RTOS task design.

> **Note:** ESP-IDF itself uses FreeRTOS internally on common ESP32 targets. Therefore, “bare-metal” in this introductory laboratory means direct, low-level MCU-oriented GPIO programming rather than a completely RTOS-free runtime.

---

## 3.2 General-Purpose Input/Output

**GPIO** stands for General-Purpose Input/Output.

A GPIO pin can typically operate as:

-  digital input,
-  digital output,
-  peripheral signal,
-  interrupt source.

For this laboratory:

```
GPIO Output → LED
```

GPIO Input  ← Push Button

---

# 💡 4. Digital Output

A digital output drives a pin to a logical state.

GPIO={1,0,​HIGHLOW​

For a typical active-high LED connection:

```
ESP32 GPIO
```

    │

    ▼

 Resistor

    │

    ▼

   LED

    │

    ▼

   GND

When

GPIO=1,

the LED is ON.

When

GPIO=0,

the LED is OFF.

---

# 🔘 5. Digital Input

A GPIO configured as an input reads the voltage level applied to the pin.

Conceptually,

VGPIO​≈0 V⇒Logic 0

and

VGPIO​≈3.3 V⇒Logic 1.

A push button provides a simple digital input.

---

# ⚠️ 6. ESP32 Voltage Level

ESP32 GPIO uses approximately

3.3 V

logic.

Do not directly apply a 5 V signal to a GPIO pin unless the specific board/interface provides appropriate level conversion.

---

# 🔌 7. Example Hardware Configuration

For this laboratory, an example configuration is:

| DeviceESP32 GPIO |        |
| ---------------- | ------ |
| LED              | GPIO 2 |
| Push Button      | GPIO 4 |

The exact pins can be changed according to the development board.

Always check the board documentation because some ESP32 boards reserve particular GPIOs for bootstrapping, flash, USB, or other onboard functions.

---

# 💡 8. LED Circuit

Connect the LED as follows:

```
GPIO 2
```

  │

  │

 [330 Ω]

  │

  ▼

 LED

  │

 GND

The resistor limits LED current.

A typical value is

R=220–330 Ω.

---

# 🔘 9. Push-Button Circuit

One convenient configuration uses an internal pull-up resistor.

```
          ESP32
```

            │

        GPIO 4

            │

            ├──── Internal Pull-Up

            │

          Button

            │

            ▼

           GND

When the button is not pressed:

BUTTON=1.

When the button is pressed:

BUTTON=0.

Therefore, the button is **active-low**.

---

# 🧠 10. Why Use a Pull-Up?

An unconnected digital input can float between HIGH and LOW.

This produces unpredictable behavior.

The pull-up resistor establishes a known default state:

Button Released→Logic 1​

and pressing the button connects the input to ground:

Button Pressed→Logic 0​

---

# 🛠️ 11. Development Environment

A typical ESP32 development workflow uses:

```
Source Code
```

    │

    ▼

ESP-IDF

    │

    ▼

Compiler / Linker

    │

    ▼

Firmware Binary

    │

    ▼

USB / Serial

    │

    ▼

ESP32

The main stages are:

Write→Build→Flash→Monitor​

---

# 💻 12. ESP-IDF Installation

Install the current ESP-IDF development environment appropriate for the operating system.

The environment normally provides:

-  compiler toolchain,
-  CMake build system,
-  ESP-IDF libraries,
-  flashing utilities,
-  serial monitor,
-  debugging tools.

After installation, verify the command-line environment with:

```
idf.py --version
```

---

# 📁 13. Basic ESP-IDF Project Structure

A simple project can have the following organization:

```
lab1_gpio/
```

│

├── CMakeLists.txt

│

└── main/

    ├── CMakeLists.txt

    └── main.c

The main application is placed in:

```
main/main.c
```

---

# ⚙️ 14. Target Configuration

Select the appropriate ESP32 target.

For the original ESP32, for example:

```
idf.py set-target esp32
```

Other ESP32-family devices require their corresponding target.

The target determines characteristics such as:

-  CPU architecture,
-  memory organization,
-  peripheral implementation,
-  toolchain,
-  supported GPIO features.

---

# 🔨 15. Building the Project

Compile the firmware using:

```
idf.py build
```

The build process performs approximately:

```
C Source
```

   │

   ▼

Compiler

   │

   ▼

Object Files

   │

   ▼

Linker

   │

   ▼

Firmware

Compilation errors must be corrected before flashing.

---

# 📥 16. Flashing the ESP32

Connect the ESP32 board to the computer through USB.

Then use:

```
idf.py flash
```

If a serial port must be specified, the exact command depends on the operating system and detected port.

After flashing, the ESP32 executes the new firmware.

---

# 🖥️ 17. Serial Monitor

The serial monitor can be opened using:

```
idf.py monitor
```

Build, flash, and monitor can also commonly be combined:

```
idf.py build flash monitor
```

Serial output is useful for:

-  debugging,
-  button-state observation,
-  program-status messages.

---

# 💡 18. Experiment 1 — Blink an LED

The first program periodically changes the GPIO output:

```
LED ON
```

  │

Delay

  │

LED OFF

  │

Delay

  │

  └────────► Repeat

The desired sequence is

0→1→0→1→⋯

---

# 💻 19. LED Blink Program

A simple ESP-IDF implementation is:

```
#include "driver/gpio.h"
```

\#include "esp\_rom\_sys.h"



\#define LED\_PIN GPIO\_NUM\_2



void app\_main(void)

{

gpio\_reset\_pin(LED\_PIN);



gpio\_set\_direction(

LED\_PIN,

GPIO\_MODE\_OUTPUT

    );



while (1)

    {

gpio\_set\_level(

LED\_PIN,

1

        );



esp\_rom\_delay\_us(500000);



gpio\_set\_level(

LED\_PIN,

0

        );



esp\_rom\_delay\_us(500000);

    }

}

Here, the ROM delay routine provides a simple blocking delay without explicitly creating an application FreeRTOS task delay.

---

# ⏱️ 20. LED Timing

The program keeps the LED ON for

TON​=0.5 s

and OFF for

TOFF​=0.5 s.

Therefore,

T=TON​+TOFF​=1 s.

The blinking frequency is approximately

f=T1​=1 Hz.

---

# 🔬 21. Experiment 1 Tasks

1.  Build the project.
2.  Flash the ESP32.
3.  Verify that the LED blinks.
4.  Change the delay to 250 ms.
5.  Observe the new blinking frequency.
6.  Change the delay to 1 s.
7.  Record the observed behavior.

---

# 📊 22. LED Blink Results

| ON TimeOFF TimeExpected PeriodExpected FrequencyObserved |         |       |        |   |
| -------------------------------------------------------- | ------- | ----- | ------ | - |
| 1000 ms                                                  | 1000 ms | 2 s   | 0.5 Hz |   |
| 500 ms                                                   | 500 ms  | 1 s   | 1 Hz   |   |
| 250 ms                                                   | 250 ms  | 0.5 s | 2 Hz   |   |

Students should complete the final column experimentally.

---

# 🔘 23. Experiment 2 — Read a Push Button

The next experiment configures GPIO 4 as an input with an internal pull-up.

The system behavior is:

```
Push Button
```

     │

     ▼

GPIO Input

     │

     ▼

CPU Reads GPIO

     │

     ▼

Serial Output

---

# 💻 24. Push-Button Program

```
#include <stdio.h>
```



\#include "driver/gpio.h"

\#include "esp\_rom\_sys.h"



\#define BUTTON\_PIN GPIO\_NUM\_4



void app\_main(void)

{

gpio\_reset\_pin(BUTTON\_PIN);



gpio\_set\_direction(

BUTTON\_PIN,

GPIO\_MODE\_INPUT

    );



gpio\_set\_pull\_mode(

BUTTON\_PIN,

GPIO\_PULLUP\_ONLY

    );



while (1)

    {

int button =

gpio\_get\_level(

BUTTON\_PIN

            );



printf(

"Button = %d\n",

button

        );



esp\_rom\_delay\_us(200000);

    }

}

---

# 📊 25. Expected Button Results

For the active-low circuit:

| ButtonGPIO Level |   |
| ---------------- | - |
| Released         | 1 |
| Pressed          | 0 |

Thus,

Pressed=(GPIO==0)​

---

# 💡 26. Experiment 3 — Button-Controlled LED

Now combine input and output operations.

The system architecture is:

```
Push Button
```

     │

     ▼

GPIO Input

     │

     ▼

ESP32 CPU

     │

     ▼

Decision Logic

     │

     ▼

GPIO Output

     │

     ▼

    LED

---

# 🧠 27. Control Algorithm

The required algorithm is:

```
START
```

  │

  ▼

Configure LED Output

  │

  ▼

Configure Button Input

  │

  ▼

Read Button

  │

  ▼

Button Pressed?

 ┌──────┴──────┐

Yes            No

 │              │

 ▼              ▼

LED ON        LED OFF

 │              │

 └──────┬───────┘

        │

        ▼

      Repeat

---

# 💻 28. Button-Controlled LED Program

```
#include <stdio.h>
```



\#include "driver/gpio.h"

\#include "esp\_rom\_sys.h"



\#define LED\_PIN     GPIO\_NUM\_2

\#define BUTTON\_PIN  GPIO\_NUM\_4



void app\_main(void)

{

gpio\_reset\_pin(LED\_PIN);

gpio\_reset\_pin(BUTTON\_PIN);



gpio\_set\_direction(

LED\_PIN,

GPIO\_MODE\_OUTPUT

    );



gpio\_set\_direction(

BUTTON\_PIN,

GPIO\_MODE\_INPUT

    );



gpio\_set\_pull\_mode(

BUTTON\_PIN,

GPIO\_PULLUP\_ONLY

    );



while (1)

    {

int button =

gpio\_get\_level(

BUTTON\_PIN

            );



if (button == 0)

        {

gpio\_set\_level(

LED\_PIN,

1

            );



printf("Button pressed - LED ON\n");

        }

else

        {

gpio\_set\_level(

LED\_PIN,

0

            );

        }



esp\_rom\_delay\_us(10000);

    }

}

---

# 📊 29. Expected Functional Results

| Button StateGPIO InputLED Output |   |     |
| -------------------------------- | - | --- |
| Released                         | 1 | OFF |
| Pressed                          | 0 | ON  |

The relationship can be written as

LED=BUTTON.

This inversion occurs because the button uses an active-low configuration.

---

# 🧠 30. Polling

The previous program repeatedly executes:

```
gpio_get_level(BUTTON_PIN);
```

This method is called **polling**.

```
Read Button
```

    │

    ▼

Process

    │

    ▼

Read Button

    │

    ▼

Process

    │

    └────► Repeat

Polling is easy to understand but continuously consumes CPU execution time.

---

# ⚠️ 31. Push-Button Bouncing

Mechanical push buttons do not always produce a perfect transition.

Instead of:

```
0 ─────────── 1
```

the signal may behave like:

```
0 ──1─0─1─0──1
```

for a short time.

This phenomenon is called **contact bounce**.

---

# 🛠️ 32. Software Debouncing

One simple approach is to wait briefly after detecting a button press.

```
if (gpio_get_level(BUTTON_PIN) == 0)
```

{

esp\_rom\_delay\_us(20000);



if (gpio\_get\_level(BUTTON\_PIN) == 0)

    {

// Valid button press

    }

}

Here,

20000 μs=20 ms.

This is sufficient for an introductory demonstration, although production systems often use more robust nonblocking debounce techniques.

---

# 🔢 33. Experiment 4 — Button Press Counter

Extend the program so that each valid button press increments a counter:

COUNTnext​=COUNT+1.

Example serial output:

```
Button Press Count = 1
```

Button Press Count = 2

Button Press Count = 3

---

# 💻 34. Button Counter Example

```
#include <stdio.h>
```



\#include "driver/gpio.h"

\#include "esp\_rom\_sys.h"



\#define BUTTON\_PIN GPIO\_NUM\_4



void app\_main(void)

{

int count = 0;

int previous = 1;



gpio\_reset\_pin(BUTTON\_PIN);



gpio\_set\_direction(

BUTTON\_PIN,

GPIO\_MODE\_INPUT

    );



gpio\_set\_pull\_mode(

BUTTON\_PIN,

GPIO\_PULLUP\_ONLY

    );



while (1)

    {

int current =

gpio\_get\_level(

BUTTON\_PIN

            );



if ((previous == 1) &&

            (current == 0))

        {

esp\_rom\_delay\_us(20000);



if (gpio\_get\_level(BUTTON\_PIN) == 0)

            {

count++;



printf(

"Button Press Count = %d\n",

count

                );

            }

        }



previous = current;



esp\_rom\_delay\_us(1000);

    }

}

This also introduces **edge detection**.

---

# 📈 35. Edge Detection

Instead of continuously responding to the button's level, an application can detect a transition.

For an active-low button, a press corresponds to:

1→0.

Therefore,

(previous=1)∧(current=0)​

indicates a falling edge.

---

# 🧱 36. GPIO and Memory-Mapped I/O

Although the application uses functions such as

```
gpio_set_level();
```

the hardware operation ultimately reaches GPIO peripheral registers.

Conceptually:

```
gpio_set_level()
```

       │

       ▼

ESP-IDF GPIO Driver

       │

       ▼

Low-Level Register Operation

       │

       ▼

GPIO Peripheral

       │

       ▼

Physical Pin

       │

       ▼

LED

This connects the laboratory to the theory of **memory-mapped I/O**.

---

# 🔬 37. HAL versus Register-Level Programming

The laboratory code primarily uses GPIO driver/HAL-style functions:

```
gpio_set_direction();
```

gpio\_set\_level();

gpio\_get\_level();

A lower-level implementation would manipulate peripheral registers more directly.

The conceptual difference is:

```
HAL / Driver
```

Application

    │

    ▼

gpio\_set\_level()

    │

    ▼

Register

versus:

```
Register-Level
```

Application

    │

    ▼

GPIO Register

Both ultimately control the same GPIO hardware.

---

# 📊 38. Comparison

| CharacteristicRegister-LevelHAL/Driver |           |          |
| -------------------------------------- | --------- | -------- |
| Hardware control                       | Very high | High     |
| Code complexity                        | Higher    | Lower    |
| Portability                            | Lower     | Higher   |
| Readability                            | Lower     | Higher   |
| Hardware knowledge required            | High      | Moderate |
| Development speed                      | Slower    | Faster   |

---

# 🧪 39. Optional Advanced Experiment — Direct Register Investigation

Using the ESP32 technical reference manual and device header files:

1.  Identify the GPIO peripheral.
2.  Find the output-related registers.
3.  Identify the register responsible for setting GPIO output bits.
4.  Identify the register responsible for clearing GPIO output bits.
5.  Determine which bit corresponds to the selected LED GPIO.
6.  Compare this operation with `gpio_set_level()`.

Do not assume register addresses from another ESP32-family device; register layouts vary among ESP32, ESP32-S2, ESP32-S3, ESP32-C3, and other variants.

---

# 🔍 40. Debugging with Serial Output

Use `printf()` to inspect program behavior.

For example:

```
printf(
```

"Button=%d LED=%d\n",

button,

led

);

Expected output may be:

```
Button=1 LED=0
```

Button=1 LED=0

Button=0 LED=1

Button=0 LED=1

Button=1 LED=0

Serial debugging provides visibility into software state that cannot always be observed directly through LEDs.

---

# 🧪 41. Laboratory Procedure

### Part A — Environment Setup

1.  Install ESP-IDF.
2.  Connect the ESP32 board.
3.  Verify the development tools.
4.  Create a new ESP32 project.
5.  Select the correct ESP32 target.
6.  Build an example project.

### Part B — LED Blinking

1.  Connect the LED.
2.  Configure the GPIO as output.
3.  Write the blink program.
4.  Build and flash.
5.  Observe the LED.
6.  Modify the blink frequency.

### Part C — Push-Button Input

1.  Connect the push button.
2.  Enable the pull-up resistor.
3.  Configure the GPIO as input.
4.  Read the GPIO state.
5.  Display the state using the serial monitor.

### Part D — Integrated System

1.  Read the button.
2.  Control the LED.
3.  Add software debounce.
4.  Verify the complete system.

---

# 📋 42. Experimental Results

Complete the following table.

| ExperimentExpected ResultObserved ResultPass/Fail |                      |   |   |
| ------------------------------------------------- | -------------------- | - | - |
| ESP32 builds successfully                         | No compilation error |   |   |
| Firmware flashing                                 | Successful           |   |   |
| LED HIGH                                          | LED ON               |   |   |
| LED LOW                                           | LED OFF              |   |   |
| Button released                                   | GPIO = 1             |   |   |
| Button pressed                                    | GPIO = 0             |   |   |
| Button pressed                                    | LED ON               |   |   |
| Button released                                   | LED OFF              |   |   |
| Serial monitor                                    | Correct button state |   |   |

---

# 📊 43. Timing Experiment

| DelayExpected Blink FrequencyMeasured/Observed |        |   |
| ---------------------------------------------- | ------ | - |
| 100 ms ON/OFF                                  | 5 Hz   |   |
| 250 ms ON/OFF                                  | 2 Hz   |   |
| 500 ms ON/OFF                                  | 1 Hz   |   |
| 1000 ms ON/OFF                                 | 0.5 Hz |   |

Students should explain the relationship

f=TON​+TOFF​1​.

---

# 💬 44. Discussion Questions

1.  What is bare-metal programming?
2.  Why is GPIO important in embedded systems?
3.  What is the difference between GPIO input and output?
4.  Why is a resistor connected in series with an LED?
5.  Why should a digital input not be left floating?
6.  What is the purpose of an internal pull-up resistor?
7.  Why does the push button read `0` when pressed in this circuit?
8.  What is polling?
9.  What are the disadvantages of continuous polling?
10.  What is push-button bouncing?
11.  What is software debouncing?
12.  What is memory-mapped I/O?
13.  How does `gpio_set_level()` ultimately control hardware?
14.  What is the difference between register-level programming and HAL/driver programming?
15.  Why should the exact ESP32 device be identified before directly accessing registers?

---

# 🧠 45. Post-Lab Exercises

1. **Change Blink Frequency**
    Modify the LED to blink at 2 Hz.
2. **Two LEDs**
    Alternate between two LEDs.
3. **Button Toggle**
    Press the button once to turn the LED ON and press it again to turn it OFF.
4. **Button Counter**
    Count button presses and print the value.
5. **Binary Counter**
    Connect four LEDs and display a 4-bit binary count.
6. **Two Buttons**
    Use one button to increment and another to decrement a counter.
7. **LED Pattern**
    Create a running-light sequence.
8. **Improved Debouncing**
    Implement a state-based debounce algorithm without a long blocking delay.
9. **Register Investigation**
    Identify the GPIO registers for the selected ESP32 device.
10. **Interrupt Extension**
     Replace button polling with a GPIO interrupt.

---

# 🚀 46. Advanced Exercise — Button Toggle State

Instead of directly following the button level, maintain an internal LED state:

LEDnext​=LEDcurrent​​

for every valid button press.

The behavior should be:

```
Initial: LED OFF
```



Press 1 → LED ON

Press 2 → LED OFF

Press 3 → LED ON

Press 4 → LED OFF

This introduces the concept of **stored state**, which is fundamental to embedded controllers and finite-state machines.

---

# 🔄 47. Embedded-System Data Flow

This laboratory demonstrates two fundamental MCU data paths.

### Output path

CPU→GPIO Output→LED​

### Input path

Push Button→GPIO Input→CPU​

Combining them produces:

Sensor/Input→Processing→Actuator/Output​

This is one of the fundamental architectures of embedded and cyber-physical systems.

---

# 🧾 48. Expected Learning Outcomes

After completing this laboratory, students will be able to:

-  Configure an ESP32 development environment.
-  Build and flash ESP32 firmware.
-  Use a serial monitor for debugging.
-  Explain GPIO input and output operation.
-  Configure an LED as a digital output.
-  Implement LED blinking.
-  Configure a push button as a digital input.
-  Use an internal pull-up resistor.
-  Read button states using polling.
-  Control an LED from a button.
-  Explain switch bouncing and basic debouncing.
-  Understand the connection between driver APIs and hardware registers.
-  Describe the basic MCU development workflow.

---

# 📘 49. References

1.  Espressif Systems, *ESP32 Technical Reference Manual*.
2.  Espressif Systems, *ESP-IDF Programming Guide*.
3.  Espressif Systems, *ESP32 Series Datasheet*.
4.  M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly.
5.  D. E. Simon, *An Embedded Software Primer*, Addison-Wesley.
6.  J. Catsoulis, *Designing Embedded Hardware*, O'Reilly.

---

## 🔑 50. Key Concept

The fundamental workflow introduced in this laboratory is

Write→Build→Flash→Run→Debug​

For digital output:

Software→GPIO Register→GPIO Pin→LED​

For digital input:

Push Button→GPIO Pin→GPIO Register→Software​

The complete laboratory demonstrates the core embedded-system relationship:

Input→MCU Processing→Output​

This provides the foundation for subsequent laboratories involving **GPIO interrupts, ADC, PWM, timers, UART, I²C, SPI, FreeRTOS, sensors, actuators, and IoT systems**.
