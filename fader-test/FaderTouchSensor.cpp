#define P(VALUE) Serial.print((VALUE))
#define PL(VALUE) Serial.println((VALUE))
#define P_LBL(LABEL, VALUE) Serial.print((LABEL)); Serial.println((VALUE))

#include "FaderTouchSensor.h"

FaderTouchSensor::FaderTouchSensor() {}

void FaderTouchSensor::update(int16_t touchValue) {
  m_touchValue = touchValue;
  m_history[m_historyCount % historySize] = m_touchValue;
  m_historyCount++;

  // calculate from the amount of fluctuation
  // signal varies a lot when touched, not a lot when not touched
  const auto averageCount = min(historySize, m_historyCount);

  int16_t minVal = 1023;
  int16_t maxVal = 0;

  for (auto i = 0; i < averageCount; ++i) {
    const auto& val = m_history[i];
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
  }

  // fader position is affecting the touch value
  const auto fluctuation = maxVal - minVal;
  m_prevTouching = m_isTouching;
  m_isTouching = fluctuation > m_fluctuationThresh;
}


bool FaderTouchSensor::isTouching() {
  return m_isTouching;
}

bool FaderTouchSensor::tapStarted() {
  return m_isTouching && !m_prevTouching;
}

bool FaderTouchSensor::tapEnded() {
  return !m_isTouching && m_prevTouching;
}

void FaderTouchSensor::setFluctuationThresh(uint16_t thresh) {
  m_fluctuationThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getFluctuationThresh() {
  return m_fluctuationThresh;
}
