# 📘 Theory: Power Management, Sleep Modes, and LPWAN Technologies  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Power consumption is a major design consideration in **embedded systems, wireless sensor networks, and Internet of Things (IoT)** devices. Many embedded nodes operate from batteries or energy-harvesting sources and are expected to operate for months or years without maintenance.

The **ESP32** provides several mechanisms for reducing energy consumption, including:

- CPU frequency management,
- peripheral power control,
- modem power management,
- light sleep,
- deep sleep,
- wake-up sources,
- duty-cycled operation.

For long-distance IoT communication, the ESP32 can also be integrated with external **Low-Power Wide-Area Network (LPWAN)** technologies such as **LoRaWAN** or cellular IoT technologies such as **NB-IoT** and **LTE-M**, depending on the external radio/modem hardware used.

A low-power IoT architecture can therefore be summarized as:

$$
\boxed{
\text{Low-Power IoT} =
\text{Power Management}
+
\text{Sleep Modes}
+
\text{Energy-Efficient Communication}
}
$$

---

# ⚡ 2. Power Management in Embedded Systems

**Power management** refers to techniques used to reduce the energy consumed by an embedded system while maintaining the required functionality.

A typical ESP32 IoT node contains several power-consuming components:

```text
┌──────────────────────────┐
│        ESP32 Node        │
├──────────────────────────┤
│ CPU                      │
│ Memory                   │
│ Sensors                  │
│ GPIO / Peripherals       │
│ Wi-Fi                    │
│ Bluetooth / BLE          │
│ External LPWAN Radio     │
└──────────────────────────┘
```

The total system power can be approximated as:

$$
P_{\text{total}}
=
P_{\text{CPU}}
+
P_{\text{memory}}
+
P_{\text{radio}}
+
P_{\text{sensor}}
+
P_{\text{peripheral}}
$$

In many wireless IoT applications, radio communication can represent a significant portion of the energy consumption.

---

# 🔋 3. Power and Energy

Power is the rate at which electrical energy is consumed:

$$
P=VI
$$

where:

- $P$ = power in watts,
- $V$ = voltage,
- $I$ = current.

Energy consumed over time is:

$$
E=Pt
$$

or:

$$
E=VIt
$$

Therefore, reducing either **current consumption** or **active operating time** can reduce the total energy used by the system.

---

# ⏱️ 4. Average Power Consumption

An IoT node usually does not need to operate continuously.

Consider:

```text
Wake
 │
 ▼
Measure
 │
 ▼
Process
 │
 ▼
Transmit
 │
 ▼
Sleep
```

The average power becomes approximately:

$$
P_{\text{avg}}
=
\frac{
P_{\text{active}}t_{\text{active}}
+
P_{\text{sleep}}t_{\text{sleep}}
}{
t_{\text{active}}+t_{\text{sleep}}
}
$$

If:

$$
t_{\text{sleep}} \gg t_{\text{active}}
$$

then average power can be greatly reduced.

---

# 🔄 5. Duty Cycling

**Duty cycling** is one of the most important techniques for low-power embedded systems.

The duty cycle is:

$$
D=
\frac{t_{\text{active}}}{T}
$$

where:

$$
T=t_{\text{active}}+t_{\text{sleep}}
$$

Thus:

$$
D=
\frac{t_{\text{active}}}
{t_{\text{active}}+t_{\text{sleep}}}
$$

For example, if an ESP32 operates for 1 second and sleeps for 59 seconds:

$$
D=\frac{1}{60}\approx1.67\%
$$

The device is active for only a small fraction of the total operating time.

---

# 📊 6. Active and Sleep Operation

```text
Power
  ▲
  │       ┌───────┐
  │       │Active │
  │       │       │
  │───────┘       └────────────────
  │                    Sleep
  └───────────────────────────────► Time
```

A typical low-power IoT cycle is:

$$
\boxed{
\text{Wake}
\rightarrow
\text{Sense}
\rightarrow
\text{Process}
\rightarrow
\text{Communicate}
\rightarrow
\text{Sleep}
}
$$

---

# 🧠 7. ESP32 Power-Management Strategy

An ESP32 application can reduce power consumption at several levels:

```text
Application
    │
    ├── Reduce processing
    ├── Reduce sampling
    ├── Reduce transmissions
    │
    ▼
Power Management
    │
    ├── CPU frequency control
    ├── Peripheral management
    ├── Wi-Fi/BLE management
    └── Sleep modes
```

A good design does not simply put the processor to sleep. It considers the complete system.

---

# ⚙️ 8. Dynamic Frequency Scaling

The CPU does not always need to operate at maximum frequency.

Conceptually:

$$
\boxed{
\text{Lower CPU Activity}
\rightarrow
\text{Potentially Lower Power Consumption}
}
$$

Dynamic frequency management can adjust CPU operating conditions according to processing requirements.

Example:

```text
High Workload
     │
     ▼
Higher CPU Frequency

Low Workload
     │
     ▼
Lower CPU Frequency / Idle
```

