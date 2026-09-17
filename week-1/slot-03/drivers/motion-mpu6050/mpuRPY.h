#ifndef MPU_RPY_H
#define MPU_RPY_H

#include <Arduino.h>
#include <Wire.h>

/**
 * MPU6050 Roll-Pitch-Yaw helper (complementary filter)
 * - Uses burst read of 14 bytes (accel+temp+gyro)
 * - Gyro bias calibration (keep still during calibration)
 * - Complementary filter for roll/pitch; yaw integrates gyro Z (drifts without magnetometer)
 */
class MpuRPY {
public:
  // address: 0x68 (AD0 low) or 0x69 (AD0 high)
  // wire: I2C bus (default Wire)
  explicit MpuRPY(uint8_t address = 0x68, TwoWire& wire = Wire);

  // Initialize device and I2C
  // For ESP32 you can pass custom pins. If initWire=false, you must have called wire.begin() already.
  bool begin(int sda = -1, int scl = -1, bool initWire = true);

  // Optional: set full-scale ranges (update scales accordingly)
  // accelFS: 2,4,8,16 (g)
  // gyroFS : 250,500,1000,2000 (deg/s)
  bool configureRanges(uint8_t accelFS = 2, uint16_t gyroFS = 250);

  // Calibrate gyro bias (keep the board still!)
  void calibrateGyro(unsigned samples = 500);

  // Read the IMU raw registers (burst read), update roll/pitch/yaw using complementary filter
  // Returns false if I2C read fails
  bool update();

  // Set complementary filter alpha (0..1). Higher -> more gyro trust.
  void setComplementaryAlpha(float alpha) { _alpha = constrain(alpha, 0.0f, 1.0f); }

  // Accessors (degrees)
  float roll()  const { return _rollDeg;  }
  float pitch() const { return _pitchDeg; }
  float yaw()   const { return _yawDeg;   } // integrates gyro Z; drifts over time

  // Access to last raw values
  int16_t axRaw() const { return _axRaw; }
  int16_t ayRaw() const { return _ayRaw; }
  int16_t azRaw() const { return _azRaw; }
  int16_t gxRaw() const { return _gxRaw; }
  int16_t gyRaw() const { return _gyRaw; }
  int16_t gzRaw() const { return _gzRaw; }
  int16_t tempRaw() const { return _tempRaw; }

  // Helpers for physical units
  float accelToG(int16_t v)  const { return float(v) / _accelScale; }
  float gyroToDPS(int16_t v) const { return float(v) / _gyroScale;  }
  float tempToC(int16_t v)   const { return (v / 340.0f) + 36.53f;  }

  // Last dt (seconds)
  float lastDt() const { return _dt; }

  // Public constants
  static constexpr uint8_t MPU6050_ADDR_LOW  = 0x68;
  static constexpr uint8_t MPU6050_ADDR_HIGH = 0x69;

private:
  // I2C
  TwoWire* _wire;
  uint8_t  _addr;

  // Raw samples
  int16_t _axRaw{0}, _ayRaw{0}, _azRaw{0};
  int16_t _gxRaw{0}, _gyRaw{0}, _gzRaw{0};
  int16_t _tempRaw{0};

  // Scales
  float _accelScale{16384.0f}; // ±2g default
  float _gyroScale {131.0f};   // ±250 dps default

  // Gyro biases (deg/s)
  float _gxBias{0.0f}, _gyBias{0.0f}, _gzBias{0.0f};

  // Complementary filter
  float _rollDeg{0.0f};
  float _pitchDeg{0.0f};
  float _yawDeg{0.0f};
  float _alpha{0.98f}; // gyro trust

  // Timing
  unsigned long _lastMicros{0};
  float _dt{0.01f};

  // Internal helpers
  bool writeReg(uint8_t reg, uint8_t val);
  bool readBurst14();
  void primeFromAccel(); // initial roll/pitch from gravity

  // Register map (subset)
  static constexpr uint8_t REG_PWR_MGMT_1   = 0x6B;
  static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
  static constexpr uint8_t REG_GYRO_CONFIG  = 0x1B;
  static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
};

#endif // MPU_RPY_H
