
📘 Theory: Polling, Interrupts, ISRs, Debouncing, and Hardware Timers
ESP32 MCU Case Study
🧩 1. Introduction

Embedded systems continuously interact with the physical world through sensors, switches, communication interfaces, and actuators. A microcontroller must therefore determine when an external or internal event occurs and execute the appropriate response.

Two fundamental techniques are used:

Polling — the CPU repeatedly checks whether an event has occurred.
Interrupts — hardware notifies the CPU when an event occurs.

The ESP32 supports both approaches and also provides hardware timers for precise periodic or one-shot events.

A typical embedded event-processing architecture can be represented as

Event→Detection→Processing→Response
	​


For example:

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

This chapter introduces polling, hardware interrupts, interrupt service routines (ISRs), switch debouncing, and hardware timers using the ESP32 as the MCU case study.

🧠 2. Polling
2.1 Polling Concept

Polling is a software technique in which the CPU repeatedly checks the status of a peripheral or input.

For example:

if (digitalRead(BUTTON_PIN) == LOW)
{
    // Button pressed
}

When placed inside loop(), the CPU repeatedly reads the button.

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

The basic polling algorithm is

Read→Check→Process→Repeat
	​

2.2 ESP32 Polling Example

Consider a push button connected to GPIO 4 and an LED connected to GPIO 2.

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

Because INPUT_PULLUP is used,

Released=HIGH

and

Pressed=LOW.
⏱️ 3. Polling Rate and Response Time

Suppose the program checks an input once every

T
p
	​


seconds.

The polling frequency is

f
p
	​

=
T
p
	​

1
	​

.

For example, if

T
p
	​

=10 ms,

then

f
p
	​

=
0.01
1
	​

=100 Hz.

The event response can be delayed until the next polling operation.

Therefore, in a simple periodic polling system,

0≤T
response
	​

≲T
p
	​


before considering additional processing delays.

A shorter polling interval generally provides faster response but causes the CPU to check the input more frequently.

📊 4. Advantages and Limitations of Polling
Characteristic	Polling
Implementation	Simple
Debugging	Easy
CPU usage	Can be high
Event latency	Depends on polling interval
Hardware support	Minimal
Low-power operation	Less efficient
Multiple events	Becomes harder as system grows
Suitable for	Simple or slowly changing inputs

Polling works well for:

simple push buttons,
slowly changing sensors,
introductory MCU programs,
applications where precise event timing is unnecessary.
⚡ 5. Interrupts
5.1 Interrupt Concept

An interrupt allows hardware to signal the CPU when an event occurs.

Instead of repeatedly asking

“Has the event occurred?”

the CPU performs other work until the hardware reports

“The event has occurred.”

The architecture becomes

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

Conceptually,

Event→Interrupt→ISR→Return
	​

🔔 6. Interrupt Sources

An ESP32 can respond to many types of hardware events, including:

GPIO transitions,
hardware timers,
UART activity,
ADC-related events,
communication peripherals,
internal peripheral events.

For a push button, a GPIO transition can generate an interrupt.

For example:

HIGH→LOW

can represent a button press when an internal pull-up is used.

🧠 7. Interrupt Service Routine

An Interrupt Service Routine (ISR) is a function executed in response to an interrupt.

The sequence is approximately

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

The ISR should generally be:

short,
deterministic,
non-blocking,
limited to essential work.

A useful design principle is

ISR=Detect+Record+Exit
	​


rather than performing lengthy application processing inside the ISR.

💻 8. ESP32 GPIO Interrupt Example

Using the Arduino ESP32 environment:

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

The ISR does only one important operation:

buttonPressed = true;

The main program performs the actual processing.

🔍 9. IRAM_ATTR

ESP32 Arduino examples often declare an ISR using:

void IRAM_ATTR buttonISR()

IRAM_ATTR requests placement of the function in instruction RAM, which can be important for interrupt code that must remain executable under conditions where flash access is unavailable or constrained.

For introductory programming, the important pattern is:

void IRAM_ATTR ISR_Name()
{
    // Minimal ISR operations
}
🔄 10. Interrupt Trigger Modes

GPIO interrupts can respond to different signal conditions.

Common Arduino trigger modes include:

Mode	Meaning
RISING	LOW → HIGH
FALLING	HIGH → LOW
CHANGE	Either transition
HIGH	High-level condition, where supported
LOW	Low-level condition, where supported

For an active-low push button using INPUT_PULLUP, the press event is typically

HIGH→LOW.

Therefore,

