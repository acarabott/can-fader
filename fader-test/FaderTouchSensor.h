#ifndef FADER_TOUCH_SENSOR_H
#define FADER_TOUCH_SENSOR_H

#include <Arduino.h>
class FaderTouchSensor
{
public:
  static const uint16_t faderMax = 1019;

  FaderTouchSensor();

  void update(int16_t touchValue);
  bool isTouching();

  void setFluctuationThresh(uint16_t thresh);
  uint16_t getFluctuationThresh();


protected:
  static const uint8_t historySize = 30;
  int16_t  m_touchValue;
  int16_t  m_history[historySize] = { 0 };
  size_t   m_historyCount;
  uint16_t m_touchMin = 1023;
  uint16_t m_fluctuationThresh = 3;
};


#endif // FADER_TOUCH_SENSOR_H
