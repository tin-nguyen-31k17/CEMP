/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

struct IconSize {
  int width;
  int height;
};

struct IconPosition {
  int x;
  int y;
};

const IconPosition iconPositions[] = {
  {16, 16},    // EC
  {16, 160},   // Temperature
  {176, 160},  // ORP
  {176, 16}    // pH
};

const IconSize iconSizes[] = {
  {64, 64},    // EC
  {64, 64},    // Temperature
  {65, 65},    // ORP
  {64, 64}     // pH
};

struct IconData {
  const unsigned char* onImage;
  const unsigned char* offImage;
  bool state;
};

IconData icons[] = {
  {EC, EC_off, true},
  {Temp, Temp_off, true},
  {ORP, ORP_off, true},
  {pH, pH_off, true}
};

void toggleIcon(int iconIndex) {
  IconData& icon = icons[iconIndex];
  icon.state = !icon.state;

  const IconPosition& pos = iconPositions[iconIndex];
  const IconSize& size = iconSizes[iconIndex];
  M5.Lcd.fillRect(pos.x, pos.y, size.width, size.height, TFT_SCREEN_BG);
  M5.Lcd.drawPng(icon.state ? icon.onImage : icon.offImage, sizeof(pH), pos.x, pos.y, size.width, size.height);
}

void button(void* pvParameters) {
  for (;;) {
    M5.update();
    auto touchDetails = M5.Touch.getDetail();
      for (int i = 0; i < 4; ++i) {
        const IconPosition& pos = iconPositions[i];
        const IconSize& size = iconSizes[i];
        if (touchDetails.state == 1 && 
          touchDetails.x >= pos.x && touchDetails.x <= pos.x + size.width && 
          touchDetails.y >= pos.y && touchDetails.y <= pos.y + size.height) {
          toggleIcon(i);
          break;
        }
      }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void bootLogo() {
    M5.Lcd.fillScreen(TFT_SCREEN_BG);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("CEMP Mobility Water Monitoring Station", 50, 165);
    M5.Lcd.drawPng(logo, sizeof(logo), 100, 40, 120, 120);
    delay(3000);
}

// View battery
void viewBattery() {
  char buf[8];

  uint8_t batteryLevel;
  boolean batteryCharging;

  batteryLevel = map(getBatteryLevel(), 0, 100, 0, 17);
  batteryCharging = isCharging();

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
  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(180);

  for (uint8_t i = 0; i < 5; i++) {
    M5.Displays(0).drawString("Please wait! Initializing...", 160, 100);
    delay(1000);
    M5.Displays(0).drawString("", 160, 100);
    delay(500);
  }
}

void fadeall() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(100);
  }
}

