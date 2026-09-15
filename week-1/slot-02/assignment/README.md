# Assignment — Button-Controlled Start/Stop Timer with LED and Event Counter

## Objective

Develop an ESP32 application that combines a **GPIO button interrupt** and a **hardware timer interrupt**. The push button acts as a **Start/Stop control** for the timer, while the timer toggles an LED every 1 second.

---

## System Requirements

1. Connect an **LED to GPIO2**.
2. Connect an active-low **push button to GPIO14** using `INPUT_PULLUP`.
3. Configure a hardware timer to generate an event every **1 second**.
4. Use a `FALLING` GPIO interrupt to detect button presses.
5. Each valid button press increments and displays the **button counter**.
6. The first button press **starts/enables** the timer.
7. The next button press **stops/disables** the timer.
8. Subsequent button presses alternate between **START** and **STOP**.
9. When the timer is stopped, set the LED to **OFF**.
10. Implement **button debouncing** to prevent multiple events from one physical press.

---

## System Operation

```text
System Start
    │
    ▼
Timer STOPPED
    │
    │ Button Press #1
    ▼
Timer START
    │
    ├── 1 s → Toggle LED
    ├── 1 s → Toggle LED
    ├── 1 s → Toggle LED
    │
    │ Button Press #2
    ▼
Timer STOP
LED OFF
    │
    │ Button Press #3
    ▼
Timer START
    │
    ├── 1 s → Toggle LED
    └── ...
```

---

## Expected Serial Monitor Output

```text
System Started
Timer = STOPPED

Button Pressed! Count = 1
Timer = STARTED

Button Pressed! Count = 2
Timer = STOPPED

Button Pressed! Count = 3
Timer = STARTED
```
<!---
---

## Starter Code — Arduino-ESP32 Core 3.x

```cpp
#include <Arduino.h>

#define LED_PIN     2
#define BUTTON_PIN 14

hw_timer_t *timer = NULL;

volatile bool timerEvent = false;
volatile bool buttonEvent = false;

bool timerRunning = false;
bool ledState = false;

unsigned int buttonCount = 0;

// ---------------- Timer ISR ----------------
void ARDUINO_ISR_ATTR onTimer() {
    timerEvent = true;
}

// ---------------- Button ISR ----------------
void ARDUINO_ISR_ATTR onButton() {
    buttonEvent = true;
}

void setup() {

    Serial.begin(115200);

    // Configure LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Configure active-low button
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Attach button interrupt
    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        onButton,
        FALLING
    );

    // Create timer with 1-MHz frequency
    timer = timerBegin(1000000);

    // Attach timer ISR
    timerAttachInterrupt(timer, &onTimer);

    // Alarm every 1,000,000 ticks = 1 second
    timerAlarm(timer, 1000000, true, 0);

    // Timer initially stopped
    timerStop(timer);

    Serial.println("System Started");
    Serial.println("Timer = STOPPED");
}

void loop() {

    // ---------------- Button Event ----------------
    if (buttonEvent) {

        buttonEvent = false;

        // TODO: Add software debounce

        buttonCount++;

        Serial.print("Button Pressed! Count = ");
        Serial.println(buttonCount);

        // Toggle timer state
        timerRunning = !timerRunning;

        if (timerRunning) {

            // Restart timer from zero
            timerWrite(timer, 0);

            // Start timer
            timerStart(timer);

            Serial.println("Timer = STARTED");
        }
        else {

            // Stop timer
            timerStop(timer);

            // Turn LED off
            ledState = false;
            digitalWrite(LED_PIN, LOW);

            Serial.println("Timer = STOPPED");
        }
    }

    // ---------------- Timer Event ----------------
    if (timerEvent) {

        timerEvent = false;

        if (timerRunning) {

            ledState = !ledState;

            digitalWrite(
                LED_PIN,
                ledState
            );
        }
    }
}
```
--->
---

## Student Task

Complete or modify the program to implement the following behavior:

| Button Count | Timer State | LED Operation |
|---:|---|---|
| Initial | STOPPED | OFF |
| 1 | STARTED | Toggle every 1 s |
| 2 | STOPPED | OFF |
| 3 | STARTED | Toggle every 1 s |
| 4 | STOPPED | OFF |
| 5 | STARTED | Toggle every 1 s |

The fundamental control logic is:

$$
\text{Button Press}
\rightarrow
\text{Toggle Timer State}
\rightarrow
\begin{cases}
\text{START}, & \text{if stopped} \\
\text{STOP}, & \text{if running}
\end{cases}
$$

---

## Program Flow

```text
                 ESP32
                   │
       ┌───────────┴───────────┐
       │                       │
 Push Button              Hardware Timer
   GPIO14                     1 s
       │                       │
       ▼                       ▼
Button Interrupt         Timer Interrupt
       │                       │
       ▼                       ▼
 Button Event              Timer Event
       │                       │
       ▼                       ▼
 Increment Count           Toggle LED
       │                     GPIO2
       ▼
START / STOP Timer
```
<!---
---

## Checkpoint Questions

1. Why is `FALLING` used for the active-low push-button interrupt?
2. Why are `buttonEvent` and `timerEvent` declared as `volatile`?
3. Why should the Interrupt Service Routine (ISR) be kept short?
4. Why should `Serial.print()` be performed in `loop()` rather than inside the ISR?
5. Why is **debouncing** necessary for the push button?
6. What happens when `timerStop()` is executed?
7. What is the purpose of `timerStart()`?
8. Why is `timerWrite(timer, 0)` used before restarting the timer?
9. How can the button interrupt and timer interrupt operate independently?
10. What is the advantage of this **event-driven approach** compared with using `delay()`?

---

## Extension — Add a Second Button

Add another push button to control the LED blinking rate.

```text
Button 1 → START / STOP timer

Button 2 → Change blinking interval
```

For example:

```text
Button 2 Press #1 → 1.00 s
Button 2 Press #2 → 0.50 s
Button 2 Press #3 → 0.25 s
Button 2 Press #4 → 1.00 s
```

The extended system becomes:

```text
Button 1
   │
   └──→ START / STOP Timer

Button 2
   │
   └──→ Change Timer Interval
              │
              ▼
        Hardware Timer
              │
              ▼
          Toggle LED
```

---

## Expected Learning Outcomes

After completing this assignment, students should be able to:

1. Configure a **GPIO interrupt** for push-button input.
2. Configure an ESP32 **hardware timer interrupt**.
3. Apply **event flags** for communication between ISRs and the main program.
4. Use a button event to **start and stop a hardware timer**.
5. Develop a simple **event-driven embedded application** without using blocking delays.

---

## Submission

Submit:

- Completed ESP32 source code
- Photo or video showing the LED operation
- Screenshot of the Serial Monitor showing the button count
- Demonstration of **START/STOP timer control**
- Results of button debouncing
- Answers to the checkpoint questions
- Short discussion of how the **button interrupt and timer interrupt operate independently**

---

## Key Concept

This assignment demonstrates the event-driven architecture:

```text
Hardware Event
      ↓
   Interrupt
      ↓
     ISR
      ↓
  Event Flag
      ↓
Application Logic
      ↓
System Action
```

or:

$$
\boxed{
\text{Interrupt}
\rightarrow
\text{Event Flag}
\rightarrow
\text{Application}
\rightarrow
\text{START/STOP Control}
}
$$
--->
