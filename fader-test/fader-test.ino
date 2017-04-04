#include "FaderTouchSensor.h"
#include "FaderMover.h"
#include "FaderPresets.h"
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
FaderPresets presets;

void setup() {
  pinMode(g_motor_direction_pin, OUTPUT);
  pinMode(g_motor_pwm_pin, OUTPUT);

  Serial.begin(115200);
}

uint8_t g_click = 25;
void loop() {
  const auto servoValue = analogRead(g_servo_pin);
  const auto touchValue = analogRead(g_touch_pin);

  g_prevLineValue = g_lineValue;
  g_lineValue = analogRead(g_line_pin);

  const auto error = g_target - g_lineValue;

  faderMover.update(g_lineValue);
  faderMover.isMoving() ? touchSensor.disable() : touchSensor.enable();
  touchSensor.update(touchValue);

  if (presets.isPreset(g_lineValue) && g_lineValue != g_prevLineValue) {
    faderMover.tick(20);
  }

  const auto touching = touchSensor.isTouching();
  if (touchSensor.tapStarted()) { PL("touch start"); }
  if (touchSensor.tapEnded()) { PL("touch end"); }

  const auto tapCount = touchSensor.tapCount();
  if (tapCount == 2) {
    presets.add(g_lineValue);
  }
  else if (tapCount == 3) {
    presets.remove(g_lineValue);
    faderMover.tick(100);
  }


  // Presets

  // haptic feedback, increasing intensity with position
  // if (g_lineValue % g_click == 0)
  // {
  //   const auto intensity = map(g_lineValue, 0, LINE_MAX, 1, 100);
  //   faderMover.tick(intensity);
  // }



  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    if (read_value >= 0) {
      g_errorThresh = constrain(read_value, 0, 100);
    }
    else {
      const auto position = constrain(abs(read_value), 0, 100);
      // g_click = abs(read_value);
      faderMover.moveTo(position);
      // g_touch_history_size = constrain(abs(read_value), 0, 100);
      // P_LBL("touch hist size: ", g_touch_history_size);
      // g_touch_thresh = constrain(abs(read_value), 0, ANALOG_IN_MAX);
      // P_LBL("touch thresh: ", g_touch_thresh);
    }
  }
}
