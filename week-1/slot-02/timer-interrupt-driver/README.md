# Timer-Controlled Interrupt Driver for ESP32

This driver encapsulates the **ESP32 hardware timer, timer interrupt, event flag, and timer control functions** into a reusable `TimerDriver` class.

The application can use simple functions such as:

```cpp
timer1.begin();
timer1.start();
timer1.stop();
timer1.restart();
timer1.isTriggered();
```

---

## Driver Architecture

```text
Application
    ↓
Timer Driver
    ↓
ESP32 Hardware Timer
    ↓
Timer Interrupt
    ↓
ISR
    ↓
Event Flag
    ↓
isTriggered()
    ↓
Application Event
```

---

## Project Structure

Create three files:

```text
project/
├── timer_driver.h
├── timer_driver.cpp
└── main.ino
```

---

## 1. `timer_driver.h`

```cpp
#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <Arduino.h>

class TimerDriver {

private:
    hw_timer_t *timer;

    uint32_t timerFrequency;
    uint64_t interval;

    volatile bool timerEvent;
    bool running;

    // Static ISR helper
    static void ARDUINO_ISR_ATTR isrHandler(void *arg);

public:
    TimerDriver(
        uint64_t intervalUs,
        uint32_t frequency = 1000000
    );

    void begin();

    void start();

    void stop();

    void restart();

    bool isTriggered();

    bool isRunning();

    void setInterval(uint64_t intervalUs);
};

#endif
```

---

## 2. `timer_driver.cpp`

```cpp
#include "timer_driver.h"

// Constructor
TimerDriver::TimerDriver(
    uint64_t intervalUs,
    uint32_t frequency
) {
    timer = NULL;

    timerFrequency = frequency;
    interval = intervalUs;

    timerEvent = false;
    running = false;
}

// Initialize hardware timer
void TimerDriver::begin() {

    // Create timer
    // Default: 1 MHz → 1 tick = 1 us
    timer = timerBegin(timerFrequency);

    // Attach ISR and pass this object
    timerAttachInterruptArg(
        timer,
        isrHandler,
        this
    );

    // Configure auto-reload alarm
    timerAlarm(
        timer,
        interval,
        true,
        0
    );

    // Start in STOPPED state
    timerStop(timer);

    running = false;
}

// Timer ISR
void ARDUINO_ISR_ATTR TimerDriver::isrHandler(void *arg) {

    TimerDriver *timerObj =
        static_cast<TimerDriver *>(arg);

    // Keep ISR short
    timerObj->timerEvent = true;
}

// Start timer
void TimerDriver::start() {

    if (timer != NULL) {

        timerWrite(timer, 0);

        timerStart(timer);

        running = true;
    }
}

// Stop timer
void TimerDriver::stop() {

    if (timer != NULL) {

        timerStop(timer);

        running = false;
    }
}

// Restart timer
void TimerDriver::restart() {

    if (timer != NULL) {

        timerWrite(timer, 0);

        timerStart(timer);

        running = true;
    }
}

// Check timer event
bool TimerDriver::isTriggered() {

    if (timerEvent) {

        timerEvent = false;

        return true;
    }

    return false;
}

// Check timer state
bool TimerDriver::isRunning() {

    return running;
}

// Change timer interval
void TimerDriver::setInterval(uint64_t intervalUs) {

    interval = intervalUs;

    if (timer != NULL) {

        timerAlarm(
            timer,
            interval,
            true,
            0
        );
    }
}
```

---

## 3. Simple `main.ino`

This example toggles an LED every **1 second**.

```cpp
#include "timer_driver.h"

#define LED_PIN 2

// 1,000,000 us = 1 second
TimerDriver timer1(1000000);

bool ledState = false;

void setup() {

    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    timer1.begin();
    timer1.start();

    Serial.println("Timer Driver Started");
}

void loop() {

    if (timer1.isTriggered()) {

        ledState = !ledState;

        digitalWrite(
            LED_PIN,
            ledState
        );

        Serial.println(
            ledState ? "LED ON" : "LED OFF"
        );
    }
}
```

---

## Expected Operation

```text
Start
  ↓
Timer = 1 second
  ↓
Timer Interrupt
  ↓
Event Flag
  ↓
isTriggered()
  ↓
Toggle LED
  ↓
Repeat
```

---

## Expected Serial Monitor Output

```text
Timer Driver Started
LED ON
LED OFF
LED ON
LED OFF
...
```

---

# Timer Control Functions

## `start()`

Start the hardware timer.

```cpp
timer1.start();
```

The timer begins counting from zero and generates periodic interrupts.

---

## `stop()`

Stop the hardware timer.

```cpp
timer1.stop();
```

