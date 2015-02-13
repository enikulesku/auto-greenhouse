#include "greenhouse.h"

const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void Greenhouse::init() {
    if (debugMode) {
        return;
    }

    // Controls initialization
    for (i = 0; i < CONTROLS_COUNT; i++) {
        pinMode(controlPins[i], OUTPUT);
    }
}


void Greenhouse::reset() {
    for (i = 0; i < CONTROLS_COUNT; i++) {
        changeControl(controlPins[i], false);
        controlStartTime[i] = 0;
    }

    if (!debugMode) {
        return;
    }

    Serial.print(START);
    Serial.print(RESET);
    Serial.print(SEP_COMMA);
    Serial.print(debugId);
    Serial.print(END);
    Serial.print(LS);
    Serial.flush();
}

void Greenhouse::changeControl(uint8_t controlType, boolean on) {
    //ToDo: review
    if (controlStates[controlType] == on) {
        return;
    }

    controlStates[controlType] = on;
    if (on) {
        controlStartTime[controlType] = timeSeconds;
    }


    if (debugMode) {
        return;
    }

    digitalWrite(controlPins[controlType], on ? HIGH : LOW);
}

void Greenhouse::doControl() {
    if (debugMode) {
        printSensors();
    }

    process();

    if (debugMode) {
        printControls();
    }
}

void Greenhouse::process() {
    //ToDo: put logic here
}

void Greenhouse::printSensors() {
    Serial.print(START);
    Serial.print(SENSORS);
    Serial.print(SEP_COMMA);

    Serial.print(debugId);
    Serial.print(SEP_COMMA);
    Serial.print(timeSeconds);
    Serial.print(SEP_COMMA);
    Serial.print(humidity);
    Serial.print(SEP_COMMA);
    Serial.print(temperature);
    Serial.print(SEP_COMMA);
    Serial.print(soilMoisture);
    Serial.print(SEP_COMMA);
    Serial.print(sunriseSeconds);
    Serial.print(SEP_COMMA);
    Serial.print(sunsetSeconds);

    Serial.print(END);
    Serial.print(LS);

    Serial.flush();
};

void Greenhouse::printControls() {
    Serial.print(START);
    Serial.print(CONTROL);
    Serial.print(SEP_COMMA);
    Serial.print(debugId);

    for (i = 0; i < CONTROLS_COUNT; i++) {
        Serial.print(SEP_COMMA);

        Serial.print(controlStates[i]);
    }

    Serial.print(END);
    Serial.print(LS);

    Serial.flush();
}

long date2seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    year -= 1970;

    uint32_t days = day;

    for (uint8_t i = 1; i < month; ++i) {
        days += pgm_read_byte(daysInMonth + i - 1);
    }

    if (month > 2 && year % 4 == 0) {
        ++days;
    }

    days += 365 * year + (year + 3) / 4 - 1;

    return ((days * 24L + hour) * 60 + minute) * 60 + second;
}
