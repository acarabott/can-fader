#include "FaderMover.h"

FaderMover::FaderMover(uint8_t motorPin, uint8_t directionPin) :
  m_motorPin(motorPin),
  m_directionPin(directionPin),
  m_lastMovedTime(millis()),
  m_lastTickedTime(millis())
{
}

void FaderMover::update(uint16_t position) {
  m_prevPosition = m_currentPosition;
  m_currentPosition = position;

  const auto now = millis();

  if (m_delayedSet && now > m_delayedTime) {
    m_moving = true;
    m_target = m_delayedTarget;
    m_delayedSet = false;
  }

  if (m_moving) {
    const auto timeThresh = m_adjusting ? m_moveInterval : 0;

    if (now - m_lastMovedTime > timeThresh) {
      m_lastMovedTime = now;
      m_adjusting = !m_adjusting;

      if (m_adjusting) {
        const int16_t error = m_target - position;
        const auto absError = abs(error);

        if (absError > m_errorThresh) {
          setDirection(error > 0 ? HIGH : LOW);

          setSpeed(absError > m_motorSlowThresh
            ? 100
            : map(absError, 1, m_motorSlowThresh, 0, 100));
        }
        else {
          stopMotor();
          m_moving = false;
        }
      }
      else {
        stopMotor();
      }
    }
  }

  if (m_tickingState != TickingState::Not) {
    const auto curTime = millis();
    if (curTime - m_lastTickedTime > m_curTickDuration) {
      switch (m_tickingState) {
        case TickingState::Forward : {
          setSpeed(m_curTickSpeed);
          m_tickingState = TickingState::ForwardStop;
          break;
        }
        case TickingState::ForwardStop : {
          stopMotor();
          m_tickingState = TickingState::Backward;
          toggleDirection();
          break;
        }
        case TickingState::Backward : {
          setSpeed(m_curTickSpeed);
          m_tickingState = TickingState::BackwardStop;
          break;
        }
        case TickingState::BackwardStop : {
          stopMotor();
          m_tickingState = TickingState::Not;
          break;
        }
        default : {
          m_tickingState = TickingState::Not;
          stopMotor();
          break;
        }
      }

      m_lastTickedTime = curTime;
    }
  }
}

void FaderMover::moveTo(uint16_t position) {
  setPosition(position);
}

// position 0 - 100
void FaderMover::moveToNorm(uint8_t position) {
  setPosition(getAbsPosition(position));
}

bool FaderMover::isMoving() { return m_moving; }

void FaderMover::tick(uint8_t intensity) {
  if (m_moving || m_currentPosition == m_prevPosition) return;

  m_tickingState = TickingState::Forward;
  const auto c_intensity = constrain(intensity, 0, 100);
  m_curTickDuration = map(c_intensity, 0, 100, 1, 5);
  m_curTickSpeed = c_intensity;
}

void FaderMover::feedback(uint16_t position, uint8_t intensity, uint64_t delay) {
  if (m_moving) return;

  const auto maxOffset = m_maxPosition;
  const auto c_intensity = constrain(intensity, 0, 100);
  const auto offset = map(c_intensity, 0, 100, 0, maxOffset);
  const auto dest = position + (maxOffset < m_maxPosition ? offset : -offset);

  setPosition(dest);
  setPosition(position, delay);
}

void FaderMover::setPosition(uint16_t absPosition, uint64_t delay) {
  m_moving = true;

  const auto constrained = constrain(absPosition, 0, m_maxPosition);

  if (delay == 0) { m_target = constrained; }
  else {
    m_delayedTarget = constrained;
    m_delayedTime = millis() + delay;
    m_delayedSet = true;
  }
}

void FaderMover::stopMotor() {
  setSpeed(0);
}

void FaderMover::setErrorThresh(uint16_t errorThresh) {
  m_errorThresh = constrain(errorThresh, 0, 1023);
}

void FaderMover::setDirection(decltype(HIGH) direction) {
  m_currentDirection = direction;
  digitalWrite(m_directionPin, direction);
}

void FaderMover::toggleDirection() {
  setDirection(m_currentDirection == LOW ? HIGH : LOW);
}

void FaderMover::setSpeed(uint8_t speed) {
  const uint8_t constrained = constrain(speed, 0, 100);
  const uint8_t scaled = map(constrained, 0, 100, 0, 255);
  const uint8_t pwm = m_currentDirection == LOW ? scaled : 255 - scaled;
  setMotorPwm(pwm);
}

void FaderMover::setMotorPwm(uint8_t pwm) {
  analogWrite(m_motorPin, pwm);
}

uint16_t FaderMover::getAbsPosition(uint8_t normPosition) {
  return map(constrain(normPosition, 0, 100), 0, 100, 0, m_maxPosition);
}

uint8_t FaderMover::getNormPosition(uint16_t absPosition) {
  return map(constrain(absPosition, 0, m_maxPosition), 0, m_maxPosition, 0, 100);
}
