# Phase 1: MPU6050 Time-Domain and FFT Features on ESP32

**Scope:** sensor acquisition, feature extraction, signal inspection, and labeled CSV collection. TinyML is introduced in Phase 2.

## 1. Objectives

By completing this lab, students can:

1. Read acceleration and angular velocity from an MPU6050.
2. Explain the difference between time-domain and frequency-domain features.
3. Calculate 42 features from a 64-reading window.
4. Inspect FFT peaks and understand sampling limits.
5. Save labeled recordings for later TinyML training.

## 2. Equipment and Software

- ESP32 DevKit/WROOM or a compatible board with suitable pins.
- MPU6050 breakout compatible with 3.3V supply and I2C logic.
- Breadboard, jumper wires, USB cable.
- Arduino IDE with ESP32 board support.
- Optional: Python 3 for cleaning copied serial output into CSV.

The package contains the complete sensor driver and radix-2 FFT implementation.
No external MPU6050 library, FFT library, TinyML library, or model is required.

## 3. Wiring

| MPU6050 connection | ESP32 connection |
|---|---|
| VCC | 3.3V, with a compatible breakout |
| GND | GND |
| SDA | GPIO4 |
| SCL | GPIO5 |
| AD0 | GND, selecting address 0x68 |
| INT | Unconnected; data-ready status is polled over I2C |

Use I2C pull-ups to 3.3V, commonly already fitted to a breakout. Verify your
board's pin availability. Change `Wire.begin(4,5)` if your wiring differs.
GPIO5 is a strapping pin on classic ESP32; avoid other circuitry forcing its
level during reset. Supply voltage must suit your particular breakout.

## 4. Measurement Settings

| Parameter | Value |
|---|---|
| Acceleration range | +/-4g |
| Gyroscope range | +/-500 degrees/second |
| Sensor output rate | 50Hz |
| Sensor low-pass setting | Approximately 21Hz acceleration / 20Hz gyro |
| Samples per window | 64 |
| Approximate window duration | 1.28 seconds |
| First-to-last sample span | 63/50 = 1.26 seconds |
| FFT bin spacing | 50/64 = 0.78125Hz |
| Feature frequency bins | k=1..31, 0.78125..24.21875Hz |
| Serial speed | 115200 baud |

The sketch polls the sensor's data-ready flag rather than reading repeatedly
without checking for a new update. It rejects incomplete windows if a sample
gap exceeds 30ms, and rejects complete windows whose measured average sample
rate differs from 50Hz by more than 5%. Frequency labels still use nominal
50Hz; actual frequency accuracy depends on clock accuracy and sampling jitter.
This does not measure every form of timing error or guarantee no missed samples.

Frequency content above 25Hz can alias. The sensor's low-pass filter attenuates
such content but is not an ideal anti-alias filter. Avoid high-frequency vibration
for this introductory lab. Output/processing introduces gaps BETWEEN windows;
the acquisition is not a continuous FIFO recording.

## 5. Feature Definitions

The six signals are ax, ay, az in g and gx, gy, gz in degrees/second. Gravity is
retained in the time-domain acceleration. For each signal, calculate:

| Domain | Feature | Interpretation |
|---|---|---|
| Time | Mean | Average signed measurement |
| Time | Population standard deviation | Variation around mean; divide by N |
| Time | Minimum | Lowest signed measurement |
| Time | Maximum | Highest signed measurement |
| Frequency | Peak frequency | Frequency of largest selected spectral-power bin |
| Frequency | Peak fraction | Largest bin power divided by total selected-bin power |
| Frequency | Spectral centroid | Power-weighted average selected-bin frequency |

**6 axes x 7 features = 42 model inputs.**

Feature ordering is **seven features per axis**, then the next axis:

```text
ax_mean, ax_std, ax_min, ax_max, ax_peak_hz, ax_peak_fraction, ax_centroid_hz,
ay_mean, ay_std, ay_min, ay_max, ay_peak_hz, ay_peak_fraction, ay_centroid_hz,
az_mean, az_std, az_min, az_max, az_peak_hz, az_peak_fraction, az_centroid_hz,
gx_mean, gx_std, gx_min, gx_max, gx_peak_hz, gx_peak_fraction, gx_centroid_hz,
gy_mean, gy_std, gy_min, gy_max, gy_peak_hz, gy_peak_fraction, gy_centroid_hz,
gz_mean, gz_std, gz_min, gz_max, gz_peak_hz, gz_peak_fraction, gz_centroid_hz
```

This ordering differs from placing all 24 time features before all 18 FFT
features. Phase 2 must use the exact order above.

### FFT calculation

