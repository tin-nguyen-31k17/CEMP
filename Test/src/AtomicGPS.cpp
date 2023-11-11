
  /*This is an example used SerialBT,you can can view gps data by connecting 
   * to Bluetooth assistant on your mobilephone or Serial Monitor
   * the GPS log will be written to SD card
   * 
   */
  // do effective when you want NMEA out
  //#define NMEA_OUT

  #define deltaLat 0.001
  #define deltaLon 0.001

  #include "M5Atom.h"
  #include "GPSAnalyse.h"
  #include <SPI.h>
  #include "FS.h"
  #include <SD.h>
  #include <BluetoothSerial.h>

  BluetoothSerial SerialBT;
  GPSAnalyse GPS;

  uint64_t chipid;
  char chipname[256];

  const char filename[] = "/GPSdata.txt";
  File txtFile;

  float Lat;
  float Lon;
  String Utc;

  bool writeLog(String filename) {          //Write GPSdata to SDcard
    txtFile = SD.open(filename, FILE_APPEND);
    if(txtFile){
      txtFile.printf("%.5f",Lat);
      txtFile.print(", ");
      txtFile.printf("%.5f",Lon);
      txtFile.print(", ");
  /*
      txtFile.print(Lat);
      txtFile.print(", ");
      txtFile.print(Lon);
      txtFile.print(", ");
  */
      txtFile.println(Utc);
      txtFile.close();
    }else{
      return false;
    }
    return true;
  }

  void setup() {

      M5.begin(true,false,true); 
      chipid = ESP.getEfuseMac();
      sprintf( chipname, "SerialBT_%04X", (uint16_t)(chipid >> 32));
      Serial.printf("Bluetooth: %s\n", chipname);   
      SPI.begin(23,33,19,-1);
      if(!SD.begin(-1, SPI, 40000000)){
        Serial.println("initialization failed!");
      } 
      sdcard_type_t Type = SD.cardType();

        Serial.printf("SDCard Type = %d \r\n",Type);
        Serial.printf("SDCard Size = %d \r\n" , (int)(SD.cardSize()/1024/1024));

  //    M5.dis.fillpix(0x00004f);
        M5.dis.drawpix(0,0x00004f);

  Serial.begin(115200);     
      Serial1.begin(9600,SERIAL_8N1,32,26);
      SerialBT.begin(chipname);
      GPS.setTaskName("GPS");
      GPS.setTaskPriority(2);
      GPS.setSerialPtr(Serial1);
      GPS.start();
  }

  #ifdef NMEA_OUT
  void loop() {
    if (Serial1.available()) {
      char c = Serial1.read();
      Serial.write(c);
      SerialBT.write(c);
    }
  }
  #endif

  #ifndef NMEA_OUT
  float prevLat=0.0;
  float prevLon=0.0;
  void loop() {
      GPS.upDate();
      Lat = GPS.s_GNRMC.Latitude;
      Lon = GPS.s_GNRMC.Longitude;
      Utc = GPS.s_GNRMC.Utc;
      SerialBT.printf("Latitude= %.5f \r\n",Lat);
      SerialBT.printf("Longitude= %.5f \r\n",Lon);
      SerialBT.printf("DATA= %s \r\n",Utc);
      Serial.printf("Latitude= %.5f \r\n",Lat);
      Serial.printf("Longitude= %.5f \r\n",Lon);
      Serial.printf("DATA= %s \r\n",Utc);
      if ((Lat != 0)&&(Lon != 0)) {
        if ((abs(Lat-prevLat)<deltaLat)&&(abs(Lon-prevLon)<deltaLon)) writeLog(filename); // write them when Lat/Lon effective
        prevLat=Lat; prevLon=Lon;
      };
      delay(1000);
  }
  #endif