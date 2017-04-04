#include "FaderPresets.h"

FaderPresets::FaderPresets() {
  removeAll();
}

bool FaderPresets::isPreset(int16_t value) {
  for (auto i = 0; i < maxPresets; ++i) {
    if (m_presets[i] != emptyPreset && value == m_presets[i]) return true;
  }
  return false;
}

void FaderPresets::add(int16_t value) {
  if (isPreset(value)) { return; }

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
  for (auto i = 0; i < maxPresets; ++i) {
    if (value == m_presets[i]) { m_presets[i] = emptyPreset; }
  }
}

void FaderPresets::removeAll() {
  for (auto i = 0; i < maxPresets; ++i) {
    m_presets[i] = emptyPreset;
  }
}