1. Copy one axis into a temporary buffer, leaving the original readings intact.
2. Subtract that window's mean.
3. Multiply by a symmetric Hamming window.
4. Run the 64-point forward FFT.
5. Calculate positive-frequency bin powers.
6. Exclude DC (k=0) and Nyquist (k=32) from frequency features.

Plain-text formulas:

```text
mean = sum(x[i]) / N
std = sqrt(sum((x[i] - mean)^2) / N)
frequency[k] = k * Fs / N
w[i] = 0.54 - 0.46*cos(2*pi*i/(N-1))
X = FFT((x - mean) * w)
P[k] = 2*abs(X[k])^2 / (N*sum(w[i]^2)), for k=1..31
peak_fraction = max(P[k]) / sum(P[k])
centroid_hz = sum(frequency[k]*P[k]) / sum(P[k])
```

These powers are window-normalized bin powers, not a calibrated power spectral
density. The spectrum display divides them by their selected-bin sum and is
therefore dimensionless. Peak fraction is not peak amplitude, and is generally
less than one even for a pure sinusoid because windowing spreads power.

### Avoid interpreting idle noise as a gesture frequency

In feature mode, set all three frequency features to zero when time-domain
standard deviation is below **0.02g** for acceleration or **1 degree/second**
for gyro. These are starting thresholds for a classroom demonstration, not
sensor specifications. Tune with actual idle recordings. The `active_mask`
metadata records axes above threshold: bit0=ax through bit5=gz.

Spectrum mode intentionally shows the ungated normalized spectrum so students
can inspect noise. A large normalized noise peak does not imply large motion.

## 6. Install and Upload the Complete Code

All required source code is supplied in this folder:

| File | Purpose |
|---|---|
| `Phase1_MPU6050/Phase1_MPU6050.ino` | Complete acquisition, commands and CSV output |
| `Phase1_MPU6050/MPU6050Simple.h` | Driver interface |
| `Phase1_MPU6050/MPU6050Simple.cpp` | Driver implementation, data-ready polling and scaling |
| `Phase1_MPU6050/SignalFeatures.h` | Complete radix-2 FFT and 42-feature extraction |
| `clean_feature_csv.py` | Optional serial-log cleaner; Python standard library only |

1. Extract the package.
2. Open `Phase1_MPU6050.ino` inside its matching folder.
3. Keep all four Arduino source files together.
4. Select the correct ESP32 board and USB port, then upload.
5. Open Serial Monitor at **115200 baud**.
6. The sketch runs its synthetic FFT self-test, initializes the sensor, and
   starts paused. Send one command character at a time; newline is accepted.

The bundled driver is a Phase 1 variant: its filter is set to 21/20Hz and it
adds `dataReady()`. Use these bundled files rather than substituting an older
44Hz-filter version. The older library/attachments are not modified.

## 7. Serial Commands

| Send | Result |
|---|---|
| `c` | Start/resume capture as a new recording |
| `p` | Pause; discard incomplete window |
| `r` | Raw six-axis CSV mode |
| `f` | 42-feature CSV mode (default) |
| `s` | Spectrum CSV mode for selected axis |
| `x`, `y`, `z` | Select acceleration X, Y, Z for spectrum |
| `X`, `Y`, `Z` | Select gyro X, Y, Z for spectrum |
| `0`..`4` | Select a manual label and start a new recording |
| `h` | Print current mode's CSV header |
| `t` | Pause and run the synthetic FFT test |
| `?` | Print help |

Mode changes discard an incomplete window but do not start acquisition if paused.
Use `c` or a label digit after selecting your mode. `t` always pauses capture.

## 8. Experiment 1: Sensor and Time-Domain Inspection

1. Send `r`, then `c` to show physical six-axis readings.
2. Keep the sensor flat and still. Acceleration magnitude should be near 1g,
   distributed among axes according to orientation. Gyro should be near zero,
   subject to bias and noise.
3. Tilt slowly left/right, then rotate around each axis.
4. Send `p`, then `f`, then `4` to collect idle-labeled feature windows.
5. Compare means and standard deviations before and during motion.

Answer: Why can the acceleration mean change when tilting without translating?

## 9. Experiment 2: Verify FFT Arithmetic

1. Send `t`.
2. Confirm **Synthetic 3.125Hz sine test: PASS**.
3. The test uses a sinusoid at bin4: 4*50/64 = 3.125Hz on all six synthetic axes.
4. Send `c` to resume real sensor acquisition.

This verifies arithmetic only. It does not verify real sensor sampling, timing,
mounting, or recognition performance. The self-test is also available after
sensor initialization failure; fix wiring and reset to resume actual acquisition.

