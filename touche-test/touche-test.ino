#include "Touche.h"

Touche touche(A5);

bool training = false;
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
    training = true;
  }

  if (training && !touche.training()) {
    training = false;
    Serial.println("finished training");
  }

  if (touche.gestureChanged()) {
    const auto gesture = touche.currentGesture();
    Serial.print("gesture changed: ");
    Serial.println(gesture);
  }
}
