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

  const auto thresh = touchValue < m_fluctuationSplit
    ? m_lowFluctuationThresh
    : m_highFluctuationThresh;

  m_isTouching = fluctuation > thresh;

  // check for multi tap
  const auto now = millis();
  if (!m_inTapWindow) { m_tapCountResult = 0; }
  m_inTapWindow  = now - m_prevTapTime < m_multiTapThresh;

  if (tapStarted()) {
    m_tapCount = m_inTapWindow ? m_tapCount + 1 : 1;
    m_prevTapTime = now;
  }

  if (tapEnded()) { m_tapCountResult = m_tapCount; }
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

uint8_t FaderTouchSensor::tapCount() {
  return m_inTapWindow || m_isTouching || !m_enabled ? 0 : m_tapCountResult;
}

void FaderTouchSensor::enable() { m_enabled = true; }
void FaderTouchSensor::disable() { m_enabled = false; }
bool FaderTouchSensor::isEnabled() { return m_enabled; }

uint16_t FaderTouchSensor::getLowFluctuationThresh() {
  return m_lowFluctuationThresh;
}
void FaderTouchSensor::setLowFluctuationThresh(uint16_t thresh) {
  m_lowFluctuationThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getHighFluctuationThresh() {
  return m_highFluctuationThresh;
}
void FaderTouchSensor::setHighFluctuationThresh(uint16_t thresh) {
  m_highFluctuationThresh = constrain(thresh, 0, 1023);
}

uint16_t FaderTouchSensor::getFluctuationSplit() {
  return m_fluctuationSplit;
}
void FaderTouchSensor::setFluctuationSplit(uint16_t thresh) {
  m_fluctuationSplit = constrain(thresh, 0, 1023);
}
