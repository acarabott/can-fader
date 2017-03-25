#include "FaderMover.h"

FaderMover::FaderMover(uint8_t motorPin, uint8_t directionPin) :
  m_motorPin(motorPin),
  m_directionPin(directionPin),
  m_lastMovedTime(millis())
{
}

void FaderMover::update(uint16_t position) {
  if (!m_moving) return;

  const auto curTime = millis();
  const auto timeThresh = m_adjusting ? 5 : 0;

  if (curTime - m_lastMovedTime > timeThresh) {
    m_lastMovedTime = curTime;
    m_adjusting = !m_adjusting;

    if (m_adjusting) {
      const int16_t error = m_target - position;
      const auto absError = abs(error);

      if (absError > m_errorThresh) {
        setDirection(error > 0 ? LOW : HIGH);
        setMotorPwm(absError > m_motorSlowThresh
          ? m_motorFastPwm
          : map(absError, 1, m_motorSlowThresh, m_motorSlowPwm, m_motorFastPwm));
      }
      else {
        setMotorPwm(0);
        m_moving = false;
      }
    }
    else {
      setMotorPwm(0);
    }
  }
}

// position 0 - 100
void FaderMover::moveTo(uint8_t position) {
  m_moving = true;
  // m_adjusting = true;
  m_target = getAbsPosition(position);
}

void FaderMover::setErrorThresh(uint16_t errorThresh) {
  m_errorThresh = constrain(errorThresh, 0, 1023);
}

void FaderMover::setDirection(decltype(HIGH) direction) {
  digitalWrite(m_directionPin, direction);
}

void FaderMover::setMotorPwm(uint8_t value) {
  analogWrite(m_motorPin, constrain(value, 0, 255));
}

uint16_t FaderMover::getAbsPosition(uint8_t normPosition) {
  return map(constrain(normPosition, 0, 100), 0, 100, 0, m_maxPosition);
}

uint8_t FaderMover::getNormPosition(uint16_t absPosition) {
  return map(constrain(absPosition, 0, m_maxPosition), 0, m_maxPosition, 0, 100);
}
