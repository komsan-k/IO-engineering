# LAB 2 — GPIO Interrupts, Button Debouncing, and Hardware Timer LED Blinking

## 1. Objective

The objective of this lab is to:

1. Understand the difference between **polling** and **interrupt-driven input handling**.
2. Configure a push button to generate a GPIO interrupt.
3. Write and use an Interrupt Service Routine (ISR).
4. Understand interrupt latency and event-driven execution.
5. Detect button events using falling-edge interrupts.
6. Observe the effect of mechanical switch bounce.
7. Implement software debouncing for interrupt-driven input.
8. Use a hardware timer to generate periodic events.
9. Blink an LED at a fixed frequency using a timer.
10. Separate asynchronous button events from periodic timer events.
11. Understand the restrictions that apply inside an ISR.
12. Use `volatile` variables for data shared between ISR and main code.
13. Build a simple event-driven embedded application using button interrupts and timer-based LED control.

---

## 2. Background

In the previous lab, the push button was read using **polling**.

The CPU repeatedly performed:

```text
Read Button
    ↓
Check State
    ↓
Read Again
    ↓
Check Again
```

This approach is simple, but the CPU continues checking the input even when no event occurs.

An interrupt-driven system instead follows:

```text
Main Program
     ↓
Perform Other Work
     ↓
Button Event Occurs
     ↓
Interrupt Generated
     ↓
ISR Executes
     ↓
Return to Main Program
```

This is called **event-driven programming**.

---

## 3. Polling vs. Interrupt

### Polling

```text
CPU
 ↓
Read Button
 ↓
No Event
 ↓
Read Again
 ↓
No Event
 ↓
Read Again
```

The CPU actively checks the input.

### Interrupt

```text
CPU
 ↓
Main Program
 ↓
Button Press
 ↓
Interrupt
 ↓
ISR
 ↓
Main Program
```

The CPU responds only when the event occurs.

---

## 4. Interrupt Concept

An interrupt temporarily changes the normal program flow.

Conceptually:

```text
Main Program
     ↓
Instruction
     ↓
Instruction
     ↓
Interrupt Event
     ↓
Save Context
     ↓
Execute ISR
     ↓
Restore Context
     ↓
Continue Main Program
```

The function executed in response to the interrupt is called an **Interrupt Service Routine (ISR)**.

---

## 5. Interrupt Service Routine

A basic ISR may look like:

```cpp
void IRAM_ATTR buttonISR() {

  // Handle interrupt event
}
```

On ESP32, `IRAM_ATTR` is commonly used for interrupt routines that should reside in instruction RAM.

The ISR should normally be:

```text
Short
+
Fast
+
Non-Blocking
```

---

## 6. Important ISR Rules

Avoid performing long operations inside an ISR.

Do not use an ISR for:

```text
Long Delays
Large Computations
Blocking Operations
Complex Serial Printing
Network Communication
Long Sensor Transactions
```

Instead, use the ISR to set a flag or signal another part of the application.

Recommended pattern:

```text
Interrupt Event
      ↓
ISR
      ↓
Set Flag
      ↓
Return Quickly
      ↓
Main Program Handles Event
```

---

## 7. Hardware Requirements

Use:

- ESP32 development board
- USB cable
- Push button
- LED
- 220–330 $\Omega$ resistor
- 10 k $\Omega$ resistor if an external pull-up is used
- Breadboard
- Jumper wires

Suggested pins:

```text
LED    → GPIO2
Button → GPIO4
```

The exact pins may be modified to match the board.

---

## 8. Push-Button Circuit

