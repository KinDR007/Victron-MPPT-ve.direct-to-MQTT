# Victron-MPPT-ve.direct-to-MQTT
read data from victron mppt a transport to mqtt server with esp8266 nodemcu v1

and in HA config

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
```
