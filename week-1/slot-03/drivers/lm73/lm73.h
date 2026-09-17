#ifndef LM73_H
#define LM73_H

#include <Arduino.h>
#include <Wire.h>

// Default I2C address for LM73 (verify with your hardware/datasheet)
#ifndef LM73_DEFAULT_ADDRESS
#define LM73_DEFAULT_ADDRESS 0x4D
#endif

// ---------------------------
// Simple Moving Average (SMA)
// ---------------------------
class SimpleMovingAverage {
public:
  explicit SimpleMovingAverage(size_t window = 10);
  ~SimpleMovingAverage();

  void   reset();
  void   resize(size_t window);
  float  update(float x);         // returns current average
  bool   primed() const { return _primed; }
  size_t size()   const { return _n; }

private:
  float*  _buf;
  size_t  _n;
  size_t  _idx;
  bool    _primed;
  float   _sum;
};

// -------------
// LM73 Driver
// -------------
class LM73 {
public:
  // address: I2C address (default 0x4D)
  // wire:    reference to TwoWire (default Wire)
  explicit LM73(uint8_t address = LM73_DEFAULT_ADDRESS, TwoWire& wire = Wire);

  // Begin I2C. If sda/scl are -1, uses default pins (depends on board core).
  // For ESP32 you can pass custom pins, e.g., begin(4, 5).
  // initWire=true will call wire.begin(sda, scl). Set false if you already called begin() elsewhere.
  bool begin(int sda = -1, int scl = -1, bool initWire = true);

  // Read temperature (Celsius). Returns true on success and sets outC;
  // returns false if I2C error or not enough bytes.
  bool readCelsius(float& outC);

  // Convenience: returns NAN on failure instead of bool.
  float readCelsius();

  // Optional: enable/resize SMA window (pass window >= 1 to enable).
  void  enableSMA(size_t window = 10);
  void  disableSMA();
  bool  smaEnabled() const { return _sma != nullptr; }

  // If SMA is enabled, this updates the filter with the latest reading and returns the smoothed value.
  // If SMA is disabled, just returns the raw value.
  float readCelsiusSmoothed();

  // Accessors
  uint8_t address() const { return _address; }

private:
  TwoWire* _wire;
  uint8_t  _address;
  SimpleMovingAverage* _sma;

  // LM73 register map (minimal set)
  static constexpr uint8_t REG_TEMPERATURE = 0x00;

  bool readRaw14b(int16_t& raw14);
};

#endif // LM73_H