## 10. Experiment 3: Real Motion Spectrum

1. Send `s`, select `x` (or another axis), then `c`.
2. Move the sensor smoothly back and forth, approximately twice per second.
3. Inspect output frequency bins and their relative power.
4. Repeat with slower and faster motion. Hand movement is not a calibrated
   sinusoid: expect broad peaks and harmonics, not an exact 2Hz measurement.
5. Compare to a still sensor. Return to `f` for thresholded frequency features.

Optional plotting: copy one spectrum window to a spreadsheet and plot
`frequency_hz` versus `relative_bin_power`. Use one window/axis per curve.
The Serial Plotter's horizontal axis is normally sample index, not an automatic
frequency scale, so exported spectrum CSV is clearer for a frequency chart.

## 11. Experiment 4: Build a Labeled Feature Dataset

| Label | Gesture | Intended action in Phase 2 |
|---|---|---|
| 0 | Tilt left | OFF |
| 1 | Tilt right | ON |
| 2 | Move up | Increase brightness |
| 3 | Move down | Decrease brightness |
| 4 | Idle | No command |

1. Send `f`.
2. Send a label digit to start a new recording, then perform that gesture.
3. Send `p` before repositioning or changing gesture.
4. Repeat several independent recordings per class. As a lab target, collect
   **five recordings per class, with ten accepted windows per recording**.
   This produces 250 feature rows and is a teaching dataset, not a guarantee
   of a sufficient validation sample size.
5. Record multiple users/sessions or sensor mountings if those should generalize.
6. Keep idle recordings for assessing unwanted commands in Phase 2.

Labels are ground truth supplied by the operator, not model predictions.
Automatic windows can cut a gesture or mix motion with idle; inspect and reject
poorly aligned windows. Phase 1 demonstrates continuous feature collection.
Precise isolated-gesture datasets benefit from prompted, synchronized captures.

## 12. Save CSV Data

Copy feature-mode Serial Monitor text into a uniquely named file, for example
`session_01.txt`. Save only feature-mode sections where possible. Then run:

```bash
python clean_feature_csv.py session_01.txt phase1_features.csv
```

The helper removes comments, repeated headers, wrong-mode rows and unlabeled
rows, and adds `session_id` based on the source filename. It refuses to overwrite
an existing destination. A unique source filename is needed for each session
because ESP32 recording IDs restart after reset.

The 42 named feature columns are the future model inputs. Exclude `session_id`,
`recording_id`, `window_id`, `start_ms`, `effective_fs_hz`, `active_mask`,
`label_id`, and `label` from the model input array.

## 13. Expected Observations

| Condition | Time-domain behavior | Frequency behavior |
|---|---|---|
| Idle | Low standard deviation; gravity in acceleration mean | Gated frequency features often zero |
| Slow tilt | Signed acceleration means change | Low-frequency content can appear |
| Repeated motion | Larger variation | Peaks near motion frequency and harmonics |
| Up versus down | Differences depend on motion and mounting | Magnitude spectra may overlap |

FFT magnitude and mean/std/min/max discard much of the order of events.
These features do not guarantee that up and down are distinguishable. Keep
this limitation visible and consider temporal features or sequence models later.

## 14. Student Submission

- Working Arduino sketch and wiring photograph.
- Labeled 42-feature CSV and description of collection procedure.
- One time-domain observation and one frequency-spectrum plot.
- Table comparing idle, tilt and repeated motion.
- Brief answers: effect of window length, meaning of peak fraction, why DC
  is removed, why magnitude alone does not reliably encode direction.

## 15. Phase 2 Extension

Train a new five-class network using these **42 features in the documented
order**. Split by session/recording, calculate normalization on training data
only, then deploy using EloquentTinyML. Compare the 24 time features alone to
all 42 features using identical splits. The previous 24-input model, header,
and CSV are not directly compatible with this 64-reading/42-feature pipeline.

## 16. Validation and References

The bundled FFT and feature calculations are checked on host C++ against a
direct DFT, constant signals and known sinusoids. The CSV cleaner is checked
with representative serial rows. ESP32 core compilation, hardware timing,
I2C behavior and physical measurements remain to be verified on your board.

Sensor register and scaling reference:
[TDK InvenSense MPU-6000/MPU-6050 Register Map](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf).

### Complete source code

The following appendices reproduce all four Arduino source files. The separate
copies in `Phase1_MPU6050/` are the files to compile.


#### Phase1_MPU6050.ino

