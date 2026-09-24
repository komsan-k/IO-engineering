# ESP32 FreeRTOS Series

## Task, Queue, Mutex, Semaphore, Interrupt, and Smart Lighting

This six-lab series introduces **FreeRTOS programming on the ESP32** through progressively more complete embedded applications. Students begin with a single task and finish with an LDR-based smart-lighting system using multiple tasks, queues, a mutex, ADC measurement, PWM control, and serial logging.

## Learning Path

```text
Lab 1: Basic FreeRTOS Task
        ↓
Lab 2: Multiple Tasks and Priorities
        ↓
Lab 3: LDR Producer–Consumer Queue
        ↓
Lab 4: Shared Data, Race Condition, and Mutex
        ↓
Lab 5: Button Interrupt and Binary Semaphore
        ↓
Lab 6: Complete LDR Smart Lighting System
```

## Required Hardware

- ESP32 development board
- LDR
- 10 kΩ resistor
- Two LEDs
- Two 220–330 Ω resistors
- Push button
- Breadboard and jumper wires
- USB cable

## Software

- Arduino IDE with the ESP32 board package
- Serial Monitor configured for `115200` baud

## Common Pin Assignment

| Device | ESP32 pin | Notes |
|---|---:|---|
| LDR ADC input | GPIO 36 | Input-only ADC pin |
| PWM LED | GPIO 12 | External LED through resistor |
| Status LED | GPIO 2 | Built-in LED on many boards |
| Push button | GPIO 16 | Uses internal pull-up |

> Check the pinout of the specific ESP32 board before connecting hardware. Never apply more than 3.3 V to an ESP32 GPIO.

---

# Lab 1 — Basic FreeRTOS Task

## Objectives

1. Explain the purpose of a FreeRTOS task.
2. Create a task using `xTaskCreate()`.
3. Use `vTaskDelay()` without blocking other tasks.
4. Identify the roles of `setup()` and `loop()` in an ESP32 FreeRTOS application.

## Concept

A task is an independently scheduled function. A typical task contains an infinite loop and a blocking operation such as `vTaskDelay()`.

```cpp
void taskFunction(void *parameter) {
  while (true) {
    // Task work
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

## Complete Program

```cpp
#include <Arduino.h>

constexpr uint8_t LED_PIN = 2;

