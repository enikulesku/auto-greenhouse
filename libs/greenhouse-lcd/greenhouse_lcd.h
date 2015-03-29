#ifndef greenhouse_lcd_h
#define greenhouse_lcd_h

#include "../utils/utils.h"
#include "../greenhouse/greenhouse.h"
#include "../LCDI2C/LiquidCrystal_I2C.h"
#include "../i2ckeypad/i2ckeypad.h"

class LCDGreenhouse : public Handler {
private:
    Greenhouse *greenhouse;
    LiquidCrystal_I2C *lcd;
    i2ckeypad *keypad;

    DateTime dateTime;

    boolean backLightStatus;

    unsigned long backLightLastSwitch;
    boolean backLightBlinkStatus;

    void printDate(long seconds);
    void printTime(long seconds, boolean showSeconds);

public:
    LCDGreenhouse(Greenhouse *greenhouse, LiquidCrystal_I2C *lcd, i2ckeypad *keypad);

    void onReset();

    void onControl();

    void onReadSensors() {};

    boolean onLoop();
};

#endif