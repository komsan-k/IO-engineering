# Button-Pressed Interrupt Driver for ESP32

This driver encapsulates the **GPIO interrupt, event flag, and software debounce handling** into a reusable button class.

---

## Driver Architecture

```text
Push Button
    ↓
GPIO Interrupt
    ↓
Button ISR
    ↓
Event Flag
    ↓
Debounce
    ↓
isPressed()
    ↓
Application
```

---

## Project Structure

Create three files:

```text
project/
├── button.h
├── button.cpp
└── main.ino
```

---

## 1. `button.h`

```cpp
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {

private:
    uint8_t pin;
    unsigned long debounceTime;

    volatile bool interruptFlag;

    unsigned long lastPressTime;

    // ISR helper
    static void ARDUINO_ISR_ATTR isrHandler(void *arg);

public:
    Button(uint8_t buttonPin, unsigned long debounceMs = 50);

    void begin();

    bool isPressed();
};

#endif
```

---

## 2. `button.cpp`

```cpp
#include "button.h"

// Constructor
Button::Button(uint8_t buttonPin, unsigned long debounceMs) {
    pin = buttonPin;
    debounceTime = debounceMs;

    interruptFlag = false;
    lastPressTime = 0;
}

// Initialize button
void Button::begin() {

    // Active-low button
    pinMode(pin, INPUT_PULLUP);

    // Attach interrupt and pass this object to ISR
    attachInterruptArg(
        pin,
        isrHandler,
        this,
        FALLING
    );
}

// Static ISR
void ARDUINO_ISR_ATTR Button::isrHandler(void *arg) {

    Button *button =
        static_cast<Button *>(arg);

    // Keep ISR short
    button->interruptFlag = true;
}

// Check for valid button press
bool Button::isPressed() {

    if (interruptFlag) {

        interruptFlag = false;

        unsigned long currentTime = millis();

        // Software debounce
        if (
            currentTime - lastPressTime
            >= debounceTime
        ) {

            lastPressTime = currentTime;

            return true;
        }
    }

    return false;
}
```

---

## How the Driver Works

The Interrupt Service Routine (ISR) performs only a minimal operation:

```cpp
button->interruptFlag = true;
```

The ISR does **not** perform:

- Serial communication
- Long calculations
- Blocking delays
- Application processing

Instead, the application checks the event through:

```cpp
btn1.isPressed();
```

The processing sequence is:

```text
Button Press
    ↓
FALLING Edge
    ↓
GPIO Interrupt
    ↓
ISR Sets Flag
    ↓
isPressed()
    ↓
Debounce Check
    ↓
Valid Button Event
```

---

## 3. Simple `main.ino`

The following example counts valid button presses.

```cpp
#include "button.h"

#define BUTTON_PIN 14

Button btn1(BUTTON_PIN);

unsigned int count = 0;

void setup() {

    Serial.begin(115200);

    btn1.begin();

    Serial.println("Button Interrupt Driver Started");
}

void loop() {

    if (btn1.isPressed()) {

        count++;

        Serial.print("Button Pressed! Count = ");
        Serial.println(count);
    }
}
```

---

## Expected Output

```text
Button Interrupt Driver Started

Button Pressed! Count = 1
Button Pressed! Count = 2
Button Pressed! Count = 3
Button Pressed! Count = 4
```

---

# Using the Button Driver with a Hardware Timer

The button driver can be combined with a hardware timer to create a **START/STOP LED blinking application**.

The button:

```text
Button Press #1 → START timer
Button Press #2 → STOP timer
Button Press #3 → START timer
Button Press #4 → STOP timer
```

When enabled, the timer toggles the LED every **1 second**.

---

## Application Architecture

```text
                  Application
                 /           \
                /             \
       Button Driver      Hardware Timer
            │                  │
            │                  ▼
            │             Timer Event
            │                  │
            ▼                  ▼
      GPIO Interrupt       LED Control
            │
            ▼
       Push Button
```

---

## Example Program

```cpp
#include "button.h"

#define LED_PIN 2

Button btn1(14);

hw_timer_t *timer = NULL;

volatile bool timerEvent = false;

bool timerRunning = false;
bool ledState = false;

// Timer ISR
void ARDUINO_ISR_ATTR onTimer() {
    timerEvent = true;
}

void setup() {

    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize button interrupt driver
    btn1.begin();

    // Create timer at 1 MHz
    timer = timerBegin(1000000);

    // Attach timer interrupt
    timerAttachInterrupt(
        timer,
        &onTimer
    );

    // Interrupt every 1 second
    timerAlarm(
        timer,
        1000000,
        true,
        0
    );

    // Timer initially stopped
    timerStop(timer);

    Serial.println("System Started");
    Serial.println("Timer = STOPPED");
}

void loop() {

    // ---------------- Button Event ----------------
    if (btn1.isPressed()) {

        timerRunning = !timerRunning;

        if (timerRunning) {

            // Reset and start timer
            timerWrite(timer, 0);
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

---

## Expected Operation

| Button Event | Timer State | LED Operation |
|---|---|---|
| Initial | STOPPED | OFF |
| Press #1 | STARTED | Toggle every 1 s |
| Press #2 | STOPPED | OFF |
| Press #3 | STARTED | Toggle every 1 s |
| Press #4 | STOPPED | OFF |

---

## Expected Serial Monitor Output

```text
System Started
Timer = STOPPED

Timer = STARTED
Timer = STOPPED
Timer = STARTED
Timer = STOPPED
```

---

## Button Driver Abstraction

Without a driver:

```text
Application
    ↓
GPIO Interrupt
    ↓
ISR
    ↓
Event Flag
    ↓
Debounce
    ↓
Application Action
```

With the button driver:

```text
Application
    ↓
isPressed()
    ↓
Button Driver
    ↓
GPIO Interrupt
    ↓
Push Button
```

Therefore, the application only needs:

```cpp
if (btn1.isPressed()) {
    // Application action
}
```

---

## Key Concept

The button driver provides a reusable **interrupt-driven hardware abstraction**:

$$
\boxed{
\text{Application}
\rightarrow
\text{Button Driver}
\rightarrow
\text{GPIO Interrupt}
\rightarrow
\text{Hardware}
}
$$

This separates **hardware-specific interrupt handling** from the main application logic and provides a foundation for more advanced event-driven and RTOS-based embedded systems.
