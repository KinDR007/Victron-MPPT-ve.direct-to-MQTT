/*
  PID  0xA043      -- Product ID for BlueSolar MPPT 100/15
  FW  119     -- Firmware version of controller, v1.19
  SER#  HQXXXXXXXXX   -- Serial number
  V 13790     -- Battery voltage, mV
  I -10     -- Battery current, mA
  VPV 15950     -- Panel voltage, mV
  PPV 0     -- Panel power, W
  CS  5     -- Charge state, 0 to 9
  ERR 0     -- Error code, 0 to 119
  LOAD  ON      -- Load output state, ON/OFF
  IL  0     -- Load current, mA
  H19 0       -- Yield total, kWh
  H20 0     -- Yield today, kWh
  H21 397     -- Maximum power today, W
  H22 0       -- Yield yesterday, kWh
  H23 0     -- Maximum power yesterday, W
  HSDS  0     -- Day sequence number, 0 to 365
  Checksum  l:A0002000148   -- Message checksum
*/
#include "config.h"
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial Vser(13, 15); // RX | TX on nodemcu



WiFiClient espClient;
PubSubClient client(espClient);


//String value;
String label, val;
char mptt_location[16];
float floatValue;
char buf[45];
char char_current[6];
char panel_power[6];
char laadstatus[12];
char prod_yesterday[6];
char max_power_h[6];
char prod_today[6];
byte len = 12;
int intValue;



