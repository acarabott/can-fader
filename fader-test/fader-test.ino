#include "FaderTouchSensor.h"
#include "FaderMover.h"
#include "FaderPresets.h"
#include "Touche.h"

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

int16_t g_lineValue = 0;
int16_t g_prevLineValue = 0;

bool g_tactileEnabled = false;
uint8_t g_click = 50;
uint8_t g_maxClickIntensity = 90;

FaderTouchSensor touchSensor;
FaderMover faderMover(g_motor_pwm_pin, g_motor_direction_pin);
FaderPresets presets;
Touche touche(A5);
bool g_toucheTraining = false;
bool g_prevGesture = 0;

void setup() {
  pinMode(g_motor_direction_pin, OUTPUT);
  pinMode(g_motor_pwm_pin, OUTPUT);

  touche.setup();

  Serial.begin(115200);
}

void loop() {
  const auto servoValue = analogRead(g_servo_pin);
  const auto touchValue = analogRead(g_touch_pin);

  g_prevLineValue = g_lineValue;
  g_lineValue = analogRead(g_line_pin);

  faderMover.update(g_lineValue);
  faderMover.isMoving() ? touchSensor.disable() : touchSensor.enable();
  touchSensor.update(touchValue);

  if (presets.isPreset(g_lineValue) &&
      g_lineValue != g_prevLineValue &&
      !g_tactileEnabled)
  {
    faderMover.tick(20);
  }

  const auto touching = touchSensor.isTouching();
  // if (touchSensor.tapStarted()) { PL("touch start"); }
  // if (touchSensor.tapEnded()) { PL("touch end"); }

  touche.update();

  if (g_toucheTraining && !touche.training()) {
    g_toucheTraining = false;
    Serial.println("finished training");
  }

  if (touche.gestureChanged()) {
    const auto gesture = touche.currentGesture();

    P_LBL("gesture: ", gesture);

    if (gesture == 1 && g_prevGesture == 0) {
      PL("touch start");
    }
    else if (gesture == 0 && g_prevGesture == 1) {
      P_LBL("prev: ", g_prevGesture);
      PL("touch end");
    }

    P_LBL("assigning: ", gesture);
    g_prevGesture = gesture;
  }

  const auto tapCount = touchSensor.tapCount();

  if (tapCount > 1 && false) {
    // P_LBL("tap count: ", tapCount);
    auto success = false;
    uint64_t delay = 150;

    if (tapCount == 2)      { success = presets.add(g_lineValue); }
    else if (tapCount == 3) { success = presets.remove(g_lineValue); }
    else if (tapCount == 4) {
      success = presets.removeAll();
      delay = 500;
    }
    else if (tapCount == 5) {
      g_tactileEnabled = !g_tactileEnabled;
      success = true;
    }

    if (success) { faderMover.feedback(g_lineValue, 100, delay); }
  }

  // haptic feedback, increasing intensity with position
  if (g_tactileEnabled &&  g_lineValue % g_click == 0) {
    const auto intensity = map(g_lineValue, 0, LINE_MAX, 1, g_maxClickIntensity);
    faderMover.tick(intensity);
  }

  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();

    if (read_value >= 0) {
      // const auto position = constrain(read_value, 0, ANALOG_IN_MAX);
      // P_LBL("position: ", position);
      // faderMover.moveTo(position);
      // g_click = abs(read_value);
      P_LBL("training: ", read_value);
      touche.startTraining(read_value);
      g_toucheTraining = true;
    }
    else {
      // g_maxClickIntensity = constrain(abs(read_value), 0, 100);
      touche.setLag(abs(read_value));
    }
  }
}