```cpp
/* Phase 1: MPU6050 time-domain + FFT feature laboratory.
   No model/TinyML dependency. Open Serial Monitor at 115200 baud.
   See LAB_Phase1.md for installation, commands and complete experiments.
   Pins SDA4/SCL5. 64 fresh sensor samples at nominal 50Hz per window.
*/
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "MPU6050Simple.h"
#include "SignalFeatures.h"

MPU6050Simple mpu;
float samples[SAMPLE_COUNT][AXIS_COUNT];
FeatureResult result;
int sampleIndex = 0;
int selectedAxis = 0;
int selectedLabel = -1;
bool running = false;
char outputMode = 'f';
uint32_t recordingId = 0, windowId = 0;
uint32_t firstUs = 0, lastUs = 0, firstMs = 0, lastPollUs = 0;
const char* const axisNames[] = {"ax","ay","az","gx","gy","gz"};
const char* const stats[] = {"mean","std","min","max","peak_hz","peak_fraction","centroid_hz"};
const char* const labels[] = {"tilt_left","tilt_right","up","down","idle"};

void printHeader() {
  if (outputMode == 'f') {
    Serial.print("recording_id,window_id,start_ms,effective_fs_hz,active_mask");
    for (int a = 0; a < 6; ++a) for (int s = 0; s < 7; ++s) {
      Serial.print(','); Serial.print(axisNames[a]); Serial.print('_'); Serial.print(stats[s]);
    }
    Serial.println(",label_id,label");
  } else if (outputMode == 'r') Serial.println("time_ms,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps");
  else Serial.println("window_id,axis,frequency_hz,relative_bin_power");
}

void help() {
  Serial.println("# 0..4=label/start recording; p=pause; c=continue/new recording");
  Serial.println("# f=42-feature CSV; r=raw CSV; s=spectrum CSV; h=header; t=FFT self-test; ?=help");
  Serial.println("# Spectrum axis: x/y/z=accel, X/Y/Z=gyro. Start unlabeled using c.");
  Serial.println("# Manual labels: 0=left 1=right 2=up 3=down 4=idle. Not predictions.");
}

void resetCapture() { sampleIndex = 0; lastPollUs = micros(); }
void startCapture() {
  ++recordingId; resetCapture(); running = true;
  Serial.print("# Recording "); Serial.print(recordingId);
  Serial.print(" label="); Serial.println(selectedLabel < 0 ? "unlabeled" : labels[selectedLabel]);
}

void selfTest() {
  running = false; resetCapture();
  float test[SAMPLE_COUNT][AXIS_COUNT];
  for (int i = 0; i < SAMPLE_COUNT; ++i) for (int a = 0; a < AXIS_COUNT; ++a)
    test[i][a] = (a < 3 ? 0.5f : 20.0f)*sinf(2*SIGNAL_PI*4*i/SAMPLE_COUNT);
  FeatureResult check; extract42(test, check);
  bool passed = check.activeMask == 63;
  for (int a = 0; a < AXIS_COUNT; ++a)
    if (fabsf(check.values[a*7+4]-3.125f) > .001f) passed = false;
  Serial.print("# Synthetic 3.125Hz sine test: "); Serial.println(passed ? "PASS" : "FAIL");
  Serial.println("# This tests feature arithmetic, not the sensor or sampling. Send c to resume.");
}

void commands() {
  while (Serial.available()) {
    const char ch = static_cast<char>(Serial.read());
    if (ch >= '0' && ch <= '4') { selectedLabel = ch-'0'; startCapture(); }
    else if (ch == 'p') { running = false; resetCapture(); Serial.println("# Paused"); }
    else if (ch == 'c') startCapture();
    else if (ch == 'f' || ch == 'r' || ch == 's') {
      outputMode = ch; resetCapture(); printHeader();
    } else if (ch == 'x' || ch == 'y' || ch == 'z') { selectedAxis=ch-'x'; resetCapture(); }
    else if (ch == 'X' || ch == 'Y' || ch == 'Z') { selectedAxis=3+ch-'X'; resetCapture(); }
    else if (ch == 'h') printHeader();
    else if (ch == 't') selfTest();
    else if (ch == '?') help();
  }
}

void setup() {
  Serial.begin(115200); delay(1000);
  help(); printHeader();
  // Self-test is usable even when the MPU6050 is absent.
  selfTest();
  Wire.begin(4,5); Wire.setClock(400000); Wire.setTimeOut(50);
  if (!mpu.begin(Wire,0x68,MPU6050Simple::AccelRange::G4,MPU6050Simple::GyroRange::DPS500)) {
    Serial.print("# Sensor init failed: "); Serial.println(mpu.errorMessage());
    Serial.println("# Self-test t remains available. Fix wiring and reset before acquisition.");
    while (true) { commands(); running = false; delay(1); }
  }
  Serial.println("# Ready, paused. DLPF21Hz accel/20Hz gyro, sensor50Hz. Send c or label0..4.");
}

void processWindow() {
  const float effectiveFs = (SAMPLE_COUNT-1)*1000000.0f/(lastUs-firstUs);
  if (effectiveFs < 47.5f || effectiveFs > 52.5f) {
    Serial.println("# Window rejected: observed sample rate outside 50Hz +/-5%"); return;
  }
  ++windowId;
  if (outputMode == 'f') {
    extract42(samples,result);
    Serial.print(recordingId); Serial.print(','); Serial.print(windowId); Serial.print(',');
    Serial.print(firstMs); Serial.print(','); Serial.print(effectiveFs,3); Serial.print(',');
    Serial.print(result.activeMask);
    for (int i=0;i<FEATURE_COUNT;++i) { Serial.print(','); Serial.print(result.values[i],6); }
    Serial.print(','); Serial.print(selectedLabel); Serial.print(',');
    Serial.println(selectedLabel < 0 ? "unlabeled" : labels[selectedLabel]);
  } else if (outputMode == 's') {
    float signal[SAMPLE_COUNT],power[SAMPLE_COUNT/2+1]; float mean=0,total=0;
    for (int i=0;i<SAMPLE_COUNT;++i) { signal[i]=samples[i][selectedAxis]; mean+=signal[i]; }
    spectrum64(signal,mean/SAMPLE_COUNT,power);
    for (int k=1;k<SAMPLE_COUNT/2;++k) total+=power[k];
    for (int k=1;k<SAMPLE_COUNT/2;++k) {
      Serial.print(windowId); Serial.print(','); Serial.print(axisNames[selectedAxis]); Serial.print(',');
      Serial.print(k*SAMPLE_RATE_HZ/SAMPLE_COUNT,6); Serial.print(',');
      Serial.println(total > 1e-12f ? power[k]/total : 0,6);
    }
  }
}

void loop() {
  commands();
  if (!running) { delay(1); return; }
  if (static_cast<uint32_t>(micros()-lastPollUs)<1000) return;
  lastPollUs=micros();
  bool fresh=false;
  if (!mpu.dataReady(fresh)) {
    sampleIndex=0; Serial.print("# Status read failed: "); Serial.println(mpu.errorMessage()); return;
  }
  if (!fresh) { delay(1); return; }
  const uint32_t nowUs=micros();
  if (sampleIndex>0 && static_cast<uint32_t>(nowUs-lastUs)>30000) {
    sampleIndex=0; Serial.println("# Sample gap >30ms: incomplete window discarded");
  }
  float reading[6];
  if (!mpu.readFeatures(reading)) {
    sampleIndex=0; Serial.print("# Sensor read failed: "); Serial.println(mpu.errorMessage()); return;
  }
  for (int a=0;a<6;++a) if (!isfinite(reading[a])) {
    sampleIndex=0; Serial.println("# Invalid reading: window discarded"); return;
  }
  if (sampleIndex==0) { firstUs=nowUs; firstMs=millis(); }
  lastUs=nowUs;
  for (int a=0;a<6;++a) samples[sampleIndex][a]=reading[a];
  if (outputMode=='r') {
    Serial.print(millis());
    for (int a=0;a<6;++a) { Serial.print(','); Serial.print(reading[a],6); }
    Serial.println();
  }
  ++sampleIndex;
  if (sampleIndex==SAMPLE_COUNT) {
    sampleIndex=0;
    processWindow();
    // Processing/printing creates a gap BETWEEN windows. Clear a pending
    // data-ready flag so the next window starts with newly arriving data.
    bool ignored; mpu.dataReady(ignored);
  }
}

```


