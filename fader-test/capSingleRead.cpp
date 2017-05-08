#include "capSingleRead.h"

// reading with single wire as per https://github.com/martin2250/ADCTouch
uint16_t capSingleRead(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  ADMUX |= 0b11111;
  ADCSRA |= (1 << ADSC);          // start conversion
  while(!(ADCSRA & (1 << ADIF))); // wait for conversion to finish
  ADCSRA |= (1 << ADIF);          // reset the flag
  pinMode(pin, INPUT);

  return analogRead(pin);
}