Use an active-low button:

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
Released → HIGH
Pressed  → LOW
```

A button press causes:

```text
HIGH → LOW
```

which is a **falling edge**.

---

## 9. Experiment 1 — Configure a GPIO Interrupt

Define:

```cpp
#define BUTTON_PIN 4
```

Configure the input:

```cpp
pinMode(
  BUTTON_PIN,
  INPUT_PULLUP
);
```

Attach an interrupt:

```cpp
attachInterrupt(
  digitalPinToInterrupt(BUTTON_PIN),
  buttonISR,
  FALLING
);
```

The trigger mode:

```text
FALLING
```

means that the ISR is called when the signal changes:

```text
HIGH → LOW
```

---

## 10. Basic Button ISR

Create:

```cpp
volatile bool buttonEvent = false;
```

ISR:

```cpp
void IRAM_ATTR buttonISR() {

  buttonEvent = true;
}
```

The main program checks:

```cpp
if (buttonEvent) {

  buttonEvent = false;

  Serial.println(
    "Button interrupt detected"
  );
}
```

---

## 11. Complete Basic Interrupt Program

```cpp
#include <Arduino.h>

#define BUTTON_PIN 4

volatile bool buttonEvent = false;

void IRAM_ATTR buttonISR() {

  buttonEvent = true;
}

void setup() {

  Serial.begin(115200);

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonISR,
    FALLING
  );
}

void loop() {

  if (buttonEvent) {

    buttonEvent = false;

    Serial.println(
      "Button interrupt detected"
    );
  }
}
```

---

## 12. Expected Result

When the button is pressed:

```text
Button Press
    ↓
Falling Edge
    ↓
GPIO Interrupt
    ↓
buttonISR()
    ↓
buttonEvent = true
    ↓
Main Program Responds
```

Possible Serial output:

```text
Button interrupt detected
Button interrupt detected
Button interrupt detected
```

However, one physical button press may produce multiple messages.

This occurs because of **switch bounce**.

---

## 13. Mechanical Switch Bounce

A real push button does not always transition cleanly from:

```text
HIGH → LOW
```

Instead, it may produce:

```text
HIGH ─────┐
          └─┐ ┌─┐
            └─┘ └──── LOW
```

or logically:

```text
1 → 0 → 1 → 0 → 1 → 0
```

Each falling transition may generate another interrupt.

Therefore:

```text
One Physical Press
       ↓
Several Electrical Edges
       ↓
Several Interrupts
```

---

## 14. Debouncing

Debouncing is the process of rejecting rapid false transitions caused by a mechanical switch.

A simple timing approach is:

```text
Interrupt
   ↓
Check Time Since Previous Valid Event
   ↓
Greater Than Debounce Interval?
      ┌──────┴──────┐
     Yes            No
      ↓              ↓
Accept Event      Ignore Event
```

Typical debounce intervals may be in the range of several tens of milliseconds.

For this lab, use:

```text
50 ms
```

as an example.

---

## 15. Experiment 2 — ISR Debounce Using Time

Define:

```cpp
volatile bool buttonEvent = false;

volatile unsigned long lastInterruptTime = 0;
```

ISR:

```cpp
void IRAM_ATTR buttonISR() {

  unsigned long currentTime =
      millis();

  if (
    currentTime
    - lastInterruptTime
    > 50
  ) {

    buttonEvent = true;

    lastInterruptTime =
        currentTime;
  }
}
```

This is simple for teaching, although timing functions used inside ISRs should be selected carefully for a production design.

A cleaner pattern is to timestamp or flag the event in the ISR and perform debounce validation outside the ISR.

---

## 16. Recommended Debounce Architecture

Use:

```text
Button Edge
    ↓
ISR
    ↓
Set Event Flag
    ↓
Main Program
    ↓
Check Debounce Timing
    ↓
Accept or Reject
```

This keeps the ISR short.

---

## 17. Experiment 3 — Debounce Outside the ISR

Declare:

```cpp
volatile bool buttonInterrupt = false;

unsigned long lastValidPress = 0;

