# 📘 Theory: Polling, Interrupts, ISRs, Debouncing, and Hardware Timers
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Embedded systems continuously interact with the physical world through sensors, switches, communication interfaces, and actuators. A microcontroller must therefore determine **when an external or internal event occurs** and execute the appropriate response.

Two fundamental techniques are used:

- **Polling** — the CPU repeatedly checks whether an event has occurred.
- **Interrupts** — hardware notifies the CPU when an event occurs.

The ESP32 supports both approaches and also provides **hardware timers** for precise periodic or one-shot events.

A typical embedded event-processing architecture can be represented as

$$
\boxed{
\text{Event}
\rightarrow
\text{Detection}
\rightarrow
\text{Processing}
\rightarrow
\text{Response}
}
$$

For example:

```text
Push Button
    │
    ▼
GPIO Input
    │
    ▼
Event Detection
    │
    ▼
ESP32 CPU
    │
    ▼
LED / Counter / Control Action
```

This chapter introduces **polling, hardware interrupts, interrupt service routines (ISRs), switch debouncing, and hardware timers** using the ESP32 as the MCU case study.

---

## 🧠 2. Polling

### 2.1 Polling Concept

Polling is a software technique in which the CPU repeatedly checks the status of a peripheral or input.

For example:

```cpp
if (digitalRead(BUTTON_PIN) == LOW)
{
    // Button pressed
}
```

When placed inside `loop()`, the CPU repeatedly reads the button.

```text
       ┌───────────────┐
       │               │
       ▼               │
Read Button            │
    │                  │
    ▼                  │
Pressed?               │
 ┌──┴──┐               │
Yes    No              │
 │      │              │
 ▼      │              │
Process │              │
 │      │              │
 └──────┴──────────────┘
```

The basic polling algorithm is

$$
\boxed{
\text{Read}
\rightarrow
\text{Check}
\rightarrow
\text{Process}
\rightarrow
\text{Repeat}
}
$$

---

### 2.2 ESP32 Polling Example

Consider a push button connected to GPIO 4 and an LED connected to GPIO 2.

```cpp
#define LED_PIN     2
#define BUTTON_PIN  4

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
    int buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }
}
```

Because `INPUT_PULLUP` is used,

$$
\text{Released}=HIGH
$$

and

$$
\text{Pressed}=LOW.
$$

---

## ⏱️ 3. Polling Rate and Response Time

Suppose the program checks an input once every

$$
T_p
$$

seconds.

The polling frequency is

$$
f_p=\frac{1}{T_p}.
$$

For example, if

$$
T_p=10~ms,
$$

then

$$
f_p=\frac{1}{0.01}=100~Hz.
$$

The event response can be delayed until the next polling operation.

Therefore,

$$
0\leq T_{\text{response}}\lesssim T_p
$$

before considering additional processing delays.

A shorter polling interval generally provides faster response but causes the CPU to check the input more frequently.

---

## 📊 4. Advantages and Limitations of Polling

| Characteristic | Polling |
|---|---|
| Implementation | Simple |
| Debugging | Easy |
| CPU usage | Can be high |
| Event latency | Depends on polling interval |
| Hardware support | Minimal |
| Low-power operation | Less efficient |
| Multiple events | Becomes harder as system grows |
| Suitable for | Simple or slowly changing inputs |

Polling works well for:

- simple push buttons,
- slowly changing sensors,
- introductory MCU programs,
- applications where precise event timing is unnecessary.

---

## ⚡ 5. Interrupts

### 5.1 Interrupt Concept

An **interrupt** allows hardware to signal the CPU when an event occurs.

Instead of repeatedly asking whether an event has occurred, the CPU performs other work until hardware reports the event.

```text
Normal Program
     │
     │
     │     Event
     │       │
     │       ▼
     │   Interrupt
     │       │
     └──────►│
             ▼
            ISR
             │
             ▼
          Return
             │
             ▼
      Normal Program
```

Conceptually,

$$
\boxed{
\text{Event}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
\rightarrow
\text{Return}
}
$$

---

## 🔔 6. Interrupt Sources

An ESP32 can respond to many types of hardware events, including:

