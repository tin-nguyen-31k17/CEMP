#ifndef INC_WIFI_SETUP_H_
#define INC_WIFI_SETUP_H_

#include <WiFi.h>
#include <EEPROM.h>
#include <Preferences.h>

class Wifi_esp32 {
  private:
    Preferences preferences;
    String ssid;
    String password;

  private:
    void saveWifiCredentials(String, String); // Hàm để lưu giá trị wifi
    bool check_savedWifi(); // Hàm để check wifi đã lưu hay chưa
    void setupSmartConfig(); // Hàm để chạy smart config
    void printSuccess(); // Hàm để in sau khi thành công
    
  public:
    Wifi_esp32(); // Constructor mặc định, hệ thống sẽ tự smartconfig và nhớ wifi
    Wifi_esp32(String, String); // Constructor khi ta muốn truyền pass và ID vào
    void setupWifi(); // Ta chỉ gọi duy nhất hàm này trong setup
};
/*
Cách xài:
#include "wifi_setup.h"


*/
#endif /* INC_WIFI_SETUP_H_ */
