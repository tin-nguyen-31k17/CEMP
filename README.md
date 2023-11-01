# CEMP Mobility Water Monitoring Station Project

## Overview

This project is based on the code available at [GitHub - BKInnovation_codeC](https://github.com/HyHyZhaLee/BKInnovation_codeC). It extends the functionality to include two communication channels: WiFi for connecting to the internet and MQTT server and ESPNow for one-way communication between the WaterNode and WaterGateway. Please note that the WiFi router's channel should be set to either 0, 1, 3, or 5 to avoid interference with ESPNow's operation.

## Features

1. **ESPNow Communication:** The project implements ESPNow to enable one-way communication between the WaterNode and WaterGateway. The WaterNode sends sensor data to the WaterGateway.
   
2. **WiFi Connectivity:** The WaterGateway uses WiFi to connect to the internet and an MQTT server for data publishing. The code includes the necessary WiFi setup.

## Prerequisites

- PlatformIO or Arduino IDE for programming the ESP32 devices.
- An ESP32-based MCU, in this project I used the M5 Atom Lite for both the node and the gateway.

## Setup and Configuration

1. Clone or download the project code from here [GitHub - CEMP](https://github.com/tin-nguyen-31k17/CEMP).

2. Configure the code for both the WaterNode and WaterGateway by adjusting the WiFi credentials and other project-specific settings.

3. Ensure that your router's WiFi channel is set to either 0, 1 for some specific router is 3 or 5 (tested) to prevent interference with ESPNow's operation.

4. Upload the code to your WaterNode and WaterGateway devices using PlatformIO or the Arduino IDE.

## How It Works

- The WaterNode reads sensor data and sends it to the WaterGateway using the ESPNow protocol.
- The WaterGateway connects to the internet via WiFi and publishes the received data to an MQTT server.

## To-Do List

The project is a work in progress, and there are some planned enhancements:

1. **Two-Way Communication:** Implement two-way communication between the WaterNode and WaterGateway, allowing commands and data to be exchanged in both directions.

2. **Channel Auto-Configuration:** The WaterGateway will have the ability to determine its operating channel and set the WaterNode's channel accordingly to optimize communication.

3. **Power Optimization:** Optimize the code for low-power operation, allowing devices to sleep when not in use or when not connected to each other.

## Contributing

Contributions to this project are welcome. Feel free to submit pull requests or open issues on the GitHub repository to help improve the functionality and performance of the WaterNode and WaterGateway.

## License

This project is open-source and released under the [MIT License](LICENSE).