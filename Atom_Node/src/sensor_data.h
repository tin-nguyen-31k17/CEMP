#ifndef INC_SENSOR_DATA_H_
#define INC_SENSOR_DATA_H_

#include <ArduinoJson.h>
#include <Wire.h>

class SENSOR_DATA{
  public:
    SENSOR_DATA(){};
    String floatToString(float value);
    void controlRelay(int RELAY);
    String createWaterStationJSON(float EC, float PH, float ORP, float TEMP, float LON, float LAT, int RELAY);
};

class SENSOR_RS485{
  private:
    uint8_t* data_water_ec;
    uint8_t* data_water_ph;
    uint8_t* data_water_orp;
    uint8_t* data_water_temp;

  public:
    SENSOR_RS485();
    ~SENSOR_RS485();
    uint8_t* getDataWATER_EC();
    uint8_t* getDataWATER_PH();
    uint8_t* getDataWATER_ORP();
    uint8_t* getDataWATER_TEMP();
};

#endif
