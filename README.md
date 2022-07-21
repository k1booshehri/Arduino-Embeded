## Project description (Embedded systems course, final project):
### Strain gauge:
Load cells are actually based on the fact that they use different configurations of Wheatstone bridges with strain gauges and the HX711 module only amplifies and discretizes the difference of 
potential generated in the bridge caused by the mechanical deformation.
<br/>
So a strain gauge is placed in the patient's mask filter which will change the voltage after each exhale.
### Temperature and Humidity:
The DHT22 sensor is connected to Arduino UNO. Arduino UNO connects to the WiFi network using ESP8266. Arduino UNO pushes data to the server via MQTT protocol by using PubSubClient library for Arduino.
The application that is running on Arduino UNO is written using Arduino SDK which is quite simple and easy to understand.
