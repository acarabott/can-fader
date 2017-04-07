#include "Touche.h"

Touche touche(A5);

void setup()
{
  touche.setup();
  Serial.begin(115200);
}

void loop()
{
  touche.update();

  if (Serial.available() > 0) {
    const auto readValue = Serial.parseInt();
    Serial.print("starting training: ");
    Serial.println(readValue);
    touche.startTraining(readValue);
  }

  if (touche.gestureChanged()) {
    const auto gesture = touche.currentGesture();
    Serial.print("gesture changed: ");
    Serial.println(gesture);
  }
}
