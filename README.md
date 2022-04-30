# FYP-22
This repository is for Final Year Project. 


![ProjectFlow](https://github.com/sudolmin/FYP-22/blob/main/media/images/FYP-flow-Working%20Flow.drawio.png?raw=true)

# Description
This project can be divided into the following levels:

## Hardware Level

Wemos D1 mini board with ESP8266 WiFi module is connected to two sensors.

![d1mini](https://github.com/sudolmin/FYP-22/blob/main/media/images/d1mini.jpg?raw=true)

1. MQ2 gas sensor - measures Methane, Butane, Smoke in the air

![MQ2](https://github.com/sudolmin/FYP-22/blob/main/media/images/mq2.jpg?raw=true)

2. BMP180 sensor - measures Pressure, Temperature, Altitude

![BMP180](https://github.com/sudolmin/FYP-22/blob/main/media/images/bmp180.jpg?raw=true)

### Connections and workings

![BMP180](https://github.com/sudolmin/FYP-22/blob/main/media/images/FYP-flow-Circuit%20Diagram.drawio.png?raw=true)

The program is written in **d1mini.ino** takes data as follows:
* Air quality readings from MQ2 gas sensor, and
* Room temperature from BMP180 sensor

The data points taken from those sensors are then packed into JSON format data. Finally, the JSON data is published to MQTT server \[ref **MQTT section**] running on the internet.

## Backend Level

### MQTT server
[MQTT](https://mqtt.org/) is an OASIS standard messaging protocol for the Internet of Things (IoT). It is designed as an extremely lightweight publish/subscribe messaging transport ideal for connecting remote devices with a small code footprint and minimal network bandwidth.

For this project, we are using an open-source package [Eclipse Mosquitto](https://mosquitto.org/) provided by Eclipse Foundation.

### InfluxDb and Telegraf
The data communicated through the MQTT server is not retained and will be lost forever. Therefore, we set up [InfluxDb](https://www.influxdata.com/get-influxdb/) a time-series database to store those data coming through the sensors.

The [Telegraf](https://www.influxdata.com/time-series-platform/telegraf/) service is configured, creating a flux between MQTT and InfluxDb, as Input and Output, respectively.

### Node Alert server
This server is built upon [NodeJS](https://nodejs.org/en/). The core functionality of this server is to analyse the real-time data and alert the recipients when the danger level is surpassed.

This server subscribes to two MQTT topics
1. **mq2/calc_data**, for data analysis
2. **mq2/config**, for receiving, decrypting, and saving the front-end configuration data.

The alerts are sent through two methods, namely SMS and E-mail.

## Frontend Level
The frontend is built using [ReactJS Library](https://reactjs.org/). The function of the frontend is to provide real-time data coming from the MQTT.

The frontend subscribes to topic: **mq2/calc_data**to display real-time UI data.
It also provides the user with an interface to input their email address and phone number for alerts. When those details are saved, the components are packed into an encrypted JSON data format and published to the topic: **mq2/config**.

## Team Collaborators
* Umananda Sahu
* Somesh Chaneja
* Ankit Das

