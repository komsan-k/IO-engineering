# Lab — ESP32 Light Sleep Using Timer Wake-Up

## Objective

In this lab, students will use the ESP32 **Light Sleep** mode to reduce power consumption and wake the processor automatically using a timer.

Students will learn how to:

- configure a timer wake-up source;
- enter Light Sleep mode;
- wake the ESP32 after a fixed time;
- observe that program execution continues after waking.

---

## Required Library

```cpp
#include <esp_sleep.h>
```

---

## Complete ESP32 Program

```cpp
#include <esp_sleep.h>

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println(
    "ESP32 Light Sleep Example"
  );
}

void loop()
{
  Serial.println(
    "ESP32 is awake"
  );

  // Wake up after 5 seconds
  esp_sleep_enable_timer_wakeup(
    5 * 1000000ULL
  );

  Serial.println(
    "Entering Light Sleep..."
  );

  delay(100);

  // Enter Light Sleep
  esp_light_sleep_start();

  // Program continues here
  // after waking up

  Serial.println(
    "ESP32 woke up"
  );

  Serial.println();

  delay(1000);
}
```

---

## Program Flow

```text
ESP32 Awake
   |
   v
Enable Timer Wake-Up
   |
   v
Enter Light Sleep
   |
   | 5 seconds
   v
Wake Up
   |
   v
Continue Program
   |
   v
Repeat
```

---

## Important Commands

Set the wake-up timer:

```cpp
esp_sleep_enable_timer_wakeup(
  5 * 1000000ULL
);
```

The value is in microseconds.

```text
5 seconds
=
5,000,000 microseconds
```

Enter Light Sleep:

```cpp
esp_light_sleep_start();
```

After waking, the ESP32 continues from the line immediately after:

```cpp
esp_light_sleep_start();
```

---

## How to Run

1. Upload the program to the ESP32.
2. Open the Serial Monitor.
3. Set the baud rate to:

```text
115200
```

4. Observe the sleep and wake cycle.

---

## Expected Serial Monitor

```text
ESP32 Light Sleep Example

ESP32 is awake
Entering Light Sleep...

ESP32 woke up

ESP32 is awake
Entering Light Sleep...

ESP32 woke up
```

---

## Experiment 1 — Change Sleep Time

Change:

```cpp
5 * 1000000ULL
```

to:

```cpp
10 * 1000000ULL
```

The ESP32 should now wake after approximately 10 seconds.

---

## Experiment 2 — Add a Counter

Create a counter:

```cpp
int wakeCount = 0;
```

After waking:

```cpp
wakeCount++;

Serial.print(
  "Wake Count: "
);

Serial.println(
  wakeCount
);
```

Observe that the counter continues increasing because Light Sleep does not restart the program.

---

## Checkpoint Questions

1. What is Light Sleep?
2. What function enables timer wake-up?
3. What unit is used by `esp_sleep_enable_timer_wakeup()`?
4. What function starts Light Sleep?
5. Where does the program continue after waking?
6. What happens if the timer is changed from 5 seconds to 10 seconds?
7. How is Light Sleep different from restarting the ESP32?

---

## Simple Assignment

Modify the program so the ESP32:

1. stays awake for 2 seconds;
2. enters Light Sleep for 8 seconds;
3. wakes up;
4. prints a wake counter;
5. repeats continuously.

The target behavior is:

```text
Awake
  |
  | 2 seconds
  v
Light Sleep
  |
  | 8 seconds
  v
Wake Up
  |
  v
Wake Counter + 1
  |
  v
Repeat
```