const unsigned long debounceTime = 50;
```

ISR:

```cpp
void IRAM_ATTR buttonISR() {

  buttonInterrupt = true;
}
```

Main logic:

```cpp
if (buttonInterrupt) {

  buttonInterrupt = false;

  unsigned long now =
      millis();

  if (
    now - lastValidPress
    >= debounceTime
  ) {

    lastValidPress = now;

    Serial.println(
      "Valid button press"
    );
  }
}
```

---

## 18. Complete Debounced Button Program

```cpp
#include <Arduino.h>

#define BUTTON_PIN 4

volatile bool buttonInterrupt = false;

unsigned long lastValidPress = 0;

const unsigned long debounceTime = 50;

void IRAM_ATTR buttonISR() {

  buttonInterrupt = true;
}

void setup() {

  Serial.begin(115200);

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonISR,
    FALLING
  );
}

void loop() {

  if (buttonInterrupt) {

    buttonInterrupt = false;

    unsigned long now =
        millis();

    if (
      now - lastValidPress
      >= debounceTime
    ) {

      lastValidPress =
          now;

      Serial.println(
        "Valid button press"
      );
    }
  }
}
```

---

## 19. Interrupt Event Counter

Add:

```cpp
unsigned int pressCount = 0;
```

Then:

```cpp
pressCount++;

Serial.print(
  "Button Count = "
);

Serial.println(
  pressCount
);
```

The expected behavior is:

```text
Press 1 → Count = 1
Press 2 → Count = 2
Press 3 → Count = 3
```

With correct debounce, one physical press should approximately correspond to one count increment.

---

## 20. Timer-Based LED Blinking

The second part of the lab uses a timer to toggle an LED at a fixed frequency.

The architecture is:

```text
Hardware Timer
      ↓
Periodic Interrupt
      ↓
Timer ISR
      ↓
Toggle LED State
```

Unlike a busy-wait loop:

```text
CPU
 ↓
Wait
 ↓
Toggle
 ↓
Wait
```

a timer generates events according to a hardware timing source.

---

## 21. Frequency and Period

Frequency and period are related by:

$$
f=\frac{1}{T}
$$

and:

$$
T=\frac{1}{f}.
$$

For example, for:

$$
f=2~\text{Hz},
$$

the full LED waveform period is:

$$
T=\frac{1}{2}=0.5~\text{s}.
$$

If the LED toggles every half-cycle, the timer interrupt interval is:

$$
T_{\text{toggle}}
= \frac{T}{2}
= 0.25~\text{s}.
$$

Therefore:

```text
250 ms → Toggle
250 ms → Toggle
```

produces a 2-Hz ON/OFF waveform.

---

## 22. Timer Waveform

For a 2-Hz LED signal:

```text
HIGH ─────┐       ┌──────
          │       │
LOW       └───────┘
      250 ms 250 ms
```

The full period is:

```text
500 ms
```

and:

$$
f= \frac{1}{0.5}
=
2~\text{Hz}.
$$

---

## 23. Experiment 4 — ESP32 Hardware Timer

The exact ESP32 timer API depends on the ESP32 Arduino Core version.

A typical approach is conceptually:

```cpp
hw_timer_t *timer = NULL;
```

Then:

```text
Create Timer
     ↓
Attach ISR
     ↓
Set Alarm Period
     ↓
Enable Auto-Reload
     ↓
Start Timer
```

Students should use the timer API corresponding to the installed ESP32 Arduino Core version.

---

## 24. Timer ISR

Define:

```cpp
volatile bool timerEvent = false;
```

ISR:

```cpp
void IRAM_ATTR timerISR() {

  timerEvent = true;
}
```

The main program then performs the actual LED toggle:

```cpp
if (timerEvent) {

  timerEvent = false;

  ledState =
      !ledState;

  digitalWrite(
    LED_PIN,
    ledState
  );
}
```

This keeps the timer ISR short.

---

## 25. Recommended Timer Architecture

Use:

```text
Hardware Timer
      ↓
Timer ISR
      ↓
Set timerEvent
      ↓
