/*
    Project: CEMP Mobility Water Monitoring Station
    Author: Nguyen Trong Tin
    Email:tin.nguyen.31k17@hcmut.edu.vn
    Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

#include "MQTT_helper.h"
#include "sensor_data.h"
#include "C_Cam.h"

HardwareSerial serial_ext(2);
TFT_eSprite *Spr;
uint32_t front_col = TFT_YELLOW;
uint32_t back_col = TFT_BLACK;

int dat[160];
int dat2[160];
String moji;
String moji2;

int dist = 0;
int dist_thresh = 50;
bool sendphoto_flg = 0;
typedef struct {
  uint32_t length;
  uint8_t *buf;
} jpeg_data_t;

jpeg_data_t jpeg_data;
static const int RX_BUF_SIZE = 30000;
static const uint8_t packet_img[4] = { 0xFF, 0xF1, 0xF2, 0xA1};
static const uint8_t packet_dist[4] = { 0xFF, 0xF3, 0xF4, 0xA1};
static const uint8_t packet_thresh[4] = { 0xFF, 0xF5, 0xF6, 0xA1};
ImageDataHandler imageDataHandler;

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishToMQTT(jpeg_data_t& jpegData) {
  if (jpegData.buf != NULL && jpegData.length > 0) {
    // Calculate the size of the Base64 encoded string
    int encodedSize = Base64.encodedLength(jpegData.length);
    char encodedImage[encodedSize];

    // Base64 encode the image
    Base64.encode(encodedImage, (char*)jpegData.buf, jpegData.length);

    // Publish the Base64 encoded image
    if (myMQTT.publish(myTopic, encodedImage)) {
      Serial.println("Publish successful");
    } else {
      Serial.println("Publish failed");
    }
  }
}

void setup() {
  M5.begin();
  Serial.println("### M5 Initialized");
  M5.Lcd.setRotation(1);
  //LCD Splite Initialize
  Spr = new TFT_eSprite(&M5.Lcd);
  Spr->setColorDepth(8);
  Spr->createSprite(320, 240);
  Spr->setBitmapColor(front_col, back_col);
  Spr->fillSprite(front_col);
  Spr->pushSprite(0, 0);

  Spr->setTextColor(TFT_WHITE); // White text, no background

  setup_wifi();
  
  myMQTT.connectToMQTT();

  jpeg_data.buf = (uint8_t *) malloc(sizeof(uint8_t) * RX_BUF_SIZE);
  jpeg_data.length = 0;
  if (jpeg_data.buf == NULL) {
    Serial.println("### Malloc jpeg buffer 1 error");
  }

  serial_ext.begin(115200, SERIAL_8N1, 32, 33);
  for (int x = 0; x < 160; x++)dat[x] = 0;
  for (int x = 0; x < 160; x++)dat2[x] = 0;
  Serial.println("### Setup Completed");
}

void loop() {
  Serial.println("### Loop Started");
  M5.update();

  // Button Press Logic (if required)
  if (M5.BtnA.wasPressed()) {
      Serial.println("### BtnA Pressed");
      sendphoto_flg = !sendphoto_flg;
  }

  // Handling incoming UART data
  if (serial_ext.available()) {
    Serial.println("### UART Data Available");
    uint8_t rx_buffer[10];
    int rx_size = serial_ext.readBytes(rx_buffer, 10);

    if (rx_size == 10) {
      Serial.println("### UART Data Received");
      if (memcmp(rx_buffer, packet_img, 4) == 0) {
          // Image packet received
          Serial.println("### Image Packet Detected");
          jpeg_data.length = (uint32_t)(rx_buffer[4] << 16) | (rx_buffer[5] << 8) | rx_buffer[6];
          
          if (jpeg_data.length > 0 && jpeg_data.length <= RX_BUF_SIZE) {
              free(jpeg_data.buf); // Free previous buffer if allocated
              jpeg_data.buf = (uint8_t *)malloc(jpeg_data.length); // Allocate new buffer
              
              if (jpeg_data.buf == NULL) {
                  Serial.println("### Allocating Memory for Image");
                  return; // Skip this loop iteration
              }

              int readSize = serial_ext.readBytes(jpeg_data.buf, jpeg_data.length);
              if (readSize == jpeg_data.length) {
                  // Image data successfully received
                  Serial.println("### Image Data Read Successfully");
                  Serial.println("### Publishing Image to MQTT");
                  publishToMQTT(jpeg_data); // Publish image
              }

              free(jpeg_data.buf); // Free after publishing
              jpeg_data.buf = NULL;
              jpeg_data.length = 0;
          }
      }
      else if (memcmp(rx_buffer, packet_dist, 4) == 0) {
          // Handle other packet types if needed
          Serial.println("### Other Packet Type Detected");
          Serial.println("Image packet received");
          imageDataHandler.processImageData(jpeg_data.buf, jpeg_data.length);
      }
    }
  } else {
    Serial.println("### Nothing Received!!!");
  }

  // Graphical Display Logic
  Spr->fillSprite(back_col);
  double ratio = 80 * 0.65 / dist_thresh;
  for (int x = 0; x < 160 - 1; x++) {
      Spr->drawLine(x, 65 - dat[x] * ratio, x + 1, 65 - dat[x + 1] * ratio, TFT_YELLOW);
      Spr->drawLine(x, 65 - dat2[x] * ratio, x + 1, 65 - dat2[x + 1] * ratio, TFT_RED);
  }
  moji = String(dist) + "/" + String(dist_thresh) + "    ";
  moji2 = (sendphoto_flg) ? "Photo Send" : "Message Only";
  Spr->drawString(moji, 5, 0, 2);
  Spr->drawString(moji2, 70, 65, 2);
  Spr->pushSprite(0, 0);

  vTaskDelay(10 / portTICK_RATE_MS);

  // MQTT Connection Check
  myMQTT.checkConnect(); // Keep checking the MQTT connection
  delay(10);
}
