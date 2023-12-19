#ifndef INC_SENSOR_DATA_H_
#define INC_SENSOR_DATA_H_

#include <Arduino.h>

class ImageDataHandler {
public:
    ImageDataHandler();
    ~ImageDataHandler();

    // Method to process image data (e.g., conversion, analysis)
    // This method can be expanded or modified based on your specific requirements
    void processImageData(uint8_t* imageData, uint32_t imageLength);
};

#endif // INC_SENSOR_DATA_H_