- GPIO transitions,
- hardware timers,
- UART activity,
- ADC-related events,
- communication peripherals,
- internal peripheral events.

For a push button, a GPIO transition can generate an interrupt.

For example:

$$
HIGH\rightarrow LOW
$$

can represent a button press when an internal pull-up is used.

---

## 🧠 7. Interrupt Service Routine

An **Interrupt Service Routine (ISR)** is a function executed in response to an interrupt.

The sequence is approximately

```text
Main Program
     │
     ▼
Interrupt Occurs
     │
     ▼
Suspend Normal Execution
     │
     ▼
Execute ISR
     │
     ▼
Finish ISR
     │
     ▼
Resume Normal Execution
```

The ISR should generally be:

- short,
- deterministic,
- non-blocking,
- limited to essential work.

A useful design principle is

$$
\boxed{
\text{ISR =}
\text{Detect}
+
\text{Record}
+
\text{Exit}
}
$$

rather than performing lengthy application processing inside the ISR.

---

## 💻 8. ESP32 GPIO Interrupt Example

Using the Arduino ESP32 environment:

```cpp
#define BUTTON_PIN 4

volatile bool buttonPressed = false;

void IRAM_ATTR buttonISR()
{
    buttonPressed = true;
}

void setup()
{
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

void loop()
{
    if (buttonPressed)
    {
        buttonPressed = false;

        Serial.println(
            "Button interrupt detected"
        );
    }
}
```

The ISR does only one important operation:

```cpp
buttonPressed = true;
```

The main program performs the actual processing.

---

## 🔍 9. `IRAM_ATTR`

ESP32 Arduino examples often declare an ISR using:

```cpp
void IRAM_ATTR buttonISR()
```

`IRAM_ATTR` requests placement of the function in instruction RAM, which can be important for interrupt code that must remain executable under conditions where flash access is unavailable or constrained.

For introductory programming, the important pattern is:

```cpp
void IRAM_ATTR ISR_Name()
{
    // Minimal ISR operations
}
```

---

## 🔄 10. Interrupt Trigger Modes

GPIO interrupts can respond to different signal conditions.

| Mode | Meaning |
|---|---|
| `RISING` | LOW → HIGH |
| `FALLING` | HIGH → LOW |
| `CHANGE` | Either transition |
| `HIGH` | High-level condition, where supported |
| `LOW` | Low-level condition, where supported |

For an active-low push button using `INPUT_PULLUP`, the press event is typically

$$
HIGH\rightarrow LOW.
$$

Therefore,

```cpp
FALLING
```

is a natural trigger choice.

---

## 🆚 11. Polling vs. Interrupts

### Polling

$$
\boxed{
\text{CPU asks hardware}
}
$$

### Interrupt

$$
\boxed{
\text{Hardware notifies CPU}
}
$$

| Characteristic | Polling | Interrupt |
|---|---|---|
| Event detection | CPU checks | Hardware signals CPU |
| CPU utilization | Potentially higher | Usually lower between events |
| Programming complexity | Low | Moderate |
| Response | Depends on polling rate | Usually faster |
| Event handling | Sequential checking | Event-driven |
| Low-power suitability | Lower | Better |
| Debugging | Easier | More challenging |
| Timing considerations | Simple | ISR timing important |
| Best application | Slow/simple events | Asynchronous/urgent events |

---

## 📌 12. When to Use Polling

Polling is appropriate when:

- events occur slowly,
- the system is simple,
- the input must be checked periodically anyway,
- timing requirements are relaxed,
- only a few inputs exist.

Example:

```text
Temperature Sensor
      │
      ▼
Read every 1 second
```

---

## ⚡ 13. When to Use Interrupts

Interrupts are appropriate when:

- events are asynchronous,
- fast response is important,
- events occur infrequently,
- the CPU has other work to perform,
- event timing must be captured accurately.

Examples include:

```text
Push Button
Encoder
Timer Expiration
UART Event
Pulse Counter
External Sensor Alert
```

---

## 🚨 14. Rules for Writing an ISR

An ISR should generally avoid:

- long delays,
- lengthy loops,
- large calculations,
- unnecessary printing,
- blocking operations,
- waiting for other events.

Instead:

```cpp
void IRAM_ATTR ISR()
{
    eventFlag = true;
}
```

Then:

```cpp
void loop()
{
    if (eventFlag)
    {
        eventFlag = false;

        // Perform larger task here
    }
}
```

This separates

$$
\boxed{
\text{Real-Time Event Detection}
}
$$

from

$$
\boxed{
\text{Application Processing}
}
$$

---

## 🔐 15. Shared Variables and `volatile`

A variable modified inside an ISR and read by normal program code is commonly declared `volatile`.

```cpp
volatile bool eventFlag = false;
```

This tells the compiler that the variable can change outside the ordinary sequential flow of the program.

However, `volatile` alone does **not** provide full synchronization or atomicity for every shared-data operation.

---

## 🔘 16. Mechanical Switch Debouncing

A mechanical push button does not normally generate a perfect digital transition.

An ideal button press would be

```text
HIGH ─────────────┐
                  │
                  └──────── LOW
```

A real mechanical switch may produce

```text
HIGH ──────┐ ┌─┐ ┌──┐
           └─┘ └─┘  └──── LOW
```

Thus, one physical press can generate multiple transitions.

This phenomenon is called **switch bounce** or **contact bounce**.

---

## ⚠️ 17. Debouncing and Interrupts

Switch bounce becomes particularly important with interrupts.

One physical press might produce:

```text
Physical press
     │
     ├── Interrupt 1
     ├── Interrupt 2
     ├── Interrupt 3
     └── Interrupt 4
```

Therefore,

$$
\boxed{
\text{Mechanical Button}
+
\text{Interrupt}
\Rightarrow
\text{Debouncing Required}
}
$$

---

## 🛠️ 18. Software Debouncing

One simple polling-based method is:

```cpp
if (digitalRead(BUTTON_PIN) == LOW)
{
    delay(20);

    if (digitalRead(BUTTON_PIN) == LOW)
    {
        // Confirmed button press
    }
}
```

The delay allows the electrical signal to stabilize.

---

## ⏱️ 19. Time-Based Interrupt Debouncing

For interrupt-driven applications, avoid a blocking `delay()` inside the ISR.

Instead, reject events that occur too close together.

$$
\Delta t =
t_{\text{current-}}
t_{\text{previous}}.
$$

Accept the event only if

$$
\Delta t>T_D
$$

where $T_D$ is the selected debounce interval.

---

## 💻 20. Interrupt Debounce Example

```cpp
#define BUTTON_PIN 4

volatile bool buttonEvent = false;
volatile unsigned long lastInterruptTime = 0;

void IRAM_ATTR buttonISR()
{
    unsigned long currentTime = millis();

    if (
        currentTime - lastInterruptTime
        > 50
    )
    {
        buttonEvent = true;
        lastInterruptTime = currentTime;
    }
}

void setup()
{
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

void loop()
{
    if (buttonEvent)
    {
        buttonEvent = false;

        Serial.println(
            "Valid Button Press"
        );
    }
}
```

A more robust design can let the ISR only set a flag and perform the complete debounce logic outside the ISR.

---

## 🔧 21. Hardware Debouncing

Debouncing can also be implemented using hardware.

```text
Button
  │
  ├──── R ──── GPIO
  │             │
  │             C
  │             │
 GND           GND
```

The RC time constant is

$$
\tau=RC.
$$

For example, if

$$
R=10~k\Omega
$$

and

$$
C=1~\mu F,
$$

then

$$
\tau =
10\,000
\times
1\times10^{-6}=
0.01~s =
10~ms.
$$

---

## 📊 22. Software vs. Hardware Debouncing

| Characteristic | Software | Hardware |
|---|---|---|
| Extra components | No | Yes |
| Cost | Low | Higher |
| Flexibility | High | Lower |
| Modification | Easy | Requires hardware change |
| CPU involvement | Yes | Minimal |
| Noise filtering | Algorithm-dependent | Can be strong |
| Typical use | General MCU applications | Robust/noisy interfaces |

---

## ⏲️ 23. Hardware Timers