The exact frequency-management capabilities depend on the specific ESP32 variant and software framework.

---

# 📡 9. Wireless Radio Power Consumption

Wireless communication can consume substantial energy.

A simplified radio operating model is:

```text
Radio
 │
 ├── Transmit
 ├── Receive
 ├── Idle
 └── Sleep
```

Typically:

$$
P_{\text{TX/RX}}
>
P_{\text{sleep}}
$$

Therefore, an important optimization is:

$$
\boxed{
\text{Minimize Radio-On Time}
}
$$

---

# 📦 10. Reduce Communication Frequency

Instead of transmitting every sensor sample:

```text
Sample → Transmit
Sample → Transmit
Sample → Transmit
Sample → Transmit
```

the ESP32 can buffer or process samples:

```text
Sample ─┐
Sample ─┤
Sample ─┼──► Process / Aggregate ──► Transmit
Sample ─┘
```

For example:

$$
T_{\text{avg}}
=
\frac{1}{N}
\sum_{i=1}^{N}T_i
$$

Only the average or other useful feature may need to be transmitted.

---

# 🧠 11. Edge Processing for Power Reduction

Edge processing can reduce wireless communication.

```text
Raw Sensor Data
      │
      ▼
     ESP32
      │
      ├── Filtering
      ├── Averaging
      ├── Threshold Detection
      └── TinyML
             │
             ▼
       Important Result
             │
             ▼
          Transmit
```

Instead of sending all raw data, the ESP32 can send only significant events.

$$
\boxed{
\text{Local Processing}
\rightarrow
\text{Less Communication}
\rightarrow
\text{Potential Energy Savings}
}
$$

---

# 💤 12. ESP32 Sleep Modes

ESP32 devices provide sleep mechanisms that allow parts of the chip to reduce activity when full processing is unnecessary.

Two major sleep concepts commonly used in ESP32 applications are:

- **Light Sleep**
- **Deep Sleep**

The exact capabilities and retained domains vary among ESP32-family devices.

A simplified comparison is:

```text
Active
  │
  ▼
Light Sleep
  │
  ▼
Deep Sleep

Increasing power saving
        ↓
```

---

# 😴 13. Light Sleep

In **Light Sleep**, CPU execution is suspended while sufficient system state is retained to allow relatively quick continuation after wake-up.

Conceptually:

```text
Application Running
       │
       ▼
Enter Light Sleep
       │
       ▼
CPU Suspended
       │
       ▼
Wake Event
       │
       ▼
Resume Execution
```

Light Sleep is useful when:

- wake-up latency should be relatively short,
- the system needs frequent wake-ups,
- more execution state should be retained than in deep sleep.

---

# 🌙 14. Deep Sleep

**Deep Sleep** provides substantially greater power savings by powering down more of the system.

Conceptually:

```text
Application
    │
    ▼
Configure Wake Source
    │
    ▼
Enter Deep Sleep
    │
    ▼
Low-Power State
    │
    ▼
Wake Event
    │
    ▼
Boot / Resume from Wake Path
```

Deep Sleep is suitable for applications such as:

- environmental monitoring,
- agriculture sensors,
- remote telemetry,
- battery-operated IoT devices,
- periodic data logging.

---

# 📊 15. Light Sleep vs. Deep Sleep

| Feature | Light Sleep | Deep Sleep |
|---|---|---|
| CPU execution | Suspended | Powered down/non-operational |
| Power saving | High | Higher |
| Wake-up behavior | Faster continuation | Wake follows deep-sleep boot path |
| State retention | More | Limited/selected domains |
| Suitable for | Frequent short idle periods | Long inactive periods |

The exact current consumption depends on the ESP32 variant, board design, peripherals, regulator, GPIO states, and software configuration.

---

# ⏰ 16. Wake-Up Sources

An ESP32 can wake from sleep using supported wake-up sources.

Common examples include:

- timer,
- external GPIO,
- touch input on supported devices,
- ULP-related events on supported devices,
- communication/peripheral events in supported sleep configurations.

A basic process is:

```text
Configure Wake Source
        │
        ▼
Enter Sleep
        │
        ▼
Wait
        │
        ▼
Wake Event Occurs
        │
        ▼
ESP32 Wakes
```

---

# ⏱️ 17. Timer Wake-Up

Timer wake-up is useful for periodic sensing.

Example:

```text
Sleep
  │
  │ 60 seconds
  ▼
Wake
  │
  ▼
Read Sensor
  │
  ▼
Transmit
  │
  ▼
Sleep
```

The cycle can be expressed as:

$$
T_{\text{cycle}}
=
T_{\text{active}}
+
T_{\text{sleep}}
$$

---

# 🔘 18. External Wake-Up

A GPIO event can be used to wake the ESP32 in supported configurations.

Example:

```text
ESP32 Sleeping
      │
      ▼
Push Button / Sensor Event
      │
      ▼
GPIO Wake-Up
      │
      ▼
ESP32 Active
```