Main Program
      ↓
Toggle LED
```

instead of:

```text
Hardware Timer
      ↓
Timer ISR
      ↓
Long Processing
```

---

## 26. Experiment 5 — Fixed-Frequency LED Blink

Define:

```cpp
#define LED_PIN 2

volatile bool timerEvent = false;

bool ledState = false;
```

Target frequency:

```text
2 Hz
```

Full period:

$$
T=500~\text{ms}.
$$

Toggle period:

$$
T_{\text{toggle}}=250~\text{ms}.
$$

Configure the timer to generate an interrupt every:

```text
250 ms
```

---

## 27. Expected LED Behavior

```text
Time: 0    250   500   750   1000 ms

LED:  OFF   ON    OFF   ON    OFF
```

The complete ON/OFF pattern repeats every:

```text
500 ms
```

Therefore:

$$
f_{\text{LED}}=2~\text{Hz}.
$$

---

## 28. Experiment 6 — Measure the LED Frequency

Record the timestamp whenever the LED returns to the same state.

For example:

```text
Rising Edge 1 → t1
Rising Edge 2 → t2
```

Then:

$$
T=t_2-t_1.
$$

Frequency is:

$$
f=\frac{1}{T}.
$$

Record:

| Sample | Measured Period | Calculated Frequency |
|---:|---:|---:|
| 1 | | |
| 2 | | |
| 3 | | |
| 4 | | |
| 5 | | |

---

## 29. Experiment 7 — Change Blink Frequency

Test:

```text
0.5 Hz
1 Hz
2 Hz
5 Hz
```

For each frequency:

$$
T=\frac{1}{f}
$$

and:

$$
T_{\text{toggle}}
= \frac{1}{2f}.
$$

Complete:

| LED Frequency | Full Period | Toggle Interval |
|---:|---:|---:|
| 0.5 Hz | 2 s | 1 s |
| 1 Hz | 1 s | 0.5 s |
| 2 Hz | 0.5 s | 0.25 s |
| 5 Hz | 0.2 s | 0.1 s |

---

## 30. Experiment 8 — Combine Button Interrupt and Timer

Now integrate both systems.

Architecture:

```text
                ┌─────────────┐
Button ────────→│ GPIO ISR    │
                └──────┬──────┘
                       ↓
                 Button Event
                       ↓
                   Main Logic

                ┌─────────────┐
Timer ─────────→│ Timer ISR   │
                └──────┬──────┘
                       ↓
                  Timer Event
                       ↓
                  LED Toggle
```

The button and timer operate independently.

---

## 31. Combined Application Behavior

Required behavior:

```text
Timer
  ↓
LED Blinks at Fixed Frequency

Button
  ↓
Interrupt
  ↓
Debounced Press Count
```

Thus:

```text
Periodic Event + Asynchronous Event
```

operate in the same program.

---

## 32. Event Variables

Declare:

```cpp
volatile bool buttonInterrupt = false;
volatile bool timerEvent = false;
```

These variables are shared between ISRs and normal program execution.

The keyword:

```cpp
volatile
```

informs the compiler that the variable can change unexpectedly outside the normal program flow.

---

## 33. Why `volatile` Is Important

Without `volatile`, the compiler may assume:

```text
Variable Changes
      ↓
Only Through Normal Code
```

But ISR-shared variables can change asynchronously:

```text
Main Code
   ↓
Variable

ISR
 ↓
Changes Same Variable
```

Therefore, interrupt-shared flags are commonly declared:

```cpp
volatile bool eventFlag;
```

Note that `volatile` does **not** make complex operations atomic or thread-safe. It only affects compiler assumptions about access to the variable.

---

## 34. Experiment 9 — Use Button to Enable or Disable Blinking

Modify the application so that each valid button press toggles the timer-controlled blinking state.

Required behavior:

```text
Initial
LED Blinking

Press Button
     ↓