#### MPU6050Simple.h

```cpp
#pragma once
#include <Arduino.h>
#include <Wire.h>

// Call Wire.begin(...) yourself before begin(). No dynamic allocation.
class MPU6050Simple {
public:
  enum class AccelRange : uint8_t { G2=0, G4=1, G8=2, G16=3 };
  enum class GyroRange : uint8_t { DPS250=0, DPS500=1, DPS1000=2, DPS2000=3 };
  enum class Error : uint8_t {
    None, NotInitialized, InvalidArgument, I2C, ShortRead, WrongDevice
  };
  struct RawData { int16_t ax, ay, az, temperature, gx, gy, gz; };
  struct Data { float ax_g, ay_g, az_g, temperature_c, gx_dps, gy_dps, gz_dps; };

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x68,
             AccelRange accel = AccelRange::G2,
             GyroRange gyro = GyroRange::DPS250);
  // On failure, the caller's output is left unchanged.
  bool readRaw(RawData& output);
  bool dataReady(bool& ready); // polling INT_STATUS; reading clears its flag
  bool read(Data& output);
  bool readFeatures(float output[6]); // ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps
  Error lastError() const { return error_; }
  const char* errorMessage() const;

private:
  TwoWire* wire_ = nullptr;
  uint8_t address_ = 0x68;
  bool ready_ = false;
  float accelScale_ = 16384.0f;
  float gyroScale_ = 131.0f;
  Error error_ = Error::NotInitialized;
  bool writeRegister(uint8_t reg, uint8_t value);
  bool readRegisters(uint8_t reg, uint8_t* bytes, uint8_t count);
  static int16_t decode(const uint8_t* bytes);
};

```


