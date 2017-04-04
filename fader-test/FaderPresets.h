#ifndef FADER_PRESETS_H
#define FADER_PRESETS_H

#include <Arduino.h>

class FaderPresets
{
public:
  static const size_t maxPresets = 25;
  static const int8_t emptyPreset = -1;

  FaderPresets();

  bool isPreset(int16_t value);
  void add(int16_t value);
  void remove(int16_t value);
  void removeAll();

protected:
  int16_t   m_presets[maxPresets];
  size_t    m_index = 0;
};

#endif // FADER_PRESETS_H
