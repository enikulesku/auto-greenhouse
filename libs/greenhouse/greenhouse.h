#include <stdint.h>
#include <stdint-gcc.h>

#include "Arduino.h"

#include "../RTC/RTClib.h"
#include "../LCDI2C/LiquidCrystal_I2C.h"

#define START      '^'
#define END        '$'
#define SEP_COMMA  ','

#define CONTROL    'C'
#define RESET      'R'
#define SENSORS    'S'
#define LOGGER     'L'
#define LS         '\n'

#define CONTROLS_COUNT  4
#define WATER_PUMP      0
#define LAMP            1
#define HUMIDIFIER      2
#define HEATER          3


#define SOIL_MOISTURE_DRY 800
#define SOIL_MOISTURE_MOISTURIZED 100
#define WATER_PUMP_MAX_WORK_TIME 60
#define WATER_PUMP_MIN_DELAY 300

#define REQUIRED_LIGHT_DAY_DURATION 43200 // 12 hours
#define LAMP_ALLOWED_DELTA 300

class Greenhouse {
private:
    boolean debugMode;
    boolean logToSerial;
    // Sensors state
    long timeSeconds;

    uint32_t lampWorkingDuration;
    long expectedSunriseSeconds;
    long expectedSunsetSeconds;

    long sunriseSeconds;

    long sunsetSeconds;

    int humidity;
    int temperature;
    int soilMoisture;

    //Controls
    uint8_t controlPins[CONTROLS_COUNT];

    // Controls state
    boolean controlStates[CONTROLS_COUNT];
    long controlStartTime[CONTROLS_COUNT];
    boolean controlDisabled[CONTROLS_COUNT];

    void changeControl(uint8_t controlType, boolean on);

    void process();
    void printLcd();

    //Temp
    uint8_t i;
    int debugId;

public:
    LiquidCrystal_I2C* lcd;

    void init();

    void reset();

    void doControl();

    //Sensors
    void setDateTime(long timeSeconds) {
        Greenhouse::timeSeconds = timeSeconds;
    }

    void setSunrise(long sunriseSeconds) {
        Greenhouse::sunriseSeconds = sunriseSeconds;
    }

    void setSunset(long sunsetSeconds) {
        Greenhouse::sunsetSeconds = sunsetSeconds;
    }

    void setHumidity(int humidity) {
        Greenhouse::humidity = humidity;
    }

    void setTemperature(int temperature) {
        Greenhouse::temperature = temperature;
    }

    void setSoilMoisture(int soilMoisture) {
        Greenhouse::soilMoisture = soilMoisture;
    }

    //Control
    void setDebugMode(boolean debugMode) {
        Greenhouse::debugMode = debugMode;
    }


    void setLogToSerial(boolean logToSerial) {
        Greenhouse::logToSerial = logToSerial;
    }

    void setControlPin(uint8_t controlType, uint8_t controlPin) {
        controlPins[controlType] = controlPin;
        controlDisabled[controlType] = false;
    }

    void setDisabledControl(uint8_t controlType, boolean disable) {
        controlDisabled[controlType] = disable;
    }

    void setDebugId(int debugId) {
        Greenhouse::debugId = debugId;
    }

    void printSensors();

    void printControls();

    void controlWaterPump();

    void controlLamp();

    void resetToDefault();
};

long date2seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
