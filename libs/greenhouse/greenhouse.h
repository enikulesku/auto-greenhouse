#include <stdint.h>

#include "Arduino.h"

#include "Wire/Wire.h"
#include "../DHT/DHT.h"
#include "../RTC/RTClib.h"
#include "../Sunrise/Sunrise.h"

class Greenhouse {
private:
    boolean debugMode;

    // Sensors
    DHT* dht;
    RTC_DS1307* rtc;
    Sunrise* sunriseResolver;
    uint8_t soilMoistureSensorPin;

    // Sensors state
    DateTime dateTime;
    DateTime sunrise;
    DateTime sunset;

    float humidity;
    float temperature;
    float soilMoisture;

    //Controls
    uint8_t waterPumpPin;
    uint8_t lampPin;
    uint8_t humidifierPin;
    uint8_t heaterPin;

    // Controls state
    boolean waterPumpOn;
    boolean lampOn;
    boolean humidifierOn;
    boolean heaterOn;

    void loadSensorsDataReal();
    void loadSensorsDataDebug();

    void doControlDebug();
    void doControlReal();

    void process();
public:
    void init();

    void loadSensorsData() {
        if (debugMode) {
            loadSensorsDataDebug();
        } else {
            loadSensorsDataReal();
        }
    };

    void doControl() {
        if (debugMode) {
            doControlDebug();
        } else {
            doControlReal();
        }
    };

    void setDebugMode(boolean debugMode) {
        Greenhouse::debugMode = debugMode;
    }

    void set_dht(DHT *_dht) {
        Greenhouse::dht = _dht;
    }

    void set_rtc(RTC_DS1307 *_rtc) {
        Greenhouse::rtc = _rtc;
    }

    void set_sunrise(Sunrise *sunriseResolver) {
        Greenhouse::sunriseResolver = sunriseResolver;
    }

    void setSoilMoistureSensorPin(uint8_t soilMoistureSensorPin) {
        Greenhouse::soilMoistureSensorPin = soilMoistureSensorPin;
    }

    void setWaterPumpPin(uint8_t waterPumpPin) {
        Greenhouse::waterPumpPin = waterPumpPin;
    }

    void setHumidifierPin(uint8_t humidifierPin) {
        Greenhouse::humidifierPin = humidifierPin;
    }

    void setLampPin(uint8_t lampPin) {
        Greenhouse::lampPin = lampPin;
    }

    void setHeaterPin(uint8_t heaterPin) {
        Greenhouse::heaterPin = heaterPin;
    }
};