void ledTask(void *parameter) {
  (void)parameter;

  while (true) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    vTaskDelay(pdMS_TO_TICKS(500));

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  BaseType_t result = xTaskCreate(
    ledTask,
    "LED Task",
    2048,
    nullptr,
    1,
    nullptr
  );

  if (result != pdPASS) {
    Serial.println("Task creation failed");
  }
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Activity

1. Upload the program and observe the LED and Serial Monitor.
2. Change the blinking period to 200 ms.
3. Change the ON and OFF durations to different values.

## Checkpoint Questions

1. What does `xTaskCreate()` do?
2. Why is `pdMS_TO_TICKS()` used?
3. Why must the task periodically block or delay?
4. What could happen if a high-priority task never yields the processor?

---

# Lab 2 — Multiple Tasks and Priorities

## Objectives

1. Create multiple concurrent tasks.
2. Observe tasks operating at different periods.
3. Explain task priority and starvation.
4. Pass parameters to a task.

## System Structure

```text
LED Task ────────► Toggle LED every 500 ms
Counter Task ────► Print counter every 1000 ms
Monitor Task ────► Print free heap every 5000 ms
```

## Complete Program

```cpp
#include <Arduino.h>

constexpr uint8_t LED_PIN = 2;

void ledTask(void *parameter) {
  (void)parameter;
  bool ledState = false;

  while (true) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void counterTask(void *parameter) {
  const char *label = static_cast<const char *>(parameter);
  uint32_t counter = 0;

  while (true) {
    Serial.printf("[%s] Counter = %lu\n",
                  label,
                  static_cast<unsigned long>(counter++));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void monitorTask(void *parameter) {
  (void)parameter;

  while (true) {
    Serial.printf("[Monitor] Free heap = %u bytes\n",
                  ESP.getFreeHeap());
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  xTaskCreate(ledTask, "LED", 2048, nullptr, 1, nullptr);
  xTaskCreate(counterTask, "Counter", 2048,
              (void *)"Counter Task", 2, nullptr);
  xTaskCreate(monitorTask, "Monitor", 2048, nullptr, 1, nullptr);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Activity

1. Record the period of each task.
2. Change the counter priority from 2 to 1.
3. Add a fourth task that prints a message every 2 s.
4. Remove `vTaskDelay()` from the counter task temporarily, observe the result, and restore it immediately.

## Checkpoint Questions

1. Which task has the highest priority?
2. Does a higher priority mean that a task always runs continuously?
3. What is starvation?
4. Why is blocking preferable to an empty busy-wait loop?

---

# Lab 3 — LDR Producer–Consumer Queue

## Objectives

1. Read an LDR through the ESP32 ADC.
2. Create a FreeRTOS queue.
3. Send structured sensor data between tasks.
4. Implement the producer–consumer pattern.

## LDR Connection

```text
3.3 V ── LDR ──┬── GPIO 36
                │
              10 kΩ
                │
               GND
```

This arrangement normally produces a larger ADC value under brighter light.

## Architecture

```text
LDR → Sensor Task → Queue → Display Task → Serial Monitor
```

## Complete Program

```cpp
#include <Arduino.h>

constexpr uint8_t LDR_PIN = 36;

struct LDRData {
  uint32_t sample;
  uint16_t raw;
  float normalized;
  uint32_t timestampMs;
};

QueueHandle_t ldrQueue = nullptr;

void sensorTask(void *parameter) {
  (void)parameter;
  uint32_t sample = 0;
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    LDRData data;
    data.sample = ++sample;
    data.raw = analogRead(LDR_PIN);
    data.normalized = 1 - (data.raw / 4095.0f);
    data.timestampMs = millis();

    if (xQueueSend(ldrQueue, &data, pdMS_TO_TICKS(100)) != pdPASS) {
      Serial.println("Queue full: sample dropped");
    }

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
  }
}

