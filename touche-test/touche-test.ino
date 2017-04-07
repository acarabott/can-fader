#include "Touche.h"

Touche touche(A5);


decltype(millis()) trainingStartTime = 0;

void setup()
{
  touche.setup();
  Serial.begin(115200);
}

void loop()
{
  touche.update();

  const auto now = millis();

  if (Serial.available() > 0) {
    const auto trainingIdx = Serial.parseInt();
    Serial.print("starting training: ");
    Serial.println(trainingIdx);
    touche.startTraining(trainingIdx);
    trainingStartTime = now;
  }

  if (touche.training() && now - trainingStartTime > 2000) {
    Serial.println("stopping training");
    touche.stopTraining();
  }


  if (touche.gestureChanged()) {
    const auto gesture = touche.currentGesture();
    Serial.print("gesture changed: ");
    Serial.println(gesture);
  }
}