FALLING

is a natural trigger choice.

🆚 11. Polling vs. Interrupts

The fundamental difference is:

Polling
CPU asks hardware
	​

Interrupt
Hardware notifies CPU
	​

Characteristic	Polling	Interrupt
Event detection	CPU checks	Hardware signals CPU
CPU utilization	Potentially higher	Usually lower between events
Programming complexity	Low	Moderate
Response	Depends on polling rate	Usually faster for asynchronous events
Event handling	Sequential checking	Event-driven
Low-power suitability	Lower	Better
Debugging	Easier	More challenging
Timing considerations	Simple	ISR timing important
Best application	Slow/simple events	Asynchronous/urgent events
📌 12. When to Use Polling

Polling is appropriate when:

events occur slowly,
the system is simple,
the input must be checked periodically anyway,
timing requirements are relaxed,
only a few inputs exist.

Example:

Temperature Sensor
      │
      ▼
Read every 1 second

There is usually little benefit in generating thousands of interrupts per second merely to obtain one temperature measurement each second.

⚡ 13. When to Use Interrupts

Interrupts are appropriate when:

events are asynchronous,
fast response is important,
events occur infrequently,
the CPU has other work to perform,
event timing must be captured accurately.

Examples include:

Push Button
Encoder
Timer Expiration
UART Event
Pulse Counter
External Sensor Alert
🚨 14. Rules for Writing an ISR

An ISR should generally avoid:

long delays,
lengthy loops,
large calculations,
unnecessary printing,
blocking operations,
waiting for other events.

Instead:

void IRAM_ATTR ISR()
{
    eventFlag = true;
}

Then:

void loop()
{
    if (eventFlag)
    {
        eventFlag = false;


        // Perform larger task here
    }
}

This separates

Real-Time Event Detection
	​


from

Application Processing
	​

🔐 15. Shared Variables and volatile

A variable modified inside an ISR and read by normal program code is commonly declared volatile.

Example:

volatile bool eventFlag = false;

This tells the compiler that the variable can change outside the ordinary sequential flow of the program.

Without volatile, compiler optimization could make simple ISR-shared flag handling behave unexpectedly.

However, volatile alone does not provide full synchronization or atomicity for every type of shared data operation.

🔘 16. Mechanical Switch Debouncing

A mechanical push button does not normally generate a perfect digital transition.

An ideal button press would be

HIGH ─────────────┐
                  │
                  └──────── LOW

A real mechanical switch may produce

HIGH ──────┐ ┌─┐ ┌──┐
           └─┘ └─┘  └──── LOW

Thus, one physical press can generate multiple transitions.

This phenomenon is called switch bounce or contact bounce.

⚠️ 17. Debouncing and Interrupts

Switch bounce becomes particularly important with interrupts.

Suppose the interrupt is configured as:

attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonISR,
    FALLING
);

One physical press might produce:

Physical press
     │
     ├── Interrupt 1
     ├── Interrupt 2
     ├── Interrupt 3
     └── Interrupt 4

The application may incorrectly interpret this as four button presses.

Therefore,

Mechanical Button+Interrupt⇒Debouncing Required
	​

🛠️ 18. Software Debouncing

One simple polling-based method is:

if (digitalRead(BUTTON_PIN) == LOW)
{
    delay(20);


    if (digitalRead(BUTTON_PIN) == LOW)
    {
        // Confirmed button press
    }
}

The delay allows the electrical signal to stabilize.

A typical conceptual debounce interval might be on the order of several to tens of milliseconds, but the appropriate value depends on the actual switch and application.

⏱️ 19. Time-Based Interrupt Debouncing

For interrupt-driven applications, it is preferable to avoid a blocking delay() inside the ISR.

Instead, record event timing and reject events occurring too close together.

Conceptually:

Δt=t
current
	​

−t
previous
	​

.

Accept the event only if

Δt>T
D
	​


where $T_D$ is the selected debounce interval.

💻 20. Interrupt Debounce Example
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

For production-quality designs, ISR-safe timing primitives and the exact ESP32 Arduino core behavior should be considered carefully. Another robust strategy is to let the ISR set a flag and perform the entire debounce decision outside the ISR.

🔧 21. Hardware Debouncing

Debouncing can also be implemented using hardware.

A simple RC network can suppress rapid transitions:

Button
  │
  ├──── R ──── GPIO
  │             │
  │             C
  │             │
 GND           GND

The RC time constant is

τ=RC.

For example, if

R=10 kΩ

and

C=1 μF,

then

