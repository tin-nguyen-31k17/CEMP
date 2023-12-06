import paho.mqtt.publish as publish
import json
import random
import time

# MQTT Server Information
server_uri = "mqttserver.tk"
port = 1883
client_id = "innovation"
username = "innovation"
password = "Innovation_RgPQAZoA5N"
topic = "/innovation/watermonitoring/"

# Function to generate random sensor values
def generate_random_values():
    return {
        "station_id": "water_0001",
        "station_name": "WATER 0001",
        "gps_longitude": "106.00",
        "gps_latitude": "10.00",
        "sensors": [
            {"sensor_id": "ec_0001", "sensor_name": "EC 0001", "sensor_value": str(random.uniform(80.0, 100.0)), "sensor_unit": "ms/cm"},
            {"sensor_id": "ph_0001", "sensor_name": "PH 0001", "sensor_value": str(random.uniform(6.0, 8.0)), "sensor_unit": "pH"},
            {"sensor_id": "ORP_0001", "sensor_name": "ORP 0001", "sensor_value": str(random.uniform(300.0, 500.0)), "sensor_unit": "mV"},
            {"sensor_id": "TEMP_0001", "sensor_name": "Nhiệt Độ", "sensor_value": str(random.uniform(5.0, 10.0)), "sensor_unit": "°C"},
        ]
    }

# MQTT Connection Parameters
mqtt_auth = {'username': username, 'password': password}

# Publish random values every 10 seconds
while True:
    try:
        mqtt_message = generate_random_values()
        publish.single(topic, mqtt_message, hostname=server_uri, port=port, client_id=client_id, auth=mqtt_auth)
        print("Published:", mqtt_message)
        time.sleep(10)
    except Exception as e:
        print("Error:", e)
