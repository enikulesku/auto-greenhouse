#ifndef greenhouse_h
#define greenhouse_h

#include "Arduino.h"

#include "../RTC/RTClib.h"
#include "../Sunrise/Sunrise.h"
#include "../DHT/DHT.h"

#define CONTROLS_COUNT  4
#define WATER_PUMP      0
#define LAMP            1
#define HUMIDIFIER      2
#define HEATER          3

#define SOIL_MOISTURE_DRY 50
#define SOIL_MOISTURE_MOISTURIZED 20
#define WATER_PUMP_MAX_WORK_TIME 60
#define WATER_PUMP_MIN_DELAY 600

#define REQUIRED_LIGHT_DAY_DURATION 43200 // 12 hours
#define LAMP_ALLOWED_DELTA 300

#define LIGHT_LEVEL_TURN_OFF 700
#define LIGHT_LEVEL_TURN_ON 600

#define MAX_HANDLERS 2

class Handler {
public:
    virtual void onReset() {}

    virtual void onControl() {}

    virtual void onReadSensors() {}

    virtual boolean onLoop() {
        return true;
    }
};

class Greenhouse {
private:
    //Controls
    uint8_t controlPins[CONTROLS_COUNT];

    // Controls state
    void changeControl(uint8_t controlType, boolean on);

    void fillDates();

    //Temp
    uint8_t i;
public:
    Handler *handlers[MAX_HANDLERS];

    RTC_DS1307 *rtc;
    DHT *dht;
    Sunrise *mySunrise;
    uint8_t soilMoisturePin;
    uint8_t lightPin;

    // Sensors state
    DateTime dateTime;
    DateTime sunriseDateTime;
    DateTime sunsetDateTime;

    long timeSeconds;
    long sunriseSeconds;
    long sunsetSeconds;

    uint32_t lampWorkingDuration;
    long expectedSunriseSeconds;
    long expectedSunsetSeconds;

    int humidity;
    int temperature;
    int soilMoisture;
    int lightLevel;

    boolean controlStates[CONTROLS_COUNT];
    long controlStartTime[CONTROLS_COUNT];
    boolean controlDisabled[CONTROLS_COUNT];

    boolean readOnlyMode;

    void init();

    void reset();

    void readSensors();

    void doControl();

    boolean loop();

    void setControlPin(uint8_t controlType, uint8_t controlPin) {
        controlPins[controlType] = controlPin;
        controlDisabled[controlType] = false;
    }

    void disableControl(uint8_t controlType, boolean disable) {
        controlDisabled[controlType] = disable;
    }

    void controlWaterPump();

    void controlLamp();
};

#endif
