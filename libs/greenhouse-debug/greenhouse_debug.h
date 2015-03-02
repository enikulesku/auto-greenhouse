#ifndef greenhouse_debug_h
#define greenhouse_debug_h

#include "../utils/utils.h"
#include "../greenhouse/greenhouse.h"

#define COMMAND_PARAMS_COUNT        7

class DebugGreenhouse: public Handler {
private:
    Greenhouse *greenhouse;
    Stream *serial;

    void printSensors();

    void printControls();

    int tmp;
    int i;
    int paramIndex;
    boolean commandStarted;
    boolean commandFinished;
    char commandValue[32];
    long command[COMMAND_PARAMS_COUNT];
public:
    int debugId;

    DebugGreenhouse(Greenhouse *greenhouse, Stream *serial);

    void onReset();

    void onControl();

    void onReadSensors();

    boolean onLoop();
};

#endif