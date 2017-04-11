#ifndef FADER_TOUCH_SENSOR_H
#define FADER_TOUCH_SENSOR_H

#include <Arduino.h>
class FaderTouchSensor
{
public:
  FaderTouchSensor();

  void update(int16_t touchValue);
  bool isTouching();
  bool tapStarted();
  bool tapEnded();
  uint8_t tapCount();
  void enable();
  void disable();
  bool isEnabled();
  uint16_t getTouchThresh();
  void setTouchThresh(uint16_t thresh);

protected:
  uint16_t m_faderMax = 1019;
  uint16_t m_touchValue = 0;
  uint16_t m_touchThresh = 970;
  uint16_t m_touchMin = 1023;
  bool     m_isTouching = false;
  bool     m_prevTouching = false;
  bool     m_enabled = true;
  bool     m_inTapWindow = false;
  uint64_t m_prevTapTime = 0;
  uint64_t m_multiTapThresh = 250;
  uint8_t  m_tapCount = 0;
  uint8_t  m_tapCountResult = 0;
};

#endif // FADER_TOUCH_SENSOR_H
