# Assignment — ESP32 Button Driver

## Objective

Develop a reusable button driver for the ESP32 that separates hardware input handling from the main application.

The driver should:

1. Initialize a GPIO button input.
2. Support an active-low push button.
3. Read the current button state.
4. Detect button press and release events.
5. Implement software debouncing.

---

## Hardware

- ESP32 development board
- Push button
- LED
- 220–330 Ω resistor
- Breadboard
- Jumper wires

Suggested GPIO:

```text
Button → GPIO4
LED    → GPIO2
