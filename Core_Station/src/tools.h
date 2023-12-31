/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/
int8_t getBatteryLevel() {
  return M5.Power.getBatteryLevel();
}

bool isCharging() {
  return M5.Power.isCharging();
}