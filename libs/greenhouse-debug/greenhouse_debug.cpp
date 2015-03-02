#include "greenhouse_debug.h"

DebugGreenhouse::DebugGreenhouse(Greenhouse *greenhouse, Stream *serial) {
    DebugGreenhouse::greenhouse = greenhouse;
    DebugGreenhouse::serial = serial;
}

void DebugGreenhouse::onReset() {
    p(&Serial, "^R,%d$\n", debugId);
    Serial.flush();
}

void DebugGreenhouse::onControl() {
    printSensors();
    printControls();
}

void DebugGreenhouse::onReadSensors() {
    greenhouse->dateTime = DateTime((uint32_t) command[2]);

    greenhouse->humidity = (int) command[3];
    greenhouse->temperature = (int) command[4];
    greenhouse->soilMoisture = (int) command[5];
    greenhouse->lightLevel = (int) command[6];
}

boolean DebugGreenhouse::onLoop() {
    while (Serial.available()) {
        tmp = Serial.read();

        switch (tmp) {
            case '^':
                commandStarted = true;

                i = 0;
                paramIndex = 0;
                break;

            case '$':
                if (commandStarted) {
                    commandFinished = true;
                }
            case ',':
                if (commandStarted) {
                    commandValue[i] = '\0';

                    if (paramIndex == 0) {
                        command[0] = commandValue[0];
                    } else if (paramIndex > 0 && paramIndex < COMMAND_PARAMS_COUNT) {
                        command[paramIndex] = atol(commandValue);
                    }

                    i = 0;
                    paramIndex++;

                    if (commandFinished) {
                        commandFinished = false;
                        commandStarted = false;
                        i = 0;
                        paramIndex = 0;

                        debugId = (int) command[1];

                        //ToDo: review
                        switch (command[0]) {
                            case 'S':// sensors info
                                return true;

                            case 'R':// reset
                                greenhouse->reset();
                                return false;
                        }
                    }
                }

                break;

            default:
                if (commandStarted) {
                    commandValue[i++] = (char) tmp;
                }
                break;
        }
    }

    return false;
}

void DebugGreenhouse::printSensors() {
    p(serial, "^S,%d,%ld,%d,%d,%d,%d,%ld,%ld$\n",
            debugId,
            greenhouse->timeSeconds,
            greenhouse->humidity,
            greenhouse->temperature,
            greenhouse->soilMoisture,
            greenhouse->lightLevel,
            greenhouse->sunriseSeconds,
            greenhouse->sunsetSeconds);

    serial->flush();
};

void DebugGreenhouse::printControls() {
    p(serial, "^C,%d,%d,%d,%d,%d$\n",
            debugId,
            greenhouse->controlStates[WATER_PUMP],
            greenhouse->controlStates[LAMP],
            greenhouse->controlStates[HUMIDIFIER],
            greenhouse->controlStates[HEATER]);

    serial->flush();
}

