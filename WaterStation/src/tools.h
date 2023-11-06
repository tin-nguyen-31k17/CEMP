// Get Battery level
int8_t getBatteryLevel() {
  return M5.Power.getBatteryLevel();
}

// Control if charging
bool isCharging() {
  return M5.Power.isCharging();
}