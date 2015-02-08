#include <stdint.h>
#include <stdint-gcc.h>

#include "Arduino.h"

#include "../RTC/RTClib.h"

#define START      '^'
#define END        '$'
#define SEP_COMMA  ','
#define SEP_DOT    '.'
#define SEP_COLON  ':'

#define CONTROL    'C'
#define RESET      'R'
#define SENSORS    'S'
#define LS         '\n'

#define CONTROLS_COUNT  4
#define WATER_PUMP      0
#define LAMP            1
#define HUMIDIFIER      2
#define HEATER          3

class Greenhouse {
private:
    boolean debugMode;
    // Sensors state
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;

    long timeSeconds;

    uint8_t sunriseHour;
    uint8_t sunriseMinute;

    uint8_t sunsetHour;
    uint8_t sunsetMinute;

    int humidity;
    int temperature;
    int soilMoisture;

    //Controls
    uint8_t controlPins[CONTROLS_COUNT];

    // Controls state
    boolean controlStates[CONTROLS_COUNT];
    long controlStartTime[CONTROLS_COUNT];

    void changeControl(uint8_t controlType, boolean on);

    void process();

    //Temp
    uint8_t i;
    int debugId;
public:
    void init();

    void reset();

    void doControl();

    //Sensors
    void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t seconds) {
        Greenhouse::year = year;
        Greenhouse::month = month;
        Greenhouse::day = day;
        Greenhouse::hour = hour;
        Greenhouse::minute = minute;
        Greenhouse::seconds = seconds;

        Greenhouse::timeSeconds = time2long(date2days(year, month, day), hour, minute, seconds);
    }

    void setSunrise(uint8_t hour, uint8_t minute) {
        Greenhouse::sunriseHour = hour;
        Greenhouse::sunriseMinute = minute;
    }

    void setSunset(uint8_t hour, uint8_t minute) {
        Greenhouse::sunsetHour = hour;
        Greenhouse::sunsetMinute = minute;
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

    void setControlPin(uint8_t controlType, uint8_t controlPin) {
        controlPins[controlType] = controlPin;
    }

    void setDebugId(int debugId) {
        Greenhouse::debugId = debugId;
    }

    void printSensors();

    void printControls();
};
