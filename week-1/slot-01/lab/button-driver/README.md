# Assignment — ESP32 Button Driver

## 1. Objective

Develop a reusable button driver for the ESP32 that separates hardware input handling from the main application.

The driver should:

1. Initialize a GPIO button input.
2. Support an active-low push button.
3. Read the current button state.
4. Detect button press and release events.
5. Implement software debouncing.

---

## 2. Hardware

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


## 3.Header File `LED.h`

```cpp
#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <Arduino.h>

class ButtonDriver {
private:
    uint8_t pin;
    bool activeLow;

    bool currentState;
    bool previousState;

    unsigned long lastDebounceTime;
    unsigned long debounceDelay;

public:
    ButtonDriver(
        uint8_t buttonPin,
        bool isActiveLow = true,
        unsigned long debounceMs = 50
    );

    void begin();

    bool read();

    bool isPressed();

    bool isReleased();
};

#endif
```

---

## 4. Source File `LED.cpp`

```cpp
#include "button_driver.h"

ButtonDriver::ButtonDriver(
    uint8_t buttonPin,
    bool isActiveLow,
    unsigned long debounceMs
) {
    pin = buttonPin;
    activeLow = isActiveLow;
    debounceDelay = debounceMs;

    currentState = false;
    previousState = false;

    lastDebounceTime = 0;
}

void ButtonDriver::begin() {

    if (activeLow) {
        pinMode(pin, INPUT_PULLUP);
    }
    else {
        pinMode(pin, INPUT_PULLDOWN);
    }

    previousState = read();
}

bool ButtonDriver::read() {

    bool rawState = digitalRead(pin);

    if (activeLow) {
        return rawState == LOW;
    }
    else {
        return rawState == HIGH;
    }
}

bool ButtonDriver::isPressed() {

    bool reading = read();

    if (reading != currentState) {

        if (
            millis() - lastDebounceTime
            >= debounceDelay
        ) {

            previousState = currentState;
            currentState = reading;

            lastDebounceTime = millis();

            if (
                currentState == true &&
                previousState == false
            ) {

                return true;
            }
        }
    }

    return false;
}

bool ButtonDriver::isReleased() {

    bool reading = read();

    if (reading != currentState) {

        if (
            millis() - lastDebounceTime
            >= debounceDelay
        ) {

            previousState = currentState;
            currentState = reading;

            lastDebounceTime = millis();

            if (
                currentState == false &&
                previousState == true
            ) {

                return true;
            }
        }
    }

    return false;
}
```

---

## 5. Application Example

```cpp
#include "button_driver.h"

#define BUTTON_PIN 4
#define LED_PIN    2

ButtonDriver button(BUTTON_PIN);

bool ledState = false;

void setup() {

    Serial.begin(115200);

    pinMode(
        LED_PIN,
        OUTPUT
    );

    button.begin();

    Serial.println(
        "Button Driver Started"
    );
}

void loop() {

    if (button.isPressed()) {

        ledState =
            !ledState;

        digitalWrite(
            LED_PIN,
            ledState
        );

        Serial.println(
            "Button Pressed"
        );

        Serial.print(
            "LED State: "
        );

        Serial.println(
            ledState ? "ON" : "OFF"
        );
    }
}
```

