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


#define SOIL_MOISTURE_DRY 50
#define SOIL_MOISTURE_MOISTURIZED 20
#define WATER_PUMP_MAX_WORK_TIME 60
#define WATER_PUMP_MIN_DELAY 600

#define REQUIRED_LIGHT_DAY_DURATION 43200 // 12 hours
#define LAMP_ALLOWED_DELTA 300

class Greenhouse {
private:
    uint32_t lampWorkingDuration;
    long expectedSunriseSeconds;
    long expectedSunsetSeconds;


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
    DateTime dateTime;
    uint8_t i;

public:
    int debugId;

    LiquidCrystal_I2C* lcd;

    // Sensors state
    long timeSeconds;
    long sunriseSeconds;

    long sunsetSeconds;

    int humidity;
    int temperature;
    int soilMoisture;
    int lightLevel;

    boolean debugMode;
    boolean logToSerial;

    void init();

    void reset();

    void doControl();

    void setControlPin(uint8_t controlType, uint8_t controlPin) {
        controlPins[controlType] = controlPin;
        controlDisabled[controlType] = false;
    }

    void disableControl(uint8_t controlType, boolean disable) {
        controlDisabled[controlType] = disable;
    }

    void printSensors();

    void printControls();

    void controlWaterPump();

    void controlLamp();

    void resetToDefault();

    void printTime(long seconds, boolean showSeconds);

    void printDate(long seconds);
};

static void p(Print *printer, char const *fmt, ... ){
    char buf[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);
    printer->print(buf);
}

