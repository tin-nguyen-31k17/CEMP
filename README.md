# CEMP Mobility Water Monitoring Station Project

## Overview

This project employs the M5 Atom as a node and another M5 Atom (though theoretically, any M5 ESP32-based device would suffice) as a gateway. For the station, a Core/Core 2/Core S3 can be used to display an appealing UI. This setup is a fusion that extends the functionality and features of two projects: [BKInnovation_codeC](https://github.com/HyHyZhaLee/BKInnovation_codeC) & [M5AirQuality](https://github.com/armel/M5AirQuality/).
The project introduces two communication channels: WiFi for internet connectivity and MQTT server communication, alongside ESPNow for one-way communication between the WaterNode and WaterGateway/WaterStation. It's essential to configure the WiFi router's channel to 0 or 1 to prevent interference with ESPNow's operation.

## Features

1. **ESPNow Communication:** This project implements ESPNow, facilitating one-way communication between the Node and Gateway. The Node transmits sensor data to the Gateway.
2. **GPS & DateTime:** Utilize the M5 AT6558 GPS module to fetch GPS information to the Node device and send to Gateway for tracking purpose
3. **Water Sensor Network:** This project employs various water measuring sensor, like pH, ORP, Temp & EC to determine quality of the water source.
4.  **Offline Backup Data:** Despite the fact that M5 Atom is a very resource-limited device. The program try to handle disconnection from gateway with backup buffer store on SRAM waiting to reconnect and feed data back to the Gateway.
5.  **WiFi Connectivity:** The WaterGateway uses WiFi to connect to the internet and an MQTT server for data publishing. The code includes the necessary WiFi setup.

## Prerequisites

- PlatformIO or Arduino IDE for programming the M5 ESP32 devices.
- In this project I used the M5 Atom Lite for the node/gateway and the M5 Core/Core 2/Core S3 as the station but any ESP32-based MCU device theoretically would work.

## Setup and Configuration

1. Clone or download the project code from [GitHub - CEMP](https://github.com/tin-nguyen-31k17/CEMP).

2. Configure the code for both the WaterNode and WaterGateway/Water by adjusting the WiFi credentials, and other project-specific settings.

3. Ensure that your router's WiFi channel is set to either 0, 1 (for some specific routers, 3 or 5) to prevent interference with ESPNow's operation.

4. Upload the code to your end node and Gateway/Station devices using PlatformIO or the Arduino IDE.

## How It Works

- The Node reads water sensor data, GPS information and sends it to the Gateway using the ESPNow protocol.
- The Gateway connects to the internet via WiFi and publishes the received data to an MQTT server.

## To-Do List

The project is a work in progress, and there are some planned enhancements:

1. **Two-Way Communication:** Implement two-way communication between the WaterNode and WaterGateway, allowing commands and data to be exchanged in both directions.

2. **Channel Auto-Configuration:** The rGateway will have the ability to determine its operating channel and set the Node's channel accordingly to optimize communication.

3. **Power Optimization:** Optimize the code for low-power operation, allowing devices to sleep when not in use or when not connected to each other.

## Contributing

Contributions to this project are welcome. Feel free to submit pull requests or open issues on the GitHub repository to help improve the functionality and performance of the WaterNode and WaterGateway.

## License

This project is open-source and released under the [MIT License](LICENSE).
