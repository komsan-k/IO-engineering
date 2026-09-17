# Lab: Custom Gesture Recognition Using APDS-9960 and ESP32

## 1. Introduction

The **APDS-9960** is a multi-function optical sensor capable of proximity detection, ambient-light sensing, color measurement, and directional gesture recognition. Its gesture-sensing subsystem uses infrared illumination and directional photodiodes to detect relative hand motion above the sensor.

The basic gesture outputs commonly used in embedded applications are

\[
\boxed{
\text{UP},\quad
\text{DOWN},\quad
\text{LEFT},\quad
\text{RIGHT}
}
\]

These individual gestures can be directly mapped to simple actions. For example, a RIGHT gesture may turn on an LED, while a LEFT gesture may turn it off.

More advanced interaction can be created by combining multiple basic gestures into a **gesture sequence**. For example,

\[
\text{LEFT}
\rightarrow
\text{RIGHT}
\rightarrow
\text{UP}
\]

can be defined as a custom command.

In this laboratory, students will interface an APDS-9960 sensor with an ESP32, detect basic directional gestures, and develop a **Finite-State Machine (FSM)** to recognize predefined gesture sequences.

The overall system is represented by

\[
\boxed{
\text{Hand Motion}
\rightarrow
\text{APDS-9960}
\rightarrow
\text{Gesture Event}
\rightarrow
\text{FSM}
\rightarrow
\text{Control Action}
}
\]

This experiment introduces an important embedded-system concept: transforming raw sensor events into higher-level application behavior.

---

## 2. Learning Objectives

After completing this laboratory, students should be able to:

1. Interface an APDS-9960 sensor with an ESP32 using I²C.
2. Detect basic directional gestures.
3. Represent sensor events as software states and events.
4. Design a finite-state machine for gesture-sequence recognition.
5. Implement timing constraints between gesture events.
6. Map recognized gesture sequences to actuator commands.
7. Test and evaluate the reliability of a gesture-recognition system.

---

## 3. Required Hardware

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 development board | 1 | Main processing unit |
| APDS-9960 module | 1 | Gesture sensor |
| LEDs | 2–3 | Output indicators |
| 220–330 \(\Omega\) resistors | 2–3 | LED current limiting |
| Breadboard | 1 | Circuit construction |
| Jumper wires | Several | Electrical connections |
| USB cable | 1 | Programming and serial communication |

---

## 4. APDS-9960 Sensor Summary

| Feature | Description |
|---|---|
| Sensor type | Optical gesture/proximity/light/color sensor |
| Basic gestures | UP, DOWN, LEFT, RIGHT |
| Proximity | Infrared reflection |
| Color channels | Red, Green, Blue, Clear |
| Communication | I²C |
| Interrupt | INT output available |
| Gesture principle | Directional IR photodiodes |
| MCU interface | ESP32 or other I²C-compatible MCU |

---

## 5. System Architecture

The proposed laboratory system consists of three major processing stages:

\[
\text{Sensor}
\rightarrow
\text{Gesture Processing}
\rightarrow
\text{Application}
\]

More specifically,

\[
\boxed{
\text{APDS-9960}
\xrightarrow{\mathrm{I^2C}}
\text{ESP32}
\rightarrow
\text{FSM}
\rightarrow
\text{LED/Actuator}
}
\]

The APDS-9960 performs optical sensing and provides basic gesture information. The ESP32 receives the gesture event and processes the sequence using an FSM. When a predefined sequence is successfully recognized, an associated output action is executed.

---

## 6. Hardware Connection

A typical connection between the APDS-9960 and ESP32 is shown below.

| APDS-9960 | ESP32 | Function |
|---|---|---|
| VCC/VIN | 3.3 V* | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | I²C data |
| SCL | GPIO 22 | I²C clock |
| INT | Optional GPIO | Interrupt |

\*The appropriate supply voltage should be verified for the specific APDS-9960 breakout module used in the laboratory.

The I²C architecture is therefore

\[
\text{ESP32}
\xleftrightarrow[\mathrm{SCL}]{\mathrm{SDA}}
\text{APDS-9960}.
\]

---

## 7. Experiment 1 — Basic Gesture Detection

The first experiment verifies communication between the ESP32 and APDS-9960 and detects individual directional gestures.

The expected gesture events are

\[
G=
\{
G_U,G_D,G_L,G_R
\},
\]

where

- \(G_U\): UP,
- \(G_D\): DOWN,
- \(G_L\): LEFT,
- \(G_R\): RIGHT.

