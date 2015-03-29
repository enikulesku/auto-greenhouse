#include <Arduino.h>
#include <stdint-gcc.h>
#include "greenhouse.h"

void Greenhouse::init() {
    if (!readOnlyMode) {
        // Controls initialization
        for (i = 0; i < CONTROLS_COUNT; i++) {
            pinMode(controlPins[i], OUTPUT);
            digitalWrite(controlPins[i], HIGH);
        }
    }

    reset();
}

void Greenhouse::reset() {
    for (i = 0; i < CONTROLS_COUNT; i++) {
        controlStates[i] = false;
        controlStartTime[i] = 0L;
    }

    soilMoisture = 0;
    temperature = 0;
    humidity = 0;

    for (i = 0; i < MAX_HANDLERS; i++) {
        if (!handlers[i]) {
            continue;
        }
        handlers[i]->onReset();
    }
}

void Greenhouse::readSensors() {
    if (!readOnlyMode) {
        dateTime = rtc->now();

        humidity = (int) dht->readHumidity();
        temperature = (int) dht->readTemperature();
        soilMoisture = analogRead(soilMoisturePin);
        lightLevel = analogRead(lightPin);
    }

    for (i = 0; i < MAX_HANDLERS; i++) {
        if (!handlers[i]) {
            continue;
        }
        handlers[i]->onReadSensors();
    }

    fillDates();
}

void Greenhouse::fillDates() {
    timeSeconds = dateTime.unixtime();

    mySunrise->Rise(dateTime.month(), dateTime.day());
    sunriseDateTime = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise->Hour(), mySunrise->Minute(), 0);
    sunriseSeconds = sunriseDateTime.unixtime();

    mySunrise->Set(dateTime.month(), dateTime.day());
    sunsetDateTime = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), mySunrise->Hour(), mySunrise->Minute(), 0);
    sunsetSeconds = sunsetDateTime.unixtime();
}


unsigned long Greenhouse::calculateFromLastRun(uint8_t controlType) {
    unsigned long startTime = controlStartTime[controlType];

    unsigned long result = millis();
    if (result < startTime) {
        startTime = LONG_MAX_VALUE - startTime;
    }

    result -= startTime;

    return result;
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
        controlStartTime[controlType] = millis();
    }

    if (readOnlyMode) {
        return;
    }

    digitalWrite(controlPins[controlType], on ? LOW : HIGH);
}

boolean Greenhouse::loop() {
    boolean done = true;

    for (i = 0; i < MAX_HANDLERS; i++) {
        if (!handlers[i]) {
            continue;
        }
        done &= handlers[i]->onLoop();
    }

    return done;
}

// Logic here
void Greenhouse::doControl() {
    controlWaterPump();

    controlLamp();

    for (i = 0; i < MAX_HANDLERS; i++) {
        if (!handlers[i]) {
            continue;
        }
        handlers[i]->onControl();
    }
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
        if (lightLevel <= LIGHT_LEVEL_TURN_ON && timeSeconds >= expectedSunriseSeconds && timeSeconds <= expectedSunsetSeconds) {
            changeControl(LAMP, true);
        }
    } else {
        if (lightLevel >= LIGHT_LEVEL_TURN_OFF || timeSeconds < expectedSunriseSeconds || timeSeconds > expectedSunsetSeconds) {
            changeControl(LAMP, false);
        }
    }
}

void Greenhouse::controlWaterPump() {
    if (controlDisabled[WATER_PUMP]) {
        return;
    }
    if (!controlStates[WATER_PUMP]) {
        if (soilMoisture >= SOIL_MOISTURE_DRY && calculateFromLastRun(WATER_PUMP) > WATER_PUMP_MIN_DELAY * 1000) {
            changeControl(WATER_PUMP, true);
        }
    } else {
        if (soilMoisture <= SOIL_MOISTURE_MOISTURIZED || calculateFromLastRun(WATER_PUMP) > WATER_PUMP_MAX_WORK_TIME * 1000) {
            changeControl(WATER_PUMP, false);
        }
    }
}