Blinking Disabled

Press Again
     ↓
Blinking Enabled
```

Use:

```cpp
bool blinkEnabled = true;
```

Then:

```cpp
blinkEnabled =
    !blinkEnabled;
```

---

## 35. Control Logic

The system can be represented as:

$$
E_{k+1}
= \neg E_k
$$

for each valid button press, where:

- $E=1$ means blinking enabled.
- $E=0$ means blinking disabled.

---

## 36. Experiment 10 — Change Blink Frequency with Button Presses

Use the button to cycle through:

```text
1 Hz
2 Hz
5 Hz
```

For example:

```text
Press 1 → 1 Hz
Press 2 → 2 Hz
Press 3 → 5 Hz
Press 4 → 1 Hz
```

The control sequence is:

```text
Button Interrupt
      ↓
Debounce
      ↓
Update Mode
      ↓
Change Timer Period
```

---

## 37. Frequency Mode Table

| Mode | Frequency | Toggle Interval |
|---:|---:|---:|
| 0 | 1 Hz | 500 ms |
| 1 | 2 Hz | 250 ms |
| 2 | 5 Hz | 100 ms |

Use:

$$
T_{\text{toggle}}
= \frac{1}{2f}.
$$

---

## 38. Interrupt Latency

Interrupt latency is the time between:

```text
Interrupt Event
```

and:

```text
ISR Execution
```

It can be expressed approximately as:

$$
T_{\text{latency}}
= t_{\text{ISR start}}
- t_{\text{event}}.
$$

Interrupt latency depends on:

- Current processor activity
- Interrupt priority
- Disabled interrupt periods
- Other interrupt activity
- System architecture

---

## 39. Experiment 11 — Compare Polling and Interrupt Response

Implement the button twice.

### Version A — Polling

```text
loop()
 ↓
digitalRead()
 ↓
Check Button
```

### Version B — Interrupt

```text
Button
 ↓
GPIO Interrupt
 ↓
ISR
```

Compare:

| Feature | Polling | Interrupt |
|---|---|---|
| CPU checks continuously | Yes | No |
| Event-driven | No | Yes |
| Simple implementation | Yes | Moderate |
| Suitable for infrequent events | Less efficient | Good |
| ISR required | No | Yes |

---

## 40. Experiment 12 — Observe CPU Availability

In the main loop, add:

```cpp
static unsigned long lastPrint = 0;

if (
  millis() - lastPrint >= 1000
) {

  lastPrint = millis();

  Serial.println(
    "Main program running"
  );
}
```

Observe that the main program continues executing while the timer and button events occur asynchronously.

Conceptually:

```text
Main Program
    ↓
Button ISR
    ↓
Main Program
    ↓
Timer ISR
    ↓
Main Program
```

---

## 41. Interrupt Priority Concept

In more advanced systems, interrupts may have different priorities.

Conceptually:

```text
Higher-Priority Interrupt
         ↓
Handled First

Lower-Priority Interrupt
         ↓
Handled Later
```

This lab does not require manual interrupt-priority configuration, but students should understand that real-time systems depend on interrupt scheduling and priority.

---

## 42. Common Errors

### Performing `Serial.println()` Inside an ISR

Avoid:

```cpp
void IRAM_ATTR buttonISR() {

  Serial.println(
    "Button"
  );
}
```

Prefer:

```cpp
void IRAM_ATTR buttonISR() {

  buttonInterrupt = true;
}
```

### Using `delay()` Inside an ISR

Do not use:

```cpp
delay(50);
```

inside an ISR.

An ISR should return quickly.

### No Debounce

Without debounce:

```text
One Press
   ↓
Multiple Interrupts
```

### Wrong Interrupt Mode

For an active-low button:

```text
Pressed = HIGH → LOW
```

therefore:

```cpp
FALLING
```

is appropriate.

### Forgetting `volatile`

ISR-shared flags should generally be declared:

```cpp
volatile bool flag;
```

### Long Timer ISR

Avoid:

```text
Timer Interrupt
      ↓