#### MPU6050Simple.cpp

```cpp
#include "MPU6050Simple.h"

bool MPU6050Simple::writeRegister(uint8_t reg, uint8_t value) {
  wire_->beginTransmission(address_);
  wire_->write(reg);
  wire_->write(value);
  if (wire_->endTransmission(true) != 0) { error_ = Error::I2C; return false; }
  error_ = Error::None;
  return true;
}

bool MPU6050Simple::readRegisters(uint8_t reg, uint8_t* bytes, uint8_t count) {
  wire_->beginTransmission(address_);
  wire_->write(reg);
  if (wire_->endTransmission(false) != 0) { error_ = Error::I2C; return false; }
  const size_t received = wire_->requestFrom(address_, count, true);
  if (received != count || wire_->available() < count) {
    while (wire_->available()) wire_->read();
    error_ = Error::ShortRead;
    return false;
  }
  for (uint8_t i=0; i<count; ++i) {
    const int value = wire_->read();
    if (value < 0) { error_ = Error::ShortRead; return false; }
    bytes[i] = static_cast<uint8_t>(value);
  }
  error_ = Error::None;
  return true;
}

int16_t MPU6050Simple::decode(const uint8_t* bytes) {
  const uint16_t value = (static_cast<uint16_t>(bytes[0]) << 8) | bytes[1];
  const int32_t signedValue = (value & 0x8000u) ? static_cast<int32_t>(value)-65536 : value;
  return static_cast<int16_t>(signedValue);
}

bool MPU6050Simple::begin(TwoWire& wire, uint8_t address,
                         AccelRange accel, GyroRange gyro) {
  ready_ = false;
  if ((address != 0x68 && address != 0x69) ||
      static_cast<uint8_t>(accel) > 3 || static_cast<uint8_t>(gyro) > 3) {
    error_ = Error::InvalidArgument;
    return false;
  }
  wire_ = &wire;
  address_ = address;
  uint8_t identity;
  if (!readRegisters(0x75, &identity, 1)) return false;
  // WHO_AM_I remains 0x68 even when AD0 sets bus address to 0x69.
  if (identity != 0x68) { error_ = Error::WrongDevice; return false; }
  if (!writeRegister(0x6B, 0x01)) return false; // wake, X gyro PLL
  delay(100);
  if (!writeRegister(0x6C, 0x00) || // enable all axes
      !writeRegister(0x6A, 0x00) || // disable FIFO/master mode
      !writeRegister(0x23, 0x00) || // no FIFO sources
      !writeRegister(0x38, 0x00) || // polling, no interrupts
      !writeRegister(0x1A, 0x04) || // DLPF: accel21Hz/gyro20Hz (Phase1 setting)
      !writeRegister(0x19, 19) ||   // 1000/(19+1)=50Hz sensor output
      !writeRegister(0x1C, static_cast<uint8_t>(accel) << 3) ||
      !writeRegister(0x1B, static_cast<uint8_t>(gyro) << 3)) return false;
  const float accelScales[] = {16384.0f,8192.0f,4096.0f,2048.0f};
  const float gyroScales[] = {131.0f,65.5f,32.8f,16.4f};
  accelScale_ = accelScales[static_cast<uint8_t>(accel)];
  gyroScale_ = gyroScales[static_cast<uint8_t>(gyro)];
  delay(100); // allow filters to settle
  ready_ = true;
  error_ = Error::None;
  return true;
}

bool MPU6050Simple::readRaw(RawData& output) {
  if (!ready_) { error_ = Error::NotInitialized; return false; }
  uint8_t bytes[14];
  if (!readRegisters(0x3B, bytes, sizeof(bytes))) return false;
  RawData next;
  next.ax = decode(bytes); next.ay = decode(bytes+2); next.az = decode(bytes+4);
  next.temperature = decode(bytes+6);
  next.gx = decode(bytes+8); next.gy = decode(bytes+10); next.gz = decode(bytes+12);
  output = next;
  return true;
}

bool MPU6050Simple::dataReady(bool& ready) {
  if (!ready_) { error_ = Error::NotInitialized; return false; }
  uint8_t status;
  if (!readRegisters(0x3A, &status, 1)) return false;
  ready = (status & 0x01u) != 0;
  return true;
}

bool MPU6050Simple::read(Data& output) {
  RawData raw;
  if (!readRaw(raw)) return false;
  Data next;
  next.ax_g = raw.ax/accelScale_; next.ay_g = raw.ay/accelScale_; next.az_g = raw.az/accelScale_;
  next.gx_dps = raw.gx/gyroScale_; next.gy_dps = raw.gy/gyroScale_; next.gz_dps = raw.gz/gyroScale_;
  next.temperature_c = raw.temperature/340.0f + 36.53f;
  output = next;
  return true;
}

bool MPU6050Simple::readFeatures(float output[6]) {
  if (!output) { error_ = Error::InvalidArgument; return false; }
  Data value;
  if (!read(value)) return false;
  output[0]=value.ax_g; output[1]=value.ay_g; output[2]=value.az_g;
  output[3]=value.gx_dps; output[4]=value.gy_dps; output[5]=value.gz_dps;
  return true;
}

const char* MPU6050Simple::errorMessage() const {
  switch (error_) {
    case Error::None: return "OK";
    case Error::NotInitialized: return "Sensor not initialized";
    case Error::InvalidArgument: return "Invalid argument";
    case Error::I2C: return "I2C transaction failed";
    case Error::ShortRead: return "Incomplete I2C response";
    case Error::WrongDevice: return "WHO_AM_I is not MPU6050 (0x68)";
  }
  return "Unknown error";
}

```


