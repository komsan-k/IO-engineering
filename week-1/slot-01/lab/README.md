# LAB 1 — Embedded Programming for Device Drivers

## 1. Objectives
By the end of this lab, you will be able to:
1. Understand the role of device drivers in embedded systems.
2. Implement basic device drivers for GPIO input/output.
3. Use modular programming to separate driver code from application logic.
4. Test and verify device driver functionality on embedded hardware.

---

## 2. Background

A **device driver** is a software component that enables the operating system or firmware to communicate with hardware devices. In embedded systems, drivers are usually written in **C** and can interact directly with microcontroller registers or via a Hardware Abstraction Layer (HAL).

**Examples of device drivers:**
- GPIO control (LEDs, buttons)
- Serial communication (UART, SPI, I²C)
- Timers and interrupts
- ADC/DAC interfaces

**Key Concepts:**
- **Register-level programming:** Direct manipulation of hardware registers.
- **HAL-based programming:** Using manufacturer-provided APIs for portability.

---

## 3. Lab Setup

**Hardware:**
- ESP32 DevKit board
- Onboard LED (GPIO2 for ESP32 or GPIO13 for STM32 Nucleo)
- Push button (optional, for input driver)
- USB cable

**Software:**
- Arduino IDE (for ESP32) or STM32CubeIDE (for STM32)
- Embedded C compiler/toolchain

---

## 4. Lab Tasks

### Task 1 — LED Output Driver
Create a reusable LED driver:
```cpp
void led_init(int pin) {
    pinMode(pin, OUTPUT);
}

void led_on(int pin) {
    digitalWrite(pin, HIGH);
}

void led_off(int pin) {
    digitalWrite(pin, LOW);
}

void setup() {
    led_init(2); // GPIO2 for ESP32
}

void loop() {
    led_on(2);
    delay(500);
    led_off(2);
    delay(500);
}
```

---
# Task 2 — LED Output Driver with pthreads

This task demonstrates how to use **POSIX threads (pthreads)** to create a simple **multi-threaded LED driver**.  
Each thread independently toggles an LED, running concurrently.

---

## Code Example

```cpp
#include <pthread.h>
#include <Arduino.h>   // required for pinMode, digitalWrite, delay

#define LED1 2
#define LED2 12

// Thread function for LED1
void *ThreadFunc1(void *threadid) {
   while (1) {
      
      /**** LED1 toggles ****/

   return NULL;
}

// Thread function for LED2
void *ThreadFunc2(void *threadid) {
   while (1) {

      /**** LED2 toggles ****/

   }
   return NULL;
}

void setup() {

   // Create pthreads for each LED task
   pthread_t thLED1, thLED2;
   pthread_create(&thLED1, NULL, ThreadFunc1, NULL);
   pthread_create(&thLED2, NULL, ThreadFunc2, NULL);
}

void loop() {
   // Empty: work is handled in threads
}
```

---


### Task 3 — Button Input Driver
Create a reusable button driver:
```cpp
int button_read(int pin) {
    return digitalRead(pin);
}

void setup() {
    pinMode(2, OUTPUT); // LED
    pinMode(4, INPUT_PULLUP); // Button
}

void loop() {
    if (button_read(4) == LOW) {
        digitalWrite(2, HIGH);
    } else {
        digitalWrite(2, LOW);
    }
}
```

---

### Task 4 — Modular Driver Structure
Separate drivers into `.h` and `.c` files:
- `led_driver.h` / `led_driver.c`
- `button_driver.h` / `button_driver.c`

Include in your main program for cleaner, reusable code.

---

## 5. Expected Output
- **LED Driver:** LED blinks at defined intervals.
- **Button Driver:** LED turns ON only when button is pressed.
- **Driver Files:** Independent, reusable modules.

---

## 6. Discussion Questions
1. What are the advantages of modular driver design?
2. When would you choose HAL over register-level programming?
3. How would you handle debounce in a button driver?

---

## 7. Exercises
1. Implement a PWM LED driver.
2. Add debounce logic to the button driver.
3. Use interrupts for button handling.

---

## 8. Conclusion
This lab covered the basics of embedded programming for device drivers, focusing on GPIO-based LED and button drivers. Modular programming practices were introduced to make code reusable and maintainable.

