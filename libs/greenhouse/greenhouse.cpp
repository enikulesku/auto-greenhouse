#include "greenhouse.h"

void Greenhouse::init() {
    if (debugMode) {
        printCommand("debugMode", "init");
        return;
    }

    // Controls initialization
    pinMode(waterPumpPin, OUTPUT);
    pinMode(lampPin, OUTPUT);
    pinMode(humidifierPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);
}

void Greenhouse::loadSensorsDataDebug() {

    //ToDo: read sensors data from Serial
}

void Greenhouse::loadSensorsDataReal() {

};

void Greenhouse::doControlDebug() {
    printCommand("control", "begin");

    process();

    printCommand("control", "end");
};

void Greenhouse::doControlReal() {
    process();
};

void Greenhouse::process() {
    //ToDo: put logic here
}

void Greenhouse::printCommand(char const* key, char const* value) {
    Serial.print(START);

    Serial.print(key);
    Serial.print(SEP);
    Serial.print(value);

    Serial.print(END);
}