This is useful for event-driven applications.

---

# 🚪 19. Event-Driven Power Management

Instead of repeatedly polling:

```text
Check Sensor
Check Sensor
Check Sensor
Check Sensor
```

an event-driven system can sleep:

```text
Sleep
  │
  ▼
External Event
  │
  ▼
Wake
  │
  ▼
Process
```

Thus:

$$
\boxed{
\text{Event-Driven Design}
\rightarrow
\text{Reduced Unnecessary CPU Activity}
}
$$

---

# 💾 20. State Retention

When entering low-power modes, developers must consider which information must survive sleep.

Possible storage locations include:

```text
Runtime Variables
      │
      ├── Normal RAM
      ├── RTC-retained memory
      └── Non-volatile storage
```

For long-term persistence, information can be stored in non-volatile memory.

For short sleep/wake cycles, supported RTC-retained memory can be useful.

---

# 💻 21. ESP32 Deep-Sleep Example

A simplified Arduino-style example is:

```cpp
#include <Arduino.h>

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  60

void setup()
{
    Serial.begin(115200);

    Serial.println("ESP32 is awake");

    // Perform sensor measurement here

    esp_sleep_enable_timer_wakeup(
        TIME_TO_SLEEP * uS_TO_S_FACTOR
    );

    Serial.println("Entering deep sleep");

    delay(100);

    esp_deep_sleep_start();
}

void loop()
{
}
```

The sequence is:

```text
Boot
 │
 ▼
Measure
 │
 ▼
Configure Timer
 │
 ▼
Deep Sleep
 │
 ▼
60 s
 │
 ▼
Wake
 │
 ▼
Boot
```

---

# 🔋 22. Battery-Life Estimation

A simple idealized battery-life estimate is:

$$
T_{\text{battery}}
=
\frac{C_{\text{battery}}}
{I_{\text{avg}}}
$$

where:

- $C_{\text{battery}}$ = battery capacity in Ah,
- $I_{\text{avg}}$ = average current in A.

For capacity in mAh and current in mA:

$$
T_{\text{hours}}
=
\frac{C_{\text{mAh}}}
{I_{\text{avg,mA}}}
$$

This is only an approximation because real battery life also depends on:

- battery chemistry,
- temperature,
- discharge characteristics,
- regulator efficiency,
- self-discharge,
- peak current,
- battery aging.

---

# 📊 23. Average Current

For two operating states:

$$
I_{\text{avg}}
=
\frac{
I_{\text{active}}t_{\text{active}}
+
I_{\text{sleep}}t_{\text{sleep}}
}{
t_{\text{active}}+t_{\text{sleep}}
}
$$

This equation demonstrates why long sleep periods can dramatically reduce average current.

---

# 🧮 24. Example Battery Calculation

Assume:

$$
I_{\text{active}}=80~\text{mA}
$$

$$
t_{\text{active}}=2~\text{s}
$$

$$
I_{\text{sleep}}=0.02~\text{mA}
$$

$$
t_{\text{sleep}}=58~\text{s}
$$

Then:

$$
I_{\text{avg}}
=
\frac{
(80)(2)+(0.02)(58)
}{
60
}
$$

$$
I_{\text{avg}}
\approx2.69~\text{mA}
$$

For an idealized 2000-mAh battery:

$$
T
=
\frac{2000}{2.69}
\approx743~\text{hours}
$$

or approximately:

$$
T\approx31~\text{days}
$$

This example is illustrative; a real development board may consume substantially more current because of regulators, LEDs, USB interfaces, sensors, and other components.

---

# 🌐 25. Why LPWAN?

Wi-Fi provides relatively high throughput but is not always ideal for battery-powered devices that must communicate over very long distances.

**Low-Power Wide-Area Network (LPWAN)** technologies target applications requiring combinations of:

- long communication range,
- low data rate,
- low energy consumption,
- small payloads,
- large numbers of devices,
- infrequent communication.

Conceptually:

$$
\boxed{
\text{LPWAN}
=
\text{Long Range}
+
\text{Low Data Rate}
+
\text{Low-Power-Oriented Operation}
}
$$

---

# 📡 26. LPWAN Application Areas

LPWAN technologies are commonly considered for:

- smart agriculture,
- smart metering,
- environmental monitoring,
- asset tracking,
- smart cities,
- remote sensing,
- infrastructure monitoring,
- industrial IoT.

Example:

```text
Remote Sensor
      │
      ▼
     ESP32
      │
      ▼
LPWAN Radio
      │
      ▼
Gateway / Base Station
      │
      ▼
Network / Cloud
```

---

# 🗺️ 27. Short-Range vs. Wide-Area IoT

A simplified comparison is:

| Technology | Typical Scope | Data Rate | Power Orientation |
|---|---|---:|---|
| BLE | Personal/local area | Moderate | Very low power |
| Wi-Fi | Local area | High | Higher |
| LoRaWAN | Wide area | Low | Low power |
| NB-IoT | Wide area/cellular | Low | Low-power IoT |
| LTE-M | Wide area/cellular | Higher than NB-IoT | Low-power IoT |

