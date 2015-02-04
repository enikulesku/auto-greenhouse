#ifndef Sunrise_h
#define Sunrise_h

class Sunrise{
  public:
  Sunrise(float, float, float);
  void Actual();
  void Civil();
  void Nautical();
  void Astronomical();
  int Rise(uint8_t, uint8_t);
  int Set(uint8_t, uint8_t);
  int Noon(uint8_t, uint8_t);
  uint8_t Hour();
  uint8_t Minute();
  
  private:
  int Compute(uint8_t, uint8_t, int);
  float lat,lon, zenith, rd, tz;
    uint8_t  theHour,theMinute;
};

#endif