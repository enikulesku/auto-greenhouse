#include "Arduino.h"

#include "greenhouse.h"

void blink_setup(uint8_t pin) {
  pinMode(pin, OUTPUT);     
}


void blink(unsigned long duration, uint8_t pin) {
  digitalWrite(pin, HIGH);   // set the LED on
  delay(duration);           // wait for a second
  digitalWrite(pin, LOW);    // set the LED off
  delay(duration);           // wait for a second
}
