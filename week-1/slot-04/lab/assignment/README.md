# Assignment — APDS-9960 Gesture-Controlled Traffic LEDs

## Objective

Develop a gesture-controlled LED system using an **ESP32** and **APDS-9960 gesture sensor**.

Three LEDs are used:

- 🔴 Red LED
- 🟡 Yellow LED
- 🟢 Green LED

The APDS-9960 gestures are used to **select an LED** and then turn the selected LED **ON or OFF**.

The basic control concept is:

$$
\boxed{
\text{Gesture}
\rightarrow
\text{APDS-9960}
\rightarrow
\text{ESP32}
\rightarrow
\text{LED Selection/Control}
}
$$

---

## 4. APDS-9960 Connection

Use the following I²C configuration:

| APDS-9960 | ESP32 |
|---|---|
| SDA | GPIO 4 |
| SCL | GPIO 5 |
| GND | GND |
| VCC/VIN | Appropriate supply for the breakout board |


---

## 5. LED Connection

An example GPIO assignment is:

| LED | ESP32 GPIO |
|---|---:|
| Red | GPIO 18 |
| Yellow | GPIO 19 |
| Green | GPIO 23 |


---

## 6. Gesture Commands

The four basic gestures have different functions.

| Gesture | Command |
|---|---|
| `RIGHT` | Select next LED |
| `LEFT` | Select previous LED |
| `UP` | Turn selected LED ON |
| `DOWN` | Turn selected LED OFF |

The selection should operate cyclically:

```text
RIGHT:

RED → YELLOW → GREEN → RED → ...
```

and

```text
LEFT:

RED ← YELLOW ← GREEN ← RED ← ...
```

For example, if **Red** is currently selected:

```text
RIGHT → Yellow selected
RIGHT → Green selected
RIGHT → Red selected
```

---

## LED ON/OFF Control

After selecting an LED, use the `UP` and `DOWN` gestures to control it.

$$
\begin{aligned}
\text{RIGHT} &\rightarrow \text{Select Next LED},\\
\text{LEFT}  &\rightarrow \text{Select Previous LED},\\
\text{UP}    &\rightarrow \text{Selected LED ON},\\
\text{DOWN}  &\rightarrow \text{Selected LED OFF}.
\end{aligned}
$$

For example:

```text
Current selection: RED

RIGHT
→ Selected: YELLOW

UP
→ YELLOW LED ON

RIGHT
→ Selected: GREEN

UP
→ GREEN LED ON

LEFT
→ Selected: YELLOW

DOWN
→ YELLOW LED OFF
```

The state of each LED should be maintained independently.

---

## Program Requirements

Create a program that:

1. Initializes I²C using **SDA = GPIO 4** and **SCL = GPIO 5**.
2. Detects the APDS-9960 at address `0x39`.
3. Initializes gesture detection.
4. Configures the three LED GPIO pins as outputs.
5. Sets **Red LED** as the initial selected LED.
6. Detects `LEFT`, `RIGHT`, `UP`, and `DOWN` gestures.
7. Uses `LEFT` and `RIGHT` to change the selected LED.
8. Uses `UP` to turn the selected LED ON.
9. Uses `DOWN` to turn the selected LED OFF.
10. Displays the detected gesture, selected LED, and action on the Serial Monitor.

---

## 9. Suggested Program Structure

Use an index to identify the selected LED:

```cpp
int selectedLED = 0;
```

Define:

```text
0 = RED
1 = YELLOW
2 = GREEN
```

The basic program logic is:

```text
              +------------------+
              | Read APDS-9960   |
              +--------+---------+
                       |
                       v
              +------------------+
              | Detect Gesture   |
              +--------+---------+
                       |
          +------------+------------+
          |            |            |
        LEFT         RIGHT        UP/DOWN
          |            |            |
          v            v            v
     Previous LED   Next LED    Control LED
                                   |
                              +----+----+
                              |         |
                             UP       DOWN
                              |         |
                             ON        OFF
```

---

## Suggested Logic

The selection logic can use:

```cpp
// Select next LED
selectedLED++;

if (selectedLED > 2) {
    selectedLED = 0;
}
```

For selecting the previous LED:

```cpp
selectedLED--;

if (selectedLED < 0) {
    selectedLED = 2;
}
```

Then control the selected LED:

```cpp
// UP gesture
digitalWrite(ledPins[selectedLED], HIGH);

// DOWN gesture
digitalWrite(ledPins[selectedLED], LOW);
```

---

## Expected Serial Monitor Output

Example:

```text
APDS-9960 Gesture LED Controller
--------------------------------

Selected LED: RED

Gesture: RIGHT
Selected LED: YELLOW

Gesture: UP
YELLOW LED: ON

Gesture: RIGHT
Selected LED: GREEN

Gesture: UP
GREEN LED: ON

Gesture: LEFT
Selected LED: YELLOW

Gesture: DOWN
YELLOW LED: OFF
```

---

## Assignment Tasks

1. Connect the APDS-9960 to the ESP32 using I²C.
2. Verify that the sensor address is `0x39`.
3. Connect the Red, Yellow, and Green LEDs.
4. Write a program to detect the four basic gestures.
5. Implement cyclic LED selection using `LEFT` and `RIGHT`.
6. Implement LED ON control using `UP`.
7. Implement LED OFF control using `DOWN`.
8. Maintain the ON/OFF state of each LED independently.
9. Display all gesture and LED-control events on the Serial Monitor.
10. Demonstrate the completed system to the instructor.

---


## 14. Expected Result

The completed system should allow the user to navigate among three LEDs without physical buttons:

$$
\boxed{
\text{LEFT/RIGHT}
\rightarrow
\text{Select LED}
}
$$

$$
\boxed{
\text{UP/DOWN}
\rightarrow
\text{ON/OFF}
}
$$

This assignment demonstrates the use of **gesture sensing, event processing, state selection, and actuator control** in an ESP32-based embedded system.