τ=10000×1×10
−6
=0.01 s=10 ms.

A Schmitt-trigger input or dedicated debounce circuit can provide additional signal conditioning.

📊 22. Software vs. Hardware Debouncing
Characteristic	Software	Hardware
Extra components	No	Yes
Cost	Low	Higher
Flexibility	High	Lower
Modification	Easy	Requires hardware change
CPU involvement	Yes	Minimal
Noise filtering	Algorithm-dependent	Can be strong
Typical use	General MCU applications	Robust/noisy interfaces
⏲️ 23. Hardware Timers

A hardware timer is a peripheral that counts clock-derived events independently of normal CPU instruction execution.

Conceptually:

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

Hardware timers are useful for:

periodic sampling,
control loops,
precise scheduling,
timeout generation,
periodic interrupts,
waveform/event timing.
🧠 24. Timer Principle

Suppose the timer receives a clock frequency

f
clk
	​

.

After division by a factor $D$, the timer frequency is

f
timer
	​

=
D
f
clk
	​

	​

.

Therefore, the timer tick period is

T
tick
	​

=
f
timer
	​

1
	​

=
f
clk
	​

D
	​

.

If an event is generated after $N$ timer ticks,

T
event
	​

=NT
tick
	​

.

Thus,

T
event
	​

=N
f
clk
	​

D
	​

	​


This basic relationship applies broadly to MCU timer design, although the exact ESP32 timer configuration interface depends on the chip family and software framework.

⚙️ 25. Timer Resolution

Suppose a timer is configured to increment once every

1 μs.

Then the effective timer frequency is

f
timer
	​

=1 MHz.

To generate a 1-second event:

N=
1 μs
1 s
	​

=1,000,000.

Therefore,

1,000,000 ticks=1 second
	​


for that configuration.

🔔 26. Timer Interrupt

A hardware timer can generate an interrupt when its configured alarm condition is reached.

The architecture becomes

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

This allows precise periodic operations without repeatedly checking elapsed time in the main loop.

💻 27. ESP32 Arduino Hardware Timer Concept

The exact timer API has changed across versions of the Arduino-ESP32 core. In current projects, the code should be written against the timer API provided by the installed core version.

A conceptual implementation is:

volatile bool timerEvent = false;


void IRAM_ATTR onTimer()
{
    timerEvent = true;
}


void setup()
{
    // Configure hardware timer
    // Attach onTimer() to the timer interrupt
    // Configure timer alarm period
    // Enable the timer
}


void loop()
{
    if (timerEvent)
    {
        timerEvent = false;


        // Periodic application task
    }
}

The important architecture is independent of the specific API:

Timer→Interrupt→ISR→Event Flag→Application
	​

💡 28. Timer-Controlled LED

Consider blinking an LED once per second using a hardware timer.

Instead of:

digitalWrite(LED_PIN, HIGH);
delay(500);


digitalWrite(LED_PIN, LOW);
delay(500);

the timer can periodically generate an event.

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

This allows the CPU to perform other tasks between timer events.

🆚 29. delay(), millis(), and Hardware Timer

Three common timing approaches are:

Method	Type	CPU Blocking	Precision	Complexity
delay()	Software delay	Yes	Moderate	Very low
millis()	Software time check	No	Moderate	Low
Hardware timer	Peripheral-based	No	High	Moderate

For simple applications:

delay()

may be sufficient.

For multiple concurrent software activities:

millis()

is generally more appropriate.

For precise periodic timing:

Hardware Timer

is often preferred.

🔄 30. Blocking Timing

Consider:

delay(1000);

During this period, the application's normal flow waits before proceeding.

Conceptually:

Task
 │
 ▼
delay(1000)
 │
 │ CPU/application waits
 │
 ▼
Continue

This makes simple programs easy to write but becomes problematic when multiple activities must progress independently.

⚡ 31. Non-Blocking Timing

Using millis():

unsigned long previousTime = 0;


void loop()
{
    unsigned long currentTime = millis();


    if (currentTime - previousTime >= 1000)
    {
        previousTime = currentTime;


        // Periodic action
    }


    // Other processing can continue here
}

The application can continue processing:

Check Timer
    │
    ├── Read Button
    ├── Read Sensor
    ├── Update LED
    ├── Process Communication
    └── Repeat
🧠 32. Timer vs. Interrupt

A timer and an interrupt are related but not identical concepts.

A timer measures/counts time.

An interrupt notifies the CPU about an event.

A timer can therefore be an interrupt source:

