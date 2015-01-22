#include "greenhouse.h"

void Greenhouse::init() {
    // Sensors initialization
    pinMode(soilMoistureSensorPin, INPUT);

    rtc->begin();
    dht->begin();
    sunriseResolver->Civil(); //Actual, Civil, Nautical, Astronomical

    if (!rtc->isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(__DATE__, __TIME__));
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
    dateTime = rtc->now();

    sunriseResolver->Rise(dateTime.month(), dateTime.day());
    sunrise = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), sunriseResolver->Hour(), sunriseResolver->Minute(), 0);

    sunriseResolver->Set(dateTime.month(), dateTime.day());
    sunset = DateTime(dateTime.year(), dateTime.month(), dateTime.day(), sunriseResolver->Hour(), sunriseResolver->Minute(), 0);

    humidity = dht->readHumidity();
    temperature = dht->readTemperature();
    soilMoisture = digitalRead(soilMoistureSensorPin);
};

void Greenhouse::doControl() {
    //ToDo: put logic here
};