void displayTask(void *parameter) {
  (void)parameter;
  LDRData received;

  while (true) {
    if (xQueueReceive(ldrQueue, &received, portMAX_DELAY) == pdPASS) {
      Serial.printf("[%03lu] RAW=%4u  Normalized=%.3f  Time=%lu ms\n",
                    static_cast<unsigned long>(received.sample),
                    received.raw,
                    received.normalized,
                    static_cast<unsigned long>(received.timestampMs));
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(LDR_PIN, ADC_11db);

  ldrQueue = xQueueCreate(10, sizeof(LDRData));
  if (ldrQueue == nullptr) {
    Serial.println("Queue creation failed");
    while (true) delay(1000);
  }

  xTaskCreate(sensorTask, "Sensor", 2048, nullptr, 2, nullptr);
  xTaskCreate(displayTask, "Display", 3072, nullptr, 1, nullptr);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Activity

1. Record values in darkness, normal room light, and bright light.
2. Change the queue length from 10 to 3.
3. Change the sampling period to 100 ms.
4. Add a 1 s delay to the display task and observe queue-full messages.

## Checkpoint Questions

1. Which task is the producer?
2. Which task is the consumer?
3. Does a FreeRTOS queue copy the item or store its address?
4. What does `portMAX_DELAY` mean in `xQueueReceive()`?
5. Why is a queue safer than unsynchronized global sensor variables?

---

# Lab 4 — Shared Data, Race Condition, and Mutex

## Objectives

1. Identify shared data and critical sections.
2. Explain a race condition.
3. Protect shared data with a mutex.
4. Minimize the time for which a mutex is held.

## Concept

An increment operation is not necessarily atomic:

```text
Read value → Add one → Write value
```

If two tasks execute this sequence concurrently, one update may be lost. A mutex permits only one task at a time to enter the protected critical section.

## Complete Program 

### Code-1

#### Mutex and Shared Resource

A **Mutex (Mutual Exclusion)** protects a shared resource from being accessed by multiple tasks at the same time.

```text
                 Shared Resource
                +--------------+
                |   counter    |
                +--------------+
                       ↑
                    [MUTEX]
                   🔒 / 🔓
                  /        \
             Task 1       Task 2

```cpp
#include <Arduino.h>

SemaphoreHandle_t mutex;

int counter = 0;

void task1(void *parameter)
{
  while (1)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Critical Section
    counter++;

    Serial.print("Task 1 : ");
    Serial.println(counter);

    xSemaphoreGive(mutex);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void task2(void *parameter)
{
  while (1)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Critical Section
    counter++;

    Serial.print("Task 2 : ");
    Serial.println(counter);

    xSemaphoreGive(mutex);

    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void setup()
{
  Serial.begin(115200);

  // Create Mutex
  mutex = xSemaphoreCreateMutex();

  xTaskCreate(
    task1,
    "Task 1",
    2048,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    task2,
    "Task 2",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop()
{
}
```

### Code-2

```cpp
#include <Arduino.h>

struct Statistics {
  uint32_t totalUpdates;
  uint32_t taskAUpdates;
  uint32_t taskBUpdates;
};

Statistics statistics = {};
SemaphoreHandle_t statisticsMutex = nullptr;

void updateTask(void *parameter) {
  const char taskId = *static_cast<const char *>(parameter);

  while (true) {
    if (xSemaphoreTake(statisticsMutex, portMAX_DELAY) == pdTRUE) {
      statistics.totalUpdates++;

      if (taskId == 'A') {
        statistics.taskAUpdates++;
      } else {
        statistics.taskBUpdates++;
      }

      xSemaphoreGive(statisticsMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void loggerTask(void *parameter) {
  (void)parameter;

  while (true) {
    Statistics copy;

    if (xSemaphoreTake(statisticsMutex, portMAX_DELAY) == pdTRUE) {
      copy = statistics;
      xSemaphoreGive(statisticsMutex);
    }

    Serial.printf("Total=%lu, A=%lu, B=%lu, Check=%lu\n",
                  static_cast<unsigned long>(copy.totalUpdates),
                  static_cast<unsigned long>(copy.taskAUpdates),
                  static_cast<unsigned long>(copy.taskBUpdates),
                  static_cast<unsigned long>(copy.taskAUpdates +
                                             copy.taskBUpdates));

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);

  statisticsMutex = xSemaphoreCreateMutex();
  if (statisticsMutex == nullptr) {
    Serial.println("Mutex creation failed");
    while (true) delay(1000);
  }

  static const char taskAId = 'A';
  static const char taskBId = 'B';

  xTaskCreate(updateTask, "Updater A", 2048,
              (void *)&taskAId, 1, nullptr);
  xTaskCreate(updateTask, "Updater B", 2048,
              (void *)&taskBId, 1, nullptr);
  xTaskCreate(loggerTask, "Logger", 3072, nullptr, 1, nullptr);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Verification

The following relationship should remain valid:

$$
N_{\mathrm{total}}=N_A+N_B
$$

## Activity

1. Run the protected program and verify the relationship.
2. Temporarily remove the mutex operations and compare the results.
3. Restore the mutex.
4. Explain why serial printing is performed after releasing the mutex.

## Checkpoint Questions

1. What is the shared resource?
2. Which statements form the critical section?
3. Why must every reader and writer follow the same locking rule?
4. What is priority inversion?
5. Why is a mutex generally preferred over a binary semaphore for resource protection?

---

# Lab 5 — Button Interrupt and Binary Semaphore

## Objectives

1. Configure an ESP32 GPIO interrupt.
2. Keep an Interrupt Service Routine (ISR) short.
3. signal a task from an ISR using a binary semaphore.
4. Apply software debouncing in a task.

## Button Connection

```text
GPIO 16 ── Push button ── GND
```

The program uses `INPUT_PULLUP`, so the released state is HIGH and the pressed state is LOW.

## Architecture

```text
Button press → ISR → Binary Semaphore → Button Task → Toggle LED
```

## Complete Program

```cpp
#include <Arduino.h>

constexpr uint8_t BUTTON_PIN = 16;
constexpr uint8_t LED_PIN = 2;
constexpr uint32_t DEBOUNCE_MS = 200;

SemaphoreHandle_t buttonSemaphore = nullptr;

void IRAM_ATTR buttonISR() {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(buttonSemaphore,
                        &higherPriorityTaskWoken);

  if (higherPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

void buttonTask(void *parameter) {
  (void)parameter;
  bool ledState = false;
  uint32_t lastAcceptedPress = 0;
  uint32_t pressCount = 0;

  while (true) {
    if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
      uint32_t now = millis();

      if (now - lastAcceptedPress >= DEBOUNCE_MS) {
        lastAcceptedPress = now;
        pressCount++;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);

        Serial.printf("Button press %lu: LED %s\n",
                      static_cast<unsigned long>(pressCount),
                      ledState ? "ON" : "OFF");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  buttonSemaphore = xSemaphoreCreateBinary();
  if (buttonSemaphore == nullptr) {
    Serial.println("Semaphore creation failed");
    while (true) delay(1000);
  }

  xTaskCreate(buttonTask, "Button", 2048, nullptr, 2, nullptr);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonISR,
                  FALLING);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Activity

1. Test the button and count accepted presses.
2. Change the debounce interval to 50 ms and then 500 ms.
3. Modify the task so one press starts blinking and the next press stops it.
4. Explain why `Serial.print()` is not called inside the ISR.

## Checkpoint Questions

1. Why does the ISR use `xSemaphoreGiveFromISR()`?
2. What is the purpose of `higherPriorityTaskWoken`?
3. What is button bounce?
4. Why should an ISR avoid delays and lengthy processing?
5. When should a queue be used instead of a binary semaphore?

---

# Lab 6 — ESP32 FreeRTOS LDR Smart Lighting System

## Objectives

1. Integrate ADC, PWM, tasks, queues, and a mutex.
2. Separate sensing, control, and logging responsibilities.
3. Calibrate LDR decision thresholds.
4. Implement a complete producer–consumer embedded system.

## LED Connection

```text
GPIO 12 ── 220–330 Ω ── LED anode
                           LED cathode ── GND
```

## Architecture

```text
LDR → Sensor Task → Sensor Queue → Control Task → PWM LED
                                      │
                                      └→ Log Queue → Logger Task

               Shared statistics protected by a mutex
```

## Control Requirements

| ADC reading | State | PWM duty | LED response |
|---:|---|---:|---|
| `< 1000` | DARK | 255 | ON |
| `1000–2999` | NORMAL | 128 | DIM |
| `≥ 3000` | BRIGHT | 0 | OFF |

## Complete Program

This program uses the pin-based LEDC API provided by Arduino-ESP32 3.x.

```cpp
#include <Arduino.h>

constexpr uint8_t LDR_PIN = 36;
constexpr uint8_t LED_PIN = 12;

constexpr uint32_t PWM_FREQUENCY = 5000;
constexpr uint8_t PWM_RESOLUTION = 8;
constexpr uint16_t DARK_THRESHOLD = 1000;
constexpr uint16_t BRIGHT_THRESHOLD = 3000;
constexpr TickType_t SAMPLE_PERIOD = pdMS_TO_TICKS(1000);

enum LightState : uint8_t {
  LIGHT_DARK,
  LIGHT_NORMAL,
  LIGHT_BRIGHT
};

struct LDRData {
  uint32_t sampleNumber;
  uint16_t raw;
  float normalized;
  uint32_t timestampMs;
};

struct LightingData {
  LDRData sensor;
  LightState state;
  uint8_t pwmDuty;
};

struct SystemStatistics {
  uint32_t samplesRead;
  uint32_t samplesControlled;
  uint32_t samplesLogged;
  uint16_t latestRaw;
  uint8_t latestDuty;
};

QueueHandle_t sensorQueue = nullptr;
QueueHandle_t logQueue = nullptr;
SemaphoreHandle_t statisticsMutex = nullptr;
SystemStatistics statistics = {};

LightState classifyLight(uint16_t raw) {
  if (raw < DARK_THRESHOLD) return LIGHT_DARK;
  if (raw < BRIGHT_THRESHOLD) return LIGHT_NORMAL;
  return LIGHT_BRIGHT;
}

uint8_t dutyForState(LightState state) {
  switch (state) {
    case LIGHT_DARK:   return 255;
    case LIGHT_NORMAL: return 128;
    case LIGHT_BRIGHT: return 0;
    default:           return 0;
  }
}

const char *stateToString(LightState state) {
  switch (state) {
    case LIGHT_DARK:   return "DARK";
    case LIGHT_NORMAL: return "NORMAL";
    case LIGHT_BRIGHT: return "BRIGHT";
    default:           return "UNKNOWN";
  }
}

void sensorTask(void *parameter) {
  (void)parameter;
  uint32_t sampleNumber = 0;
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    LDRData reading;
    reading.sampleNumber = ++sampleNumber;
    reading.raw = analogRead(LDR_PIN);
    reading.normalized = reading.raw / 4095.0f;
    reading.timestampMs = millis();

    if (xQueueSend(sensorQueue, &reading,
                   pdMS_TO_TICKS(100)) == pdPASS) {
      if (xSemaphoreTake(statisticsMutex,
                         pdMS_TO_TICKS(50)) == pdTRUE) {
        statistics.samplesRead++;
        statistics.latestRaw = reading.raw;
        xSemaphoreGive(statisticsMutex);
      }
    } else {
      Serial.println("[Sensor] Queue full: sample dropped");
    }

    vTaskDelayUntil(&lastWakeTime, SAMPLE_PERIOD);
  }
}

void controlTask(void *parameter) {
  (void)parameter;
  LDRData reading;

  while (true) {
    if (xQueueReceive(sensorQueue, &reading,
                      portMAX_DELAY) == pdPASS) {
      LightingData result;
      result.sensor = reading;
      result.state = classifyLight(reading.raw);
      result.pwmDuty = dutyForState(result.state);

      ledcWrite(LED_PIN, result.pwmDuty);

      if (xSemaphoreTake(statisticsMutex,
                         pdMS_TO_TICKS(50)) == pdTRUE) {
        statistics.samplesControlled++;
        statistics.latestDuty = result.pwmDuty;
        xSemaphoreGive(statisticsMutex);
      }

      if (xQueueSend(logQueue, &result,
                     pdMS_TO_TICKS(100)) != pdPASS) {
        Serial.println("[Control] Log queue full: record dropped");
      }
    }
  }
}

void loggerTask(void *parameter) {
  (void)parameter;
  LightingData result;

  while (true) {
    if (xQueueReceive(logQueue, &result,
                      portMAX_DELAY) == pdPASS) {
      Serial.printf(
        "[%03lu] LDR=%4u Norm=%.2f State=%-6s PWM=%3u Time=%lu ms\n",
        static_cast<unsigned long>(result.sensor.sampleNumber),
        result.sensor.raw,
        result.sensor.normalized,
        stateToString(result.state),
        result.pwmDuty,
        static_cast<unsigned long>(result.sensor.timestampMs)
      );

      if (xSemaphoreTake(statisticsMutex,
                         pdMS_TO_TICKS(50)) == pdTRUE) {
        statistics.samplesLogged++;
        xSemaphoreGive(statisticsMutex);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LDR_PIN, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(LDR_PIN, ADC_11db);

  if (!ledcAttach(LED_PIN, PWM_FREQUENCY, PWM_RESOLUTION)) {
    Serial.println("ERROR: PWM initialization failed");
    while (true) delay(1000);
  }
  ledcWrite(LED_PIN, 0);

  sensorQueue = xQueueCreate(10, sizeof(LDRData));
  logQueue = xQueueCreate(10, sizeof(LightingData));
  statisticsMutex = xSemaphoreCreateMutex();

  if (sensorQueue == nullptr || logQueue == nullptr ||
      statisticsMutex == nullptr) {
    Serial.println("ERROR: FreeRTOS object creation failed");
    while (true) delay(1000);
  }

  BaseType_t sensorCreated = xTaskCreate(
    sensorTask, "Sensor", 2048, nullptr, 2, nullptr
  );
  BaseType_t controlCreated = xTaskCreate(
    controlTask, "Control", 2048, nullptr, 2, nullptr
  );
  BaseType_t loggerCreated = xTaskCreate(
    loggerTask, "Logger", 3072, nullptr, 1, nullptr
  );

  if (sensorCreated != pdPASS || controlCreated != pdPASS ||
      loggerCreated != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) delay(1000);
  }

  Serial.println("FreeRTOS LDR Smart Lighting started");
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
```

## Expected Output

```text
FreeRTOS LDR Smart Lighting started
[001] LDR= 823 Norm=0.20 State=DARK   PWM=255 Time=1021 ms
[002] LDR=1562 Norm=0.38 State=NORMAL PWM=128 Time=2021 ms
[003] LDR=3468 Norm=0.85 State=BRIGHT PWM=  0 Time=3021 ms
```

## Calibration

1. Observe the ADC reading with the LDR covered.
2. Observe it under normal room lighting.
3. Illuminate it using a phone flashlight.
4. Select thresholds between the three observed regions.
5. Update `DARK_THRESHOLD` and `BRIGHT_THRESHOLD`.

If the ADC value decreases when the light becomes brighter, reverse the voltage-divider components or reverse the comparison logic.

## Assignment

Extend the program with the following features:

1. Add a `VERY_BRIGHT` state.
2. Make the NORMAL-state duty cycle proportional to the ADC value.
3. Print system statistics every 10 samples.
4. Add a button that enables and disables automatic lighting.
5. Use a binary semaphore for the button event.
6. Add hysteresis to prevent rapid state switching close to a threshold.
7. Report queue-full and mutex-timeout errors separately.
8. Draw a task-and-data-flow diagram of the final system.

## Checkpoint Questions

1. Why are sensing, control, and logging implemented as separate tasks?
2. Why are two queues used?
3. Which data require mutex protection?
4. Why does `xQueueReceive()` block indefinitely?
5. What is the ADC range at 12-bit resolution?
6. How is the normalized sensor value calculated?
7. What is the purpose of `vTaskDelayUntil()`?
8. Why might hysteresis improve this controller?
9. How should the design change if the LDR polarity is reversed?
10. Which parts of this system correspond to producer, buffer, and consumer?

---

# Summary: Selecting a FreeRTOS Mechanism

| Requirement | Recommended mechanism |
|---|---|
| Execute an independent activity | Task |
| Delay a task | `vTaskDelay()` |
| Run at a stable period | `vTaskDelayUntil()` |
| Transfer data between tasks | Queue |
| Protect a shared resource | Mutex |
| Notify a task of an event | Binary semaphore or task notification |
| Count repeated events/resources | Counting semaphore |
| Notify from an ISR | ISR-safe `...FromISR()` API |

# Relationship to pthread

| POSIX pthread | FreeRTOS |
|---|---|
| `pthread_create()` | `xTaskCreate()` |
| Thread function | Task function |
| `sleep()` | `vTaskDelay()` |
| `pthread_mutex_lock()` | `xSemaphoreTake()` on a mutex |
| `pthread_mutex_unlock()` | `xSemaphoreGive()` |
| Condition variable | Semaphore, notification, or event group |
| Circular buffer | Queue |


