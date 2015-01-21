#include <stdint.h>

#include "Arduino.h"

#include "Wire/Wire.h"
#include "../DHT/DHT.h"
#include "../RTC/RTClib.h"
#include "../Sunrise/Sunrise.h"

class Greenhouse {
private:
    DHT* _dht;
    RTC_DS1307* _rtc;
    Sunrise* _sunrise;

public:
    Greenhouse(DHT* dht, RTC_DS1307* rtc, Sunrise* sunrise);
};
