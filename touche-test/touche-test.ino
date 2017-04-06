//******************************************************************************
// Illutron take on Disney style capacitive touch sensor using only passives
// and Arduino
// Dzl 2012
// Gesture recognition ported from Processing code by Arthur Carabott
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

#define SET(x,y) (x |=(1<<y))       //-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))           // |
#define CHK(x,y) (x & (1<<y))               // |
#define TOG(x,y) (x^=(1<<y))                //-+

#define N_FREQS 200                       //How many frequencies

const float maxDist = N_FREQS * 1024;     // N_FREQS * 1024 (analogue range)
float results[N_FREQS];                   //-Filtered result buffer

#define NUM_GESTURES 3   // Comment line to disable on board gesture recognition

float gesturePoints[NUM_GESTURES][2] = {{0.0, 0.0}, {0.0, 0.0}};

uint8_t previousGesture = 0;
uint8_t currentGesture = 0;
uint32_t gestureStartTime = 0;

const uint8_t readPin = A5;

int8_t trainingIdx = -1;

void setup()
{
  TCCR1A = 0b10000010;            //-Set up frequency generator
  TCCR1B = 0b00011001;            //-+
  ICR1 = 110;
  OCR1A = 55;

  pinMode(9, OUTPUT);             //-Signal generator pin
  pinMode(8, OUTPUT);             //-Sync (test) pin

  for(int i = 0; i < N_FREQS; i++) {    //-Preset results
    results[i] = 0;               //-+
  }

  Serial.begin(115200);
}

float getDistance(float x1, float y1, float x2, float y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void loop()
{
  uint16_t maxFreq = 0;
  float maxResult = 0;

  // Reading data
  // -----------------------
  for(uint16_t i = 0; i < N_FREQS; i++)
  {
    const uint16_t v = analogRead(readPin);  //-Read response signal
    CLR(TCCR1B, 0);                          //-Stop generator
    TCNT1 = 0;                               //-Reload new frequency
    ICR1 = i;                                // |
    OCR1A = i / 2;                           //-+
    SET(TCCR1B, 0);                          //-Restart generator

    results[i] = results[i] * 0.5 + (float)(v) * 0.5;   //Filter results

    if(results[i] > maxResult) {
      maxFreq = i;
      maxResult = results[i];
    }
  }
  TOG(PORTB, 0);            //-Toggle pin 8 after each sweep (good for scope)

  // On board gesture recognition
  // ----------------------------

  if (Serial.available() > 0) {
    trainingIdx = Serial.parseInt();
    Serial.print(trainingIdx == -1 ? "stopped: " : "started: ");
    Serial.println(trainingIdx);
  }

  float closestGestureDistance = maxDist;
  for(uint8_t i = 0; i < NUM_GESTURES; i++) {
    // update gesture
    if (trainingIdx == i) {
      gesturePoints[i][0] = maxFreq;
      gesturePoints[i][1] = maxResult;
    }

    // calculate closest gesture
    const float dist = getDistance(maxFreq, maxResult, gesturePoints[i][0],
                                   gesturePoints[i][1]);

    if(dist < closestGestureDistance){
      closestGestureDistance = dist;
      currentGesture = i;
    }
  }

  if(currentGesture != previousGesture){
    gestureStartTime = millis();
    // response
    Serial.print("gesture: ");
    Serial.println(currentGesture);
    previousGesture = currentGesture;
  }


}