#### SignalFeatures.h

```cpp
#pragma once
#include <stdint.h>
#include <math.h>

constexpr int SAMPLE_COUNT = 64;
constexpr int AXIS_COUNT = 6;
constexpr int FEATURE_COUNT = 42;
constexpr float SAMPLE_RATE_HZ = 50.0f;
constexpr float SIGNAL_PI = 3.14159265358979323846f;
static_assert((SAMPLE_COUNT & (SAMPLE_COUNT - 1)) == 0, "FFT size must be a power of two");

// Ordered per axis: mean, population std, min, max, peak Hz, peak fraction,
// spectral centroid Hz. Axes: ax,ay,az,gx,gy,gz. Training must use this order.
struct FeatureResult {
  float values[FEATURE_COUNT];
  uint8_t activeMask; // bit 0=ax ... bit 5=gz, based on time-domain std
};

// In-place iterative radix-2 forward FFT. No scaling; caller computes power.
inline void fft64(float real[SAMPLE_COUNT], float imag[SAMPLE_COUNT]) {
  for (int i = 1, j = 0; i < SAMPLE_COUNT; ++i) {
    int bit = SAMPLE_COUNT >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) {
      float temp = real[i]; real[i] = real[j]; real[j] = temp;
      temp = imag[i]; imag[i] = imag[j]; imag[j] = temp;
    }
  }
  for (int length = 2; length <= SAMPLE_COUNT; length <<= 1) {
    const float angle = -2.0f * SIGNAL_PI / length;
    const float stepReal = cosf(angle), stepImag = sinf(angle);
    for (int start = 0; start < SAMPLE_COUNT; start += length) {
      float wr = 1, wi = 0;
      for (int j = 0; j < length/2; ++j) {
        const int a = start+j, b = a+length/2;
        const float tr = wr*real[b] - wi*imag[b];
        const float ti = wr*imag[b] + wi*real[b];
        real[b] = real[a]-tr; imag[b] = imag[a]-ti;
        real[a] += tr; imag[a] += ti;
        const float next = wr*stepReal - wi*stepImag;
        wi = wr*stepImag + wi*stepReal; wr = next;
      }
    }
  }
}

// Positive-bin power. Sum of bins 0..32 approximates window-weighted mean
// square, not a calibrated PSD. DC and Nyquist are set to zero for features.
inline void spectrum64(const float signal[SAMPLE_COUNT], float mean,
                       float power[SAMPLE_COUNT/2+1]) {
  float real[SAMPLE_COUNT], imag[SAMPLE_COUNT];
  float windowSquareSum = 0;
  for (int i = 0; i < SAMPLE_COUNT; ++i) {
    const float w = 0.54f - 0.46f*cosf(2.0f*SIGNAL_PI*i/(SAMPLE_COUNT-1));
    real[i] = (signal[i]-mean)*w;
    imag[i] = 0;
    windowSquareSum += w*w;
  }
  fft64(real, imag);
  power[0] = power[SAMPLE_COUNT/2] = 0;
  const float factor = 2.0f/(SAMPLE_COUNT*windowSquareSum);
  for (int k = 1; k < SAMPLE_COUNT/2; ++k)
    power[k] = factor*(real[k]*real[k]+imag[k]*imag[k]);
}

inline void extract42(const float samples[SAMPLE_COUNT][AXIS_COUNT],
                      FeatureResult& result) {
  result.activeMask = 0;
  for (int axis = 0; axis < AXIS_COUNT; ++axis) {
    float signal[SAMPLE_COUNT];
    float sum = 0, low = samples[0][axis], high = low;
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
      const float value = samples[i][axis]; signal[i] = value; sum += value;
      if (value < low) low = value;
      if (value > high) high = value;
    }
    const float mean = sum/SAMPLE_COUNT;
    float variance = 0;
    for (int i = 0; i < SAMPLE_COUNT; ++i) {
      const float d = signal[i]-mean; variance += d*d;
    }
    const float stddev = sqrtf(variance/SAMPLE_COUNT);
    const int offset = axis*7;
    result.values[offset] = mean; result.values[offset+1] = stddev;
    result.values[offset+2] = low; result.values[offset+3] = high;
    result.values[offset+4] = result.values[offset+5] = result.values[offset+6] = 0;
    // Demonstration thresholds; tune using real idle recordings.
    const float threshold = axis < 3 ? 0.02f : 1.0f;
    if (stddev < threshold) continue;
    float power[SAMPLE_COUNT/2+1]; spectrum64(signal, mean, power);
    float total = 0, weighted = 0, peakPower = -1;
    int peakBin = 1;
    for (int k = 1; k < SAMPLE_COUNT/2; ++k) {
      const float frequency = k*SAMPLE_RATE_HZ/SAMPLE_COUNT;
      total += power[k]; weighted += frequency*power[k];
      if (power[k] > peakPower) { peakPower = power[k]; peakBin = k; }
    }
    if (total <= 1e-12f) continue;
    result.activeMask |= static_cast<uint8_t>(1u << axis);
    result.values[offset+4] = peakBin*SAMPLE_RATE_HZ/SAMPLE_COUNT;
    result.values[offset+5] = peakPower/total;
    result.values[offset+6] = weighted/total;
  }
}

```


