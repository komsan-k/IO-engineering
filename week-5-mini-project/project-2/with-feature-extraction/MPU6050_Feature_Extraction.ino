/*
 Complete ESP32 MPU6050 feature-extraction/data-collection sketch.
 Samples six signals at nominal 50Hz, calculates 24 features per 50-reading
 window, then prints one labeled CSV row per window.

 Folder files: this .ino, MotionFeatures.h, MPU6050Simple.h/.cpp.
 Only ESP32 Arduino core and Wire are needed. No TinyML library/model needed.
 Pins: SDA4, SCL5. Address0x68 (AD0 grounded). See README for wiring.

 Serial Monitor 115200 baud. Commands (send one character):
   0=tilt_left/OFF, 1=tilt_right/ON, 2=up/BRIGHTER,
   3=down/DIMMER, 4=idle. A digit selects a label and starts recording.
   p=pause, r=resume as a NEW recording, h=print CSV header, ?=help.
 Labels are ground truth ENTERED BY YOU, not predicted by a model.
 Paused at startup. A label change discards the unfinished window.
 Each digit/r starts a new recording_id for grouped dataset splitting.
 Rows beginning '#' are comments and must be removed before CSV import.

 Scope: feature extraction and labeled dataset collection. The previous
 six-input model cannot consume these 24 inputs. Train a new model first
 before adding TinyML inference/gesture-controlled PWM lighting.
*/

#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "MPU6050Simple.h"
#include "MotionFeatures.h"

constexpr int kSda = 4;
constexpr int kScl = 5;
constexpr uint32_t kPeriodUs = 20000;
constexpr uint32_t kMaxGapUs = 30000; // discard partial windows after >30ms gap
constexpr uint8_t kAddress = 0x68;

MPU6050Simple mpu;
float samples[kWindowSize][kAxes];
float features[kFeatureCount];
int sampleCount = 0;
int selectedLabel = -1;
bool recording = false;
uint32_t recordingId = 0;
uint32_t windowId = 0;
uint32_t lastSampleUs = 0;
uint32_t windowStartUs = 0;
uint32_t windowStartMs = 0;
const char* const kLabels[] = {"tilt_left", "tilt_right", "up", "down", "idle"};
const char* const kAxisNames[] = {"ax", "ay", "az", "gx", "gy", "gz"};
const char* const kStatistics[] = {"mean", "std", "min", "max"};

void printHeader() {
  Serial.print("recording_id,window_id,start_ms,sample_span_ms");
  for (int axis = 0; axis < kAxes; ++axis) {
    for (int statistic = 0; statistic < 4; ++statistic) {
      Serial.print(','); Serial.print(kAxisNames[axis]); Serial.print('_');
      Serial.print(kStatistics[statistic]);
    }
  }
  Serial.println(",label_id,label");
}

void printHelp() {
  Serial.println("# Commands: 0=tilt_left 1=tilt_right 2=up 3=down 4=idle");
  Serial.println("# p=pause r=resume/new recording h=header ?=help");
  Serial.println("# Labels are manual ground truth. Keep each recording to one class.");
  Serial.println("# Remove # comments and repeated headers when saving CSV.");
}

void startRecording(int label) {
  selectedLabel = label;
  ++recordingId;
  sampleCount = 0;
  lastSampleUs = micros();
  recording = true;
  Serial.print("# Recording "); Serial.print(recordingId);
  Serial.print(" label="); Serial.println(kLabels[label]);
}

void processCommands() {
  while (Serial.available()) {
    const char command = static_cast<char>(Serial.read());
    if (command >= '0' && command <= '4') startRecording(command - '0');
    else if (command == 'p') {
      recording = false; sampleCount = 0;
      Serial.println("# Paused");
    } else if (command == 'r') {
      if (selectedLabel >= 0) startRecording(selectedLabel);
      else Serial.println("# Select a label 0..4 first");
    } else if (command == 'h') printHeader();
    else if (command == '?') printHelp();
    // Ignore newline, carriage return and other characters.
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(kSda, kScl);
  Wire.setClock(100000);
  Wire.setTimeOut(50);
  if (!mpu.begin(Wire, kAddress,
                 MPU6050Simple::AccelRange::G4,
                 MPU6050Simple::GyroRange::DPS500)) {
    Serial.print("# FATAL: "); Serial.println(mpu.errorMessage());
    while (true) delay(1000);
  }
  Serial.println("# MPU6050 ready: +/-4g, +/-500deg/s, 50Hz, 50 readings/window");
  printHelp();
  printHeader();
}

void loop() {
  processCommands();
  if (!recording) { delay(1); return; }
  const uint32_t nowUs = micros();
  const uint32_t elapsedUs = nowUs - lastSampleUs;
  if (elapsedUs < kPeriodUs) { delay(1); return; }
  if (sampleCount > 0 && elapsedUs > kMaxGapUs) {
    sampleCount = 0;
    Serial.println("# Sampling gap: discarded partial window");
  }
  // Use elapsed-time scheduling without catch-up bursts of duplicate samples.
  lastSampleUs = nowUs;
  float reading[kAxes];
  if (!mpu.readFeatures(reading)) {
    sampleCount = 0;
    Serial.print("# Read failed; window discarded: ");
    Serial.println(mpu.errorMessage());
    return;
  }
  for (int axis = 0; axis < kAxes; ++axis) {
    if (!isfinite(reading[axis])) {
      sampleCount = 0;
      Serial.println("# Invalid sensor value: window discarded");
      return;
    }
  }
  if (sampleCount == 0) {
    windowStartUs = nowUs;
    windowStartMs = millis();
  }
  for (int axis = 0; axis < kAxes; ++axis) samples[sampleCount][axis] = reading[axis];
  ++sampleCount;
  if (sampleCount != kWindowSize) return;
  extractFeatures(samples, features);
  ++windowId;
  Serial.print(recordingId); Serial.print(',');
  Serial.print(windowId); Serial.print(',');
  Serial.print(windowStartMs); Serial.print(',');
  // 50 samples have 49 intervals: nominal first-to-last span is 980ms.
  Serial.print((nowUs - windowStartUs) / 1000.0f, 3);
  for (int i = 0; i < kFeatureCount; ++i) {
    Serial.print(','); Serial.print(features[i], 6);
  }
  Serial.print(','); Serial.print(selectedLabel); Serial.print(',');
  Serial.println(kLabels[selectedLabel]);
  sampleCount = 0;
}
