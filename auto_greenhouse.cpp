/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  Example uses a static library to show off generate_arduino_library().
 
  This example code is in the public domain.
 */

#include "Arduino.h"

#include "libs/greenhouse/greenhouse.h"

#include "libs/LCDI2C/LiquidCrystal_I2C.h"
#include "libs/i2ckeypad/i2ckeypad.h"

//Sensors
#define DHT_PIN                     2
#define SOIL_MOISTURE_SENSOR_PIN    0 //A0
#define LATITUDE                    47 //{
#define LONGITUDE                   31 //{ Odesa, Ukraine, Europe - Latitude/Longitude and Timezone 	46.5/30.77, +2;
#define TIMEZONE                    +2 //{

//Controls
#define WATER_PUMP_PIN              4
#define LAMP_PIN                    5
#define HUMIDIFIER_PIN              6
#define HEATER_PIN                  7


RTC_DS1307* rtc = new RTC_DS1307();

DHT* dht = new DHT(DHT_PIN, DHT11);

Sunrise* mySunrise = new Sunrise(LATITUDE, LONGITUDE, TIMEZONE);

Greenhouse greenhouse;

LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x20, 20, 4);

i2ckeypad kpd = i2ckeypad(0x27, 4, 4);

void setup() {
    Serial.begin(9600);
    Wire.begin();

    //Sensors
    greenhouse.set_dht(dht);
    greenhouse.set_rtc(rtc);
    greenhouse.set_sunrise(mySunrise);
    greenhouse.setSoilMoistureSensorPin(SOIL_MOISTURE_SENSOR_PIN);

    //Controls
    greenhouse.setWaterPumpPin(WATER_PUMP_PIN);
    greenhouse.setLampPin(LAMP_PIN);
    greenhouse.setHumidifierPin(HUMIDIFIER_PIN);
    greenhouse.setHeaterPin(HEATER_PIN);

    greenhouse.setDebugMode(false); //ToDo: get it from CMake config

    greenhouse.init();

    lcd->init();                      // initialize the lcd

    kpd.init();

    // Print a message to the LCD.
    lcd->backlight();
    lcd->print("Hello, world!");
}

void loop() {
    greenhouse.loadSensorsData();

    greenhouse.doControl();

    lcd->clear();
    lcd->print(analogRead(SOIL_MOISTURE_SENSOR_PIN));

    delay(1000); //ToDo: review delay
}
