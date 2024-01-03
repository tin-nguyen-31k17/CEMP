# CEMP Mobility Water Monitoring Station Project

## Overview

This project, addressing critical water pollution and scarcity challenges in Vietnam, introduces an innovative, low-cost, mobile water monitoring system. It's designed for easy adaptation to various environments, notably small watercraft. The system combines advanced hardware and software technologies to provide real-time monitoring and pollution detection. This is the source code of my MDT406 implementation.

## System Components

### Atom_Gateway
- **Atom_Gateway.ino**: The main program for the Gateway module, handling data reception from Nodes and forwarding it to the server.
- **MQTT_helper.cpp/h**: Helper functions for managing MQTT connections, essential for data publishing and subscribing.
- **sensor_data.cpp/h**: Manages the processing and handling of sensor data received from the Nodes.

### Atom_Node
- **platformio.ini**: Configuration file for PlatformIO, setting up the environment for the Node module.
- **src/Atom_Node.cpp/h**: Core source code for the Node module, responsible for collecting and sending sensor data.
- **sensor_data.cpp/h**: Handles sensor data processing within the Node.

### Atom_Node_LTE
- **Atom_DTU_CAT1.h**: Handles LTE communication, enabling data transmission in areas lacking traditional network infrastructure.
- **Atom_Node_LTE.cpp/h**: Extension of the Atom Node, including LTE capabilities for wider area communication.
- **MQTT_helper.cpp/h and sensor_data.cpp/h**: Similar to Atom_Node, but with adjustments for LTE communication.

### C_Cam
- **C_Cam.cpp/h**: Source code for M5 StickC, used for visual monitoring, additional data collection from the Unit-V.
- **unitv_src**: Contains firmware and models for the camera module, enabling advanced features for image recognition.

### Client_Dashboard
- A comprehensive Flutter application providing a user interface for monitoring and controlling the system.
- Contains various useful page like Live GPS Tracking, Real-Time Graph, Sensor Control for a rich user experience.
- Supports multiple platforms (android, ios, linux, macos, web, windows), ensuring wide accessibility.

### Core_Station
- **Core_Station.cpp/h**: Central program for the Core Station, handling data from Nodes and providing an nice on-site user interface.
- **MQTT_helper.cpp/h, sensor_data.cpp/h, and tools.h**: Various utilities for handling MQTT communication, sensor data, and other functionalities.

### Icons
- Contains various icons (png, svg) used across the system.

### Tools
- **ics.py, i.h, testpub.py**: contains some utility scripts and headers for system configuration, testing, and debugging.

## Summary

Each component in this structure plays a vital role in the functionality of the CEMP Mobility Water Monitoring Station Project. From data collection (Atom_Node) to transmission (Atom_Gateway, Atom_Node_LTE), and from user interaction (Client_Dashboard) to central processing (Core_Station), these components work together seamlessly. The inclusion of a camera module (C_Cam) and extensive support across platforms for the Client Dashboard highlights the system’s versatility and user-centric design.


## Objectives

- **Real-time Alerts**: Immediate notifications for water pollution incidents.
- **Data Analysis**: Basic analysis algorithms for accurate water quality assessment.
- **User-Friendly Interface**: Easy access to the system’s interface for monitoring and decision-making.
- **Versatile Operation**: Effective functioning in various environmental conditions.
- **Scalability**: Ability to adapt the system to different scales as required.

## Challenges

- **Sensor Management**: Ensuring compatibility and reliability of numerous sensors.
- **Model Development**: Creating robust models for various water bodies and environmental conditions.
- **Network Connectivity**: Establishing stable connections, especially in remote areas.
- **Training Data**: Gathering diverse data for accurate pollution detection.
- **Sensor Integration**: Enhancing real-time monitoring and detection capabilities.

## Usage

- **Deployment**: Ideal for small watercraft, offering ease of adaptation to different environments.
- **Monitoring**: Continuously tracks various water quality parameters.
- **Data Transmission**: Uses IoT for efficient communication and data sharing.
- **User Interaction**: Through the application, users can monitor, control, and respond to data insights.

## Future Enhancements

- Further optimization for performance and efficiency.
- Expansion of sensor network and data processing capabilities.
- Enhancements to user interface and system interactivity.

## Contributing

Contributions are welcome. Feel free to submit pull requests or open issues on the GitHub repository for functionality and performance improvements.

## License

This project is open-source and available under the [MIT License](LICENSE).