Long Processing
      ↓
Other Events Delayed
```

Prefer:

```text
ISR
 ↓
Set Flag
 ↓
Main Program Handles Work
```

---

## 43. Experimental Summary

### Experiment 1 — GPIO Interrupt

Generate an interrupt from a button press.

### Experiment 2 — Observe Bounce

Press the button and observe repeated interrupt events.

### Experiment 3 — Debounce

Reject rapid false button transitions.

### Experiment 4 — Hardware Timer

Configure a periodic timer interrupt.

### Experiment 5 — Fixed-Frequency Blink

Blink an LED at 2 Hz.

### Experiment 6 — Frequency Measurement

Measure LED period and calculate frequency.

### Experiment 7 — Frequency Variation

Test several timer frequencies.

### Experiment 8 — Combined Events

Run button and timer events together.

### Experiment 9 — Enable/Disable Blink

Use the button to control timer-driven blinking.

### Experiment 10 — Frequency Selection

Use button presses to change the timer frequency.

### Experiment 11 — Polling vs. Interrupt

Compare the two input methods.

### Experiment 12 — Main Program Activity

Observe asynchronous interrupt behavior while normal code continues to run.

---

## 44. Exercises

### Exercise 1 — Button Interrupt

Configure a button on GPIO4 to generate a falling-edge interrupt.

Each valid press should increment a counter.

### Exercise 2 — Debounce

Use a:

```text
50 ms
```

debounce interval.

Show that one physical press approximately produces one valid event.

### Exercise 3 — Fixed LED Frequency

Blink an LED at:

```text
1 Hz
```

using a timer.

Calculate the required toggle interval.

### Exercise 4 — 2-Hz LED

For:

$$
f=2~\text{Hz},
$$

calculate:

$$
T
$$

and:

$$
T_{\text{toggle}}.
$$

Implement and verify experimentally.

### Exercise 5 — Button-Controlled Blink

Each valid button press should toggle between:

```text
Blinking
```

and:

```text
Stopped
```

### Exercise 6 — Three Frequency Modes

Cycle between:

```text
1 Hz
2 Hz
5 Hz
```

using button presses.

### Exercise 7 — Timing Measurement

Measure at least 10 LED periods and calculate:

$$
\bar{T}
= \frac{1}{N}
\sum_{i=1}^{N}T_i.
$$

Then calculate:

$$
f_{\text{measured}}
= \frac{1}{\bar{T}}.
$$

### Exercise 8 — Polling vs. Interrupt

Implement both button-reading methods and discuss which is more suitable for an infrequent asynchronous event.

---

## 45. Checkpoint Questions

1. What is an interrupt?
2. What is an ISR?
3. What is the difference between polling and interrupts?
4. What does `FALLING` mean?
5. Why is an active-low button suitable for a falling-edge interrupt?
6. What is mechanical switch bounce?
7. Why can one button press generate multiple interrupts?
8. What is debouncing?
9. Why should an ISR be short?
10. Why should blocking operations be avoided in an ISR?
11. Why is `Serial.println()` usually avoided in an ISR?
12. What does `volatile` mean?
13. Does `volatile` make an operation atomic?
14. What is interrupt latency?
15. What is the relationship between frequency and period?
16. What is the period of a 1-Hz signal?
17. What is the period of a 2-Hz signal?
18. Why is the timer toggle interval half the full LED period?
19. What is the toggle interval for a 5-Hz LED waveform?
20. What is the advantage of a hardware timer over a busy-wait delay?
21. What happens when a timer ISR is too long?
22. Why is it useful to set an event flag in the ISR?
23. How can a button interrupt be used to change the timer frequency?
24. How can a button enable or disable blinking?
25. Why can timer and button events operate independently?
26. What is event-driven programming?
27. What is asynchronous input?
28. Why are interrupts useful for infrequent events?
29. Why are timers useful for periodic operations?
30. How does this lab improve on the polling and busy-wait methods used previously?

---

## 46. Expected Results

Students should successfully implement:

```text
Push Button
     ↓
