#include <PID_v1.h>

// TODO
/*
- get PID to work
- include library in repo
- Wrap touch states (off/on/max)
- Wrap whole slider as class
*/
#define P(VALUE) Serial.print((VALUE))
#define PL(VALUE) Serial.println((VALUE))
#define P_LBL(LABEL, VALUE) Serial.print((LABEL)); Serial.println((VALUE))
#define ANALOG_IN_MAX 1023
#define ANALOG_OUT_MAX 255
#define CONSTRAIN_AIN(VALUE) constrain((VALUE), 0, ANALOG_IN_MAX)
#define CONSTRAIN_AOUT(VALUE) constrain((VALUE), 0, ANALOG_OUT_MAX)
#define MAP_AIN_TO_AOUT(VALUE) map((VALUE), 0, ANALOG_IN_MAX, 0, ANALOG_OUT_MAX)

const uint8_t g_servo_pin = A0;
const uint8_t g_touch_pin = A2;
const uint8_t g_line_pin = A4;

const uint8_t g_motor_direction_pin = 12;
const uint8_t g_motor_pwm_pin = 3;

double g_faderIn, g_faderOut, g_faderSet;
const double g_kp = 2;
const double g_ki = 5;
const double g_kd = 1;

PID pid(&g_faderIn, &g_faderOut, &g_faderSet, g_kp, g_ki, g_kd, DIRECT);

auto g_prevTime = millis();
auto g_interval = 500;

void setFaderPoint(uint16_t point) {
  // make sure this matches the input range
  g_faderSet = constrain(point, 0, ANALOG_IN_MAX);
  return g_faderSet;
}

void setup() {
  pinMode(g_touch_pin, INPUT);
  pinMode(g_line_pin, INPUT);
  pinMode(g_servo_pin, INPUT);

  // read current position for PID
  g_faderIn = analogRead(g_servo_pin);
  setFaderPoint(512);

  // we will use the abs value for the motor and the sign to determine direction
  pid.SetOutputLimits(-255, 255);
  // initialize PID
  pid.SetMode(AUTOMATIC);

  Serial.begin(115200);
}

void loop() {
  const auto touch_value = analogRead(g_touch_pin);
  const auto line_value = analogRead(g_line_pin);
  const auto servo_value = analogRead(g_servo_pin);

  // P_LBL("line_value: ", line_value);
  // PL(analogRead(g_line_pin));

  // calculate servo output and direction using PID
  g_faderIn = line_value;
  pid.Compute(); // this reads from g_faderIn and writes to g_faderOut

  const auto direction = g_faderOut >= 0 ? HIGH : LOW;
  const auto motor_value = constrain(abs(g_faderOut), 0, ANALOG_OUT_MAX);
  digitalWrite(g_motor_direction_pin, direction);
  analogWrite(g_motor_pwm_pin, motor_value);

  const auto curTime = millis();
  if (curTime - g_prevTime > g_interval) {
    P_LBL("set: ", g_faderSet);
    P_LBL("in: ", g_faderIn);
    P_LBL("out: ", g_faderOut);
    P_LBL("direction: ", direction);
    P_LBL("motor_value: ", motor_value);
    PL("");
    g_prevTime = curTime;
  }

  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    setFaderPoint(read_value);
    P_LBL("set point: ", g_faderSet);
  }

  // g_faderIn = analogRead(g_line_pin);

}
