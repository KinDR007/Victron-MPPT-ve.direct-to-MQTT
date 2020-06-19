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
  Serial.begin(115200);

  Vser.begin(19200);

  // Wait for hardware to initialize
  delay(1000);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OTA_HOSTNAME);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
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
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Victron", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.publish("Victron/Live", "1");
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

    if (label == "PID") {


      String WorkingString = val;
      long A = strtol(WorkingString.c_str(), NULL, 0);
      WorkingString = String(A);

      switch (A) {
        case 0x203 : client.publish("Victron/PID", "BMV-700");  break;
        case 0x204 : client.publish("Victron/PID", "BMV-702");  break;
        case 0x205 : client.publish("Victron/PID", "BMV-700H");  break;
        case 0xA381 : client.publish("Victron/PID", "BMV-712 Smart");  break;
        case 0xA04C : client.publish("Victron/PID", "BlueSolar MPPT 75/10");  break;
        case 0x300 : client.publish("Victron/PID", "BlueSolar MPPT 70/15");  break;
        case 0xA042 : client.publish("Victron/PID", "BlueSolar MPPT 75/15");  break;
        case 0xA043 : client.publish("Victron/PID", "BlueSolar MPPT 100/15");  break;
        case 0xA044 : client.publish("Victron/PID", "BlueSolar MPPT 100/30 rev1");  break;
        case 0xA04A : client.publish("Victron/PID", "BlueSolar MPPT 100/30 rev2");  break;
        case 0xA041 : client.publish("Victron/PID", "BlueSolar MPPT 150/35 rev1");  break;
        case 0xA04B : client.publish("Victron/PID", "BlueSolar MPPT 150/35 rev2");  break;
        case 0xA04D : client.publish("Victron/PID", "BlueSolar MPPT 150/45");  break;
        case 0xA040 : client.publish("Victron/PID", "BlueSolar MPPT 75/50");  break;
        case 0xA045 : client.publish("Victron/PID", "BlueSolar MPPT 100/50 rev1");  break;
        case 0xA049 : client.publish("Victron/PID", "BlueSolar MPPT 100/50 rev2");  break;
        case 0xA04E : client.publish("Victron/PID", "BlueSolar MPPT 150/60");  break;
        case 0xA046 : client.publish("Victron/PID", "BlueSolar MPPT 150/70");  break;
        case 0xA04F : client.publish("Victron/PID", "BlueSolar MPPT 150/85");  break;
        case 0xA047 : client.publish("Victron/PID", "BlueSolar MPPT 150/100");  break;
        case 0xA050 : client.publish("Victron/PID", "SmartSolar MPPT 250/100");  break;
        case 0xA051 : client.publish("Victron/PID", "SmartSolar MPPT 150/100");  break;
        case 0xA052 : client.publish("Victron/PID", "SmartSolar MPPT 150/85");  break;
        case 0xA053 : client.publish("Victron/PID", "SmartSolar MPPT 75/15");  break;
        case 0xA054 : client.publish("Victron/PID", "SmartSolar MPPT 75/10");  break;
        case 0xA055 : client.publish("Victron/PID", "SmartSolar MPPT 100/15");  break;
        case 0xA056 : client.publish("Victron/PID", "SmartSolar MPPT 100/30");  break;
        case 0xA057 : client.publish("Victron/PID", "SmartSolar MPPT 100/50");  break;
        case 0xA058 : client.publish("Victron/PID", "SmartSolar MPPT 150/35");  break;
        case 0xA059 : client.publish("Victron/PID", "SmartSolar MPPT 150/100 rev2");  break;
        case 0xA05A : client.publish("Victron/PID", "SmartSolar MPPT 150/85 rev2");  break;
        case 0xA05B : client.publish("Victron/PID", "SmartSolar MPPT 250/70");  break;
        case 0xA05C : client.publish("Victron/PID", "SmartSolar MPPT 250/85");  break;
        case 0xA05D : client.publish("Victron/PID", "SmartSolar MPPT 250/60");  break;
        case 0xA05E : client.publish("Victron/PID", "SmartSolar MPPT 250/45");  break;
        case 0xA05F : client.publish("Victron/PID", "SmartSolar MPPT 100/20");  break;
        case 0xA060 : client.publish("Victron/PID", "SmartSolar MPPT 100/20 48V");  break;
        case 0xA061 : client.publish("Victron/PID", "SmartSolar MPPT 150/45");  break;
        case 0xA062 : client.publish("Victron/PID", "SmartSolar MPPT 150/60");  break;
        case 0xA063 : client.publish("Victron/PID", "SmartSolar MPPT 150/70");  break;
        case 0xA064 : client.publish("Victron/PID", "SmartSolar MPPT 250/85 rev2");  break;
        case 0xA065 : client.publish("Victron/PID", "SmartSolar MPPT 250/100 rev2");  break;
        case 0xA201 : client.publish("Victron/PID", "Phoenix Inverter 12V 250VA 230V");  break;
        case 0xA202 : client.publish("Victron/PID", "Phoenix Inverter 24V 250VA 230V");  break;
        case 0xA204 : client.publish("Victron/PID", "Phoenix Inverter 48V 250VA 230V");  break;
        case 0xA211 : client.publish("Victron/PID", "Phoenix Inverter 12V 375VA 230V");  break;
        case 0xA212 : client.publish("Victron/PID", "Phoenix Inverter 24V 375VA 230V");  break;
        case 0xA214 : client.publish("Victron/PID", "Phoenix Inverter 48V 375VA 230V");  break;
        case 0xA221 : client.publish("Victron/PID", "Phoenix Inverter 12V 500VA 230V");  break;
        case 0xA222 : client.publish("Victron/PID", "Phoenix Inverter 24V 500VA 230V");  break;
        case 0xA224 : client.publish("Victron/PID", "Phoenix Inverter 48V 500VA 230V");  break;
        case 0xA231 : client.publish("Victron/PID", "Phoenix Inverter 12V 250VA 230V");  break;
        case 0xA232 : client.publish("Victron/PID", "Phoenix Inverter 24V 250VA 230V");  break;
        case 0xA234 : client.publish("Victron/PID", "Phoenix Inverter 48V 250VA 230V");  break;
        case 0xA239 : client.publish("Victron/PID", "Phoenix Inverter 12V 250VA 120V");  break;
        case 0xA23A : client.publish("Victron/PID", "Phoenix Inverter 24V 250VA 120V");  break;
        case 0xA23C : client.publish("Victron/PID", "Phoenix Inverter 48V 250VA 120V");  break;
        case 0xA241 : client.publish("Victron/PID", "Phoenix Inverter 12V 375VA 230V");  break;
        case 0xA242 : client.publish("Victron/PID", "Phoenix Inverter 24V 375VA 230V");  break;
        case 0xA244 : client.publish("Victron/PID", "Phoenix Inverter 48V 375VA 230V");  break;
        case 0xA249 : client.publish("Victron/PID", "Phoenix Inverter 12V 375VA 120V");  break;
        case 0xA24A : client.publish("Victron/PID", "Phoenix Inverter 24V 375VA 120V");  break;
        case 0xA24C : client.publish("Victron/PID", "Phoenix Inverter 48V 375VA 120V");  break;
        case 0xA251 : client.publish("Victron/PID", "Phoenix Inverter 12V 500VA 230V");  break;
        case 0xA252 : client.publish("Victron/PID", "Phoenix Inverter 24V 500VA 230V");  break;
        case 0xA254 : client.publish("Victron/PID", "Phoenix Inverter 48V 500VA 230V");  break;
        case 0xA259 : client.publish("Victron/PID", "Phoenix Inverter 12V 500VA 120V");  break;
        case 0xA25A : client.publish("Victron/PID", "Phoenix Inverter 24V 500VA 120V");  break;
        case 0xA25C : client.publish("Victron/PID", "Phoenix Inverter 48V 500VA 120V");  break;
        case 0xA261 : client.publish("Victron/PID", "Phoenix Inverter 12V 800VA 230V");  break;
        case 0xA262 : client.publish("Victron/PID", "Phoenix Inverter 24V 800VA 230V");  break;
        case 0xA264 : client.publish("Victron/PID", "Phoenix Inverter 48V 800VA 230V");  break;
        case 0xA269 : client.publish("Victron/PID", "Phoenix Inverter 12V 800VA 120V");  break;
        case 0xA26A : client.publish("Victron/PID", "Phoenix Inverter 24V 800VA 120V");  break;
        case 0xA26C : client.publish("Victron/PID", "Phoenix Inverter 48V 800VA 120V");  break;
        case 0xA271 : client.publish("Victron/PID", "Phoenix Inverter 12V 1200VA 230V");  break;
        case 0xA272 : client.publish("Victron/PID", "Phoenix Inverter 24V 1200VA 230V");  break;
        case 0xA274 : client.publish("Victron/PID", "Phoenix Inverter 48V 1200VA 230V");  break;
        case 0xA279 : client.publish("Victron/PID", "Phoenix Inverter 12V 1200VA 120V");  break;
        case 0xA27A : client.publish("Victron/PID", "Phoenix Inverter 24V 1200VA 120V");  break;
        case 0xA27C : client.publish("Victron/PID", "Phoenix Inverter 48V 1200VA 120V");  break;
        default:
          client.publish("Victron/PID", "model not detected ");
      }
    }
    else if (label == "I")
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
      if (floatValue > 9) {
        client.publish("Victron/Battery voltage, V", char_current);
      }
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
      switch (intValue) {
        case 0 : client.publish("Victron/Tracker operation", "Off"); break;
        case 1 : client.publish("Victron/Tracker operation", "Limited"); break;
        case 2 : client.publish("Victron/Tracker operation", "Active"); break;
        default:
          client.publish("Victron/Tracker operation", "Tracker operation not detected  !! ");
      }

    }
    else if (label == "ERR")                               // This routine reads the error code.
    {

      val.toCharArray(buf, sizeof(buf));
      intValue = atoi(buf);
      switch (intValue) {
        case 0: client.publish("Victron/Error code", "No error"); break;
        case 2: client.publish("Victron/Error code", "Battery voltage too high"); break;
        case 17: client.publish("Victron/Error code", "Charger temperature too high"); break;
        case 18: client.publish("Victron/Error code", "Charger over current"); break;
        case 19: client.publish("Victron/Error code", "Charger current reversed"); break;
        case 20: client.publish("Victron/Error code", "Bulk time limit exceeded"); break;
        case 21: client.publish("Victron/Error code", "Current sensor issue"); break;
        case 26: client.publish("Victron/Error code", "Terminals overheated"); break;
        case 28: client.publish("Victron/Error code", "Converter issue"); break;
        case 33: client.publish("Victron/Error code", "Input voltage too high (solar panel)"); break;
        case 34: client.publish("Victron/Error code", "Input current too high (solar panel)"); break;
        case 38: client.publish("Victron/Error code", "Input shutdown (excessive battery voltage)"); break;
        case 39: client.publish("Victron/Error code", "Input shutdown (due to current flow during off mode)"); break;
        case 65: client.publish("Victron/Error code", "Lost communication with one of devices"); break;
        case 66: client.publish("Victron/Error code", "Synchronised charging device configuration issue"); break;
        case 67: client.publish("Victron/Error code", "BMS connection lost"); break;
        case 68: client.publish("Victron/Error code", "Network misconfigured"); break;
        case 116: client.publish("Victron/Error code", "Factory calibration data lost"); break;
        case 117: client.publish("Victron/Error code", "Invalid/incompatible firmware"); break;
        case 119: client.publish("Victron/Error code", "User settings invalid"); break;
        default:
          client.publish("Victron/Error code", "ERROR CODE not detected  !! ");
      }

    }
    else if (label == "CS")                                  //  Charge Status
    {

      val.toCharArray(buf, sizeof(buf));
      intValue = atoi(buf);
      switch (intValue) {
        case 0 : client.publish("Victron/Charge state", "Off"); break;
        case 2 : client.publish("Victron/Charge state", "Fault"); break;
        case 3 : client.publish("Victron/Charge state", "Bulk"); break;
        case 4 : client.publish("Victron/Charge state", "Absorption"); break;
        case 5 : client.publish("Victron/Charge state", "Float"); break;
        case 7 : client.publish("Victron/Charge state", "Equalize (manual)"); break;
        case 245 : client.publish("Victron/Charge state", "Starting-up"); break;
        case 247 : client.publish("Victron/Charge state", "Auto equalize / Recondition"); break;
        case 252 : client.publish("Victron/Charge state", "External control"); break;
        default:
          client.publish("Victron/Charge state", "Charge state not detected  !! ");
      }
    }
  }
}
