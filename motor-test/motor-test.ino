#define PRINT(PRINT_ME) Serial.print(PRINT_ME)
#define PRINT_LABEL(LABEL, VALUE) Serial.print(LABEL); Serial.println(VALUE)
#define ANALOG_IN_MAX 1023
#define ANALOG_OUT_MAX 255
#define CONSTRAIN_ANALOG(VALUE) constrain((VALUE), 0, ANALOG_IN_MAX)
#define MAP_ANALOG_IN_TO_ANALOG_OUT(VALUE) map((VALUE), 0, ANALOG_IN_MAX, 0, ANALOG_OUT_MAX)

const uint8_t g_motor_direction_pin = 12;
const uint8_t g_motor_pwm_pin = 3;
const uint8_t g_motor_brake_pin = 9;
const uint8_t g_motor_sensing_pin = 0;

auto g_direction = HIGH;

auto prevTime = millis();
auto timeInterval = 500;

void setDirection(decltype(HIGH) direction) {
  g_direction = direction;
  digitalWrite(g_motor_direction_pin, direction);
}

void toggleDirection() {
  g_direction = g_direction == LOW ? HIGH : LOW;
  setDirection(g_direction);
}

void setup() {
  pinMode(g_motor_direction_pin, OUTPUT);
  pinMode(g_motor_pwm_pin, OUTPUT);
  pinMode(g_motor_brake_pin, OUTPUT);
  pinMode(g_motor_sensing_pin, INPUT);

  moveToOtherEnd();

  Serial.begin(9600);
}

void setMotorPwm(uint8_t value) {
  analogWrite(g_motor_pwm_pin, constrain(value, 0, ANALOG_OUT_MAX));
}

void moveToOtherEnd() {
  toggleDirection();
  setMotorPwm(255);
  delay(200);
  setMotorPwm(0);
}

void loop() {
  auto curTime = millis();
  if (curTime - prevTime > timeInterval) {
    const auto sensorValue = analogRead(g_motor_sensing_pin);
    prevTime = curTime;

    PRINT_LABEL("sensing: ", sensorValue);
  }

  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    if (read_value >= 0) {
      const auto constrained = CONSTRAIN_ANALOG(read_value);
      const auto output_value = constrained;
      // const auto output_value = MAP_ANALOG_IN_TO_ANALOG_OUT(constrained);
      analogWrite(g_motor_pwm_pin, output_value);

      PRINT_LABEL("outputting: ", output_value);
    }
    else if (read_value == -1) {
      moveToOtherEnd();
      PRINT_LABEL("direction: ", g_direction);
    }
  }
}
