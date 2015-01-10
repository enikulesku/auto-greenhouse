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
#include <Wire.h>
#include "libs/RTClib/RTClib.h"

RTC_DS1307 rtc;

void setup() {
    Serial.begin(9600);

    Wire.begin();
    rtc.begin();

    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(__DATE__, __TIME__));
    }

    blink_setup(); // Setup for blinking
}

void loop() {
    DateTime now = rtc.now();
    Serial.print(now.day(), DEC);
    Serial.print('-');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    blink(3000); // Blink for a second
}
