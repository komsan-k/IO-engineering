# MPU6050 Tutorial

This section walks through an MPU6050 I²C example, line by line, and adds clear comments for teaching or documentation. It also shows how to convert raw readings to physical units and how to estimate orientation using a complementary filter.

---

## 1. Explained Code 

```cpp
#include <Wire.h>              // I2C library
#define MPU6050_ADDR 0x68      // MPU6050 I2C address (AD0=LOW -> 0x68)

int16_t ax, ay, az;            // Accelerometer raw X, Y, Z
int16_t gx, gy, gz;            // Gyroscope raw X, Y, Z

void setup() {
  Serial.begin(115200);        // Serial for debugging
  Wire.begin(4, 5);            // ESP32: SDA=GPIO4, SCL=GPIO5

  // Wake up the MPU6050 (by default it starts in sleep mode).
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);            // Power Management 1 register
  Wire.write(0x00);            // Clear SLEEP bit -> wake device
  Wire.endTransmission(true);
}

void loop() {
  // 1) Set start address to ACCEL_XOUT_H (0x3B)
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false); // Repeated start

  // 2) Request 14 bytes: accel(6) + temp(2) + gyro(6)
  Wire.requestFrom(MPU6050_ADDR, 14, true);

  // 3) Read accelerometer
  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();

  // 4) Read temperature (not used below)
  int16_t temp = (Wire.read() << 8) | Wire.read();

  // 5) Read gyroscope
  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();

  // 6) Print raw results
  Serial.println("MPU6050 Readings:");
  Serial.print("Accel (raw): "); Serial.print(ax); Serial.print("  ");
  Serial.print(ay); Serial.print("  "); Serial.println(az);
  Serial.print("Gyro  (raw): "); Serial.print(gx); Serial.print("  ");
  Serial.print(gy); Serial.print("  "); Serial.println(gz);
  Serial.println("--------------------");

  delay(500); // ~2 Hz for readability
}
```

---

## 2. Key Concepts

- **I²C control:**  
  `Wire.begin(4,5)` selects ESP32 GPIO4/5 as SDA/SCL.  
  Use `beginTransmission` + `write` to address registers, and `requestFrom` to read back data.

- **Register map:**  
  `0x6B` = Power Management 1 (writing `0x00` wakes device).  
  `0x3B` = ACCEL_XOUT_H (start of contiguous accel/temp/gyro block).

- **Data format:**  
  Accelerometer and gyroscope outputs are 16-bit signed integers (`int16_t`) per axis.  
  Temperature is also 16-bit signed, converted linearly to °C.

---

## 3. Raw → Physical Units

For default full-scale ranges:

- Accelerometer ±2g  
- Gyroscope ±250 °/s  

The scale factors are:

$$
A[g] = \frac{\text{raw}}{16384}
$$

$$
G[°/s] = \frac{\text{raw}}{131}
$$

$$
T[°C] = \frac{\text{raw}}{340} + 36.53
$$

---

## 4. Human-Readable Printout with Conversions

```cpp
#include <Wire.h>
#define MPU6050_ADDR 0x68

// Raw variables
int16_t ax, ay, az, gx, gy, gz, tempRaw;

// Scale factors
const float ACCEL_SCALE = 16384.0f;  // LSB per g
const float GYRO_SCALE  = 131.0f;    // LSB per (°/s)

float accelToG(int16_t v)       { return (float)v / ACCEL_SCALE; }
float gyroToDegPerSec(int16_t v){ return (float)v / GYRO_SCALE;  }
float tempToC(int16_t v)        { return (v / 340.0f) + 36.53f;  }

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); Wire.write(0x00);  // wake
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 14, true);

  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();
  tempRaw = (Wire.read() << 8) | Wire.read();
  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();

  float ax_g = accelToG(ax), ay_g = accelToG(ay), az_g = accelToG(az);
  float gx_d = gyroToDegPerSec(gx), gy_d = gyroToDegPerSec(gy), gz_d = gyroToDegPerSec(gz);
  float tC   = tempToC(tempRaw);

  Serial.println("MPU6050 Readings (converted):");
  Serial.print("Accel (g): ");  Serial.print(ax_g,3); Serial.print("  ");
  Serial.print(ay_g,3);         Serial.print("  ");    Serial.println(az_g,3);
  Serial.print("Gyro (deg/s): "); Serial.print(gx_d,3); Serial.print("  ");
  Serial.print(gy_d,3);          Serial.print("  ");    Serial.println(gz_d,3);
  Serial.print("Temp (C): ");     Serial.println(tC,2);
  Serial.println("----------------------");
  delay(500);
}
```

---

## 5. Mapping to Roll-Pitch-Yaw (RPY)  to a Cartesian orientation

With acceleration vector **a** = (ax, ay, az) in g:

- Magnitude:  
  $$
  |\vec{a}| = \sqrt{a_x^2 + a_y^2 + a_z^2}
  $$

- Tilt angles:  
  $$
  \text{roll} = \arctan2(a_y, a_z)
  $$

  $$
  \text{pitch} = \arctan2\left(-a_x, \sqrt{a_y^2 + a_z^2}\right)
  $$

Usually reported in degrees.

---

## 6. Complementary Filter (Stable Pitch/Roll)

- Gyroscope integration → responsive, but drifts.  
- Accelerometer tilt → absolute, but noisy.  
- Complementary filter blends both:

$$
\text{roll} \leftarrow \alpha(\text{roll} + \dot\phi_\text{gyro}\,\Delta t) + (1-\alpha)\,\text{roll}_\text{acc}
$$

$$
\text{pitch} \leftarrow \alpha(\text{pitch} + \dot\theta_\text{gyro}\,\Delta t) + (1-\alpha)\,\text{pitch}_\text{acc}
$$

Where $\alpha \in [0,1]$, e.g. 0.98.  
Yaw has no gravity reference and will drift without a magnetometer.

---

## 7. Practical Notes

- **Filter weight α:**  
  Start at 0.98. Larger α = faster response, more drift. Smaller α = more stable, but noisy.  

- **Gyro bias:**  
  Recalibrate after power-up or temperature change.  

- **Yaw drift:**  
  Without a magnetometer, yaw is unreferenced. Use a 9-DoF IMU or add a magnetometer.  

- **Full-scale ranges:**  
  If changed, update `ACCEL_SCALE` and `GYRO_SCALE` accordingly.  