GPIO Interrupt
     ↓
ISR
     ↓
Debounce
     ↓
Valid Event
```

and simultaneously:

```text
Hardware Timer
     ↓
Periodic Interrupt
     ↓
Timer Event
     ↓
LED Toggle
     ↓
Fixed Frequency
```

The complete application becomes:

```text
                ESP32
          ┌───────────────┐
          │               │
Button ──→│ GPIO Interrupt│
          │       ↓       │
          │   Debounce    │
          │               │
Timer ───→│ Timer Event   │
          │       ↓       │
LED   ←───│ LED Control   │
          │               │
          └───────────────┘
```

---

## 47. Result Tables

### Button Interrupt

| Press | Raw Interrupts | Valid Debounced Events |
|---:|---:|---:|
| 1 | | |
| 2 | | |
| 3 | | |
| 4 | | |
| 5 | | |

### Timer Frequency

| Target Frequency | Expected Period | Measured Period | Measured Frequency |
|---:|---:|---:|---:|
| 0.5 Hz | 2 s | | |
| 1 Hz | 1 s | | |
| 2 Hz | 0.5 s | | |
| 5 Hz | 0.2 s | | |

---

## 48. Discussion

Students should explain how interrupt-driven input differs from polling.

Polling follows:

```text
CPU
 ↓
Check
 ↓
Check
 ↓
Check
```

while interrupts follow:

```text
Event
 ↓
Interrupt
 ↓
ISR
```

The button experiment demonstrates asynchronous event handling, while the timer experiment demonstrates periodic event generation.

The combination can be summarized as:

$$
\boxed{
\text{Event-Driven Embedded System}
= \text{Asynchronous Interrupts}
+
\text{Periodic Timer Events}
}
$$

Students should also discuss why mechanical button bounce must be filtered and why ISR execution time should be minimized.

The timer portion demonstrates that periodic operation should not depend on arbitrary software delay loops when accurate and repeatable timing is required.

---

## 49. Submission

Submit:

- GPIO button interrupt source code.
- Serial Monitor output showing interrupt events.
- Evidence of button bounce.
- Debounced button implementation.
- Raw-interrupt vs. valid-event measurements.
- Hardware timer LED blink program.
- Measured LED periods and frequencies.
- 1-Hz, 2-Hz, and 5-Hz timer results.
- Combined button-and-timer program.
- Button-controlled blink enable/disable extension.
- Optional multi-frequency extension.
- Answers to checkpoint questions.
- Brief discussion comparing polling, interrupts, busy-wait delays, and timers.

---

## 50. Conclusion

In this lab, a push button is converted from a continuously polled input into an **interrupt-driven event source**. The button generates a GPIO interrupt, the ISR records the event, and software debouncing prevents multiple false events caused by mechanical switch bounce.

A timer is then used to generate periodic events for LED blinking at a defined frequency. The relationship between frequency, full period, and toggle interval is:

$$
f=\frac{1}{T}
$$

and:

$$
T_{\text{toggle}}
= \frac{1}{2f}.
$$

The overall learning progression is:

```text
LAB 1
Polling
   ↓
Busy-Wait Timing
   ↓
LAB 2
GPIO Interrupt
   ↓
Debouncing
   ↓
Hardware Timer
   ↓
Periodic Events
```

The fundamental concept can be summarized as:

$$
\boxed{
\text{Responsive Embedded Control}
= \text{Interrupt-Driven Events}
+
\text{Timer-Based Periodic Execution}
}
$$

This lab prepares students for the next stage of MCU interfacing, where **ADC and PWM** can be used to acquire analog sensor signals and control actuators.
