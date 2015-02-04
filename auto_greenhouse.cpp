#include "Arduino.h"
#include "Wire/Wire.h"

#include "libs/LCDI2C/LiquidCrystal_I2C.h"
#include "libs/i2ckeypad/i2ckeypad.h"
#include "libs/DHT/DHT.h"
#include "libs/RTC/RTClib.h"
#include "libs/Sunrise/Sunrise.h"

#include "libs/greenhouse/greenhouse.h"

#define DEBUG_MODE                  true
#define COMMAND_PARAMS_COUNT        13

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

DateTime dateTime;

int tmp;
int i;
int paramIndex;
boolean commandStarted;
boolean commandFinished;
char commandValue[32];
int command[COMMAND_PARAMS_COUNT];

void readSensors();
void readSensorsFromSerial();
boolean parseCommand();

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

        lcd.init();                      // initialize the lcd
        kpd.init();

        // Print a message to the LCD.
        lcd.backlight();
        lcd.print("Hello, world!");
    }

    //Controls
    greenhouse.setControlPin(WATER_PUMP, WATER_PUMP_PIN);
    greenhouse.setControlPin(LAMP, LAMP_PIN);
    greenhouse.setControlPin(HUMIDIFIER, HUMIDIFIER_PIN);
    greenhouse.setControlPin(HEATER, HEATER_PIN);

    greenhouse.setDebugMode(DEBUG_MODE); //ToDo: get it from CMake config

    greenhouse.init();
}

void loop() {
    if (DEBUG_MODE) {
        if (!parseCommand()) {
            delay(10);
            return;
        }

        readSensorsFromSerial();
    } else {
        readSensors();
    }

    greenhouse.doControl();

    lcd.clear();
    lcd.print(analogRead(SOIL_MOISTURE_SENSOR_PIN));

    delay(1000); //ToDo: review delay
}


void readSensors() {
    dateTime = rtc.now();
    greenhouse.setDateTime(dateTime.year(), dateTime.month(), dateTime.day(), dateTime.hour(), dateTime.minute(), dateTime.second());

    mySunrise.Rise(dateTime.month(), dateTime.day());
    greenhouse.setSunrise(mySunrise.Hour(), mySunrise.Minute());

    mySunrise.Set(dateTime.month(), dateTime.day());
    greenhouse.setSunset(mySunrise.Hour(), mySunrise.Minute());

    greenhouse.setHumidity((int) dht.readHumidity());
    greenhouse.setTemperature((int) dht.readTemperature());
    greenhouse.setSoilMoisture(analogRead(SOIL_MOISTURE_SENSOR_PIN));
}

void readSensorsFromSerial() {
    greenhouse.setDateTime(command[0], command[1], command[2], command[3], command[4], command[5]);
    greenhouse.setSunrise(command[6], command[7]);
    greenhouse.setSunset(command[8], command[9]);
    greenhouse.setHumidity(command[10]);
    greenhouse.setTemperature(command[11]);
    greenhouse.setSoilMoisture(command[12]);
}

boolean parseCommand() {
    while (Serial.available()) {
        tmp = Serial.read();

        switch (tmp) {
            case START:
                commandStarted = true;

                i = 0;
                paramIndex = 0;
                continue;

            case END:
                if (commandStarted) {
                    commandFinished = true;
                }
            case SEP:
                if (commandStarted) {
                    commandValue[i] = '\0';
                    if (paramIndex >= 0 && paramIndex < COMMAND_PARAMS_COUNT) {
                        command[paramIndex] = atoi(commandValue);
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

                continue;

            default:
                if (commandStarted) {
                    commandValue[i++] = (char) tmp;
                }
                continue;
        }
    }

    return false;
}
