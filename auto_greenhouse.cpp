#include <Arduino.h>

#include "libs/greenhouse/greenhouse.h"

#include "libs/greenhouse-lcd/greenhouse_lcd.h"
#include "libs/greenhouse-debug/greenhouse_debug.h"

#define DEBUG_MODE                  false
#define LCD_ENABLED                 true

//Sensors
#define DHT_PIN                     2
#define SOIL_MOISTURE_SENSOR_PIN    0 //A0
#define LIGHT_SENSOR_PIN            1 //A1
#define LATITUDE                    46.5 //{
#define LONGITUDE                   30.77 //{ Odesa, Ukraine, Europe - Latitude/Longitude and Timezone 	46.5/30.77, 0;
#define TIMEZONE                    0 //{ using UTC to avoid problems with timezones

//Controls
#define WATER_PUMP_PIN              4
#define LAMP_PIN                    5
#define HUMIDIFIER_PIN              6
#define HEATER_PIN                  7

Greenhouse greenhouse;

Sunrise mySunrise(LATITUDE, LONGITUDE, TIMEZONE);
RTC_DS1307 *rtc;
DHT *dht;

LCDGreenhouse *lcdGreenhouse;
LiquidCrystal_I2C *lcd;
i2ckeypad *kpd;

int i;
unsigned long previousMillis = 0;

void setup() {
    Serial.begin(115200);

    Wire.begin();

    greenhouse.mySunrise = &mySunrise;
    mySunrise.Actual(); //Actual, Civil, Nautical, Astronomical

    if (LCD_ENABLED && !DEBUG_MODE) {
        lcd = new LiquidCrystal_I2C(0x20, 20, 4);
        kpd = new i2ckeypad(0x27, 4, 4);

        greenhouse.handlers[0] = new LCDGreenhouse(&greenhouse, lcd, kpd);

        lcd->init();
        kpd->init();

        lcd->backlight();
    }

    if (DEBUG_MODE) {
        greenhouse.handlers[1] = new DebugGreenhouse(&greenhouse, &Serial);
    } else {
        rtc = new RTC_DS1307;
        rtc->begin();

        dht = new DHT(DHT_PIN, DHT11);
        dht->begin();

        if (!rtc->isrunning()) {
            Serial.println("RTC is NOT running!");
            // following line sets the RTC to the date & time this sketch was compiled
            //rtc.adjust(DateTime(__DATE__, __TIME__));
        }
        //rtc.adjust(DateTime(1425233800));

        //Sensors
        greenhouse.soilMoisturePin = SOIL_MOISTURE_SENSOR_PIN;
        greenhouse.lightPin = LIGHT_SENSOR_PIN;

        greenhouse.rtc = rtc;
        greenhouse.dht = dht;

        //Controls
        greenhouse.setControlPin(WATER_PUMP, WATER_PUMP_PIN);
        greenhouse.setControlPin(LAMP, LAMP_PIN);
        greenhouse.setControlPin(HUMIDIFIER, HUMIDIFIER_PIN);
        greenhouse.setControlPin(HEATER, HEATER_PIN);
    }

    //Disabling
    greenhouse.disableControl(HUMIDIFIER, true); // Remove to unable HUMIDIFIER
    greenhouse.disableControl(HEATER, true); // Remove to unable HEATER

    greenhouse.readOnlyMode = DEBUG_MODE; //ToDo: get it from CMake config

    greenhouse.init();
}

void loop() {
    if (!greenhouse.loop()) {
        delay(10);
        return;
    }

    if (millis() - previousMillis >= DEBUG_MODE ? 1 : 1000) {
        previousMillis = millis();
        greenhouse.readSensors();
        greenhouse.doControl();
    }
}
