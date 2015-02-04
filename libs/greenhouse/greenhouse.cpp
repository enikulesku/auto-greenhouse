#include "greenhouse.h"

void Greenhouse::init() {
    if (debugMode) {
        return;
    }

    // Controls initialization
    for (i = 0; i < CONTROLS_COUNT; i++) {
        pinMode(controlPins[i], OUTPUT);
    }
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

void Greenhouse::doControlDebug() {
    process();

    Serial.print(START);

    for (i = 0; i < CONTROLS_COUNT; i++) {
        if (i != 0) {
            Serial.print(SEP);
        }

        Serial.print(controlStates[i]);
    }

    Serial.println(END);
};

void Greenhouse::doControlReal() {
    process();
};

void Greenhouse::process() {
    //ToDo: put logic here
}
