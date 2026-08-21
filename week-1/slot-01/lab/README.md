# 🔬 Lab 1: ESP32 Bare-Metal GPIO Programming
## Development Environment Setup, LED Blinking, and Push-Button Input

---

## 🧩 1. Objective

This laboratory introduces basic **bare-metal embedded programming** using the ESP32 microcontroller. Students will configure the development environment and implement simple GPIO operations for an LED and push button.

After completing this laboratory, students will be able to:

- Set up an ESP32 development environment.
- Understand the basic workflow of MCU firmware development.
- Configure a GPIO pin as a digital output.
- Blink an LED using software-controlled timing.
- Configure a GPIO pin as a digital input.
- Read the state of a push button.
- Control an LED according to the push-button state.
- Understand the relationship between **GPIO registers, low-level drivers, and physical pins**.
- Compare polling-based input with basic event-driven embedded-system concepts.

---

## ⚙️ 2. Equipment and Tools

| Tool / Resource | Description |
|---|---|
| **ESP32 Development Board** | Target MCU platform |
| **USB Cable** | Programming and serial communication |
| **LED** | Digital output device |
| **220–330 Ω Resistor** | LED current-limiting resistor |
| **Push Button** | Digital input device |
| **10 kΩ Resistor** | Optional external pull-up/pull-down |
| **Breadboard** | Circuit construction |
| **Jumper Wires** | Electrical connections |
| **VS Code / Terminal** | Source-code development |
| **ESP-IDF** | ESP32 development framework and toolchain |
| **Serial Monitor** | Debug and status output |

---

## 🧠 3. Background Theory

### 3.1 Bare-Metal Embedded Programming

Bare-metal programming refers to software that interacts closely with the microcontroller hardware without relying on a large operating-system environment.

The basic relationship is

$$
\boxed{
\text{Application}
\rightarrow
\text{MCU Hardware}
}
$$

At a low level, the CPU controls peripherals through hardware registers:

$$
\boxed{
\text{CPU}
\rightarrow
\text{Peripheral Registers}
\rightarrow
\text{GPIO Hardware}
}
$$

In this laboratory, ESP-IDF may provide startup, toolchain, and low-level support while the application remains a simple MCU GPIO program without application-level RTOS task design.

> **Note:** ESP-IDF itself uses FreeRTOS internally on common ESP32 targets. Therefore, “bare-metal” in this introductory laboratory means direct, low-level MCU-oriented GPIO programming rather than a completely RTOS-free runtime.

---

### 3.2 General-Purpose Input/Output

**GPIO** stands for General-Purpose Input/Output.

A GPIO pin can typically operate as:

- digital input,
- digital output,
- peripheral signal,
- interrupt source.

For this laboratory:

```text
GPIO Output → LED
GPIO Input  ← Push Button
