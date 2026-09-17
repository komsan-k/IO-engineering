#include "mpuRPY.h"

MpuRPY::MpuRPY(uint8_t address, TwoWire& wire)
: _wire(&wire), _addr(address) {}

bool MpuRPY::begin(int sda, int scl, bool initWire) {
  if (initWire) {
#if defined(ARDUINO_ARCH_ESP32)
    if (sda >= 0 && scl >= 0) _wire->begin(sda, scl);
    else                      _wire->begin();
#else
    (void)sda; (void)scl;
    _wire->begin();
#endif
  }

  delay(50);

  // Wake device (clear sleep bit)
  if (!writeReg(REG_PWR_MGMT_1, 0x00)) return false;
  delay(100);

  // Default ranges (±2g and ±250 dps) — keep scales in sync
  if (!configureRanges(2, 250)) return false;

  // Prime roll/pitch from accelerometer
  primeFromAccel();

  // Calibrate gyro biases
  calibrateGyro(500);

  _lastMicros = micros();
  return true;
}

bool MpuRPY::configureRanges(uint8_t accelFS, uint16_t gyroFS) {
  // Accel config bits [4:3]:
  // 0=±2g, 1=±4g, 2=±8g, 3=±16g
  uint8_t accelCfgBits = 0;
  switch (accelFS) {
    case 2:   accelCfgBits = 0; _accelScale = 16384.0f; break;
    case 4:   accelCfgBits = 1; _accelScale = 8192.0f;  break;
    case 8:   accelCfgBits = 2; _accelScale = 4096.0f;  break;
    case 16:  accelCfgBits = 3; _accelScale = 2048.0f;  break;
    default:  accelCfgBits = 0; _accelScale = 16384.0f; break;
  }

  // Gyro config bits [4:3]:
  // 0=±250, 1=±500, 2=±1000, 3=±2000 dps
  uint8_t gyroCfgBits = 0;
  switch (gyroFS) {
    case 250:   gyroCfgBits = 0; _gyroScale = 131.0f;   break;
    case 500:   gyroCfgBits = 1; _gyroScale = 65.5f;    break;
    case 1000:  gyroCfgBits = 2; _gyroScale = 32.8f;    break;
    case 2000:  gyroCfgBits = 3; _gyroScale = 16.4f;    break;
    default:    gyroCfgBits = 0; _gyroScale = 131.0f;   break;
  }

  if (!writeReg(REG_ACCEL_CONFIG, uint8_t(accelCfgBits << 3))) return false;
  if (!writeReg(REG_GYRO_CONFIG,  uint8_t(gyroCfgBits  << 3))) return false;
  delay(10);
  return true;
}

void MpuRPY::calibrateGyro(unsigned samples) {
  long gxSum = 0, gySum = 0, gzSum = 0;
  unsigned collected = 0;

  while (collected < samples) {
    if (!readBurst14()) continue;
    gxSum += _gxRaw;
    gySum += _gyRaw;
    gzSum += _gzRaw;
    collected++;
    delay(3);
  }

  // Convert raw to deg/s bias
  _gxBias = float(gxSum) / samples / _gyroScale;
  _gyBias = float(gySum) / samples / _gyroScale;
  _gzBias = float(gzSum) / samples / _gyroScale;
}

bool MpuRPY::update() {
  if (!readBurst14()) return false;

  // dt (s)
  unsigned long now = micros();
  _dt = float(now - _lastMicros) * 1e-6f;
  _lastMicros = now;
  if (_dt <= 0.0f || _dt > 0.1f) _dt = 0.01f; // clamp occasional anomalies

  // Convert to physical units
  float ax_g = accelToG(_axRaw);
  float ay_g = accelToG(_ayRaw);
  float az_g = accelToG(_azRaw);

  float gx_dps = gyroToDPS(_gxRaw) - _gxBias;
  float gy_dps = gyroToDPS(_gyRaw) - _gyBias;
  float gz_dps = gyroToDPS(_gzRaw) - _gzBias;

  // Accelerometer tilt (deg)
  float rollAcc  = atan2f(ay_g, az_g) * 180.0f / PI;
  float pitchAcc = atan2f(-ax_g, sqrtf(ay_g * ay_g + az_g * az_g)) * 180.0f / PI;

  // Integrate gyro (deg)
  float rollGyro  = _rollDeg  + gx_dps * _dt;
  float pitchGyro = _pitchDeg + gy_dps * _dt;
  float yawGyro   = _yawDeg   + gz_dps * _dt; // no mag => drifts

  // Complementary filter
  _rollDeg  = _alpha * rollGyro  + (1.0f - _alpha) * rollAcc;
  _pitchDeg = _alpha * pitchGyro + (1.0f - _alpha) * pitchAcc;
  _yawDeg   = yawGyro;

  return true;
}

bool MpuRPY::writeReg(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  return (_wire->endTransmission() == 0);
}

bool MpuRPY::readBurst14() {
  // set start register
  _wire->beginTransmission(_addr);
  _wire->write(REG_ACCEL_XOUT_H);
  if (_wire->endTransmission(false) != 0) return false;

  // request 14 bytes
  if (_wire->requestFrom((int)_addr, 14, true) != 14) return false;

  _axRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  _ayRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  _azRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  _tempRaw = (int16_t(_wire->read()) << 8) | _wire->read();
  _gxRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  _gyRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  _gzRaw   = (int16_t(_wire->read()) << 8) | _wire->read();
  return true;
}

void MpuRPY::primeFromAccel() {
  // Try a single read to get a decent initial angle
  if (!readBurst14()) return;
  float ax = accelToG(_axRaw);
  float ay = accelToG(_ayRaw);
  float az = accelToG(_azRaw);

  _rollDeg  = atan2f(ay, az) * 180.0f / PI;
  _pitchDeg = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / PI;
  _yawDeg   = 0.0f;
}