Actual range, throughput, latency, and energy use depend strongly on deployment and configuration.

---

# 📶 28. Major LPWAN Technologies

Important LPWAN technologies include:

```text
LPWAN
 │
 ├── LoRaWAN
 │
 ├── NB-IoT
 │
 └── LTE-M
```

Other LPWAN technologies and regional systems also exist, but these three provide useful examples for ESP32-based IoT study.

---

# 📡 29. LoRa and LoRaWAN

It is important to distinguish **LoRa** from **LoRaWAN**.

**LoRa** refers to a physical-layer radio modulation technology.

**LoRaWAN** defines a networking protocol and system architecture using LoRa radios.

Thus:

$$
\boxed{
\text{LoRa}
\neq
\text{LoRaWAN}
}
$$

Conceptually:

$$
\boxed{
\text{LoRaWAN}
=
\text{LoRa PHY}
+
\text{MAC/Network Architecture}
}
$$

---

# 🏗️ 30. LoRaWAN Architecture

A typical LoRaWAN architecture is:

```text
End Device
    │
    │ LoRa
    ▼
 Gateway
    │
    │ IP Network
    ▼
Network Server
    │
    ▼
Application Server
    │
    ▼
Application
```

Multiple gateways may receive the same uplink:

```text
                ┌──► Gateway 1 ──┐
ESP32 + LoRa ───┼──► Gateway 2 ──┼──► Network Server
                └──► Gateway 3 ──┘
```

---

# 📻 31. ESP32 + LoRa

The ESP32 generally requires an **external LoRa transceiver/module** for LoRa communication.

Example architecture:

```text
Sensor
  │
  ▼
ESP32
  │
  │ SPI
  ▼
LoRa Radio
  │
  │ RF
  ▼
LoRaWAN Gateway
```

Common LoRa radio families include devices from Semtech and modules built around compatible transceivers.

---

# 🌾 32. Smart Agriculture Example

```text
Soil Sensor
     │
     ▼
   ESP32
     │
     ▼
 LoRa Radio
     │
     │ Long-Range Link
     ▼
LoRaWAN Gateway
     │
     ▼
Cloud Platform
     │
     ▼
Farmer Dashboard
```

The node can:

1. wake periodically,
2. measure soil moisture,
3. process the measurement,
4. transmit a short packet,
5. return to sleep.

---

# 🔋 33. LoRaWAN Device Classes

LoRaWAN defines three main end-device classes:

- **Class A**
- **Class B**
- **Class C**

---

# 🅰️ 34. LoRaWAN Class A

Class A is designed for highly energy-conscious devices.

```text
Uplink
  │
  ▼
Transmit
  │
  ├── RX1
  │
  └── RX2
  │
  ▼
Sleep
```

Downlink opportunities are primarily opened after an uplink.

Class A is mandatory for LoRaWAN end devices and generally provides the lowest-power operating model.

---

# 🅱️ 35. LoRaWAN Class B

Class B adds scheduled receive opportunities synchronized using network timing/beacons.

Conceptually:

```text
Sleep
 │
 ▼
Scheduled Receive Window
 │
 ▼
Sleep
 │
 ▼
Scheduled Receive Window
```

It provides more predictable downlink opportunities than Class A at the cost of additional energy use and complexity.

---

# 🅲️ 36. LoRaWAN Class C

Class C devices keep receive windows open for much more of the time when they are not transmitting.

```text
Receive
   │
   ├── Receive
   ├── Receive
   ├── Transmit
   └── Receive
```

This reduces downlink latency but increases power consumption.

A simplified comparison is:

| Class | Power Demand | Downlink Availability |
|---|---|---|
| A | Lowest | After uplink |
| B | Medium | Scheduled |
| C | Highest | Nearly continuous when not transmitting |

---

# 📊 37. LoRaWAN Data-Rate Trade-Off

LoRa systems involve trade-offs among:

- range,
- data rate,
- airtime,
- robustness,
- energy consumption.

Conceptually:

$$
\boxed{
\text{Longer Airtime}
\leftrightarrow
\text{Greater Energy per Transmission}
}
$$

Therefore, LPWAN applications generally benefit from short, infrequent messages.

---

# 📱 38. NB-IoT

**Narrowband Internet of Things (NB-IoT)** is a cellular LPWAN technology standardized by 3GPP.

It is designed for applications such as:

- smart meters,
- environmental monitoring,
- utility infrastructure,
- stationary sensors,
- low-throughput telemetry.

Architecture:

```text
Sensor
  │
  ▼
ESP32
  │
  │ UART
  ▼
NB-IoT Modem
  │
  ▼
Cellular Network
  │
  ▼
Internet / Cloud
```

The ESP32 itself does not provide an integrated NB-IoT cellular modem in standard ESP32 configurations, so an external cellular module is generally required.

