// #include <PID_v1.h>

// TODO
// include library in repo
#define PL(VALUE) Serial.println(VALUE)
#define PRINT_LABEL(LABEL, VALUE) Serial.print(LABEL); Serial.println(VALUE)
#define ANALOG_IN_MAX 1023
#define ANALOG_OUT_MAX 255
#define CONSTRAIN_ANALOG(VALUE) constrain((VALUE), 0, ANALOG_IN_MAX)
#define MAP_ANALOG_IN_TO_ANALOG_OUT(VALUE) map((VALUE), 0, ANALOG_IN_MAX, 0, ANALOG_OUT_MAX)

const uint8_t servo_pin = A0;
const uint8_t touch_pin = A2;
const uint8_t line_pin = A4;

// double faderIn, faderOut, faderSet;
// PID pid(&faderIn, &faderOut, &faderSet, 2, 5, 1, DIRECT);

void setup() {
  pinMode(touch_pin, INPUT);
  pinMode(line_pin, INPUT);
  pinMode(servo_pin, INPUT);

  // pid.SetMode(AUTOMATIC);

  Serial.begin(115200);
}

void loop() {
  const auto touch_value = analogRead(touch_pin);
  const auto line_value = analogRead(line_pin);
  const auto servo_value = analogRead(servo_pin);

  // String withScale = "900 ";
  // withScale += touch_value;
  // withScale += " 1023";
  // PL(withScale);
  // PRINT_LABEL("touch: ", touch_value);
  // PRINT_LABEL("line: ", line_value);
  // PRINT_LABEL("servo: ", servo_value);
  // PL("");

  if (Serial.available() > 0) {
    const auto read_value = Serial.parseInt();
    const auto constrained = CONSTRAIN_ANALOG(read_value);
    const auto output_value = MAP_ANALOG_IN_TO_ANALOG_OUT(constrained);
    // PRINT_LABEL("outputting: ", output_value);
    // analogWrite(servo_pin, output_value);
  }

  // faderIn = analogRead(line_pin);

}
