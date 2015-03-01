#include "Arduino.h"
#include "Wire/Wire.h"

#include "libs/LCDI2C/LiquidCrystal_I2C.h"
#include "libs/i2ckeypad/i2ckeypad.h"
#include "libs/DHT/DHT.h"
#include "libs/RTC/RTClib.h"
#include "libs/Sunrise/Sunrise.h"

#include "libs/greenhouse/greenhouse.h"

#define DEBUG_MODE                  true
#define LOG_TO_SERIAL               true
#define COMMAND_PARAMS_COUNT        6

//Sensors
#define DHT_PIN                     2
#define SOIL_MOISTURE_SENSOR_PIN    0 //A0
#define LATITUDE                    46.5 //{
#define LONGITUDE                   30.77 //{ Odesa, Ukraine, Europe - Latitude/Longitude and Timezone 	46.5/30.77, 0;
#define TIMEZONE                    0 //{ using UTC to avoid problems with timezones

//Controls
#define WATER_PUMP_PIN              4
#define LAMP_PIN                    5
#define HUMIDIFIER_PIN              6
#define HEATER_PIN                  7


RTC_DS1307 rtc;

DHT dht(DHT_PIN, DHT11);

Sunrise mySunrise(LATITUDE, LONGITUDE, TIMEZONE);

Greenhouse greenhouse;

LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x20, 20, 4);

i2ckeypad kpd(0x27, 4, 4);

DateTime dateTime;
DateTime sunriseDateTime;
DateTime sunsetDateTime;

int tmp;
int i;
int paramIndex;
unsigned long previousMillis = 0;
boolean commandStarted;
boolean commandFinished;
char commandValue[32];
long command[COMMAND_PARAMS_COUNT];

void readSensors();
void readSensorsFromSerial();
boolean parseCommand();

void fillDates();

void setup() {
    Serial.begin(115200);

    mySunrise.Actual(); //Actual, Civil, Nautical, Astronomical

    if (!DEBUG_MODE) {
        Wire.begin();

        rtc.begin();
        dht.begin();

        if (!rtc.isrunning()) {
            Serial.println("RTC is NOT running!");
            // following line sets the RTC to the date & time this sketch was compiled
            //rtc.adjust(DateTime(__DATE__, __TIME__));
        }
        //rtc.adjust(DateTime(1425233800));

        lcd->init();                      // initialize the lcd
        kpd.init();

        // Print a message to the LCD.
        lcd->backlight();
        greenhouse.lcd = lcd;
    }

    //Controls
    greenhouse.setControlPin(WATER_PUMP, WATER_PUMP_PIN);
    greenhouse.setControlPin(LAMP, LAMP_PIN);

    greenhouse.setControlPin(HUMIDIFIER, HUMIDIFIER_PIN);
    greenhouse.disableControl(HUMIDIFIER, true); // Remove to unable HUMIDIFIER

    greenhouse.setControlPin(HEATER, HEATER_PIN);
    greenhouse.disableControl(HEATER, true); // Remove to unable HEATER

    greenhouse.debugMode = DEBUG_MODE; //ToDo: get it from CMake config
    greenhouse.logToSerial = LOG_TO_SERIAL;

    greenhouse.init();
}

void loop() {
    if (DEBUG_MODE) {
        if (!parseCommand()) {
            delay(10);
            return;
        }

        greenhouse.debugId =command[1];

        switch (command[0]) {
            case SENSORS:// sensors info
                readSensorsFromSerial();

                greenhouse.doControl();
                break;

            case RESET:// reset
                greenhouse.reset();
                break;
        }
    } else {
        if (millis() - previousMillis >= 1000) {
            previousMillis = millis();
            readSensors();
            greenhouse.doControl();
        }
    }



    delay(10); //ToDo: review delay
}

void fillDates() {
    greenhouse.timeSeconds = dateTime.unixtime();

    mySunrise.Rise(dateTime.month(), dateTime.day());
    sunriseDateTime = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise.Hour(), mySunrise.Minute(), 0);
    greenhouse.sunriseSeconds = sunriseDateTime.unixtime();

    mySunrise.Set(dateTime.month(), dateTime.day());
    sunsetDateTime = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise.Hour(), mySunrise.Minute(), 0);
    greenhouse.sunsetSeconds = sunsetDateTime.unixtime();
}

void readSensors() {
    dateTime = rtc.now();

    fillDates();

    greenhouse.humidity = (int) dht.readHumidity();
    greenhouse.temperature = (int) dht.readTemperature();
    greenhouse.soilMoisture = analogRead(SOIL_MOISTURE_SENSOR_PIN);
}

void readSensorsFromSerial() {
    dateTime = DateTime((uint32_t) command[2]);

    fillDates();

    greenhouse.humidity = (int) command[3];
    greenhouse.temperature = (int) command[4];
    greenhouse.soilMoisture = (int) command[5];
}

boolean parseCommand() {
    while (Serial.available()) {
        tmp = Serial.read();

        switch (tmp) {
            case START:
                commandStarted = true;

                i = 0;
                paramIndex = 0;
                break;

            case END:
                if (commandStarted) {
                    commandFinished = true;
                }
            case SEP_COMMA:
                if (commandStarted) {
                    commandValue[i] = '\0';

                    if (paramIndex == 0) {
                        command[0] = commandValue[0];
                    } else if (paramIndex > 0 && paramIndex < COMMAND_PARAMS_COUNT) {
                        command[paramIndex] = atol(commandValue);
                    }

                    i = 0;
                    paramIndex++;

                    if (commandFinished) {
                        commandFinished = false;
                        commandStarted = false;
                        i = 0;
                        paramIndex = 0;

                        return true;
                    }
                }

                break;

            default:
                if (commandStarted) {
                    commandValue[i++] = (char) tmp;
                }
                break;
        }
    }

    return false;
}