---

# 📶 39. LTE-M

**LTE-M**, also known as **LTE Cat-M1** in common deployments, is another cellular IoT technology.

Compared with NB-IoT, LTE-M can support application requirements involving:

- higher data rates,
- lower latency in many scenarios,
- mobility support,
- more interactive applications.

Example:

```text
ESP32
  │
  ▼
LTE-M Modem
  │
  ▼
Cellular Base Station
  │
  ▼
Mobile Network
  │
  ▼
Cloud
```

---

# 🆚 40. NB-IoT vs. LTE-M

A conceptual comparison is:

| Characteristic | NB-IoT | LTE-M |
|---|---|---|
| Cellular IoT | Yes | Yes |
| Data rate | Lower | Higher |
| Mobility | More limited | Better supported |
| Low-power features | Yes | Yes |
| Typical use | Metering/static sensing | Tracking/interactive IoT |
| External modem for ESP32 | Typically | Typically |

Availability depends on the mobile operator and country.

---

# 📊 41. BLE, Wi-Fi, LoRaWAN, NB-IoT, and LTE-M

| Feature | BLE | Wi-Fi | LoRaWAN | NB-IoT | LTE-M |
|---|---|---|---|---|---|
| Network scope | Short | Local | Wide | Wide | Wide |
| Data rate | Moderate | High | Low | Low | Moderate |
| Power orientation | Very low | Higher | Low | Low | Low |
| Infrastructure | Phone/gateway | AP/router | LoRaWAN gateway | Cellular | Cellular |
| ESP32 radio built in | Variant-dependent BLE | Commonly yes | No | No | No |
| Typical application | Wearables | Local IoT | Remote sensing | Metering | Tracking/mobile IoT |

This table is conceptual rather than a universal performance ranking.

---

# 🧠 42. Selecting a Communication Technology

The choice of wireless technology depends on:

$$
\boxed{
\text{Technology Selection}
=
f(
\text{Range},
\text{Data Rate},
\text{Power},
\text{Latency},
\text{Cost},
\text{Coverage}
)
}
$$

For example:

```text
Wearable Sensor
      │
      └──► BLE

Home Camera
      │
      └──► Wi-Fi

Remote Farm Sensor
      │
      └──► LoRaWAN

Utility Meter
      │
      └──► NB-IoT

Mobile Asset Tracker
      │
      └──► LTE-M
```

These are typical examples rather than strict rules.

---

# 🔋 43. LPWAN and Sleep Modes

LPWAN is most effective for battery-powered systems when combined with aggressive sleep management.

```text
Deep Sleep
    │
    ▼
Timer Wake-Up
    │
    ▼
Sensor Measurement
    │
    ▼
Data Processing
    │
    ▼
LPWAN Transmission
    │
    ▼
Deep Sleep
```

Thus:

$$
\boxed{
\text{Low-Power Sensor Node}
=
\text{Deep Sleep}
+
\text{Short Active Time}
+
\text{Short LPWAN Transmission}
}
$$

---

# 📡 44. Energy per Message

Communication efficiency should consider energy rather than only instantaneous power.

Energy required for a transmission can be approximated as:

$$
E_{\text{TX}}
=
P_{\text{TX}}t_{\text{TX}}
$$

Thus, a high-power transmission of very short duration may sometimes consume less energy than a lower-power radio operating for a much longer duration.

This is why **airtime** is important in low-power wireless design.

---

# 📦 45. Payload Optimization

Suppose sensor readings are transmitted as verbose text:

```json
{
  "temperature": 28.5,
  "humidity": 70.2,
  "battery": 3.8
}
```

A constrained LPWAN application may instead use a compact binary representation.

Conceptually:

```text
Temperature | Humidity | Battery
   2 B      |   2 B    |   1 B
```

Smaller payloads can reduce:

- airtime,
- energy consumption,
- network usage.

---

# 🔄 46. Transmission Interval

Another important design parameter is the transmission interval.

```text
Transmit every 1 s
→ High communication activity

Transmit every 1 min
→ Lower communication activity

Transmit every 1 h
→ Much lower communication activity
```

The appropriate interval depends on the application.

For environmental monitoring, rapidly sending unchanged data may be unnecessary.

---

# 🚨 47. Event-Based Transmission

Instead of periodic transmission:

```text
Every minute → Send
Every minute → Send
Every minute → Send
```

the system can transmit only when:

$$
|x_n-x_{n-1}|>\Delta_{\text{threshold}}
$$

For example:

```text
Temperature changes significantly
             │
             ▼
          Wake Radio
             │
             ▼
          Transmit
```

This can reduce communication energy.

---

# 🧠 48. TinyML and LPWAN

Embedded intelligence can further reduce transmissions.

```text
Sensor
  │
  ▼
ESP32
  │
  ▼
TinyML
  │
  ├── Normal → No transmission
  │
  └── Abnormal → LPWAN transmission
```

For example:

$$
\boxed{
\text{AI Event Detection}
\rightarrow
\text{Transmit Only Important Events}
}
$$

This is useful for:

- predictive maintenance,
- intrusion detection,
- health monitoring,
- environmental anomaly detection.

---

# 🪞 49. LPWAN and Digital Twins

A digital twin may receive periodic or event-driven state updates from a remote physical system.

```text
Physical Asset
      │
      ▼
Sensor + ESP32
      │
      ▼
LPWAN
      │
      ▼
Cloud Platform
      │
      ▼
Digital Twin
```

The communication frequency should match the required fidelity of the digital representation.

---

# ☀️ 50. Energy Harvesting

Low-power IoT nodes can sometimes combine sleep modes with energy harvesting.

Possible sources include:

- solar energy,
- vibration,
- thermal gradients,
- RF energy in specialized applications.

Example:

```text
Solar Panel
    │
    ▼
Power Management
    │
    ▼
Battery / Supercapacitor
    │
    ▼
ESP32 + Sensor + LPWAN
```

Energy-neutral operation conceptually requires:

$$
E_{\text{consumed}}
\le
E_{\text{harvested}}
$$

over a sufficiently long operating period.

---

# 🔌 51. Peripheral Power Management

Putting the ESP32 into deep sleep is not sufficient if external components remain active.

Consider:

```text
ESP32 Sleep
   │
   ├── Sensor still ON
   ├── LED still ON
   ├── LoRa module still ON
   └── Regulator losses
```

The system's actual current is:

$$
I_{\text{system}}
=
I_{\text{ESP32}}
+
I_{\text{sensors}}
+
I_{\text{radio}}
+
I_{\text{regulator}}
+
I_{\text{other}}
$$

Therefore, low-power design must consider the **whole board**, not just the MCU.

---

# 🔘 52. Power-Gating Sensors

An ESP32 can control the power supplied to suitable external sensors using appropriate power-switch circuitry.

```text
ESP32 GPIO
     │
     ▼
MOSFET / Load Switch
     │
     ▼
Sensor Power
```

Operation:

```text
Wake
 │
 ▼
Sensor ON
 │
 ▼
Wait for stabilization
 │
 ▼
Measure
 │
 ▼
Sensor OFF
 │
 ▼
Sleep
```

Care must be taken not to power sensors directly from GPIO pins unless electrical specifications explicitly permit it.

---

# 💡 53. Development Board vs. MCU Power

A common laboratory mistake is assuming the current measured at the USB input equals the MCU's deep-sleep current.

A development board may include:

```text
USB-UART Converter
Voltage Regulator
Power LED
Status LED
Sensors
Pull-Up Resistors
Other Components
```

Therefore:

$$
\boxed{
I_{\text{board}}
\neq
I_{\text{MCU}}
}
$$

For accurate low-power evaluation, the entire hardware design must be considered.

---

# 📏 54. Measuring Power Consumption

Useful equipment includes:

- digital multimeter,
- oscilloscope with current-sense circuit,
- source-measure unit,
- dedicated power profiler.

A simple measurement arrangement is:

```text
Power Supply
     │
     ▼
Current Meter
     │
     ▼
ESP32 System
```

Measure current during:

```text
Active
Wi-Fi TX
BLE operation
LPWAN TX
Light Sleep
Deep Sleep
```

---

# 📈 55. Energy Profiling

A current profile might appear as:

```text
Current
  ▲
  │       ┌────┐       ┌───┐
  │       │ TX │       │TX │
  │   ┌───┘    └───┐   │   │
  │───┘ Processing └───┘   └────
  │
  │_______Sleep_________________
  └────────────────────────────► Time
```

The energy used during one cycle is:

$$
E_{\text{cycle}}
=
\int_0^T V(t)I(t)\,dt
$$

If voltage is approximately constant:

$$
E_{\text{cycle}}
\approx
V\int_0^T I(t)\,dt
$$

---

# 🔄 56. Low-Power State Machine

A useful firmware architecture is:

```text
┌───────────┐
│   WAKE    │
└─────┬─────┘
      ▼
┌───────────┐
│  SENSE    │
└─────┬─────┘
      ▼
┌───────────┐
│ PROCESS   │
└─────┬─────┘
      ▼
┌───────────┐
│ TRANSMIT  │
└─────┬─────┘
      ▼
┌───────────┐
│   SLEEP   │
└─────┬─────┘
      │
      └──────────► WAKE
```

This can be represented as:

$$
\boxed{
S_0
\rightarrow
S_1
\rightarrow
S_2
\rightarrow
S_3
\rightarrow
S_4
\rightarrow
S_0
}
$$

where:

- $S_0$ = Wake,
- $S_1$ = Sense,
- $S_2$ = Process,
- $S_3$ = Transmit,
- $S_4$ = Sleep.

---

# 🧪 57. Exercise 1 — Deep-Sleep Timer

Configure an ESP32 to:

1. wake,
2. blink an LED,
3. print a message,
4. configure a 10-second wake timer,
5. enter deep sleep,
6. wake again.

Expected cycle:

```text
Wake
 │
 ▼
LED Blink
 │
 ▼
Serial Message
 │
 ▼
Deep Sleep
 │
 ▼
10 seconds
 │
 ▼
Wake
```

---

# 🧪 58. Exercise 2 — Measure Current

Measure current during:

| Operating State | Measured Current |
|---|---:|
| Active | |
| Wi-Fi connected | |
| Wi-Fi transmission | |
| Light Sleep | |
| Deep Sleep | |

Then calculate:

$$
\text{Power}=VI
$$

for each state.

---

# 🧮 59. Exercise 3 — Duty-Cycle Analysis

Assume:

```text
Active = 3 seconds
Sleep  = 57 seconds
```

Calculate:

$$
D=
\frac{3}{60}
$$

Therefore:

$$
D=0.05=5\%
$$

Modify the sleep time to:

```text
117 seconds
```

and calculate the new duty cycle.

---

# 🔋 60. Exercise 4 — Battery-Life Estimation

Given:

$$
C=2500~\text{mAh}
$$

$$
I_{\text{active}}=100~\text{mA}
$$

$$
t_{\text{active}}=2~s
$$

$$
I_{\text{sleep}}=0.05~\text{mA}
$$

$$
t_{\text{sleep}}=298~s
$$

Calculate:

1. average current,
2. ideal battery life in hours,
3. ideal battery life in days.

Then discuss why the real battery life will differ.

---

# 📡 61. Exercise 5 — Compare Wireless Technologies

Complete the table:

| Requirement | BLE | Wi-Fi | LoRaWAN | NB-IoT | LTE-M |
|---|---|---|---|---|---|
| Short-range wearable | | | | | |
| High data rate | | | | | |
| Remote farm sensor | | | | | |
| Smart meter | | | | | |
| Mobile asset tracking | | | | | |
| Smartphone connectivity | | | | | |

Explain your selections.

---

# 🌾 62. Exercise 6 — Smart Agriculture Node

Design:

```text
Soil Moisture Sensor
        │
        ▼
      ESP32
        │
        ▼
   LoRa Module
        │
        ▼
LoRaWAN Gateway
        │
        ▼
      Cloud
```

Requirements:

1. wake every 15 minutes,
2. power the sensor,
3. measure soil moisture,
4. process the reading,
5. transmit the value,
6. power down unnecessary peripherals,
7. enter deep sleep.

Draw the state machine and calculate the expected duty cycle.

---

# 🚨 63. Exercise 7 — Event-Based Transmission

Modify the sensor node so that it only transmits if:

$$
|x_n-x_{n-1}|>10
$$

Compare:

```text
Periodic Transmission
```

with:

```text
Event-Based Transmission
```

Discuss:

- energy consumption,
- network traffic,
- responsiveness,
- data completeness.

---

# 🧠 64. Advanced Exercise — Intelligent LPWAN Sensor

Develop a conceptual ESP32 node with:

```text
Sensor
  │
  ▼
ESP32
  │
  ├── Filtering
  ├── Feature Extraction
  └── TinyML
         │
         ▼
     Decision
      /     \
 Normal    Abnormal
   │          │
 Sleep      LPWAN TX
              │
              ▼
            Sleep
```

The system should:

1. remain in deep sleep when possible,
2. wake from a timer or external event,
3. sample the sensor,
4. perform local inference,
5. transmit only important events,
6. return to deep sleep.

---

# 💬 65. Discussion Questions

1. Why is power management important in embedded systems?
2. What is the difference between power and energy?
3. What is duty cycling?
4. How does duty cycling reduce average power?
5. What components contribute to ESP32 system power consumption?
6. Why can wireless communication dominate energy use?
7. What is Light Sleep?
8. What is Deep Sleep?
9. How do Light Sleep and Deep Sleep differ?
10. What is a wake-up source?
11. How does timer wake-up work?
12. Why is event-driven operation useful?
13. What is state retention?
14. How can battery life be estimated?
15. Why is theoretical battery life different from real battery life?
16. What is LPWAN?
17. Why is LPWAN useful for IoT?
18. What is the difference between LoRa and LoRaWAN?
19. What is a LoRaWAN gateway?
20. What is LoRaWAN Class A?
21. How do Classes A, B, and C differ?
22. What is NB-IoT?
23. What is LTE-M?
24. How do NB-IoT and LTE-M differ conceptually?
25. Why does an ESP32 normally need external hardware for LoRaWAN or cellular LPWAN?
26. Why should LPWAN payloads generally be compact?
27. How can edge processing reduce communication energy?
28. How can TinyML improve low-power IoT operation?
29. Why must external sensors be considered in the power budget?
30. Why can a development board consume more power than the ESP32 MCU itself?

---

