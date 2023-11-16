// View UI
void viewUI() {
  M5.Displays(0).setBrightness(brightness);
  M5.Displays(0).fillScreen(TFT_SCREEN_BG);

  M5.Displays(0).drawPng(EC, sizeof(EC), 16, 16, 64, 64);
  M5.Displays(0).drawPng(Temp, sizeof(Temp), 16, 160, 64, 64);
  M5.Displays(0).drawPng(ORP, sizeof(ORP), 176, 160, 65, 65);
  M5.Displays(0).drawPng(pH, sizeof(pH), 176, 16, 64, 64);

  M5.Displays(0).fillRect(16 + 64 * 0.05 + 8 * 0.05, 118, 64, 10, TFT_GREEN);
  M5.Displays(0).fillRect(16 + 64 * 1.05 + 8 * 1.05, 118, 64, 10, TFT_YELLOW);
  M5.Displays(0).fillRect(16 + 64 * 2.05 + 8 * 2.05, 118, 64, 10, TFT_ORANGE);
  M5.Displays(0).fillRect(16 + 64 * 3.05 + 8 * 3.05, 118, 64, 10, TFT_RED);

  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(20);

  M5.Displays(0).drawString("GOOD", 51, 138);
  M5.Displays(0).drawString("OK", 156, 138);
  M5.Displays(0).drawString("BAD", 266, 138);

  M5.Displays(0).setTextDatum(CL_DATUM);
  M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Water EC", 90, 24);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Water pH", 248, 24);
  M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Water Temp", 90, 168);
  M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Water ORP", 248, 168);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);

  // M5.Displays(0).drawString("V" + String(VERSION) + " by " + String(AUTHOR), 202, 232);
  M5.Displays(0).drawString("V" + String(VERSION), 280, 232);
}

// View battery
void viewBattery() {
  char buf[8];

  uint8_t batteryLevel;
  boolean batteryCharging;

  batteryLevel = map(getBatteryLevel(), 0, 100, 0, 17);
  batteryCharging = isCharging();

  // Always update battery level and charging status
  batteryLevelOld = batteryLevel;
  batteryCharginglOld = batteryCharging;

  M5.Displays(0).drawRect(293, 4, 21, 12, TFT_WHITE);
  M5.Displays(0).drawRect(313, 7, 4, 6, TFT_WHITE);

  for (uint8_t i = 0; i < batteryLevel; i++) {
    M5.Displays(0).drawGradientVLine(295 + i, 6, 8, TFT_GREEN, TFT_DARKGREEN);
  }

  M5.Displays(0).drawFastVLine(300, 5, 10, TFT_SCREEN_BG);
  M5.Displays(0).drawFastVLine(306, 5, 10, TFT_SCREEN_BG);

  if (batteryCharging) {
    M5.Displays(0).setTextColor(TFT_WHITE);
    M5.Displays(0).setFont(&arial6pt7b);
    M5.Displays(0).setTextDatum(CC_DATUM);
    M5.Displays(0).setTextPadding(0);
    snprintf(buf, 8, "%s", "+");
  } else {
    M5.Displays(0).setTextColor(TFT_WHITE);
    M5.Displays(0).setFont(0);
    M5.Displays(0).setTextDatum(CR_DATUM);
    M5.Displays(0).setTextPadding(0);
    snprintf(buf, 8, "%d%s", getBatteryLevel(), "%");
  }
  M5.Displays(0).drawString(buf, 290, 11);
}

// Init Led
void initLed() {
  for (uint8_t j = 0; j < NUM_LEDS; j++) {
    leds[j] = CRGB::Black;
  }
  FastLED.setBrightness(16);
  FastLED.show();
}

