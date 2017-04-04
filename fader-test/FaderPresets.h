#ifndef FADER_PRESETS_H
#define FADER_PRESETS_H

#include <Arduino.h>

class FaderPresets
{
public:
  static const size_t maxPresets = 10;
  static const int8_t emptyPreset = -1;

  FaderPresets();

  bool isPreset(int16_t value, int8_t* foundIndex = nullptr, int16_t error = 0);
  void add(int16_t value);
  void remove(int16_t value);
  void removeAll();

protected:
  int16_t   m_presets[maxPresets];
  size_t    m_index = 0;
  uint16_t  m_removeThresh = 20;
  uint16_t  m_addThresh = 50;

  bool inRange(auto value, auto min, auto max); // inclusive range
  bool isNear(auto value, auto target, auto error);
};

#endif // FADER_PRESETS_H
