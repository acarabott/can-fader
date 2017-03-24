#define P(VALUE) Serial.print((VALUE))
#define PL(VALUE) Serial.println((VALUE))
#define P_LBL(LABEL, VALUE) Serial.print((LABEL)); Serial.println((VALUE))

#include "FaderTouchSensor.h"

FaderTouchSensor::FaderTouchSensor(uint8_t touchPin, uint8_t positionPin) :
  m_touchPin(touchPin),
  m_positionPin(positionPin)
{
  // pinMode(m_touchPin, INPUT);
  // pinMode(m_positionPin, INPUT);
}

void FaderTouchSensor::update() {
  m_touchValue = analogRead(m_touchPin);

  // base level auto calibration
  m_positionValue = analogRead(m_positionPin);
  m_history[m_historyCount % historySize] = m_touchValue;
  m_historyCount++;
}


bool FaderTouchSensor::isTouching() {
  // If the fader is in the last 95% of the upper range, the touch sensor values
  // jump really high (> 1000)
  // If in this position we hvae to use the amount of fluctuation on the touch
  // value
  // When not touched, there will be very little fluctuation

  const auto averageCount = min(historySize, m_historyCount);

  float sum = 0.0;
  int16_t minVal = 1023;
  int16_t maxVal = 0;

  for (auto i = 0; i < averageCount; ++i) {
    const auto& val = m_history[i];
    // sum += val;
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
  }

  // const float avg = sum / float(historySize);
  // const auto absThresh = m_touchMin + m_touchThresh;

  // String p;
  // p += "900 ";

  // p += m_touchValue;
  // p += " ";

  // p += absThresh;
  // p += " ";

  // p += " 1023";

  // PL(p);
  // if the fader position is not affecting the touch value
  // if (m_positionValue < faderMax) {
  //   return avg > absThresh;
  // }

  // fader position is affecting the touch value
  const auto fluctuation = maxVal - minVal;
  // return avg > absThresh && fluctuation > m_rangeThresh;
  return fluctuation > m_rangeThresh;
}

void FaderTouchSensor::setTouchThresh(uint16_t thresh) {
  m_touchThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getTouchThresh() {
  return m_touchThresh;
}

void FaderTouchSensor::setRangeThresh(uint16_t thresh) {
  m_rangeThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getRangeThresh() {
  return m_rangeThresh;
}
