# CEMP Mobility Water Monitoring Station Project

## Overview

This project utilizes the M5 Atom as a node, another M5 Atom (theoretically any M5 ESP32 based device would work) as a gateway and for the station, a Core/Core 2/Core S3 can be utilize to display nice UI. It's a combination, extending functionality and features of 2 projects [BKInnovation_codeC](https://github.com/HyHyZhaLee/BKInnovation_codeC) & [M5AirQuality](https://github.com/armel/M5AirQuality/). The project introduces two communication channels: WiFi for internet connectivity for MQTT server communication, as well as ESPNow (one-way communication) between the WaterNode and WaterGateway/WaterStation. Please note that it's crucial to configure the WiFi router's channel to 0/1 to prevent interference with ESPNow's operation.

## Features

1. **ESPNow Communication:** This project implements ESPNow, facilitating one-way communication between the WaterNode and WaterGateway. The WaterNode transmits sensor data to the WaterGateway.

2. **WiFi Connectivity:** The WaterGateway uses WiFi to connect to the internet and an MQTT server for data publishing. The code includes the necessary WiFi setup.

## Prerequisites

- PlatformIO or Arduino IDE for programming the M5 ESP32 devices.
- In this project I used the M5 Atom Lite for the node/gateway and the M5 Core/Core 2/Core S3 as the station but any ESP32-based MCU device theoretically would work.

## Setup and Configuration

1. Clone or download the project code from [GitHub - CEMP](https://github.com/tin-nguyen-31k17/CEMP).

2. Configure the code for both the WaterNode and WaterGateway/Water by adjusting the WiFi credentials, and other project-specific settings.

3. Ensure that your router's WiFi channel is set to either 0, 1 (for some specific routers, 3 or 5) to prevent interference with ESPNow's operation.

4. Upload the code to your WaterNode and WaterGateway/WaterStation devices using PlatformIO or the Arduino IDE.

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
