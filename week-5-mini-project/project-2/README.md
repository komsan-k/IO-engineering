# 💡 7. Project 2: Gesture-Controlled Light

## 7.1 Project Description

Build a lighting controller that recognizes hand gestures using an **MPU6050**, an **ESP32**, and a **TinyML model**.

Attach the MPU6050 to the hand or a handheld object. The sensor measures motion while the ESP32 identifies the gesture and controls an LED.

## 7.2 Required Components

- ESP32 development board
- MPU6050 sensor module
- LED and 330 Ω resistor
- Breadboard and jumper wires
- USB cable

## 7.3 Sensor Measurements

The MPU6050 provides six measurements:

| Measurement | Signals | Unit |
|---|---|---|
| Acceleration | ax, ay, az | g |
| Angular velocity | gx, gy, gz | degrees/second |

Collect a short sequence of these measurements for each gesture.

## 7.4 Gesture Commands

| Class | Example Gesture | Lighting Action |
|---|---|---|
| 0 | Tilt left | Turn OFF |
| 1 | Tilt right | Turn ON |
| 2 | Move upward | Increase brightness |
| 3 | Move downward | Decrease brightness |
| 4 | Idle / no command | Keep the current state |

The **idle class** helps prevent unwanted commands when the user is not making a gesture.

## 7.5 System Operation

**Hand motion → MPU6050 → ESP32 → Motion features → TinyML prediction → LED control**

1. Read all six sensor signals at **50 samples/second**.
2. Group readings into a **one-second window**, containing 50 readings.
3. Calculate features such as the **mean, standard deviation, minimum, and maximum** of each signal.
4. Pass the features to the trained model.
5. Use the predicted gesture to update the LED.

Using four features for each of six signals produces **24 model inputs**.

## 7.6 Dataset and Training

Record repeated examples of each gesture, including idle periods. Use the same sensor position and sampling settings during collection and deployment.

Label each recording with its gesture class. Divide recordings into training, validation, and test sets, keeping readings from the same recording together.

Train a small classification model using TensorFlow/Keras, convert it to TensorFlow Lite, and export it as a C/C++ model header for ESP32.

**This project requires a new gesture dataset and model.** The earlier stationary/moving/shaking model does not recognize these lighting commands.

## 7.7 LED Control

Use a PWM-capable GPIO to adjust LED brightness.

| Command | Example Behavior |
|---|---|
| OFF | Set brightness to 0 |
| ON | Restore the last nonzero brightness |
| Brightness UP | Increase brightness by one step |
| Brightness DOWN | Decrease brightness by one step |
| Idle | Make no change |

Limit brightness to the supported range. Execute each gesture once, then wait for the hand to return to idle before accepting another command.

## 7.8 Core Tasks

1. Connect and test the MPU6050.
2. Collect and label gesture recordings.
3. Extract features from each time window.
4. Train and evaluate the gesture model.
5. Convert and deploy the model to ESP32.
6. Implement LED switching and PWM brightness control.
7. Test gesture recognition and unwanted activations.

## 7.9 Expected Outcome

Students demonstrate a light that responds to hand gestures and report **classification accuracy, response time, and the number of unwanted lighting commands during idle periods**.