Timer Expiration→Interrupt→ISR
	​


Similarly, GPIO hardware can also be an interrupt source:

GPIO Edge→Interrupt→ISR
	​

🏗️ 33. Event-Driven Embedded Architecture

Combining GPIO interrupts and hardware timers produces an event-driven system:

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

Instead of continuously polling everything, the processor reacts to important events.

The general model becomes

Event Sources→Interrupt Controller→CPU→ISR→Application
	​

🚦 34. Interrupt Latency

Interrupt latency is the delay between the occurrence of an interrupt-generating event and the start of its ISR.

Conceptually,

T
latency
	​

=t
ISR start
	​

−t
event
	​

.

Latency may be influenced by factors such as:

interrupt priority,
currently executing critical sections,
other interrupts,
CPU state,
memory/cache conditions,
framework and RTOS behavior.

Low interrupt latency is important for time-critical applications.

⏳ 35. ISR Execution Time

If an ISR begins at $t_s$ and finishes at $t_f$, its execution time is

T
ISR
	​

=t
f
	​

−t
s
	​

.

Long ISRs can interfere with other time-sensitive processing.

Therefore, a desirable design objective is

T
ISR
	​

≪T
event interval
	​

	​


where practical.

⚠️ 36. Interrupt Overload

Suppose interrupts arrive at frequency

f
i
	​

.

The event interval is

T
i
	​

=
f
i
	​

1
	​

.

If ISR processing takes nearly as long as, or longer than, the event interval, the processor may struggle to service events in time.

Conceptually:

T
ISR
	​

≥T
i
	​


indicates an unsustainable interrupt workload for a simple single-stream model.

This is why ISRs should be short.

🔐 37. Critical Sections and Shared Data

Both the ISR and main application may access the same variable.

For example:

volatile int pulseCount = 0;

ISR:

void IRAM_ATTR pulseISR()
{
    pulseCount++;
}

Main program:

Serial.println(pulseCount);

For simple variables and operations this may appear straightforward, but more complex shared data can require synchronization or critical sections.

The core problem is

Main Program ───┐
                ├── Shared Data
ISR ────────────┘

Care must be taken to prevent inconsistent data access.

🧩 38. Event Flag Design Pattern

One of the simplest interrupt designs uses an event flag.

ISR:

volatile bool eventFlag = false;


void IRAM_ATTR ISR()
{
    eventFlag = true;
}

Main application:

void loop()
{
    if (eventFlag)
    {
        eventFlag = false;


        processEvent();
    }
}

This provides a useful separation:

ISR→Signal Event
	​

Main Program→Process Event
	​

📦 39. Event Counter Design Pattern

A Boolean flag only indicates that at least one event occurred.

If multiple events can arrive before the main program responds, an event counter may be more appropriate.

volatile unsigned int eventCount = 0;


void IRAM_ATTR ISR()
{
    eventCount++;
}

Conceptually:

N
events
	​

=N
events
	​

+1.

The application can then process the accumulated events.

Care is still required for concurrency, overflow, and atomic access.

🔄 40. Polling and Interrupt Hybrid Design

Real embedded systems frequently combine polling and interrupts.

For example:

Push Button ─────► Interrupt
Temperature ─────► Poll every 1 s
UART ────────────► Interrupt/Event
LED ─────────────► Main Program
Timer ───────────► Interrupt

The goal is not to replace all polling with interrupts.

Instead, the appropriate method should be selected according to:

Event Rate+Timing Requirement+CPU Cost+System Complexity
	​

📊 41. Example Selection
Application	Recommended Approach
Temperature every 5 s	Polling / scheduled read
Push-button press	Polling or interrupt
High-speed encoder	Interrupt / hardware peripheral
Periodic control loop	Hardware timer
Simple LED blink	delay() or millis()
Multitask LED blink	millis() or timer
External alarm signal	Interrupt
High-rate pulse measurement	Dedicated hardware peripheral / interrupt as appropriate
🧠 42. ESP32 Application Example

Consider a smart embedded controller containing:

push button,
LED,
temperature sensor,
periodic timer,
serial communication.

A possible architecture is:

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

This demonstrates that polling, interrupts, and timers are complementary techniques.

🔬 43. Example: Button Interrupt + Timer

A useful application could perform two independent functions:

Toggle an LED when a button is pressed.
Generate a periodic sensor-sampling event.

Conceptually:

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

The main loop becomes the central application-processing layer.

⚙️ 44. Recommended ISR Architecture

A scalable design is

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

Mathematically, we can represent this as