A **hardware timer** is a peripheral that counts clock-derived events independently of normal CPU instruction execution.

```text
Clock
  │
  ▼
Prescaler / Divider
  │
  ▼
Timer Counter
  │
  ▼
Compare / Alarm
  │
  ▼
Interrupt
  │
  ▼
ISR
```

Hardware timers are useful for:

- periodic sampling,
- control loops,
- precise scheduling,
- timeout generation,
- periodic interrupts,
- waveform/event timing.

---

## 🧠 24. Timer Principle

Suppose the timer receives a clock frequency

$$
f_{clk}.
$$

After division by factor $D$,

$$
f_{timer}
=
\frac{f_{clk}}{D}.
$$

The timer tick period is

$$
T_{tick}
=
\frac{1}{f_{timer}}
=
\frac{D}{f_{clk}}.
$$

If an event occurs after $N$ timer ticks,

$$
T_{event}
=
NT_{tick}.
$$

Therefore,

$$
\boxed{
T_{event}
=
N\frac{D}{f_{clk}}
}
$$

---

## ⚙️ 25. Timer Resolution

Suppose a timer increments once every

$$
1~\mu s.
$$

Then

$$
f_{timer}=1~MHz.
$$

To generate a 1-second event:

$$
N
=
\frac{1~s}
{1~\mu s}
=
1,000,000.
$$

Therefore,

$$
\boxed{
1,000,000~\text{ticks}
=
1~\text{second}
}
$$

---

## 🔔 26. Timer Interrupt

A hardware timer can generate an interrupt when its configured alarm value is reached.

```text
Hardware Clock
     │
     ▼
Timer Counter
     │
     ▼
Timer Alarm
     │
     ▼
Interrupt
     │
     ▼
Timer ISR
     │
     ▼
Application Event
```

Thus,

$$
\boxed{
\text{Timer Expiration}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
}
$$

---

## 💻 27. ESP32 Arduino Hardware Timer Concept

The exact Arduino-ESP32 timer API depends on the installed core version.

A general structure is:

```cpp
volatile bool timerEvent = false;

void IRAM_ATTR onTimer()
{
    timerEvent = true;
}

void setup()
{
    // Configure hardware timer
    // Attach onTimer() to timer interrupt
    // Configure timer period
    // Enable timer
}

void loop()
{
    if (timerEvent)
    {
        timerEvent = false;

        // Periodic application task
    }
}
```

The important architecture is

$$
\boxed{
\text{Timer}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
\rightarrow
\text{Event Flag}
\rightarrow
\text{Application}
}
$$

---

## 💡 28. Timer-Controlled LED

Instead of using:

```cpp
digitalWrite(LED_PIN, HIGH);
delay(500);

digitalWrite(LED_PIN, LOW);
delay(500);
```

a hardware timer can periodically generate an event.

```text
Hardware Timer
     │
     ▼
Timer Interrupt
     │
     ▼
Set Flag
     │
     ▼
Main Program
     │
     ▼
Toggle LED
```

This allows the CPU to perform other tasks between timer events.

---

## 🆚 29. `delay()`, `millis()`, and Hardware Timer

| Method | Type | CPU Blocking | Precision | Complexity |
|---|---|:---:|---|---|
| `delay()` | Software delay | Yes | Moderate | Very low |
| `millis()` | Software time check | No | Moderate | Low |
| Hardware timer | Peripheral-based | No | High | Moderate |

For simple applications, `delay()` may be sufficient.

For multiple concurrent software activities, `millis()` is generally more appropriate.

For precise periodic timing, a hardware timer is often preferred.

---

## 🔄 30. Blocking Timing

```cpp
delay(1000);
```

causes the application's normal flow to wait before proceeding.

```text
Task
 │
 ▼
delay(1000)
 │
 │ Application waits
 │
 ▼
Continue
```

---

## ⚡ 31. Non-Blocking Timing

Using `millis()`:

```cpp
unsigned long previousTime = 0;

void loop()
{
    unsigned long currentTime = millis();

    if (currentTime - previousTime >= 1000)
    {
        previousTime = currentTime;

        // Periodic action
    }

    // Other processing continues here
}
```

