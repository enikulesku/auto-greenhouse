#include <Arduino.h>
#include "greenhouse.h"

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

    if (logToSerial) {
        p(&Serial, "^R,%d$\n", debugId);
        Serial.flush();
    }
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
    p(&Serial, "^S,%d,%ld,%d,%d,%d,%d,%ld,%ld$\n",
            debugId, timeSeconds, humidity, temperature, soilMoisture, lightLevel, sunriseSeconds, sunsetSeconds);

    Serial.flush();
};

void Greenhouse::printControls() {
    p(lcd, "^C,%d,%d,%d,%d,%d$\n", debugId, controlStates[WATER_PUMP], controlStates[LAMP], controlStates[HUMIDIFIER], controlStates[HEATER]);

    Serial.flush();
}


void Greenhouse::printLcd() {
    if (!lcd) {
        return;
    }

    lcd->setCursor(0, 0);
    p(lcd, "M=%04d H=%02d%% T=%02dC", soilMoisture, humidity, temperature);

    lcd->setCursor(0, 1);
    p(lcd, "Pu=%d La=%d Hu=%d He=%d", controlStates[WATER_PUMP], controlStates[LAMP], controlStates[HUMIDIFIER], controlStates[HEATER]);

    lcd->setCursor(0, 2);
    printTime(timeSeconds, true);

    lcd->print(" ");
    printTime(sunriseSeconds, false);

    lcd->print(" ");
    printTime(sunsetSeconds, false);

    lcd->setCursor(0, 3);
    printDate(timeSeconds);

    lcd->setCursor(9, 3);
    printTime(expectedSunriseSeconds, false);

    lcd->print(" ");
    printTime(expectedSunsetSeconds, false);
}

void Greenhouse::printDate(long seconds) {
    dateTime = DateTime((uint32_t) seconds);

    p(lcd, "%02d.%02d.%02d", dateTime.day(), dateTime.month(), dateTime.year() % 100);
}

void Greenhouse::printTime(long seconds, boolean showSeconds) {
    dateTime = DateTime((uint32_t) seconds);

    p(lcd, "%02d:%02d", dateTime.hour(), dateTime.minute());
    if (showSeconds) {
        p(lcd, ":%02d", dateTime.second());
    }
}
