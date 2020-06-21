# Victron MPPT ve.direct to MQTT


Read data from #victron mppt charger and transport to #mqtt server with esp8266 nodemcu v1

Home Assistant dashboard
![alt text](https://github.com/KinDR007/Victron-MPPT-ve.direct-to-MQTT/blob/master/HA.png?raw=true)


![alt text](https://github.com/KinDR007/Victron-MPPT-ve.direct-to-MQTT/blob/master/MQTTExplorerVictronToMQTT.png?raw=true)


Set params in config.h
```
const char* ssid = "SSID";
const char* password = "password_to_your_wifi";
const char* mqtt_server = "ip_adress_to_MQTT_server eg.192.168.1.201";
const char* mqtt_user = ""; //username to mqtt leave blank if you dont use username 
const char* mqtt_pass = ""; //password to mqtt leave blank if you dont use password 

#define OTA_HOSTNAME                    "VictronMPPT" // name of esp8266 in LAN 
#define MQTT_ROOT                       "Victron"
```


Home Assitant example config

```
       #. Victron
     - platform: mqtt
       name: "Victron - Battery voltage"
       state_topic: "Victron/Battery voltage, V"
       unit_of_measurement: "V"
       icon: mdi:mdi-battery
       unique_id: sensor.text.victron.battery.voltage
       
     - platform: mqtt
       name: "Victron - Battery current"
       state_topic: "Victron/Battery current, I"
       unit_of_measurement: "I"
       icon: mdi:mdi-current-dc
       unique_id: sensor.text.victron.panel.current
       
     - platform: mqtt
       name: "Victron PV power"
       state_topic: "Victron/Panel power, W"
       unit_of_measurement: "W"
       icon: mdi:gauge
       unique_id: sensor.text.victron.panel.power
       
     - platform: mqtt
       name: "Victron Panel voltage"
       state_topic: "Victron/Panel voltage"
       unit_of_measurement: "V"  
       unique_id: sensor.text.victron.panel.voltage
       
     - platform: mqtt
       name: "Victron Tracker operation"
       state_topic: "Victron/Tracker operation"
       unique_id: sensor.text.victron.tracker.operation
       icon: mdi:mdi-solar-power

     - platform: mqtt
       name: "Victron Yield today"
       state_topic: "Victron/Yield today, kWh"
       unique_id: sensor.text.victron.yield.today
       icon: mdi:gauge  
       unit_of_measurement: "Kw/h"
       
     - platform: mqtt
       name: "Victron - Maximum power today"
       state_topic: "Victron/Maximum power today, W"
       unique_id: sensor.text.victron.maximum.power.today
       unit_of_measurement: "W"
       icon: mdi:gauge       
       
     - platform: mqtt
       name: "Victron/Yield total, kWh"
       state_topic: "Victron/Yield total, kWh"
       unique_id: sensor.text.victron.yield.total
       icon: mdi:gauge  
       unit_of_measurement: "Kw/h"
       
     - platform: mqtt
       name: "Victron Charge state"
       state_topic: "Victron/Charge state"
       unique_id: sensor.text.victron.charge.state
       icon: mdi:mdi-solar-power
```

#NodeMCU pinout and wirring
![alt text](https://github.com/KinDR007/Victron-MPPT-ve.direct-to-MQTT/blob/master/nodemcu.png?raw=true)



