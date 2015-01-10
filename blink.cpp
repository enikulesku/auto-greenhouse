/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  Example uses a static library to show off generate_arduino_library().
 
  This example code is in the public domain.
 */
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "blink_lib.h"

void setup() {
    Serial.begin(9600);                
    blink_setup(); // Setup for blinking
}

void loop() {
    Serial.println(Serial.readString());
    blink(3000); // Blink for a second
}
