#include "FaderPresets.h"

FaderPresets::FaderPresets() {
  removeAll();
}

bool FaderPresets::isPreset(int16_t value, int8_t* foundIndex, int16_t error) {
  for (auto i = 0; i < maxPresets; ++i) {
    if (m_presets[i] != emptyPreset && isNear(m_presets[i], value, error)) {
      *foundIndex = i;
      return true;
    }
  }
  *foundIndex = -1;
  return false;
}

void FaderPresets::add(int16_t value) {
  int8_t foundIndex;
  if (isPreset(value, &foundIndex, m_addThresh)) { return; }

  m_presets[m_index] = value;
  m_index++;
  m_index %= maxPresets;

  // find next emptpy slot
  if (m_presets[m_index] != emptyPreset) {
    auto i = 0;
    auto found = false;
    while (i < maxPresets && !found) {
      if (m_presets[i] == emptyPreset) {
        m_index = i;
        found = true;
      }
      i++;
    }
    // if we don't find an empty slot, we'll just overwrite the current index...
  }
}

void FaderPresets::remove(int16_t value) {
  int8_t foundIndex;
  if (isPreset(value, &foundIndex, m_removeThresh)) {
    m_presets[foundIndex] = emptyPreset;
  }
}

void FaderPresets::removeAll() {
  for (auto i = 0; i < maxPresets; ++i) {
    m_presets[i] = emptyPreset;
  }
}

bool FaderPresets::inRange(auto value, auto min, auto max) {
  return value >= min && value <= max;
}

bool FaderPresets::isNear(auto value, auto target, auto error) {
  return inRange(value, target - error, target + error);
}