E→I→ISR→Q→P
	​


where:

$E$ = hardware event,
$I$ = interrupt,
$ISR$ = interrupt service routine,
$Q$ = event storage/notification mechanism,
$P$ = application processing.
🆚 45. Complete Comparison
Feature	Polling	GPIO Interrupt	Hardware Timer
Trigger	Software check	External/internal GPIO event	Clock/time
CPU continuously checks?	Yes	No	No
Asynchronous event support	Limited by poll rate	Excellent	Time-based
Timing precision	Loop dependent	Event dependent	High
Complexity	Low	Moderate	Moderate
ISR required	No	Usually	Usually for interrupt-driven use
Typical application	Sensor/button	Button/encoder/alarm	Sampling/control
Power efficiency	Lower	Better	Better
Real-time suitability	Limited	Good	Good
💬 46. Discussion Questions
What is polling?
What is an interrupt?
How does polling differ from interrupt-driven processing?
What is an ISR?
Why should an ISR be short?
What does IRAM_ATTR indicate in ESP32 Arduino programs?
What is the purpose of volatile?
Does volatile automatically make all shared-data operations thread-safe?
What is switch bounce?
Why can switch bounce cause multiple interrupts?
What is software debouncing?
What is hardware debouncing?
What does an RC debounce circuit do?
What is a hardware timer?
What is the difference between a timer and an interrupt?
What is interrupt latency?
Why can long ISRs create problems?
When should polling be preferred?
When should interrupts be preferred?
Why might a system use both polling and interrupts?
🧠 47. Exercises
Exercise 1 — Polling

Create a program that reads a push button using:

digitalRead()

and turns an LED ON while the button is pressed.

Exercise 2 — GPIO Interrupt

Replace the polling implementation with:

attachInterrupt()

and toggle the LED whenever the button is pressed.

Exercise 3 — Debouncing

Add software debouncing so that:

1 physical press=1 software event
	​

Exercise 4 — Event Counter

Use an interrupt to count button presses:

Press 1 → Count = 1
Press 2 → Count = 2
Press 3 → Count = 3

Display the result in Serial Monitor.

Exercise 5 — Hardware Timer

Configure an ESP32 hardware timer to generate a periodic event and toggle an LED.

Exercise 6 — Hybrid System

Create a system where:

Button → GPIO Interrupt → Toggle LED


Timer → Timer Interrupt → Sample Sensor


Main Loop → Display Results
🚀 48. Advanced Exercise

Design a small event-driven ESP32 system with:

GPIO 4 — push button,
GPIO 2 — LED,
GPIO interrupt — button detection,
debouncing — reject false transitions,
hardware timer — periodic event,
Serial Monitor — display event count.

The architecture should follow:

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
🧾 49. Expected Learning Outcomes

After studying this theory, students should be able to:

Explain polling and interrupt-driven processing.
Compare polling with hardware interrupts.
Explain how an ESP32 responds to GPIO events.
Describe the role of an ISR.
Apply basic ISR design rules.
Explain volatile in ISR-shared variables.
Identify switch-bounce problems.
Implement software and hardware debounce concepts.
Explain hardware timer operation.
Calculate timer periods from clock, divider, and count values.
Explain interrupt latency and ISR execution time.
Select polling, interrupts, or timers for different applications.
Design a simple event-driven ESP32 application.
📘 50. References
Espressif Systems, ESP32 Technical Reference Manual.
Espressif Systems, ESP32 Series Datasheet.
Espressif Systems, ESP-IDF Programming Guide.
Espressif Systems, Arduino Core for ESP32 Documentation.
Arduino, Arduino Language Reference.
J. W. Valvano, Embedded Systems: Real-Time Interfacing to ARM Cortex-M Microcontrollers.
M. Barr and A. Massa, Programming Embedded Systems, O'Reilly.
D. E. Simon, An Embedded Software Primer, Addison-Wesley.
🔑 51. Key Concepts
Polling
CPU→Check Event→Process→Repeat
	​

Interrupt
Hardware Event→Interrupt→ISR
	​

Debouncing
Noisy Switch Transitions→Debounce→Single Valid Event
	​

Hardware Timer
Clock→Timer→Alarm→Interrupt
	​


Together, these concepts form the basis of an event-driven embedded system:

Inputs / Timers→Event Detection→ESP32 Processing→Outputs
	​


The natural next step is to apply these concepts in a practical laboratory on ESP32 GPIO interrupts, push-button debouncing, and hardware-timer-controlled events.