#### Optional CSV cleaner

```python
#!/usr/bin/env python3
"""Clean copied Serial Monitor text into a feature CSV (standard library only).
Usage: python clean_feature_csv.py serial_log.txt phase1_features.csv
Removes # comments and repeated headers. Only labeled 42-feature rows are kept.
Adds session_id derived from the source filename to keep recording IDs distinct.
Use a different source filename for each acquisition session.
"""
import argparse
import csv
import math
from pathlib import Path

AXES = ['ax','ay','az','gx','gy','gz']
STATS = ['mean','std','min','max','peak_hz','peak_fraction','centroid_hz']
FEATURES = [f'{a}_{s}' for a in AXES for s in STATS]
HEADER = ['recording_id','window_id','start_ms','effective_fs_hz','active_mask'] + FEATURES + ['label_id','label']
LABELS = ['tilt_left','tilt_right','up','down','idle']

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path)
    parser.add_argument('destination',type=Path)
    args=parser.parse_args()
    valid=[]
    with args.source.open(encoding='utf-8-sig') as file:
        for row in csv.reader(file):
            if len(row)!=len(HEADER) or row==HEADER or row[0].startswith('#'):
                continue
            try:
                label=int(row[-2])
                if label not in range(5) or row[-1]!=LABELS[label]: continue
                features=[float(v) for v in row[5:47]]
                if len(features)!=42 or not all(math.isfinite(v) for v in features): continue
                if not 47.5 <= float(row[3]) <= 52.5: continue
            except (ValueError,IndexError):
                continue
            valid.append(row)
    if not valid:
        raise SystemExit('No valid labeled 42-feature rows found. Use f mode and a label 0..4.')
    # Refuse accidental overwrite of an existing data file.
    with args.destination.open('x',newline='',encoding='utf-8') as file:
        writer=csv.writer(file)
        writer.writerow(['session_id']+HEADER)
        writer.writerows([[args.source.stem]+row for row in valid])
    print(f'Saved {len(valid)} labeled windows to {args.destination}')

if __name__=='__main__': main()

```



