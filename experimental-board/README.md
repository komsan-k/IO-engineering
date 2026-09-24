# KidBright Advanced User Board V1.6 --- Interface Guide

## 1. Introduction

The **KidBright Advanced User Board V1.6** is an ESP32-based embedded
learning and prototyping board that integrates digital and analog I/O,
sensors, display, communication interfaces, switches, buzzer, servo
connectors, and expansion ports on a single board.

This guide summarizes the interfaces shown in the supplied **KidBright
Advanced User Diagram V1.6** and is intended as a quick reference for
laboratory work and Arduino/ESP32 programming.

> **Note:** The GPIO assignments below are transcribed from the uploaded
> board diagram. Always verify the board revision before connecting
> external hardware.

## KidBright Advanced User Board V1.6

The following figure illustrates the hardware interfaces and GPIO assignments of the **KidBright Advanced User Board V1.6**.

<p align="center">
  <img src="Diagram_KB_1.6-01.jpg"
       alt="KidBright Advanced User Board V1.6 Interface Diagram"
       width="900">
</p>

<p align="center">
  <b>Figure 1.</b> KidBright Advanced User Board V1.6 interface and GPIO diagram.
</p>
------------------------------------------------------------------------

## 2. Main Interface Summary

  ---------------------------------------------------------------------------------
  Interface /     Label on           ESP32 GPIO / Type          Typical Use
  Device          Board                    Signal               
  --------------- ------------- ----------------- ------------- -------------------
  Digital I/O     IO23                     GPIO23 Digital I/O   External digital
                                                                device

  Digital I/O     IO19                     GPIO19 Digital I/O   External digital
                                                                device

  Digital I/O     IO18                     GPIO18 Digital I/O   External digital
                                                                device

  I2C SDA         SDA0                     GPIO21 I2C           On-board I2C bus

  I2C SCL         SCL0                     GPIO22 I2C           On-board I2C bus

  Analog input    VN                           VN Analog input  External analog
                                                                signal

  LDR             LDR                      GPIO36 Analog input  Ambient light
                                                                sensing

  IoT interface   IOT                      GPIO12 Digital I/O   IoT-related
                                                                interface

  Wi-Fi interface WIFI                      GPIO2 Digital I/O   Board Wi-Fi-related
                                                                interface

  Buzzer          BUZZER                   GPIO13 Digital       Sound / alarm
                                                  output        

  Switch 1        SW1                      GPIO16 Digital input User push button

  Switch 2        SW2                      GPIO14 Digital input User push button

  Servo 1         servo1                   GPIO17 PWM-capable   Servo control
                                                  output        

  Servo 2         servo2                   GPIO15 PWM-capable   Servo control
                                                  output        

  USB Type-A      USB Type A      GPIO25 shown in Board         USB/power-related
  Power                                   diagram interface     interface

  RGB             GERORA RGB                  --- On-board      RGB indication
                                                  device        

  Accelerometer & ---                         --- On-board      Motion sensing
  Gyroscope                                       sensor        
  ---------------------------------------------------------------------------------

------------------------------------------------------------------------

## 3. Three-Pin Input Connectors

The board provides four labeled input channels.

  Input       GPIO Direction   Suggested Function
  ------- -------- ----------- -----------------------------
  I1        GPIO32 Input       Digital/analog sensor input
  I2        GPIO33 Input       Digital/analog sensor input
  I3        GPIO34 Input       Sensor input
  I4        GPIO35 Input       Sensor input

The diagram also identifies the associated **3V3** and **GND** supply
connections.

### Example Arduino Definition

``` cpp
#define INPUT_I1 32
#define INPUT_I2 33
#define INPUT_I3 34
#define INPUT_I4 35
```

------------------------------------------------------------------------

## 4. Three-Pin Output Connectors

Two output channels are provided.

  Output       GPIO Direction   Typical Application
  -------- -------- ----------- --------------------------------
  O1         GPIO26 Output      LED, relay interface, actuator
  O2         GPIO27 Output      LED, relay interface, actuator

### Example Arduino Definition

``` cpp
#define OUTPUT_O1 26
#define OUTPUT_O2 27
```