void setup() {
 // Serial.begin(19200);

  Vser.begin(19200);

  // Wait for hardware to initialize
  delay(1000);
  // Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OTA_HOSTNAME);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //  Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
 //   Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
 //   Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
 //   Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
 //   Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883);
  client.publish("Victron/Live", "0");


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
//    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Victron", mqtt_user, mqtt_pass)) {
//      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (Vser.available())
  {
    label = Vser.readStringUntil('\t');                // this is the actual line that reads the label from the MPPT controller
    val = Vser.readStringUntil('\r\r\n');              // this is the line that reads the value of the label
    char charBufL[label.length() + 1];
    label.toCharArray(charBufL, label.length() + 1);

    if (label == "I")                                       
    { // In this case I chose to read charging current
      val.toCharArray(buf, sizeof(buf));                    
      float floatValue = atof(buf);                         
      floatValue = floatValue / 1000;                      
      dtostrf(floatValue, len, 2, char_current);              
      client.publish("Victron/Battery current, I", char_current);                  
    }
    else if (label == "V")
    {
      val.toCharArray(buf, sizeof(buf));
      float floatValue = atof(buf);
      floatValue = floatValue / 1000;
      dtostrf(floatValue, len, 2, char_current);
      client.publish("Victron/Battery voltage, V", char_current);                 
    }
    else if (label == "PPV")
    {
      val.toCharArray(buf, sizeof(buf));                   
      floatValue = atof(buf);                               
      dtostrf(floatValue, len, 0, panel_power);          
      panel_power[len] = ' '; panel_power[len + 1] = 0;
      client.publish("Victron/Panel power, W", panel_power);                  
    }
    else if (label == "VPV") //Solar
    {
      val.toCharArray(buf, sizeof(buf));                    
      float floatValue = atof(buf);                         
      floatValue = floatValue / 1000;                       
      dtostrf(floatValue, len, 2, char_current);              
      client.publish("Victron/Panel voltage", char_current);                  
    }
    else if (label == "H20")
    {
      val.toCharArray(buf, sizeof(buf));
      float floatValue = atof(buf);
      floatValue = floatValue / 100;
      dtostrf(floatValue, len, 2, prod_today);
      prod_today[len] = ' '; prod_today[len + 1] = 0;
      client.publish("Victron/Yield today, kWh", prod_today);                 
    }
    else if (label == "H22")                                                                      //Yield yesterday, kWh
    {
      val.toCharArray(buf, sizeof(buf));
      float floatValue = atof(buf);
      floatValue = floatValue / 100;
      dtostrf(floatValue, len, 2, prod_yesterday);
      prod_yesterday[len] = ' '; prod_yesterday[len + 1] = 0;
      client.publish("Victron/Yield yesterday, kWh", prod_yesterday);                 
    }
    else if (label == "H19")                                                                        //-- Yield total, kWh
    {
      val.toCharArray(buf, sizeof(buf));
      float floatValue = atof(buf);
      floatValue = floatValue / 100;
      dtostrf(floatValue, len, 2, prod_yesterday);
      prod_yesterday[len] = ' '; prod_yesterday[len + 1] = 0;
      client.publish("Victron/Yield total, kWh", prod_yesterday);                 
    }
    
    else if (label == "H21")                                                                        //Maximum power today, W
    {
      val.toCharArray(buf, sizeof(buf));
      floatValue = atof(buf);
      dtostrf(floatValue, len, 0, max_power_h);
      max_power_h[len] = ' '; max_power_h[len + 1] = 0;
      client.publish("Victron/Maximum power today, W", max_power_h);                  

    }
    else if (label == "H23")                                                                          //Maximum power yesterday, W
    {
      val.toCharArray(buf, sizeof(buf));
      floatValue = atof(buf);
      dtostrf(floatValue, len, 0, max_power_h);
      max_power_h[len] = ' '; max_power_h[len + 1] = 0;
      client.publish("Victron/Maximum power yesterday, W", max_power_h);                 

    }
    else if (label == "FW")                                                                           //FW 119     -- Firmware version of controller, v1.19
    {
      val.toCharArray(buf, sizeof(buf));
      float floatValue = atof(buf);
      floatValue = floatValue / 100;
      dtostrf(floatValue, len, 2, prod_today);
      prod_today[len] = ' '; prod_today[len + 1] = 0;
      client.publish("Victron/Firmware version", prod_today);                  

    }
    else if (label == "HSDS")                                                                         //Day sequence number (0..364)
    {
      val.toCharArray(buf, sizeof(buf));
      floatValue = atof(buf);
      dtostrf(floatValue, len, 0, panel_power);
      panel_power[len] = ' '; panel_power[len + 1] = 0;
      client.publish("Victron/Day sequence number", panel_power);

    }
    else if (label == "MPPT")
    {
      val.toCharArray(buf, sizeof(buf));
      intValue = atof(buf);
      if (intValue == 0)
      {
        client.publish("Victron/Tracker operation", "Off");
      }
      else if (intValue == 1)
      {
        client.publish("Victron/Tracker operation", "Limited");
      }
      else if (intValue == 2)
      {
        client.publish("Victron/Tracker operation", "Active");
      }

    }
    else if (label == "ERR")                               // This routine reads the error code.
    {

      val.toCharArray(buf, sizeof(buf));
      intValue = atoi(buf);
      if (intValue == 0)
      {
        client.publish("Victron/Error code", "No error");
      }
      else if (intValue == 2)
      {
        client.publish("Victron/Error code", "Battery voltage too high"); //'2': 'Battery voltage too high',
      }
      else if (intValue == 17)  // '17': 'Charger temperature too high',
      {
        client.publish("Victron/Error code", "Charger temperature too high");
      }
      else if (intValue == 18)  //'18': 'Charger over current',
      {
        client.publish("Victron/Error code", "Charger over current");
      }
      else if (intValue == 19)  // '19': 'Charger current reversed',
      {
        client.publish("Victron/Error code", "Charger current reversed");
      }
      else if (intValue == 20)  //'20': 'Bulk time limit exceeded',
      {
        client.publish("Victron/Error code", "Bulk time limit exceeded");
      }
      else if (intValue == 21)  //     '21': 'Current sensor issue',
      {
        client.publish("Victron/Error code", "Current sensor issue");
      }
      else if (intValue == 26)  //'26': 'Terminals overheated',
      {
        client.publish("Victron/Error code", "Terminals overheated");
      }
      else if (intValue == 28)  // '28': 'Converter issue',  # (dual converter models only)
      {
        client.publish("Victron/Error code", "Converter issue");
      }
      else if (intValue == 33)  //  '33': 'Input voltage too high (solar panel)',
      {
        client.publish("Victron/Error code", "Input voltage too high (solar panel)");
      }
      else if (intValue == 34)  //   '34': 'Input current too high (solar panel)',
      {
        client.publish("Victron/Error code", "Input current too high (solar panel)");
      }
      else if (intValue == 38)  //    '38': 'Input shutdown (excessive battery voltage)',
      {
        client.publish("Victron/Error code", "Input shutdown (excessive battery voltage)");
      }
      else if (intValue == 39)  //        '39': 'Input shutdown (due to current flow during off mode)',
      {
        client.publish("Victron/Error code", "Input shutdown (due to current flow during off mode)");
      }
      else if (intValue == 65)  //        '65': 'Lost communication with one of devices',
      {
        client.publish("Victron/Error code", "Lost communication with one of devices");
      }
      else if (intValue == 66)  //     '66': 'Synchronised charging device configuration issue',
      {
        client.publish("Victron/Error code", "Synchronised charging device configuration issue");
      }
      else if (intValue == 67)  //    '67': 'BMS connection lost',
      {
        client.publish("Victron/Error code", "BMS connection lost");
      }
      else if (intValue == 68)  //     '68': 'Network misconfigured',
      {
        client.publish("Victron/Error code", "Network misconfigured");
      }
      else if (intValue == 116)  //    '116': 'Factory calibration data lost',
      {
        client.publish("Victron/Error code", "Factory calibration data lost");
      }
      else if (intValue == 117)  //        '117': 'Invalid/incompatible firmware',
      {
        client.publish("Victron/Error code", "Invalid/incompatible firmware");
      }
      else if (intValue == 119)  //    '119': 'User settings invalid'
      {
        client.publish("Victron/Error code", "User settings invalid");
      }

    }
    else if (label == "CS")                                  //  Charge Status
    {

      val.toCharArray(buf, sizeof(buf));
      intValue = atoi(buf);
      if (intValue == 0)
      {
        client.publish("Victron/Charge state", "Off");
      }
      else if (intValue == 2)
      {
        client.publish("Victron/Charge state", "Fault");
      }
      else if (intValue == 3)
      {
        client.publish("Victron/Charge state", "Bulk");
      }
      else if (intValue == 4)
      {
        client.publish("Victron/Charge state", "Absorption");
      }
      else if (intValue == 5)
      {
        client.publish("Victron/Charge state", "Float");
      }
      else if (intValue == 7)
      {
        client.publish("Victron/Charge state", "Equalize (manual)");
      }
      else if (intValue == 245)
      {
        client.publish("Victron/Charge state", "Starting-up");
      }
      else if (intValue == 247)
      {
        client.publish("Victron/Charge state", "Auto equalize / Recondition");
      }
      else if (intValue == 252)
      {
        client.publish("Victron/Charge state", "External control");
      }

    }
  }

}