The application can continue processing:

```text
Check Timer
    │
    ├── Read Button
    ├── Read Sensor
    ├── Update LED
    ├── Process Communication
    └── Repeat
```

---

## 🧠 32. Timer vs. Interrupt

A timer and an interrupt are related but not identical.

A **timer** measures or counts time.

An **interrupt** notifies the CPU about an event.

A timer can therefore be an interrupt source:

$$
\boxed{
\text{Timer Expiration}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
}
$$

GPIO hardware can also be an interrupt source:

$$
\boxed{
\text{GPIO Edge}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
}
$$

---

## 🏗️ 33. Event-Driven Embedded Architecture

Combining GPIO interrupts and hardware timers produces an event-driven system:

```text
          ┌── Push Button
          │       │
          │       ▼
          │   GPIO Interrupt
          │
          │
ESP32 CPU ◄── Timer Interrupt
          │
          │
          ├── Sensor Event
          │
          └── Communication Event
```

The general model becomes

$$
\boxed{
\text{Event Sources}
\rightarrow
\text{Interrupt Controller}
\rightarrow
\text{CPU}
\rightarrow
\text{ISR}
\rightarrow
\text{Application}
}
$$

---

## 🚦 34. Interrupt Latency

**Interrupt latency** is the delay between the occurrence of an interrupt-generating event and the start of its ISR.

$$
T_{latency}
=
t_{\text{ISR start}}
-
t_{\text{event}}.
$$

Latency can be influenced by:

- interrupt priority,
- current critical sections,
- other interrupts,
- CPU state,
- memory/cache conditions,
- framework and RTOS behavior.

---

## ⏳ 35. ISR Execution Time

If an ISR begins at $t_s$ and finishes at $t_f$,

$$
T_{ISR}=t_f-t_s.
$$

Long ISRs can interfere with other time-sensitive processing.

Therefore,

$$
\boxed{
T_{ISR}\ll T_{\text{event interval}}
}
$$

is generally desirable.

---

## ⚠️ 36. Interrupt Overload

Suppose interrupts arrive at frequency

$$
f_i.
$$

The event interval is

$$
T_i=\frac{1}{f_i}.
$$

If

$$
T_{ISR}\geq T_i,
$$

the interrupt workload may become unsustainable.

This is one reason ISRs should be short.

---

## 🔐 37. Critical Sections and Shared Data

Both the ISR and main application may access the same variable.

For example:

```cpp
volatile int pulseCount = 0;
```

ISR:

```cpp
void IRAM_ATTR pulseISR()
{
    pulseCount++;
}
```

Main program:

```cpp
Serial.println(pulseCount);
```

The shared-data structure is

```text
Main Program ───┐
                ├── Shared Data
ISR ────────────┘
```

More complex shared data can require synchronization or critical sections.

---

## 🧩 38. Event Flag Design Pattern

One simple interrupt pattern uses an event flag.

```cpp
volatile bool eventFlag = false;

void IRAM_ATTR ISR()
{
    eventFlag = true;
}
```

Main application:

```cpp
void loop()
{
    if (eventFlag)
    {
        eventFlag = false;

        processEvent();
    }
}
```

This provides the separation

$$
\boxed{
\text{ISR}
\rightarrow
\text{Signal Event}
}
$$

and

$$
\boxed{
\text{Main Program}
\rightarrow
\text{Process Event}
}
$$

---

## 📦 39. Event Counter Design Pattern

A Boolean flag only indicates that at least one event occurred.

If multiple events may occur before the main program responds, use a counter.

```cpp
volatile unsigned int eventCount = 0;

void IRAM_ATTR ISR()
{
    eventCount++;
}
```

Conceptually,

$$
N_{events}
=
N_{events}+1.
$$

Care is still required for:

- concurrency,
- overflow,
- atomic access.

---

## 🔄 40. Polling and Interrupt Hybrid Design

Real embedded systems often combine polling and interrupts.

```text
Push Button ─────► Interrupt
Temperature ─────► Poll every 1 s
UART ────────────► Interrupt/Event
LED ─────────────► Main Program
Timer ───────────► Interrupt
```

