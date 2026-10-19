#pragma once
#include <math.h>

constexpr int kWindowSize = 50;
constexpr int kAxes = 6;
constexpr int kFeatureCount = 24;

// Input rows: ax_g, ay_g, az_g, gx_dps, gy_dps, gz_dps.
// Output: mean, population std, min, max for ax, then ay, az, gx, gy, gz.
inline void extractFeatures(const float window[kWindowSize][kAxes],
                            float output[kFeatureCount]) {
  for (int axis = 0; axis < kAxes; ++axis) {
    float sum = 0.0f;
    float minimum = window[0][axis];
    float maximum = minimum;
    for (int i = 0; i < kWindowSize; ++i) {
      const float value = window[i][axis];
      sum += value;
      if (value < minimum) minimum = value;
      if (value > maximum) maximum = value;
    }
    const float mean = sum / kWindowSize;
    float squaredDifferences = 0.0f;
    for (int i = 0; i < kWindowSize; ++i) {
      const float difference = window[i][axis] - mean;
      squaredDifferences += difference * difference;
    }
    output[axis * 4]     = mean;
    output[axis * 4 + 1] = sqrtf(squaredDifferences / kWindowSize);
    output[axis * 4 + 2] = minimum;
    output[axis * 4 + 3] = maximum;
  }
}
