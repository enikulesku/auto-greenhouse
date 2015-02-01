#include "greenhouse.h"

void Greenhouse::init() {
    if (debugMode) {
        //ToDo: serial print init
        return;
    }

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
    soilMoisture = analogRead(soilMoistureSensorPin);
};

void Greenhouse::doControlDebug() {
    //ToDo: log start to serial

    process();

    //ToDo: log end to serial
};

void Greenhouse::doControlReal() {
    process();
};

void Greenhouse::process() {
    //ToDo: put logic here
}
