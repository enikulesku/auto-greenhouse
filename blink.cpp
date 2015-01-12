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
#include "libs/Sunrise/Sunrise.h"
#include "libs/RTC/RTClib.h"
#include "libs/DHT/DHT.h"
#include "libs/LCDI2C/LiquidCrystal_I2C.h"

RTC_DS1307 rtc;

DHT dht(7, DHT11);

Sunrise mySunrise(47, 31, 2);//Odesa, Ukraine, Europe - Latitude/Longitude and Timezone 	46.5/30.77, +2
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
    Serial.begin(9600);

    Wire.begin();
    rtc.begin();
    dht.begin();
    mySunrise.Civil(); //Actual, Civil, Nautical, Astronomical

    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(__DATE__, __TIME__));
    }

    lcd.init();                      // initialize the lcd

    // Print a message to the LCD.
    lcd.backlight();
    lcd.print("Hello, world!");

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


    byte hour, minute;
    int time;
    // t= minutes past midnight of sunrise (6 am would be 360)
    time =mySunrise.Rise(now.month(), now.day()); // (month,day) - january=1
    if(time >=0){
        hour =mySunrise.Hour();
        minute =mySunrise.Minute();
        Serial.print("The sun rises today 30 at ");
        Serial.print(hour, DEC);
        Serial.print(":");
        if(minute <10) Serial.print("0");
        Serial.println(minute,DEC);
    }else{ //
        Serial.println("There are either penguins or polar bears around here!");
    }
    // t= minutes past midnight of sunrise (6 am would be 360)
    time =mySunrise.Set(now.month(), now.day()); // (month,day) - january=1
    if(time >=0){
        hour =mySunrise.Hour();
        minute =mySunrise.Minute();
        Serial.print("The sun sets today at ");
        Serial.print(hour, DEC);
        Serial.print(":");
        if(minute <10) Serial.print("0");
        Serial.println(minute,DEC);
    }else{ //
        Serial.println("There are either penguins or polar bears around here!");
    }
    // t= minutes past midnight of sunrise (6 am would be 360)
    time =mySunrise.Noon(now.month(), now.day()); // (month,day) - january=1
    if(time >=0){
        hour =mySunrise.Hour();
        minute =mySunrise.Minute();
        Serial.print("Noon at ");
        Serial.print(hour, DEC);
        Serial.print(":");
        if(minute <10) Serial.print("0");
        Serial.print(minute,DEC);
        Serial.print("   T:");
//        Serial.println(sunNoon, DEC);
    }else{ //
        Serial.println("There are either penguins or polar bears around here!");
    }

    blink(3000); // Blink for a second
}
