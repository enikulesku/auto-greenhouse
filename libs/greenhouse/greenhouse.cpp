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

    errorCode = 0;
    previousMillis = 0;
    previousSeconds = 0;

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

    if (readOnlyMode) {
        return;
    }

    digitalWrite(controlPins[controlType], on ? LOW : HIGH);
}

boolean Greenhouse::loop() {
    boolean done = true;

    if (errorCode != 0) {
        done = false;
    } else {
        errorCode = validate();
        if (errorCode != 0) {
            done = false;
            onError(errorCode);
        }
    }

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
        if (soilMoisture >= SOIL_MOISTURE_DRY && timeSeconds - controlStartTime[WATER_PUMP] > WATER_PUMP_MIN_DELAY) {
            changeControl(WATER_PUMP, true);
        }
    } else {
        if (soilMoisture <= SOIL_MOISTURE_MOISTURIZED || timeSeconds - controlStartTime[WATER_PUMP] > WATER_PUMP_MAX_WORK_TIME) {
            changeControl(WATER_PUMP, false);
        }
    }
}

void Greenhouse::onError(uint8_t errorCode) {
    Greenhouse::errorCode = errorCode;

    for (i = 0; i < CONTROLS_COUNT; i++) {
        changeControl(i, false);
    }
}

uint8_t Greenhouse::validate() {
    if (readOnlyMode) {
        return 0;
    }

    if (abs(millis() - previousMillis) > 5000) {
        if (previousSeconds != 0 && previousSeconds == timeSeconds) {
            return 1;
        }

        previousSeconds = timeSeconds;
        previousMillis = millis();
    }

    return 0;
}
