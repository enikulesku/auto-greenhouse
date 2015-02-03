#include <stdint.h>

#include "Arduino.h"

#include "Wire/Wire.h"
#include "../DHT/DHT.h"
#include "../RTC/RTClib.h"
#include "../Sunrise/Sunrise.h"

#define START    '^'
#define END      '$'
#define SEP   ','

class Greenhouse {
private:
    boolean debugMode;
    // Sensors state
    DateTime dateTime;
    DateTime sunrise;
    DateTime sunset;

    float humidity;
    float temperature;
    int soilMoisture;

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

    // Debug mode



    void printCommand(char const* key, char const* value);
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

    //Sensors


    void setDateTime(DateTime const &dateTime) {
        Greenhouse::dateTime = dateTime;
    }

    void setSunrise(DateTime const &sunrise) {
        Greenhouse::sunrise = sunrise;
    }

    void setSunset(DateTime const &sunset) {
        Greenhouse::sunset = sunset;
    }

    void setHumidity(float humidity) {
        Greenhouse::humidity = humidity;
    }

    void setTemperature(float temperature) {
        Greenhouse::temperature = temperature;
    }

    void setSoilMoisture(int soilMoisture) {
        Greenhouse::soilMoisture = soilMoisture;
    }

//Control
    void setDebugMode(boolean debugMode) {
        Greenhouse::debugMode = debugMode;
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
