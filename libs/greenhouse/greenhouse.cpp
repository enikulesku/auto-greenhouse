#include "greenhouse.h"

Greenhouse::Greenhouse(DHT* dht, RTC_DS1307* rtc, Sunrise* sunrise) {
    _dht = dht;
    _rtc = rtc;
    _sunrise = sunrise;
}
