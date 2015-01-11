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
#include "libs/DHTlib/DHT.h"

RTC_DS1307 rtc;

DHT dht(7, DHT11);

void setup() {
    Serial.begin(9600);

    Wire.begin();
    rtc.begin();
    dht.begin();

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

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %; ");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");

    blink(3000); // Blink for a second
}
