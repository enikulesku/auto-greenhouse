#include <Arduino.h>
#include "greenhouse.h"

const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void Greenhouse::init() {
    if (!debugMode) {
        // Controls initialization
        for (i = 0; i < CONTROLS_COUNT; i++) {
            pinMode(controlPins[i], OUTPUT);
            digitalWrite(controlPins[i], HIGH);
        }
    }

    resetToDefault();
}


void Greenhouse::reset() {
    resetToDefault();

    if (!logToSerial) {
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

void Greenhouse::resetToDefault() {
    for (i = 0; i < CONTROLS_COUNT; i++) {
        controlStates[i] = false;
        controlStartTime[i] = 0L;
    }

    soilMoisture = 0;
    temperature = 0;
    humidity = 0;
}

void Greenhouse::changeControl(uint8_t controlType, boolean on) {
    if (controlDisabled[controlType]) {
        return;
    }

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

    digitalWrite(controlPins[controlType], on ? LOW : HIGH);
}

void Greenhouse::doControl() {
    if (logToSerial) {
        printSensors();
    }

    process();

    printLcd();

    if (logToSerial) {
        printControls();
    }
}

void Greenhouse::process() {
    controlWaterPump();

    controlLamp();
}

void Greenhouse::controlLamp() {
    if (controlDisabled[LAMP]) {
        return;
    }

    lampWorkingDuration = REQUIRED_LIGHT_DAY_DURATION - (sunsetSeconds - sunriseSeconds);

    if (lampWorkingDuration < LAMP_ALLOWED_DELTA) {
        return;
    }

    expectedSunriseSeconds = sunriseSeconds - lampWorkingDuration / 2;
    expectedSunsetSeconds = sunsetSeconds + lampWorkingDuration / 2;

    if (!controlStates[LAMP]) {
        if (timeSeconds < sunriseSeconds && timeSeconds >= expectedSunriseSeconds) {
            changeControl(LAMP, true);
        }

        if (timeSeconds > sunsetSeconds && timeSeconds <= expectedSunsetSeconds) {
            changeControl(LAMP, true);
        }
    } else {
        if (timeSeconds >= sunriseSeconds && timeSeconds <= sunsetSeconds) {
            changeControl(LAMP, false);
        }

        if (timeSeconds < expectedSunriseSeconds || timeSeconds > expectedSunsetSeconds) {
            changeControl(LAMP, false);
        }
    }
}

void Greenhouse::controlWaterPump() {
    if (controlDisabled[WATER_PUMP]) {
        return;
    }
    if (!controlStates[WATER_PUMP]) {
        if (soilMoisture >= SOIL_MOISTURE_DRY && timeSeconds - controlStartTime[WATER_PUMP] > WATER_PUMP_MIN_DELAY) {
            changeControl(WATER_PUMP, true);
        }
    } else {
        if (soilMoisture <= SOIL_MOISTURE_MOISTURIZED || timeSeconds - controlStartTime[WATER_PUMP] > WATER_PUMP_MAX_WORK_TIME) {
            changeControl(WATER_PUMP, false);
        }
    }
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


void Greenhouse::printLcd() {
    if (!lcd) {
        return;
    }

    lcd->setCursor(0, 0);
    lcd->print("M=");
    lcd->print(soilMoisture);
    if (soilMoisture < 1000) {
        lcd->print(" ");
    }
    if (soilMoisture < 100) {
        lcd->print(" ");
    }
    if (soilMoisture < 10) {
        lcd->print(" ");
    }

    lcd->setCursor(6, 0);
    lcd->print(" H=");
    lcd->print(humidity);
    lcd->print(" T=");
    lcd->print(temperature);

    lcd->setCursor(0, 1);
    lcd->print("P=");
    lcd->print(controlStates[WATER_PUMP]);
    lcd->print(" L=");
    lcd->print(controlStates[LAMP]);
    lcd->print(" Hu=");
    lcd->print(controlStates[HUMIDIFIER]);
    lcd->print(" He=");
    lcd->print(controlStates[HEATER]);

    lcd->setCursor(0, 2);
    lcd->print(controlStartTime[WATER_PUMP]);
}

long date2seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    uint32_t days = day;

    for (uint8_t i = 1; i < month; ++i) {
        days += pgm_read_byte(daysInMonth + i - 1);
    }

    if (month > 2 && year % 4 == 0) {
        ++days;
    }

    days += 365 * (year - 1970) + (year - 1972 + 3) / 4 - 1;

    return ((days * 24L + hour) * 60 + minute) * 60 + second;
}
