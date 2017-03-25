#ifndef FADER_MOVER_H
#define FADER_MOVER_H

#include "Arduino.h"

class FaderMover
{
public:
  FaderMover(uint8_t motorPin, uint8_t directionPin);

  void update(uint16_t value);
  void moveTo(uint8_t position);
  void tick(uint8_t intensity);
  void setErrorThresh(uint16_t errorThresh);

protected:
  void setDirection(decltype(HIGH) direction);
  void toggleDirection();
  void setMotorPwm(uint8_t value);
  void setPosition(uint16_t absPosition);

  uint16_t getAbsPosition(uint8_t normPosition);
  uint8_t getNormPosition(uint16_t absPosition);

  uint8_t            m_motorPin;
  uint8_t            m_directionPin;
  uint16_t           m_currentPosition = 0;
  uint16_t           m_prevPosition = 0;
  decltype(HIGH)     m_currentDirection = HIGH;
  bool               m_moving = false;
  bool               m_adjusting = false;
  uint16_t           m_target = 0;
  uint8_t            m_pwm = 0;
  uint16_t           m_errorThresh = 5;
  decltype(millis()) m_lastMovedTime;
  uint16_t           m_maxPosition = 1019;
  uint8_t            m_motorSlowThresh = 30;
  uint16_t           m_motorSlowPwm = 150;
  uint16_t           m_motorFastPwm = 255;
  uint16_t           m_moveInterval = 5;
};

#endif // FADER_MOVER_H
