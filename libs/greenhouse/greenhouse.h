#include <stdint.h>
#include <stdint-gcc.h>

#include "Arduino.h"

#include "../RTC/RTClib.h"

#define START      '^'
#define END        '$'
#define SEP_COMMA  ','

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
    long timeSeconds;

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

    void setControlPin(uint8_t controlType, uint8_t controlPin) {
        controlPins[controlType] = controlPin;
    }

    void setDebugId(int debugId) {
        Greenhouse::debugId = debugId;
    }

    void printSensors();

    void printControls();
};

long date2seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