A simple application can map these events as follows:

| Gesture | Action |
|---|---|
| UP | Display `UP` |
| DOWN | Display `DOWN` |
| LEFT | Display `LEFT` |
| RIGHT | Display `RIGHT` |

### Procedure

1. Connect the APDS-9960 to the ESP32.
2. Initialize the I²C interface.
3. Initialize the gesture sensor.
4. Enable gesture detection.
5. Move your hand above the sensor.
6. Read the detected gesture.
7. Display the result using the Serial Monitor.

The basic processing algorithm is

\[
\text{Read Sensor}
\rightarrow
\text{Gesture Available?}
\rightarrow
\text{Identify Gesture}
\rightarrow
\text{Display Result}.
\]

---

## 8. Experiment 2 — Gesture-Controlled LEDs

The next experiment uses directional gestures as control commands.

For example,

\[
\begin{aligned}
\text{RIGHT} &\rightarrow \text{LED ON},\\
\text{LEFT}  &\rightarrow \text{LED OFF},\\
\text{UP}    &\rightarrow \text{LED Toggle},\\
\text{DOWN}  &\rightarrow \text{All LEDs OFF}.
\end{aligned}
\]

This experiment demonstrates an event-driven sensing-and-control architecture:

\[
\boxed{
\text{Gesture}
\rightarrow
\text{Event}
\rightarrow
\text{Decision}
\rightarrow
\text{Actuator}
}
\]

Students should verify that each gesture consistently produces the intended output.

---

## 9. Experiment 3 — Custom Gesture Sequence

Individual gestures can be combined to create higher-level commands.

Consider the custom sequence

\[
\boxed{
\text{LEFT}
\rightarrow
\text{RIGHT}
\rightarrow
\text{UP}
}
\]

and define this sequence as the command

\[
C_1=\{\text{LEFT},\text{RIGHT},\text{UP}\}.
\]

When the ESP32 recognizes the complete sequence, it performs a predefined action such as turning on an LED.

This requires the microcontroller to remember previously detected gestures rather than treating every gesture independently.

---

## 10. Finite-State Machine Design

A finite-state machine provides a systematic method for recognizing the gesture sequence.

For

\[
\text{LEFT}
\rightarrow
\text{RIGHT}
\rightarrow
\text{UP},
\]

define four states:

\[
S=
\{
S_0,S_1,S_2,S_3
\}.
\]

Their meanings are:

| State | Description |
|---|---|
| \(S_0\) | Waiting for LEFT |
| \(S_1\) | LEFT detected; waiting for RIGHT |
| \(S_2\) | LEFT → RIGHT detected; waiting for UP |
| \(S_3\) | Complete gesture recognized |

The desired transitions are

\[
S_0
\xrightarrow{\text{LEFT}}
S_1
\xrightarrow{\text{RIGHT}}
S_2
\xrightarrow{\text{UP}}
S_3.
\]

After executing the command,

\[
S_3\rightarrow S_0.
\]

The overall FSM can therefore be represented as

\[
\boxed{
S_0
\xrightarrow{L}
S_1
\xrightarrow{R}
S_2
\xrightarrow{U}
S_3
\xrightarrow{\text{Action}}
S_0
}
\]

where \(L\), \(R\), and \(U\) represent LEFT, RIGHT, and UP gestures, respectively.

---

## 11. Gesture Timing Constraint

A practical gesture-sequence recognizer should include a time constraint. Without one, a LEFT gesture performed several minutes before RIGHT and UP could incorrectly be interpreted as part of the same sequence.

Let the detected gesture times be

\[
t_1,t_2,\ldots,t_K.
\]

For consecutive gestures to belong to the same sequence,

\[
t_{i+1}-t_i
\leq
T_{\max},
\]

where \(T_{\max}\) is the maximum permitted interval.

For example,

\[
T_{\max}=2~\mathrm{s}
\]

may be used initially and adjusted experimentally.

If

\[
t_{i+1}-t_i>T_{\max},
\]

the FSM returns to

\[
S_0.
\]

Thus, the recognition problem becomes dependent on both **gesture order** and **gesture timing**.

---

## 12. FSM Pseudocode

