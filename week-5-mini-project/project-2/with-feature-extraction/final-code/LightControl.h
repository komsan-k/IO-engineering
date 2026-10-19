#pragma once

// Hardware-independent command logic. Duty cycle is 0..255 (8-bit PWM).
class LightControl {
public:
  int brightness = 0;
  int savedBrightness = 128;
  bool armed = false; // require a complete idle window before the first command

  void disarm() { armed = false; }

  // Returns true only when a command was accepted. Caller updates PWM then.
  bool accept(int label) {
    if (label == 4) { armed = true; return false; }
    if (label < 0 || label > 3 || !armed) return false;
    armed = false; // no repeated commands until another idle window
    switch (label) {
      case 0:
        if (brightness > 0) savedBrightness = brightness;
        brightness = 0;
        break;
      case 1:
        brightness = savedBrightness;
        break;
      case 2:
        brightness += 32;
        if (brightness > 255) brightness = 255;
        if (brightness > 0) savedBrightness = brightness;
        break;
      case 3:
        if (brightness > 0) savedBrightness = brightness;
        brightness -= 32;
        if (brightness < 0) brightness = 0;
        if (brightness > 0) savedBrightness = brightness;
        break;
    }
    return true;
  }
};
