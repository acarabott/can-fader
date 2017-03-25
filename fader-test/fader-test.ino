#include "FaderTouchSensor.h"
#include "FaderMover.h"
// TODO

// - Wrap touch states (off/on/max)
// - Wrap whole slider as class

#define P(VALUE) Serial.print((VALUE))
#define PL(VALUE) Serial.println((VALUE))
#define P_LBL(LABEL, VALUE) Serial.print((LABEL)); Serial.println((VALUE))
#define ANALOG_IN_MAX 1023
#define ANALOG_OUT_MAX 255

#define LINE_MAX 1019

const uint8_t g_servo_pin = A0;
const uint8_t g_touch_pin = A2;
const uint8_t g_line_pin = A4;

const uint8_t g_motor_direction_pin = 12;
const uint8_t g_motor_pwm_pin = 3;

auto g_direction = HIGH;
int16_t g_target = 512;

auto g_prevTime = millis();
auto g_interval = 20; // 50Hz AC

auto g_errorThresh = 5;

auto g_moving = false;
int16_t g_lineValue = 0;
int16_t g_prevLineValue = 0;

FaderTouchSensor touchSensor;
FaderMover faderMover(g_motor_pwm_pin, g_motor_direction_pin);

const size_t g_num_presets = 25;
#define EMPTY_PRESET -1
int16_t g_presets[g_num_presets];
uint8_t g_preset_index = 0;
const uint16_t g_preset_error_thresh = 5;

void clearPresets() {
  for (auto i = 0; i < g_num_presets; ++i) {
    g_presets[i] = EMPTY_PRESET;
  }
}

void addPreset(int16_t value) {
  g_presets[g_preset_index] = value;

  g_preset_index++;
  g_preset_index %= g_num_presets;
}

bool inRange(int64_t value, int64_t min, int64_t max) {
  return value >= min && value <= max;
}

bool isNear(int64_t value, int64_t target, int64_t error) {
  return inRange(value, target - error, target + error);
}


void deletePreset(int16_t value) {
  for (auto i = 0; i < g_num_presets; ++i) {
    if (isNear(value, g_presets[i], g_preset_error_thresh)) {
      g_presets[i] = EMPTY_PRESET;
    }
  }
}

bool isPreset(int16_t value) {
  for (auto i = 0; i < g_num_presets; ++i) {
    const auto& preset = g_presets[i];
    if (preset != EMPTY_PRESET &&
        isNear(value, preset, g_preset_error_thresh)) {
      return true;
    }
  }
  return false;
}

void setup() {
  pinMode(g_motor_direction_pin, OUTPUT);
  pinMode(g_motor_pwm_pin, OUTPUT);

  clearPresets();
  Serial.begin(115200);
}

void loop() {
  const auto servoValue = analogRead(g_servo_pin);
  const auto touchValue = analogRead(g_touch_pin);

  g_prevLineValue = g_lineValue;
  g_lineValue = analogRead(g_line_pin);

  const auto error = g_target - g_lineValue;

  touchSensor.update(touchValue);
  faderMover.update(g_lineValue);

  const auto touching = touchSensor.isTouching();

  if (touchSensor.tapStarted())
  {
    // PL("tap start!");
  }

  if (touchSensor.tapEnded()){
    // PL("tap end!");
  }

  // Presets
  // if (isPreset(g_lineValue) && g_lineValue != g_prevLineValue) {
  //   faderMover.tick(80);
  // }

  // haptic feedback, increasing intensity with position
  if (g_lineValue % 100 == 0)
  {
    const auto intensity = g_lineValue / 10;
    faderMover.tick(intensity);
  }



  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    if (read_value >= 0) {
      g_errorThresh = constrain(read_value, 0, 100);
    }
    else if (read_value == -6666) {
      addPreset(g_lineValue);
    }
    else {
      const auto position = constrain(abs(read_value), 0, 100);
      faderMover.moveTo(position);
      // g_touch_history_size = constrain(abs(read_value), 0, 100);
      // P_LBL("touch hist size: ", g_touch_history_size);
      // g_touch_thresh = constrain(abs(read_value), 0, ANALOG_IN_MAX);
      // P_LBL("touch thresh: ", g_touch_thresh);
    }
  }
}
