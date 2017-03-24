#ifndef FADER_TOUCH_SENSOR_H
#define FADER_TOUCH_SENSOR_H

#include <Arduino.h>
class FaderTouchSensor
{
public:
  static const uint16_t faderMax = 1019;

  FaderTouchSensor(uint8_t touchPin, uint8_t positionPin);

  void update();
  bool isTouching();

  void setTouchThresh(uint16_t thresh);
  uint16_t getTouchThresh();

  void setRangeThresh(uint16_t thresh);
  uint16_t getRangeThresh();


protected:
  static const uint8_t  historySize = 30;
         const uint8_t  m_touchPin;
         const uint8_t  m_positionPin;
               int16_t  m_touchValue;
               int16_t  m_positionValue;
               int16_t  m_history[historySize] = { 0 };
               size_t   m_historyCount;
               uint16_t m_touchMin = 1023;
               uint16_t m_touchThresh = 15; // relative threshold
               uint16_t m_rangeThresh = 3;
};


#endif // FADER_TOUCH_SENSOR_H