```cpp
switch (state) {

    case WAIT_LEFT:

        if (gesture == LEFT) {
            state = WAIT_RIGHT;
            previousTime = millis();
        }

        break;


    case WAIT_RIGHT:

        if (millis() - previousTime > TIMEOUT) {
            state = WAIT_LEFT;
        }

        else if (gesture == RIGHT) {
            state = WAIT_UP;
            previousTime = millis();
        }

        break;


    case WAIT_UP:

        if (millis() - previousTime > TIMEOUT) {
            state = WAIT_LEFT;
        }

        else if (gesture == UP) {
            state = GESTURE_COMPLETE;
        }

        break;


    case GESTURE_COMPLETE:

        // Execute custom command
        digitalWrite(LED_PIN, HIGH);

        state = WAIT_LEFT;

        break;
}
```

The implementation should also define how unexpected gestures are handled. Depending on the desired behavior, an incorrect gesture can either reset the FSM immediately or be ignored until the timeout expires.

---

## 13. Experiment 4 — Multiple Custom Gestures

Extend the system to recognize several gesture sequences.

For example:

| Custom Command | Gesture Sequence | Application |
|---|---|---|
| \(C_1\) | LEFT → RIGHT → UP | LED 1 ON |
| \(C_2\) | RIGHT → LEFT → DOWN | LED 1 OFF |
| \(C_3\) | UP → UP → RIGHT | LED 2 ON |
| \(C_4\) | DOWN → DOWN → LEFT | All LEDs OFF |

A custom command can generally be represented as

\[
C_k=
\{
G_1,G_2,\ldots,G_L
\},
\]

where \(L\) is the number of gestures forming command \(C_k\).

The embedded recognition architecture then becomes

\[
\boxed{
\text{APDS-9960}
\rightarrow
\text{Basic Gesture}
\rightarrow
\text{Gesture Sequence}
\rightarrow
\text{FSM}
\rightarrow
\text{Custom Command}
}
\]

---

## 14. Performance Evaluation

Gesture recognition should be evaluated experimentally rather than from successful demonstrations alone.

For each predefined gesture sequence, perform \(N\) trials and record the number of successful recognitions \(N_c\).

The recognition accuracy can be calculated as

\[
\text{Accuracy}
=
\frac{N_c}{N}
\times100\%.
\]

For example, if a command is correctly detected in 18 of 20 attempts,

\[
\text{Accuracy}
=
\frac{18}{20}\times100
=
90\%.
\]

Students should also record false recognitions, missed gestures, and timeout events.

A suggested results table is:

| Gesture Sequence | Trials | Correct | Incorrect | Accuracy |
|---|---:|---:|---:|---:|
| LEFT → RIGHT → UP | 20 | -- | -- | -- |
| RIGHT → LEFT → DOWN | 20 | -- | -- | -- |
| UP → UP → RIGHT | 20 | -- | -- | -- |

---

## 15. Discussion Questions

1. How does the APDS-9960 determine the direction of a hand gesture?
2. Why is I²C suitable for communication between the APDS-9960 and ESP32?
3. What is the difference between a basic gesture and a custom gesture sequence?
4. Why is an FSM useful for recognizing gesture sequences?
5. Why should a timeout be included in the FSM?
6. What happens when an incorrect gesture occurs in the middle of a sequence?
7. How does the choice of \(T_{\max}\) affect recognition performance?
8. Can two different custom commands contain some of the same basic gestures?
9. What factors may cause incorrect gesture recognition?
10. What are the limitations of the APDS-9960 for recognizing complex hand gestures?

---

## 16. Assignment

Design and implement a **custom gesture-controlled embedded system** using an ESP32 and APDS-9960.

The system must:

1. Detect the four basic directional gestures.
2. Implement at least **three custom gesture sequences**.
3. Use an FSM to recognize each sequence.
4. Include a timeout mechanism.
5. Control at least two output devices or functions.
6. Display detected gestures and FSM states on the Serial Monitor.
7. Evaluate each custom sequence using at least 20 trials.
8. Calculate the recognition accuracy for each command.

Students should submit the program source code, FSM state diagram, experimental results, and a brief discussion of recognition errors.

---

## 17. Conclusion

This laboratory demonstrates how simple sensor outputs can be transformed into higher-level embedded-system commands. Although the APDS-9960 primarily provides basic directional gesture information, software can combine these events into more sophisticated interaction patterns.

The key concept can be summarized as

\[
\boxed{
\text{Physical Gesture}
\rightarrow
\text{Optical Sensing}
\rightarrow
\text{Basic Gesture}
\rightarrow
\text{Temporal Sequence}
\rightarrow
\text{FSM Recognition}
\rightarrow
\text{Application Action}.
}
\]

The experiment therefore extends beyond basic sensor interfacing and introduces students to **event-driven programming, temporal event processing, state-machine design, and human--machine interaction** in embedded systems.