------------------------------------------------------------------------

## 5. On-Board LDR Sensor

The board includes an **LDR (Light Dependent Resistor)** connected to:

  Device       GPIO Interface
  -------- -------- --------------------
  LDR        GPIO36 ADC / Analog input

### Arduino Example

``` cpp
#define LDR_PIN 36

void setup() {
  Serial.begin(115200);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);

  Serial.print("LDR = ");
  Serial.println(ldrValue);

  delay(500);
}
```

------------------------------------------------------------------------

## 6. Push Buttons

Two user switches are available.

  Switch       GPIO Type
  -------- -------- ---------------
  SW1        GPIO16 Digital input
  SW2        GPIO14 Digital input

### Arduino Example

``` cpp
#define SW1_PIN 16
#define SW2_PIN 14

void setup() {
  Serial.begin(115200);

  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
}

void loop() {
  Serial.print("SW1 = ");
  Serial.print(digitalRead(SW1_PIN));

  Serial.print("  SW2 = ");
  Serial.println(digitalRead(SW2_PIN));

  delay(200);
}
```

------------------------------------------------------------------------

## 7. Buzzer

The on-board buzzer is identified in the diagram as:

  Device       GPIO Direction
  -------- -------- -----------
  Buzzer     GPIO13 Output

### Arduino Definition

``` cpp
#define BUZZER_PIN 13
```

The buzzer can be used for alarms, notifications, and simple
sound-generation experiments.

------------------------------------------------------------------------

## 8. Servo Interfaces

Two servo interfaces are provided through the pin headers.

  Servo       Signal GPIO Power   Ground
  --------- ------------- ------- --------
  Servo 1          GPIO17 5V      G
  Servo 2          GPIO15 5V      G

### Arduino Definitions

``` cpp
#define SERVO1_PIN 17
#define SERVO2_PIN 15
```

These interfaces are suitable for experiments involving position control
and embedded actuators.

------------------------------------------------------------------------

## 9. I2C Interfaces

The diagram shows two I2C signal groups.

### I2C Bus 0

  Signal       GPIO
  -------- --------
  SDA0       GPIO21
  SCL0       GPIO22

This bus is associated in the diagram with the **HT16K33**.

### I2C Bus 1 / Expansion Connector

  Signal      GPIO
  -------- -------
  SDA1       GPIO4
  SCL1       GPIO5

The external **I2C connector** is labeled:

``` text
SDA1 : GPIO4
SCL1 : GPIO5
```

The diagram also associates these signals with the **LM73MK-1**.

### Arduino Example

``` cpp
#include <Wire.h>

#define SDA1_PIN 4
#define SCL1_PIN 5

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA1_PIN, SCL1_PIN);

  Serial.println("I2C initialized");
}

void loop() {
}
```

------------------------------------------------------------------------

## 10. HT16K33 Interface

The board diagram identifies the **HT16K33** display controller using:

  Signal       GPIO
  -------- --------
  SDA0       GPIO21
  SCL0       GPIO22

The HT16K33 is associated with the on-board LED matrix/display section.

------------------------------------------------------------------------

## 11. LM73MK-1 Interface

The diagram identifies an **LM73MK-1** device using:

  Signal      GPIO
  -------- -------
  SDA1       GPIO4
  SCL1       GPIO5

This shares the SDA1/SCL1 I2C signal pair shown at the external I2C
connector.

------------------------------------------------------------------------

## 12. General Expansion Header

The upper expansion header shown in the diagram exposes the following
labeled signals:

  Header Signal   Description
  --------------- ------------------
  3V3             3.3-V supply
  GND             Ground
  IO23            GPIO23
  IO19            GPIO19
  IO18            GPIO18
  SDA0            GPIO21 I2C data
  SCL0            GPIO22 I2C clock
  3V3             3.3-V supply
  GND             Ground
  VN              Analog input

This header is useful for connecting external sensors, communication
modules, and prototype circuits.

------------------------------------------------------------------------