void initPhysical() {
  // Init Display
  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(180);

#if BOARD != CORES3
  for (uint8_t i = 0; i < 5; i++) {
    leds[4 - i] = CRGB::DarkBlue;
    leds[5 + i] = CRGB::DarkBlue;

    FastLED.setBrightness(16);
    FastLED.show();

    M5.Displays(0).drawString("Please wait! Initializing...", 160, 100);
    delay(1000);
    M5.Displays(0).drawString("", 160, 100);
    delay(500);
  }
#else
  for (uint8_t i = 0; i < 5; i++) {
    M5.Displays(0).drawString("Please wait! Initializing...", 160, 100);
    delay(1000);
    M5.Displays(0).drawString("", 160, 100);
    delay(500);
  }
#endif
}

// Fade all led
void fadeall() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(100);
  }
}

// Led task
void led(void *pvParameters) {
  for (;;) {
    // First slide the led in one direction
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = m5goColor;
      ;
      // Show the leds
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Now go in the other direction
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[(NUM_LEDS)-1 - i] = m5goColor;
      ;
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

// Check button
void button(void *pvParameters) {
  uint8_t step = 2;
  uint8_t min  = 4;
  uint8_t max  = 255;

  uint8_t btnA = 0;
  uint8_t btnB = 0;
  uint8_t btnC = 0;

  uint16_t brightnessOld = 0;

  struct Button {
    char name[16];   // name
    int x;           // x
    int y;           // y
    int w;           // width
    int h;           // height
    int d;           // distance
    boolean active;  // active, if true, check this button, else bypass
    boolean read;    // read, if true, button is push, else false
  };

#if BOARD == CORES3
  Button myBtn[] = {
    {"myBtnA", 0, 160, 100, 80, 1000, true, false},
    {"myBtnB", 110, 160, 100, 80, 1000, true, false},
    {"myBtnC", 220, 160, 100, 80, 1000, true, false},
  };
#else
  Button myBtn[] = {
    {"myBtnA", 0, 240, 100, 80, 1000, true, false},
    {"myBtnB", 110, 240, 100, 80, 1000, true, false},
    {"myBtnC", 220, 240, 100, 80, 1000, true, false},
  };
#endif

  for (;;) {
    M5.update();

    if (M5.getBoard() == m5::board_t::board_M5Stack || M5.getBoard() == m5::board_t::board_M5StackCore2) {
      step = 4;
      min  = 4;
      max  = 255;

      btnA = M5.BtnA.isPressed();
      btnB = M5.BtnB.isPressed();
      btnC = M5.BtnC.isPressed();
    } else if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
      step = 16;
      min  = 64;
      max  = 255;

      auto t = M5.Touch.getDetail();
      auto c = M5.Touch.getCount();

      uint8_t limit = sizeof myBtn / sizeof myBtn[0];

      int distanceBtn     = 0;
      int distanceMin     = 1000;
      int distanceCurrent = 1000;

      if (c == 1) {
        for (uint8_t i = 0; i < limit; i++) {
          myBtn[i].read = false;
          if (myBtn[i].active == true) {
            distanceCurrent = (int)(sqrt(pow(t.x - (myBtn[i].x + (myBtn[i].w / 2)), 2) +
                                         pow(t.y - (myBtn[i].y + (myBtn[i].h / 2)), 2)));
            myBtn[i].d      = distanceCurrent;
            if (distanceCurrent < distanceMin) {
              distanceMin = distanceCurrent;
              distanceBtn = i;
            }
          }
        }

        if (t.state == 1 || t.state == 3 || t.state == 5) {
          myBtn[distanceBtn].read = true;
        }
      }

      btnA = myBtn[0].read;
      btnB = myBtn[1].read;
      btnC = myBtn[2].read;
    }

    if (btnA || btnC) {
      if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      brightnessOld = brightness;

      if (btnA) {
        brightnessOld -= step;
        brightnessOld = (brightnessOld <= min) ? min : brightnessOld;
      } else if (btnC) {
        brightnessOld += step;
        brightnessOld = (brightnessOld >= max) ? max : brightnessOld;
      }

      if (brightnessOld != brightness) {
        brightness = brightnessOld;
        M5.Displays(0).setBrightness(brightness);
        preferences.putUInt("brightness", brightness);
        Serial.println(brightness);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// Check water quality
void checkWaterQuality(float EC, float pH, float ORP, float Temp) {
  // Normalize the sensor values to range from 0 to 1
  float normalizedEC = EC / 2000;
  float normalizedpH = (14 - pH) / 14; // assuming neutral pH is best
  float normalizedORP = (ORP + 2000) / 4000; // assuming higher ORP is better
  float normalizedTemp = (60 - Temp) / 60; // assuming lower temperature is better

  // Calculate a weighted sum of the sensor values
  float weightedSum = 0.4 * normalizedEC + 0.2 * normalizedpH + 0.2 * normalizedORP + 0.2 * normalizedTemp;

  // Check the water quality based on the weighted sum
  if (weightedSum < 0.25) {
    M5.Displays(0).fillRect(16 + 64 * 0.05 + 8 * 0.05, 108, 64, 2, TFT_WHITE);
    m5goColor = CRGB::Green;
  } else if (weightedSum < 0.5) {
    M5.Displays(0).fillRect(16 + 64 * 1.05 + 8 * 1.05, 108, 64, 2, TFT_WHITE);
    m5goColor = CRGB::Yellow;
  } else if (weightedSum < 0.75) {
    M5.Displays(0).fillRect(16 + 64 * 2.05 + 8 * 2.05, 108, 64, 2, TFT_WHITE);
    m5goColor = CRGB::Orange;
  } else {
    M5.Displays(0).fillRect(16 + 64 * 3.05 + 8 * 3.05, 108, 64, 2, TFT_WHITE);
    m5goColor = CRGB::Red;
  }
}

// Detail for each range of `weightedSum`:

// - **`weightedSum < 0.25` (Green)**: This indicates that the water quality is good. In a real-world scenario, this could mean that the water is safe for most uses, including drinking, cooking, bathing, and irrigation. The sensor readings are in the optimal range for all or most of the parameters. For example, the EC (Electrical Conductivity) might be low, indicating low levels of dissolved salts and other inorganic substances. The pH might be close to neutral, which is generally considered optimal for most aquatic life and for human use. The ORP (Oxidation-Reduction Potential) might be high, indicating that the water has a good capacity to break down contaminants. The temperature might be within a suitable range for most aquatic life.

// - **`0.25 <= weightedSum < 0.5` (Yellow)**: This indicates that the water quality is acceptable, but not optimal. In a real-world scenario, this could mean that the water is safe for some uses, such as irrigation, but might not be safe for others, such as drinking or cooking, without treatment. The sensor readings are in the acceptable range for all or most of the parameters, but some readings may be outside the optimal range. For example, the EC might be moderately high, indicating a moderate level of dissolved salts and other inorganic substances. The pH might be slightly acidic or slightly alkaline. The ORP might be moderately high. The temperature might be slightly higher or lower than the optimal range for most aquatic life.

// - **`0.5 <= weightedSum < 0.75` (Orange)**: This indicates that the water quality is poor. In a real-world scenario, this could mean that the water is not safe for many uses without treatment. The sensor readings are outside the optimal range for several parameters, and may be outside the acceptable range for some parameters. For example, the EC might be high, indicating a high level of dissolved salts and other inorganic substances. The pH might be moderately acidic or moderately alkaline. The ORP might be moderately low, indicating that the water has a reduced capacity to break down contaminants. The temperature might be significantly higher or lower than the optimal range for most aquatic life.

// - **`weightedSum >= 0.75` (Red)**: This indicates that the water quality is very poor or potentially hazardous. In a real-world scenario, this could mean that the water is not safe for any use without significant treatment. The sensor readings are outside the acceptable range for most or all of the parameters. For example, the EC might be very high, indicating a very high level of dissolved salts and other inorganic substances. The pH might be highly acidic or highly alkaline. The ORP might be low, indicating that the water has a poor capacity to break down contaminants. The temperature might be extremely higher or lower than the optimal range for most aquatic life.