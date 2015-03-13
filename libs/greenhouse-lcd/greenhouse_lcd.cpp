#include "greenhouse_lcd.h"

LCDGreenhouse::LCDGreenhouse(Greenhouse *greenhouse, LiquidCrystal_I2C *lcd, i2ckeypad *keypad) {
    LCDGreenhouse::greenhouse = greenhouse;
    LCDGreenhouse::lcd = lcd;
    LCDGreenhouse::keypad = keypad;
}

void LCDGreenhouse::onControl() {
    lcd->setCursor(0, 0);
    p(lcd, "M%04d H%02d T%02d L%04d",
            greenhouse->soilMoisture,
            greenhouse->humidity,
            greenhouse->temperature,
            greenhouse->lightLevel);

    lcd->setCursor(0, 1);
    p(lcd, "Pu=%d La=%d Hu=%d He=%d",
            greenhouse->controlStates[WATER_PUMP],
            greenhouse->controlStates[LAMP],
            greenhouse->controlStates[HUMIDIFIER],
            greenhouse->controlStates[HEATER]);

    lcd->setCursor(0, 2);
    printTime(greenhouse->timeSeconds, true);

    lcd->print(" ");
    printTime(greenhouse->sunriseSeconds, false);

    lcd->print(" ");
    printTime(greenhouse->sunsetSeconds, false);

    lcd->setCursor(0, 3);
    printDate(greenhouse->timeSeconds);

    lcd->setCursor(9, 3);
    printTime(greenhouse->expectedSunriseSeconds, false);

    lcd->print(" ");
    printTime(greenhouse->expectedSunsetSeconds, false);
}

void LCDGreenhouse::onReset() {
    //ToDo: some notification
}

boolean LCDGreenhouse::onLoop() {
    switch (keypad->get_key()) {
        case 'A':
            (backLightStatus = !backLightStatus) ? lcd->backlight() : lcd->noBacklight();
            break;
    }

    return true;
}

void LCDGreenhouse::printDate(long seconds) {
    dateTime = DateTime((uint32_t) seconds);

    p(lcd, "%02d.%02d.%02d", dateTime.day(), dateTime.month(), dateTime.year() % 100);
}

void LCDGreenhouse::printTime(long seconds, boolean showSeconds) {
    dateTime = DateTime((uint32_t) seconds);

    p(lcd, "%02d:%02d", dateTime.hour(), dateTime.minute());
    if (showSeconds) {
        p(lcd, ":%02d", dateTime.second());
    }
}