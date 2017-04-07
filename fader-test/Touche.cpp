#include "Touche.h"

#define bitTog(value, bit) ((value) ^= (1UL << (bit)))

Touche::Touche(uint8_t readPin) : m_readPin(readPin) {
  clearGestures();
}

void Touche::setup() {
  TCCR1A = 0b10000010;            //-Set up frequency generator
  TCCR1B = 0b00011001;
  ICR1 = 110;
  OCR1A = 55;

  pinMode(9, OUTPUT);             //-Signal generator pin
  pinMode(8, OUTPUT);             //-Sync (test) pin
}

void Touche::update() {
  uint16_t maxFreq = 0;
  float maxResult = 0;

  // read data
  for (auto i = 0; i < numFreqs; ++i) {
    const auto value = analogRead(m_readPin);   // read response signal
    bitClear(TCCR1B, 0);                        // stop generator
    TCNT1 = 0;                                  // set new frequency
    ICR1 = i;
    OCR1A = i / 2;
    bitSet(TCCR1B, 0);                          // start generator

    m_results[i] = m_results[i] * 0.5 + float(value) * 0.5; // filter result

    if (m_results[i] > maxResult) {
      maxFreq = i;
      maxResult = m_results[i];
    }
  }
  bitTog(PORTB, 0); // toggle pin 8 after each sweep, (good for scope)

  // gestures
  float closestGestureDistance = maxDist;
  for (auto i = 0; i < numGestures; ++i) {
    // train new gesture
    if (m_training) {
      if (m_trainingGesture == i) {
        m_gesturePoints[i][0] = maxFreq;
        m_gesturePoints[i][1] = maxResult;
      }
    }

    // calculate closest gesture
    const auto dist = getDistance(m_gesturePoints[i][0], m_gesturePoints[i][1],
                                  maxFreq, maxResult);
    if (dist < closestGestureDistance) {
      closestGestureDistance = dist;
      m_currentGesture = i;
    }
  }

  const auto now = millis();

  if (m_training && now - m_trainingStartTime > m_trainingDuration) {
    stopTraining();
  }

  // check if gesture changed
  // this has a lag defined by m_gestureLag, as 'low pass filter'
  // implemented with an internal state and a public state
  if (!m_gestureChanged && m_currentGesture != m_previousGesture) {
    m_gestureChanged = true;
    m_gestureChangedTime = millis();
  }

  m_gestureChangedPublic = false;
  const auto lagWindowFinished = millis() - m_gestureChangedTime > m_gestureLag;
  if (m_gestureChanged && lagWindowFinished) {
    m_gestureChanged = false;
    m_currentGesturePublic = m_currentGesture;
    m_gestureChangedPublic = true;
  }

  m_previousGesture = m_currentGesture;
}

void Touche::startTraining(uint8_t gesture, uint64_t duration) {
  if (gesture >= 0 && gesture < numGestures) {
    m_training = true;
    m_trainingGesture = gesture;
    m_trainingStartTime = millis();
    m_trainingDuration = duration;
  }
}

void Touche::stopTraining() { m_training = false; }

bool Touche::training() { return m_training; }

bool Touche::gestureChanged() { return m_gestureChangedPublic; }

uint8_t Touche::currentGesture() { return m_currentGesturePublic; }

void Touche::clearGestures() {
  for (auto i = 0; i < numGestures; ++i) {
    for (auto j = 0; j < numGesturePoints; ++j) {
      m_gesturePoints[i][j] = -1.0;
    }
  }
}

float Touche::getDistance(float x1, float y1, float x2, float y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}
