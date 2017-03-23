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
auto g_interval = 500;

auto g_errorThresh = 5;

auto g_moving = false;
uint16_t g_prevLineValue = 0;

const size_t g_num_presets = 25;
#define EMPTY_PRESET -1
int16_t g_presets[g_num_presets];
uint8_t g_preset_index = 0;
const uint16_t g_preset_error_thresh = 5;

void clearPresets() {
  for (size_t i = 0; i < g_num_presets; ++i) {
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
  for (size_t i = 0; i < g_num_presets; ++i) {
    if (isNear(value, g_presets[i], g_preset_error_thresh)) {
      g_presets[i] = EMPTY_PRESET;
    }
  }
}

bool isPreset(int16_t value) {
  for (size_t i = 0; i < g_num_presets; ++i) {
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

void setDirection(decltype(HIGH) direction) {
  g_direction = direction;
  digitalWrite(g_motor_direction_pin, direction);
}

void toggleDirection() {
  g_direction = g_direction == LOW ? HIGH : LOW;
  setDirection(g_direction);
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

void setTarget(uint16_t value) {
  g_moving = true;
  g_target = constrain(value, 0, LINE_MAX);
}

void tick(float intensity) {
  const auto c_intensity = constrain(intensity, 0.0, 1.0);
  uint8_t dur = map(c_intensity, 0.0, 1.0, 1, 5);

  setMotorPwm(255);
  delay(dur);
  setMotorPwm(0);
  toggleDirection();
  setMotorPwm(255);
  delay(dur);
  setMotorPwm(0);
}

void loop() {
  const auto servoValue = analogRead(g_servo_pin);
  const auto touchValue = analogRead(g_touch_pin);
  const auto lineValue = analogRead(g_line_pin);

  const auto error = g_target - lineValue;

  // P_LBL("line: ", lineValue);
  if (g_moving) {
    if (abs(error) > g_errorThresh) {
      auto direction = error > 0 ? LOW : HIGH;
      auto pwm = abs(error) > 30 ? 255 : map(abs(error), 1, 30, 150, 255);
      setDirection(direction);
      setMotorPwm(pwm);
      delay(5);
      setMotorPwm(0);
    } else {
      setMotorPwm(0);
      g_moving = false;
    }
  }

  // Presets
  if (isPreset(lineValue) && lineValue != g_prevLineValue) {
    tick(0.8);
  }

  // regular ticks
  // if (lineValue != g_prevLineValue) {
  //   if (lineValue % 300 == 0)
  //   {
  //     tick(1.0);
  //   }
  //   else if (lineValue % 100 == 0)
  //   {
  //     tick(0.5);
  //   }
  // }



  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    if (read_value >= 0) {
      g_errorThresh = constrain(read_value, 0, 100);
    }
    else if (read_value == -6666) {
      addPreset(lineValue);
    }
    else {
      setTarget(abs(read_value));
    }
  }
}
