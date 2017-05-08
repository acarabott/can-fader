#include "FaderTouchSensor.h"
#include "capSingleRead.h"

FaderTouchSensor::FaderTouchSensor(uint8_t pin) {
  m_pin = pin;
}

void FaderTouchSensor::update() {
  if (!m_enabled) return;
  m_prevTouching = m_isTouching;

  m_touchValue = capSingleRead(m_pin);
  m_isTouching = m_touchValue > m_touchThresh;

  // check for multi tap
  const auto now = millis();
  if (!m_inTapWindow) { m_tapCountResult = 0; }
  m_inTapWindow = now - m_prevTapTime < m_multiTapThresh;

  if (tapStarted()) {
    m_tapCount = m_inTapWindow ? m_tapCount + 1 : 1;
    m_prevTapTime = now;
  }

  if (tapEnded()) { m_tapCountResult = m_tapCount; }
}

uint16_t FaderTouchSensor::getTouchValue() {
  return m_touchValue;
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

uint16_t FaderTouchSensor::getTouchThresh() { return m_touchThresh; }

void FaderTouchSensor::setTouchThresh(uint16_t thresh) {
  m_touchThresh = constrain(thresh, 0, m_faderMax);
}