The appropriate method should be selected according to

$$
\boxed{
\text{Event Rate}
+
\text{Timing Requirement}
+
\text{CPU Cost}
+
\text{System Complexity}
}
$$

---

## 📊 41. Example Selection

| Application | Recommended Approach |
|---|---|
| Temperature every 5 s | Polling / scheduled read |
| Push-button press | Polling or interrupt |
| High-speed encoder | Interrupt / hardware peripheral |
| Periodic control loop | Hardware timer |
| Simple LED blink | `delay()` or `millis()` |
| Multitask LED blink | `millis()` or timer |
| External alarm signal | Interrupt |
| High-rate pulse measurement | Dedicated hardware peripheral / interrupt |

---

## 🧠 42. ESP32 Application Example

Consider a smart embedded controller containing:

- push button,
- LED,
- temperature sensor,
- periodic timer,
- serial communication.

A possible architecture is:

```text
                  ESP32
                    │
       ┌────────────┼────────────┐
       │            │            │
       ▼            ▼            ▼
Push Button      Timer       Temperature
 Interrupt      Interrupt      Polling
       │            │            │
       └────────────┼────────────┘
                    │
                    ▼
             Application Logic
                    │
              ┌─────┴─────┐
              ▼           ▼
             LED        Serial
```

This demonstrates that polling, interrupts, and timers are complementary techniques.

---

## 🔬 43. Example: Button Interrupt + Timer

A useful application can perform two independent functions:

1. Toggle an LED when a button is pressed.
2. Generate a periodic sensor-sampling event.

```text
Button
  │
  ▼
GPIO Interrupt
  │
  ▼
Button ISR
  │
  ▼
buttonEvent
  │
  └───────────────┐
                  ▼
              Main Loop
                  ▲
  ┌───────────────┘
  │
timerEvent
  ▲
  │
Timer ISR
  ▲
  │
Hardware Timer
```

---

## ⚙️ 44. Recommended ISR Architecture

```text
Hardware Event
     │
     ▼
Interrupt
     │
     ▼
Short ISR
     │
     ▼
Flag / Counter / Queue
     │
     ▼
Main Application / Task
     │
     ▼
Complex Processing
```

This can be summarized as

$$
\boxed{
E
\rightarrow
I
\rightarrow
ISR
\rightarrow
Q
\rightarrow
P
}
$$

where:

- $E$ = hardware event,
- $I$ = interrupt,
- $ISR$ = interrupt service routine,
- $Q$ = event notification/storage,
- $P$ = application processing.

---

## 🆚 45. Complete Comparison

| Feature | Polling | GPIO Interrupt | Hardware Timer |
|---|---|---|---|
| Trigger | Software check | GPIO event | Clock/time |
| CPU continuously checks? | Yes | No | No |
| Asynchronous events | Limited by poll rate | Excellent | Time-based |
| Timing precision | Loop dependent | Event dependent | High |
| Complexity | Low | Moderate | Moderate |
| ISR required | No | Usually | Usually for interrupt-driven use |
| Typical application | Sensor/button | Button/encoder/alarm | Sampling/control |
| Power efficiency | Lower | Better | Better |
| Real-time suitability | Limited | Good | Good |

---

## 💬 46. Discussion Questions

1. What is polling?
2. What is an interrupt?
3. How does polling differ from interrupt-driven processing?
4. What is an ISR?
5. Why should an ISR be short?
6. What does `IRAM_ATTR` indicate in ESP32 Arduino programs?
7. What is the purpose of `volatile`?
8. Does `volatile` automatically make all shared-data operations thread-safe?
9. What is switch bounce?
10. Why can switch bounce cause multiple interrupts?
11. What is software debouncing?
12. What is hardware debouncing?
13. What does an RC debounce circuit do?
14. What is a hardware timer?
15. What is the difference between a timer and an interrupt?
16. What is interrupt latency?
17. Why can long ISRs create problems?
18. When should polling be preferred?
19. When should interrupts be preferred?
20. Why might a system use both polling and interrupts?

---

## 🧠 47. Exercises

### Exercise 1 — Polling

Create a program that reads a push button using:

```cpp
digitalRead()
```

and turns an LED ON while the button is pressed.

### Exercise 2 — GPIO Interrupt

Replace polling with:

```cpp
attachInterrupt()
```

and toggle the LED whenever the button is pressed.

### Exercise 3 — Debouncing

Add software debouncing so that

$$
\boxed{
1~\text{physical press}
=
1~\text{software event}
}
$$

### Exercise 4 — Event Counter

Use an interrupt to count button presses:

```text
Press 1 → Count = 1
Press 2 → Count = 2
Press 3 → Count = 3
```

Display the result in Serial Monitor.

### Exercise 5 — Hardware Timer

Configure an ESP32 hardware timer to generate a periodic event and toggle an LED.

### Exercise 6 — Hybrid System

Create a system where:

```text
Button → GPIO Interrupt → Toggle LED

Timer → Timer Interrupt → Sample Sensor

Main Loop → Display Results
```

---

## 🚀 48. Advanced Exercise

Design a small event-driven ESP32 system with:

- **GPIO 4** — push button,
- **GPIO 2** — LED,
- **GPIO interrupt** — button detection,
- **debouncing** — reject false transitions,
- **hardware timer** — periodic event,
- **Serial Monitor** — display event count.

The architecture should follow:

```text
                  ┌── Button
                  │
                  ▼
             GPIO Interrupt
                  │
                  ▼
                 ISR
                  │
                  ▼
              Event Flag
                  │
                  │
Hardware Timer ───┤
      │           │
      ▼           │
 Timer Interrupt  │
      │           │
      ▼           │
   Timer ISR      │
      │           │
      └─────┬─────┘
            ▼
        Main Loop
            │
       ┌────┼────┐
       ▼    ▼    ▼
      LED Counter Serial
```

---

## 🧾 49. Expected Learning Outcomes

After studying this theory, students should be able to:

- Explain polling and interrupt-driven processing.
- Compare polling with hardware interrupts.
- Explain how an ESP32 responds to GPIO events.
- Describe the role of an ISR.
- Apply basic ISR design rules.
- Explain `volatile` in ISR-shared variables.
- Identify switch-bounce problems.
- Implement software and hardware debounce concepts.
- Explain hardware timer operation.
- Calculate timer periods from clock, divider, and count values.
- Explain interrupt latency and ISR execution time.
- Select polling, interrupts, or timers for different applications.
- Design a simple event-driven ESP32 application.

---

## 📘 50. References

1. Espressif Systems, *ESP32 Technical Reference Manual*.
2. Espressif Systems, *ESP32 Series Datasheet*.
3. Espressif Systems, *ESP-IDF Programming Guide*.
4. Espressif Systems, *Arduino Core for ESP32 Documentation*.
5. Arduino, *Arduino Language Reference*.
6. J. W. Valvano, *Embedded Systems: Real-Time Interfacing to ARM Cortex-M Microcontrollers*.
7. M. Barr and A. Massa, *Programming Embedded Systems*, O'Reilly.
8. D. E. Simon, *An Embedded Software Primer*, Addison-Wesley.

---

## 🔑 51. Key Concepts

### Polling

$$
\boxed{
\text{CPU}
\rightarrow
\text{Check Event}
\rightarrow
\text{Process}
\rightarrow
\text{Repeat}
}
$$

### Interrupt

$$
\boxed{
\text{Hardware Event}
\rightarrow
\text{Interrupt}
\rightarrow
\text{ISR}
}
$$

### Debouncing

$$
\boxed{
\text{Noisy Switch Transitions}
\rightarrow
\text{Debounce}
\rightarrow
\text{Single Valid Event}
}
$$

### Hardware Timer

$$
\boxed{
\text{Clock}
\rightarrow
\text{Timer}
\rightarrow
\text{Alarm}
\rightarrow
\text{Interrupt}
}
$$

Together, these concepts form the basis of an **event-driven embedded system**:

$$
\boxed{
\text{Inputs / Timers}
\rightarrow
\text{Event Detection}
\rightarrow
\text{ESP32 Processing}
\rightarrow
\text{Outputs}
}
$$
