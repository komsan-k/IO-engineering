#include "lm73.h"

// ---------------------------
// SimpleMovingAverage Impl.
// ---------------------------
SimpleMovingAverage::SimpleMovingAverage(size_t window)
: _buf(nullptr), _n(0), _idx(0), _primed(false), _sum(0.0f) {
  resize(window);
}

SimpleMovingAverage::~SimpleMovingAverage() {
  delete[] _buf;
}

void SimpleMovingAverage::reset() {
  _idx = 0;
  _primed = false;
  _sum = 0.0f;
  if (_buf && _n > 0) {
    for (size_t i = 0; i < _n; ++i) _buf[i] = 0.0f;
  }
}

void SimpleMovingAverage::resize(size_t window) {
  if (window == 0) window = 1; // avoid division by zero
  delete[] _buf;
  _n = window;
  _buf = new float[_n];
  reset();
}

float SimpleMovingAverage::update(float x) {
  if (!_buf || _n == 0) return x;
  _sum -= _buf[_idx];
  _buf[_idx] = x;
  _sum += x;

  _idx++;
  if (_idx >= _n) {
    _idx = 0;
    _primed = true;
  }

  return _sum / (_primed ? _n : _idx);
}

// ---------------------------
// LM73 Driver Impl.
// ---------------------------
LM73::LM73(uint8_t address, TwoWire& wire)
: _wire(&wire), _address(address), _sma(nullptr) {}

bool LM73::begin(int sda, int scl, bool initWire) {
  if (initWire) {
#if defined(ARDUINO_ARCH_ESP32)
    // ESP32 allows custom I2C pins
    if (sda >= 0 && scl >= 0) {
      _wire->begin(sda, scl);
    } else {
      _wire->begin(); // default pins
    }
#else
    // Other boards typically ignore custom pins here
    (void)sda; (void)scl;
    _wire->begin();
#endif
  }
  // Basic presence check: try reading temperature register once
  float t = readCelsius();
  return !isnan(t);
}

bool LM73::readRaw14b(int16_t& raw14) {
  _wire->beginTransmission(_address);
  _wire->write(REG_TEMPERATURE);
  if (_wire->endTransmission() != 0) {
    return false; // I2C error on setting register
  }

  // Read 2 bytes: MSB, LSB (LM73 outputs 16-bit where lower 2 bits are not used -> 14-bit data)
  uint8_t toRead = 2;
  uint8_t read = _wire->requestFrom((int)_address, (int)toRead);
  if (read != toRead) {
    return false; // not enough data
  }

  uint8_t msb = _wire->read();
  uint8_t lsb = _wire->read();

  int16_t val = (static_cast<int16_t>(msb) << 8) | static_cast<int16_t>(lsb);
  // Shift to 14 bits: LM73 temperature is in bits 15:2, LSBs [1:0] are not used
  val >>= 2;

  raw14 = val;
  return true;
}

bool LM73::readCelsius(float& outC) {
  int16_t raw14 = 0;
  if (!readRaw14b(raw14)) {
    return false;
  }
  // 0.03125 °C per LSB (i.e., 1/32 °C)
  outC = static_cast<float>(raw14) * 0.03125f;
  return true;
}

float LM73::readCelsius() {
  float c;
  if (!readCelsius(c)) return NAN;
  return c;
}

void LM73::enableSMA(size_t window) {
  if (_sma) {
    _sma->resize(window);
  } else {
    _sma = new SimpleMovingAverage(window);
  }
}

void LM73::disableSMA() {
  delete _sma;
  _sma = nullptr;
}

float LM73::readCelsiusSmoothed() {
  float c = readCelsius();
  if (isnan(c)) return c;
  if (_sma) return _sma->update(c);
  return c;
}
