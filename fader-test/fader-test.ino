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

// Don't use pins 8 or 9, used by Touche internally

const uint8_t g_touch_pin = A0;
const uint8_t g_line_pin = A1;
const uint8_t g_gesture_pin = A5;

const uint8_t g_motor_direction_pin = 12;
const uint8_t g_motor_pwm_pin = 3;

int16_t g_lineValue = 0;
int16_t g_prevLineValue = 0;

bool g_tactileEnabled = false;
uint8_t g_click = 50;
uint8_t g_maxClickIntensity = 100;

FaderTouchSensor touchSensor(g_touch_pin);
FaderMover faderMover(g_motor_pwm_pin, g_motor_direction_pin);
FaderPresets presets;
Touche touche(g_gesture_pin);
// bool g_toucheTraining = false;
bool g_prevGesture = 0;

void setup() {
  pinMode(g_motor_direction_pin, OUTPUT);
  pinMode(g_motor_pwm_pin, OUTPUT);

  touche.setup();

  Serial.begin(115200);
}

void logValue(String& s, int value) {
  s += value;
  s += ",";
}

void loop() {
  g_prevLineValue = g_lineValue;
  g_lineValue = analogRead(g_line_pin);
  Serial.print("l:");
  Serial.println(g_lineValue);

  // faderMover.update(g_lineValue);
  // faderMover.isMoving() ? touchSensor.disable() : touchSensor.enable();
  touchSensor.update();

  // const auto touching = touchSensor.isTouching();
  if (touchSensor.tapStarted()) { PL("m:touch start"); }
  if (touchSensor.tapEnded()) { PL("m:touch end"); }

  // if (presets.isPreset(g_lineValue) &&
  //     g_lineValue != g_prevLineValue &&
  //     !g_tactileEnabled)
  // {
  //   faderMover.tick(20);
  // }

  touche.update();

  Serial.print("d:");
  touche.printResults();

  // Serial.print("t:");
  // Serial.print("0,");
  // Serial.print(touchSensor.getTouchValue());
  // Serial.print(",");
  // Serial.print(touchSensor.getTouchThresh());
  // Serial.print(",");
  // Serial.print("1023");
  // Serial.println("");

  // if (g_toucheTraining && !touche.training()) {
  //   g_toucheTraining = false;
  //   Serial.println("finished training");
  // }

  // if (touche.gestureChanged()) {
  //   const auto gesture = touche.currentGesture();

  //   P_LBL("gesture: ", gesture);

  //   if (gesture == 1 && g_prevGesture == 0) {
  //     PL("touch start");
  //   }
  //   else if (gesture == 0 && g_prevGesture == 1) {
  //     P_LBL("prev: ", g_prevGesture);
  //     PL("touch end");
  //   }

  //   P_LBL("assigning: ", gesture);
  //   g_prevGesture = gesture;
  // }

  // const auto tapCount = touchSensor.tapCount();

  // if (tapCount > 1) {
  //   // P_LBL("tap count: ", tapCount);
  //   auto success = false;
  //   uint64_t delay = 150;

  //   if (tapCount == 2)      { success = presets.add(g_lineValue); }
  //   else if (tapCount == 3) { success = presets.remove(g_lineValue); }
  //   else if (tapCount == 4) {
  //     success = presets.removeAll();
  //     delay = 500;
  //   }
  //   else if (tapCount == 10) {
  //     g_tactileEnabled = !g_tactileEnabled;
  //     success = true;
  //   }

  //   if (success) {
  //     faderMover.feedback(g_lineValue, 100, delay);
  //   }
  // }

  // // haptic feedback, increasing intensity with position
  // if (g_tactileEnabled &&  g_lineValue % g_click == 0) {
  //   const auto intensity = map(g_lineValue, 0, LINE_MAX, 1, g_maxClickIntensity);
  //   faderMover.tick(intensity);
  // }

  // if (Serial.available() > 0) {
  //   const auto read_value = Serial.parseInt();
  //   P_LBL("move to: ", read_value);
  //   faderMover.moveToNorm(read_value);

  //   // if (read_value >= 0) {
  //   //   // const auto position = constrain(read_value, 0, ANALOG_IN_MAX);
  //   //   P_LBL("training: ", read_value);
  //   //   touche.startTraining(read_value);
  //   //   g_toucheTraining = true;
  //   // }
  //   // else {
  //   //   touche.setLag(abs(read_value));
  //   // }
  // }
}
