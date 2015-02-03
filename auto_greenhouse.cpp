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

#define DEBUG_MODE                  true

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


RTC_DS1307 rtc;

DHT dht(DHT_PIN, DHT11);

Sunrise mySunrise(LATITUDE, LONGITUDE, TIMEZONE);

Greenhouse greenhouse;

LiquidCrystal_I2C lcd(0x20, 20, 4);

i2ckeypad kpd(0x27, 4, 4);

int tmp;
int i;
int paramIndex;
boolean commandStarted;
char commandValue[32];

void readSensors() {
    DateTime dateTime = rtc.now();
    greenhouse.setDateTime(dateTime);

    mySunrise.Rise(dateTime.month(), dateTime.day());
    greenhouse.setSunrise(DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise.Hour(), mySunrise.Minute(), 0));

    mySunrise.Set(dateTime.month(), dateTime.day());
    greenhouse.setSunset(DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise.Hour(), mySunrise.Minute(), 0));

    greenhouse.setHumidity(dht.readHumidity());
    greenhouse.setTemperature(dht.readTemperature());
    greenhouse.setSoilMoisture(analogRead(SOIL_MOISTURE_SENSOR_PIN));
}

boolean readSensorsFromSerial() {
    while (Serial.available()) {
        tmp = Serial.read();

        switch (tmp) {
            case START:
                commandStarted = true;

                i = 0;
                paramIndex = 0;
                continue;

            case SEP:
                if (commandStarted) {
                    commandValue[i] = '\0';

                    //ToDo
                    switch (paramIndex) {
                        case 0:
                            DateTime dateTime = DateTime(atoi(commandValue));
                            greenhouse.setDateTime(dateTime);
                            break;

                        case 1:
                            DateTime sunrise = DateTime(atoi(commandValue));
                            greenhouse.setSunrise(sunrise);
                            break;

                        case 2:
                            DateTime sunset = DateTime(atoi(commandValue));
                            greenhouse.setSunset(sunset);
                            break;
                    }

                    i = 0;
                    paramIndex++;
                }

                continue;

            case END:
                if (commandStarted) {
                    commandStarted = false;
                    i = 0;
                    paramIndex = 0;

                    return true;
                }
                break;

            default:
                if (commandStarted) {
                    commandValue[i++] = tmp;
                }
                continue;
        }

        return false;
    }

}

void setup() {
    Serial.begin(9600);

    if (!DEBUG_MODE) {
        Wire.begin();

        rtc.begin();
        dht.begin();
        mySunrise.Civil(); //Actual, Civil, Nautical, Astronomical

        if (!rtc.isrunning()) {
            Serial.println("RTC is NOT running!");
            // following line sets the RTC to the date & time this sketch was compiled
            //rtc.adjust(DateTime(__DATE__, __TIME__));
        }
    }

    //Controls
    greenhouse.setWaterPumpPin(WATER_PUMP_PIN);
    greenhouse.setLampPin(LAMP_PIN);
    greenhouse.setHumidifierPin(HUMIDIFIER_PIN);
    greenhouse.setHeaterPin(HEATER_PIN);

    greenhouse.setDebugMode(DEBUG_MODE); //ToDo: get it from CMake config

    greenhouse.init();

    lcd.init();                      // initialize the lcd

    kpd.init();

    // Print a message to the LCD.
    lcd.backlight();
    lcd.print("Hello, world!");
}

void loop() {
    if (DEBUG_MODE) {
        if (!readSensorsFromSerial()) {
            delay(50);
            return;
        }
    } else {
        readSensors();
    }

    greenhouse.loadSensorsData();

    greenhouse.doControl();

    lcd.clear();
    lcd.print(analogRead(SOIL_MOISTURE_SENSOR_PIN));

    delay(1000); //ToDo: review delay
}
