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
  bool tapStarted();
  bool tapEnded();
  bool didDoubleTap();
  void enable();
  void disable();
  bool isEnabled();

  uint16_t getLowFluctuationThresh();
  void setLowFluctuationThresh(uint16_t thresh);

  uint16_t getHighFluctuationThresh();
  void setHighFluctuationThresh(uint16_t thresh);

  uint16_t getFluctuationSplit();
  void setFluctuationSplit(uint16_t thresh);

protected:
  static const uint8_t historySize = 30;
  int16_t  m_touchValue;
  int16_t  m_history[historySize] = { 0 };
  uint64_t m_historyCount;
  uint16_t m_touchMin = 1023;
  uint16_t m_fluctuationSplit = 1000; // the signal range changes dramatically when > this
  uint16_t m_lowFluctuationThresh = 5; // fluctuation threshold when above threshold
  uint16_t m_highFluctuationThresh = 3; // fluctuation threshold when below threshold
  bool     m_isTouching = false;
  bool     m_prevTouching = false;
  bool     m_enabled = true;
  bool     m_doDoubleTap = false;
  bool     m_doubleTapTriggered = false;
  uint64_t m_prevTapTime = 0;
  uint64_t m_doubleTapThresh = 300;
};


#endif // FADER_TOUCH_SENSOR_H