void led(void *pvParameters) {
  for (;;) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = m5goColor;
      ;
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[(NUM_LEDS)-1 - i] = m5goColor;
      ;
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

// Function to convert CRGB to a format compatible with LGFX (e.g., rgb565_t)
lgfx::v1::rgb565_t convertCRGBtoRGB565(const CRGB& color) {
    // Extract individual color components from CRGB
    uint8_t red = color.red;
    uint8_t green = color.green;
    uint8_t blue = color.blue;

    // Convert to RGB565 format
    uint16_t rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
    return lgfx::v1::rgb565_t(rgb565);
}

const char* waterQualityMessages[] = {
    "Excellent - Potable, fit for human consumption",
    "Very Good - Suitable for sensitive aquatic species",
    "Good - Suitable for livestock, irrigation",
    "Moderate - Caution advised, possible contaminants",
    "Poor - Not suitable for direct consumption",
    "Very Poor - Risky for all living organisms",
    "Extremely Dangerous - Toxic, do not consume or use"
};

// Calculate weighted sum based on normalized sensor readings
float calculateWeightedSum(float normalizedEC, float normalizedpH, float normalizedORP, float normalizedTemp) {
    return 0.4 * normalizedEC + 0.2 * normalizedpH + 0.2 * normalizedORP + 0.2 * normalizedTemp;
}

// Determine the quality level based on weighted sum
int determineQualityLevel(float weightedSum) {
    return static_cast<int>(weightedSum * 7);
}

// Display the water quality message on the screen
void displayWaterQualityMessage(int qualityLevel) {
    M5.Displays(0).drawString(waterQualityMessages[qualityLevel], 18, 138);
}

// Draw the color-coded indicator on a ruler
void drawQualityIndicator(int qualityLevel) {
    CRGB color;
    switch (qualityLevel) {
        case 0: color = CRGB::Green; break;
        case 1: color = CRGB::GreenYellow; break;
        case 2: color = CRGB::Yellow; break;
        case 3: color = CRGB::Orange; break;
        case 4: color = CRGB::DarkOrange; break;
        case 5: color = CRGB::Red; break;
        default: color = CRGB::DarkRed; break;
    }
    // Convert CRGB to RGB565
    lgfx::v1::rgb565_t convertedColor = convertCRGBtoRGB565(color);

    // Use the converted color with LGFX functions
    M5.Displays(0).fillRect(16 + 64 * qualityLevel, 108, 64, 2, convertedColor);
}

// Main function to check water quality
void checkWaterQuality(float EC, float pH, float ORP, float Temp) {
    // Normalize sensor values
    float normalizedEC = EC / 2000;
    float normalizedpH = (14 - pH) / 14;
    float normalizedORP = (ORP + 2000) / 4000;
    float normalizedTemp = (60 - Temp) / 60;
    
    // Calculate weighted sum and determine quality level
    float weightedSum = calculateWeightedSum(normalizedEC, normalizedpH, normalizedORP, normalizedTemp);
    int qualityLevel = determineQualityLevel(weightedSum);

    // Display results
    displayWaterQualityMessage(qualityLevel);
    drawQualityIndicator(qualityLevel);
}

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

// Detail for each range of `weightedSum`:

// - **`weightedSum < 0.25` (Green)**: This indicates that the water quality is good. In a real-world scenario, this could mean that the water is safe for most uses, including drinking, cooking, bathing, and irrigation. The sensor readings are in the optimal range for all or most of the parameters. For example, the EC (Electrical Conductivity) might be low, indicating low levels of dissolved salts and other inorganic substances. The pH might be close to neutral, which is generally considered optimal for most aquatic life and for human use. The ORP (Oxidation-Reduction Potential) might be high, indicating that the water has a good capacity to break down contaminants. The temperature might be within a suitable range for most aquatic life.

// - **`0.25 <= weightedSum < 0.5` (Yellow)**: This indicates that the water quality is acceptable, but not optimal. In a real-world scenario, this could mean that the water is safe for some uses, such as irrigation, but might not be safe for others, such as drinking or cooking, without treatment. The sensor readings are in the acceptable range for all or most of the parameters, but some readings may be outside the optimal range. For example, the EC might be moderately high, indicating a moderate level of dissolved salts and other inorganic substances. The pH might be slightly acidic or slightly alkaline. The ORP might be moderately high. The temperature might be slightly higher or lower than the optimal range for most aquatic life.

// - **`0.5 <= weightedSum < 0.75` (Orange)**: This indicates that the water quality is poor. In a real-world scenario, this could mean that the water is not safe for many uses without treatment. The sensor readings are outside the optimal range for several parameters, and may be outside the acceptable range for some parameters. For example, the EC might be high, indicating a high level of dissolved salts and other inorganic substances. The pH might be moderately acidic or moderately alkaline. The ORP might be moderately low, indicating that the water has a reduced capacity to break down contaminants. The temperature might be significantly higher or lower than the optimal range for most aquatic life.

// - **`weightedSum >= 0.75` (Red)**: This indicates that the water quality is very poor or potentially hazardous. In a real-world scenario, this could mean that the water is not safe for any use without significant treatment. The sensor readings are outside the acceptable range for most or all of the parameters. For example, the EC might be very high, indicating a very high level of dissolved salts and other inorganic substances. The pH might be highly acidic or highly alkaline. The ORP might be low, indicating that the water has a poor capacity to break down contaminants. The temperature might be extremely higher or lower than the optimal range for most aquatic life.