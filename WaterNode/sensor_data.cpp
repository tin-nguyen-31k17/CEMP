
#include "sensor_data.h"

String SENSOR_DATA::floatToString(float value) {
  char buffer[20];
  sprintf(buffer, "%.2f", value);
  return String(buffer);
}
String SENSOR_DATA::createWaterStationJSON(float EC, float PH, float ORP, float TEMP) {
  DynamicJsonDocument doc(1024);

  doc["station_id"] = "water_0001";
  doc["station_name"] = "WATER 0001";
  doc["gps_longitude"] = 106.99;
  doc["gps_latitude"] = 10.2;

  JsonArray sensors = doc.createNestedArray("sensors");

  JsonObject ec_sensor = sensors.createNestedObject();
  ec_sensor["sensor_id"] = "ec_0001";
  ec_sensor["sensor_name"] = "EC 0001";
  ec_sensor["sensor_value"] = floatToString(EC);
  ec_sensor["sensor_unit"] = "ms/cm";

  JsonObject ph_sensor = sensors.createNestedObject();
  ph_sensor["sensor_id"] = "ph_0001";
  ph_sensor["sensor_name"] = "PH 0001";
  ph_sensor["sensor_value"] = floatToString(PH);
  ph_sensor["sensor_unit"] = "pH";

  JsonObject orp_sensor = sensors.createNestedObject();
  orp_sensor["sensor_id"] = "ORP_0001";
  orp_sensor["sensor_name"] = "ORP 0001";
  orp_sensor["sensor_value"] = floatToString(ORP);
  orp_sensor["sensor_unit"] = "mV";

  JsonObject temp_sensor = sensors.createNestedObject();
  temp_sensor["sensor_id"] = "TEMP_0001";
  temp_sensor["sensor_name"] = "Nhiệt Độ";
  temp_sensor["sensor_value"] = floatToString(TEMP);
  temp_sensor["sensor_unit"] = "°C";

  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println("Data to pub:");
  serializeJsonPretty(doc, Serial);
  doc.clear();
  Serial.println();
  return jsonString;
}
///////////////////////////////////////

SENSOR_RS485::SENSOR_RS485(){
  data_water_ec = new uint8_t[8]{0x04, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x5E};
  data_water_ph = new uint8_t[8]{0x02, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xF8};
  data_water_orp = new uint8_t[8]{0x05, 0x03, 0x00, 0x01, 0x00, 0x02, 0x94, 0x4F};
  data_water_temp = new uint8_t[8]{0x05, 0x03, 0x00, 0x03, 0x00, 0x02, 0x35, 0x8F};
};

SENSOR_RS485::~SENSOR_RS485() {
  delete[] data_water_ec;
  delete[] data_water_ph;
  delete[] data_water_orp;
  delete[] data_water_temp;
};

uint8_t* SENSOR_RS485::getDataWATER_EC(){
  return data_water_ec;
};

uint8_t* SENSOR_RS485::getDataWATER_PH(){
  return data_water_ph;
};

uint8_t* SENSOR_RS485::getDataWATER_ORP(){
  return data_water_orp;
};

uint8_t* SENSOR_RS485::getDataWATER_TEMP(){
  return data_water_temp;
};
