# FYP-22
This repository is for Final Year Project. 

# Description
This project can be divided in following levels:

## Hardware Level

Wemos D1 mini board with ESP8266 WiFi module in-built, is connected to two sensor

1. MQ2 gas sensor - measures Methane, Butane, Smoke in air
2. BMP180 sensor - measures Pressure, Temperature, Altitude

### Connections and workings

The program written in **d1mini.ino** takes data as following:
* Air quality readings from MQ2 gas sensor, and
* Room temperature from BMP180 sensor

Tha datapoints taken from those sensor is then packed into a JSON format data. The JSON data is published to MQTT server \[ref **MQTT section**] running on the internet.

## Backend Level

### MQTT server
[MQTT](https://mqtt.org/) is an OASIS standard messaging protocol for the Internet of Things (IoT). It is designed as an extremely lightweight publish/subscribe messaging transport that is ideal for connecting remote devices with a small code footprint and minimal network bandwidth.

For this project, we are using an open source package [Eclipse Mosquitto](https://mosquitto.org/) provided by Eclipse Foundation.

### InfluxDb and Telegraf
The data communicated through the MQTT sever is not retained and will be lost forever. In order to store those data coming through the sensors, we set up [InfluxDb](https://www.influxdata.com/get-influxdb/), that is a time-series database.

The [Telegraf](https://www.influxdata.com/time-series-platform/telegraf/) service is configured so it creates a flux between MQTT and InfluxDb, as Input and Output respectively.

### Node Alert server
This server is built upon [NodeJS](https://nodejs.org/en/), the core functionailty for this server is to analyse the real time data and alert the recipients, when danger level is surpassed.

This server subscribes to two MQTT topics
1. **mq2/calc_data**, for data analysis
2. **mq2/config**, for recieving, decrypting and saving the configuration data from the frontend.

The alerts are sent through two methods, namely SMS and E-mail

## Frontend Level
The frontend is built using [ReactJS Library](https://reactjs.org/), the function of the frontend is to provide real-time data coming from the MQTT.

The frontend subscribes to topic: **mq2/calc_data**, for displaying real-time data on the UI.
It also provide user with interface to input their email address and phone number for alerts to come through. When those details are saved, the details are packed into a encrypted JSON data format, and published to topic: **mq2/config**.

## Team Collaborators
* Umananda Sahu
* Somesh Chaneja
* Ankit Das

