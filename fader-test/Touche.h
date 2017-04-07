#ifndef TOUCHE_H
#define TOUCHE_H

//******************************************************************************
// Illutron take on Disney style capacitive touch sensor using only passives
// and Arduino
// Dzl 2012
//
// Ported by Arthur Carabott 2017
//******************************************************************************


//                              10n
// PIN 9 --[10k]-+-----10mH---+--||-- OBJECT
//               |            |
//              3.3k          |
//               |            V 1N4148 diode
//              GND           |
//                            |
//Analog 0 ---+------+--------+
//            |      |
//          100pf   1MOmhm
//            |      |
//           GND    GND


#include "Arduino.h"

class Touche
{
public:
  Touche(uint8_t readPin);

  void    setup();
  void    update();
  void    startTraining(uint8_t gesture, uint64_t duration = 1000);
  void    stopTraining();
  bool    training();
  bool    gestureChanged();
  uint8_t currentGesture();
  void    clearGestures();

  static const uint8_t signalPin = 9;
  static const uint8_t syncPin = 8;

protected:

  float getDistance(float x1, float y1, float x2, float y2);

  static const uint16_t numFreqs = 200;
  static const uint32_t maxDist = numFreqs * 1024;
  static const size_t   numGestures = 3;
  static const size_t   numGesturePoints = 2;

  uint8_t m_readPin = 0;

  float m_results[numFreqs] = { 0.0 };
  float m_gesturePoints[numGestures][numGesturePoints];

  bool      m_training = false;
  uint64_t  m_trainingStartTime = 0;
  uint64_t  m_trainingDuration = 1000;

  bool      m_gestureChanged = false;
  uint8_t   m_trainingGesture = 0;
  uint8_t   m_previousGesture = 0;
  uint8_t   m_currentGesture = 0;
  uint16_t  m_gestureLag = 100;
  uint64_t  m_gestureChangedTime = 0;

  uint8_t   m_currentGesturePublic = 0;
  bool      m_gestureChangedPublic = false;
};

#endif // TOUCHE_H