No timer events are generated while the timer is stopped.

---

## `restart()`

Reset the timer count and start the timer again.

```cpp
timer1.restart();
```

---

## `isTriggered()`

Check whether a timer interrupt has occurred.

```cpp
if (timer1.isTriggered()) {

    // Timer event
}
```

---

## `isRunning()`

Check whether the timer is currently running.

```cpp
if (timer1.isRunning()) {

    Serial.println("Timer is running");
}
```

---

## `setInterval()`

Change the timer interrupt interval.

```cpp
timer1.setInterval(500000);
```

For a 1-MHz timer, this changes the interval to:

```text
500,000 us = 0.5 s
```

---

# Timer Frequency and Interval

The default timer frequency is:

$$
f_{\text{timer}} = 1~\text{MHz}
$$

Therefore:

$$
T_{\text{tick}}
= \frac{1}{f_{\text{timer}}}
= 1~\mu s
$$

For a 1-second timer interval:

$$
N
= 1,000,000~\text{ticks}
$$

Therefore:

```cpp
TimerDriver timer1(1000000);
```

creates a timer with a **1-second event interval**.

---

## Example Timer Intervals

| Time Interval | Microseconds | Code |
|---:|---:|---|
| 0.1 s | 100,000 | `timer1.setInterval(100000);` |
| 0.25 s | 250,000 | `timer1.setInterval(250000);` |
| 0.5 s | 500,000 | `timer1.setInterval(500000);` |
| 1.0 s | 1,000,000 | `timer1.setInterval(1000000);` |
| 2.0 s | 2,000,000 | `timer1.setInterval(2000000);` |

---
<!---
# Combining the Button and Timer Drivers

The previous **Button Interrupt Driver** can be combined with the `TimerDriver`.

The button controls whether the timer is **STARTED** or **STOPPED**.

```text
Button Press #1 → START Timer
Button Press #2 → STOP Timer
Button Press #3 → START Timer
Button Press #4 → STOP Timer
```

---

## Project Structure

```text
project/
│
├── button.h
├── button.cpp
├── timer_driver.h
├── timer_driver.cpp
└── main.ino
```

---

## Combined Example

```cpp
#include "button.h"
#include "timer_driver.h"

#define LED_PIN 2

// Button on GPIO14
Button btn1(14);

// Timer interval = 1 second
TimerDriver timer1(1000000);

bool ledState = false;
bool timerEnabled = false;

unsigned int buttonCount = 0;

void setup() {

    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize drivers
    btn1.begin();
    timer1.begin();

    Serial.println("System Started");
    Serial.println("Timer = STOPPED");
}

void loop() {

    // ---------------- Button Event ----------------

    if (btn1.isPressed()) {

        buttonCount++;

        Serial.print("Button Count = ");
        Serial.println(buttonCount);

        // Toggle timer state
        timerEnabled = !timerEnabled;

        if (timerEnabled) {

            timer1.start();

            Serial.println("Timer = STARTED");
        }
        else {

            timer1.stop();

            ledState = false;
            digitalWrite(LED_PIN, LOW);

            Serial.println("Timer = STOPPED");
        }
    }

    // ---------------- Timer Event ----------------

    if (timer1.isTriggered()) {

        ledState = !ledState;

        digitalWrite(
            LED_PIN,
            ledState
        );
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

Button Count = 1
Timer = STARTED

Button Count = 2
Timer = STOPPED

Button Count = 3
Timer = STARTED

Button Count = 4
Timer = STOPPED
```

---

# Combined Driver Architecture

```text
                    Application
                   /           \
                  /             \
         Button Driver       Timer Driver
              │                   │
              ▼                   ▼
       GPIO Interrupt       Timer Interrupt
              │                   │
              ▼                   ▼
         Push Button          Timer Event
                                  │
                                  ▼
                              LED Control
```

The application does not need to directly manage the low-level interrupt configuration.

Instead, it uses:

```cpp
if (btn1.isPressed()) {

    timer1.start();
}
```

and:

```cpp
if (timer1.isTriggered()) {

    // Perform periodic task
}
```

---

# Key Concept

The driver-based architecture separates the application from hardware-specific interrupt handling:

$$
\boxed{
\text{Application}
\rightarrow
\text{Driver Abstraction}
\rightarrow
\text{Interrupt Hardware}
}
$$

For this application:

```text
Button Hardware
      ↓
Button Driver
      ↓
Button Event
      ↓
Application
      ↓
START / STOP
      ↓
Timer Driver
      ↓
Hardware Timer
      ↓
Timer Event
      ↓
LED Control
```

This structure provides a foundation for developing more advanced **event-driven, multitasking, and RTOS-based embedded applications**.

--->