# 🧾 66. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain power and energy consumption,
- calculate electrical power,
- calculate approximate energy consumption,
- explain duty cycling,
- calculate average current,
- estimate battery life,
- describe ESP32 power-management principles,
- explain Light Sleep,
- explain Deep Sleep,
- identify wake-up sources,
- implement timer-based deep sleep,
- describe event-driven low-power operation,
- explain state retention,
- identify radio-related energy costs,
- optimize sensor and communication intervals,
- explain LPWAN,
- distinguish LoRa from LoRaWAN,
- explain LoRaWAN architecture,
- distinguish LoRaWAN Classes A, B, and C,
- explain NB-IoT,
- explain LTE-M,
- compare BLE, Wi-Fi, LoRaWAN, NB-IoT, and LTE-M,
- select a suitable wireless technology for an IoT application,
- integrate sleep modes with LPWAN communication,
- design event-driven IoT nodes,
- explain power gating,
- perform basic energy profiling,
- design a low-power ESP32 system.

---

# 🔑 67. Key Concepts

### Electrical Power

$$
\boxed{
P=VI
}
$$

### Energy

$$
\boxed{
E=Pt=VIt
}
$$

### Duty Cycle

$$
\boxed{
D=
\frac{t_{\text{active}}}
{t_{\text{active}}+t_{\text{sleep}}}
}
$$

### Average Current

$$
\boxed{
I_{\text{avg}}
=
\frac{
I_{\text{active}}t_{\text{active}}
+
I_{\text{sleep}}t_{\text{sleep}}
}{
t_{\text{active}}+t_{\text{sleep}}
}
}
$$

### Ideal Battery Life

$$
\boxed{
T_{\text{battery}}
=
\frac{C_{\text{battery}}}
{I_{\text{avg}}}
}
$$

### Low-Power ESP32 Operation

$$
\boxed{
\text{Wake}
\rightarrow
\text{Sense}
\rightarrow
\text{Process}
\rightarrow
\text{Transmit}
\rightarrow
\text{Sleep}
}
$$

### LPWAN

$$
\boxed{
\text{LPWAN}
=
\text{Long Range}
+
\text{Low Data Rate}
+
\text{Low-Power-Oriented Communication}
}
$$

### Low-Power Intelligent IoT

$$
\boxed{
\text{Sensor}
+
\text{ESP32}
+
\text{Edge Intelligence}
+
\text{LPWAN}
+
\text{Sleep Management}
}
$$

---

# 🎯 68. Summary

Power management is fundamental to the design of **battery-powered ESP32 and IoT systems**. The energy consumption of a device depends not only on the ESP32 processor but also on its wireless radios, sensors, external modules, voltage regulators, and other peripherals.

A key strategy is **duty-cycled operation**:

$$
\boxed{
\text{Wake}
\rightarrow
\text{Measure}
\rightarrow
\text{Process}
\rightarrow
\text{Communicate}
\rightarrow
\text{Sleep}
}
$$

The ESP32 provides sleep mechanisms such as **Light Sleep** and **Deep Sleep**. Light Sleep is useful for relatively short inactive periods where faster continuation is desirable, while Deep Sleep is especially useful for long-duration low-power sensing applications.

For remote IoT systems, LPWAN technologies extend the communication range while supporting applications with relatively small and infrequent data transfers. Important technologies include **LoRaWAN, NB-IoT, and LTE-M**.

A remote sensor architecture may therefore be:

```text
Sensor
  │
  ▼
ESP32
  │
  ├── Local Processing
  ├── TinyML
  └── Power Management
          │
          ▼
      LPWAN Radio
          │
          ▼
 Gateway / Cellular Network
          │
          ▼
        Cloud
```

The most effective low-power design combines **sleep modes, short active periods, efficient sensing, compact data representation, reduced radio-on time, and appropriate wireless technology selection**.

Thus:

$$
\boxed{
\text{Efficient IoT}
=
\text{Power Management}
+
\text{Duty Cycling}
+
\text{Edge Processing}
+
\text{Efficient Communication}
}
$$

These principles provide the foundation for developing **smart agriculture nodes, environmental monitors, asset trackers, smart meters, industrial sensors, digital-twin endpoints, and intelligent low-power embedded systems**.

---

# 📘 References

1. Espressif Systems, *ESP-IDF Programming Guide — Power Management*.
2. Espressif Systems, *ESP-IDF Programming Guide — Sleep Modes*.
3. Espressif Systems, *ESP32 Series Datasheets and Technical Reference Manuals*.
4. LoRa Alliance, *LoRaWAN Specification*.
5. LoRa Alliance, *LoRaWAN Regional Parameters*.
6. 3GPP, specifications for *Cellular Internet of Things, NB-IoT, and LTE-M*.
7. Bluetooth SIG, *Bluetooth Core Specification*.
8. FreeRTOS, *FreeRTOS Kernel Documentation*.
9. J. P. Vasseur and A. Dunkels, *Interconnecting Smart Objects with IP: The Next Internet*, Morgan Kaufmann.
10. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*, O'Reilly Media.

---