## 13. Power Interfaces

  -----------------------------------------------------------------------
  Interface               Voltage / Signal        Purpose
  ----------------------- ----------------------- -----------------------
  3V3                     3.3 V                   Logic/sensor supply

  5V                      5 V                     Servo/external-device
                                                  supply

  GND / G                 Ground                  Common reference

  Terminal connector      5 V                     External 5-V connection

  USB Type-A Power        USB power interface     External
                                                  USB/power-related
                                                  connection
  -----------------------------------------------------------------------

> Ensure that the voltage requirements of an external sensor or actuator
> are compatible with the board before connection.

------------------------------------------------------------------------

## 14. Complete GPIO Quick Reference

      GPIO Board Function / Label
  -------- -----------------------------------
     GPIO2 WIFI
     GPIO4 SDA1
     GPIO5 SCL1
    GPIO12 IOT
    GPIO13 Buzzer
    GPIO14 SW2
    GPIO15 Servo 2
    GPIO16 SW1
    GPIO17 Servo 1
    GPIO18 IO18
    GPIO19 IO19
    GPIO21 SDA0
    GPIO22 SCL0
    GPIO23 IO23
    GPIO25 USB Type-A Power label in diagram
    GPIO26 O1
    GPIO27 O2
    GPIO32 I1
    GPIO33 I2
    GPIO34 I3
    GPIO35 I4
    GPIO36 LDR
        VN Analog input

------------------------------------------------------------------------

## 15. Interface Classification

  Category                Interfaces
  ----------------------- ----------------------------------------------------
  **Analog sensing**      LDR GPIO36, input channels, VN
  **Digital input**       SW1 GPIO16, SW2 GPIO14, I1--I4
  **Digital output**      O1 GPIO26, O2 GPIO27, Buzzer GPIO13
  **Actuator control**    Servo 1 GPIO17, Servo 2 GPIO15
  **I2C communication**   SDA0/SCL0 GPIO21/22, SDA1/SCL1 GPIO4/5
  **Expansion GPIO**      GPIO23, GPIO19, GPIO18
  **On-board display**    HT16K33 via SDA0/SCL0
  **On-board sensing**    LDR, LM73MK-1, accelerometer & gyroscope
  **Power**               3V3, 5V, GND, terminal connector, USB Type-A Power

------------------------------------------------------------------------

## 16. Typical Laboratory Applications

The KidBright Advanced User Board can support experiments such as:

1.  **Digital Input/Output** --- switches and LEDs.
2.  **ADC Experiments** --- LDR and analog sensors.
3.  **PWM and Actuator Control** --- servo motors and output devices.
4.  **I2C Communication** --- external sensors and on-board peripherals.
5.  **FreeRTOS Tasks** --- concurrent sensor and actuator tasks.
6.  **Interrupt Programming** --- push-button event handling.
7.  **IoT Applications** --- sensor acquisition and network
    communication.
8.  **Embedded Control Systems** --- sensor-to-actuator applications.

------------------------------------------------------------------------

## 17. Simplified Board Interface Map

``` text
                         KidBright Advanced V1.6
                                  |
       +--------------------------+--------------------------+
       |                          |                          |
       v                          v                          v
    INPUTS                     OUTPUTS                  COMMUNICATION
       |                          |                          |
 I1  GPIO32                  O1 GPIO26                 I2C0
 I2  GPIO33                  O2 GPIO27               SDA GPIO21
 I3  GPIO34                  Buzzer GPIO13           SCL GPIO22
 I4  GPIO35                  Servo1 GPIO17
 LDR GPIO36                  Servo2 GPIO15             I2C1
 SW1 GPIO16                                           SDA GPIO4
 SW2 GPIO14                                           SCL GPIO5
```

------------------------------------------------------------------------

## 18. Summary

The **KidBright Advanced User Board V1.6** integrates the major
interfaces required for embedded-system laboratories:

-   GPIO input and output
-   ADC sensing
-   User switches
-   LDR light sensing
-   Buzzer
-   Servo interfaces
-   I2C communication
-   On-board display interface
-   Motion sensing
-   Expansion connectors
-   3.3-V and 5-V power interfaces

The board is therefore suitable for progressing from basic Arduino
programming to **sensor interfacing, multitasking, FreeRTOS,
communication, and IoT applications**.
