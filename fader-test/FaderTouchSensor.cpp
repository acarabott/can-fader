#include "FaderTouchSensor.h"

FaderTouchSensor::FaderTouchSensor() {}

void FaderTouchSensor::update(int16_t touchValue) {
  if (!m_enabled) return;

  m_touchValue = touchValue;
  m_history[m_historyCount % historySize] = m_touchValue;
  m_historyCount++;

  // calculate from the amount of fluctuation
  // signal varies a lot when touched, not a lot when not touched
  const auto windowSize = min(historySize, m_historyCount);

  int16_t minVal = 1023;
  int16_t maxVal = 0;
  for (auto i = 0; i < windowSize; ++i) {
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
  return m_isTouching && m_enabled;
}

bool FaderTouchSensor::tapStarted() {
  return isTouching() && !m_prevTouching;
}

bool FaderTouchSensor::tapEnded() {
  return !isTouching() && m_prevTouching;
}

void FaderTouchSensor::enable() { m_enabled = true; }
void FaderTouchSensor::disable() { m_enabled = false; }
bool FaderTouchSensor::isEnabled() { return m_enabled; }

void FaderTouchSensor::setFluctuationThresh(uint16_t thresh) {
  m_fluctuationThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getFluctuationThresh() {
  return m_fluctuationThresh;
}
