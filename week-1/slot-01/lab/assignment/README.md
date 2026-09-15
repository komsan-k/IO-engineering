# LED Driver (Arduino)

## 🎯 Objective
Learn how to implement a simple **device driver** for an LED in Arduino by:
1. Writing the basic LED code.  
2. Wrapping it inside a class.  
3. Creating a reusable library (`LED.h` and `LED.cpp`).  
4. Using the driver in applications.  
5. Extending it to handle multiple LEDs with different intervals.  

---

## 1. Basic LED Code

```cpp
#define pin 13

void setup() { 
  pinMode(pin, OUTPUT); 
}

void LEDon() { 
  digitalWrite(pin, HIGH); 
}

void LEDoff() { 
  digitalWrite(pin, LOW); 
}

void LEDtoggle() { 
  LEDon(); 
  delay(250); 
  LEDoff(); 
  delay(250); 
}

void loop() { 
  LEDtoggle(); 
}
```

---

## 2. LED Class Abstraction

```cpp
class LED {
  public:
    LED(int pin);    // Constructor
    void on();       // Turn LED ON
    void off();      // Turn LED OFF
    void toggle();   // Blink LED
    void interval(int p); // Delay/interval

  private:
    int _pin;        // LED pin
};
```

---

## 3. Header File `LED.h`

```cpp
#ifndef LED_h
#define LED_h

#include "Arduino.h"

class LED {
  public:
    LED(int pin);
    void on();
    void off();
    void toggle(int interval = 250);  // toggle with interval
  private:
    int _pin;
};

#endif
```

---

## 4. Source File `LED.cpp`

```cpp
#include "Arduino.h"
#include "LED.h"

LED::LED(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void LED::on() {
  digitalWrite(_pin, HIGH);
}

void LED::off() {
  digitalWrite(_pin, LOW);
}

void LED::toggle(int interval) {
  on();
  delay(interval);
  off();
  delay(interval);
}
```

---

## 5. Application Example

```cpp
#include "LED.h"

// Create LED objects
LED led1(13);
LED led2(12);

void setup() {}

void loop() {
  led1.toggle(200);   // LED1 blinks faster
  led2.toggle(500);   // LED2 blinks slower
}
```

---

## 🧪 Labwork Assignment
1. Implement `toggle(int interval)` (already shown above).  
2. Add two LED components (`led1`, `led2`).  
   - Observe how they blink at different rates.  
   - Each object maintains its own state and timing.  

---

## 📚 References
- [Arduino Library Tutorial](https://www.arduino.cc/en/Hacking/LibraryTutorial)

---

## Exercise — Complete the LED and Button Driver Program

Complete the following program to control two LEDs using two push buttons.

- **Button 1 (GPIO14)** toggles **LED 1 (GPIO2)**.
- **Button 2 (GPIO16)** toggles **LED 2 (GPIO12)**.

```cpp
#include "LED.h"
#include "button.h"

// Create LED objects
LED led1(2);
LED led2(12);

// Create Button objects
button btn1(14);
button btn2(16);

void setup() {
    // Complete the initialization here
}

void loop() {
    // Button 1 toggles LED 1
    if (btn1.isPressed()) {
        led1.toggle();
    }

    // Button 2 toggles LED 2
    if (btn2.isPressed()) {
        led2.toggle();
    }
}
```

### Expected Operation

| Input | Output |
|---|---|
| Press Button 1 | Toggle LED 1 |
| Press Button 2 | Toggle LED 2 |

### Task

Complete the `setup()` function by initializing the **LED** and **button** driver objects.
