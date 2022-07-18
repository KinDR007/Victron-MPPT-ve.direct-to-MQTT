/*************************************************************************************
 ReadVeDirectFrameHandler
 Uses VeDirectFrameHandler library

 This example and library tested with Wemod D1 using Software Serial.
 If using with a platform containing 2 harware UART's, use those, not SoftwareSerial.
 Tested with Victron MPPT 100/30.

 VEDirect Device:
   pin 1 - gnd
   pin 2 - RX
   pin 3 - TX
   pin 4 - not connected

  Be aware that MPPTs use 5V signaling. So you might need a voltage divider for the 3,3 V input pins of the board

 History:
   2022.07.13 - 0.1 - initial release
**************************************************************************************/

#include "config.h"
#include <WiFiUdp.h>
#include <WiFi.h>
#include <mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#include "VeDirectFrameHandler.h"

VeDirectFrameHandler myve;
  

WiFiClient espClient;
PubSubClient client(espClient);


// Function declaration
void PublishData();
void EverySecond();
void ReadVEData();

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OTA_HOSTNAME);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void reconnectWifi() {
  // check wifi
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void setup() {
  Serial.begin(115200);

  Serial2.begin(19200, SERIAL_8N1, rxPin, txPin);
  Serial2.flush();

  // Wait for hardware to initialize
  delay(1000);
  Serial.println("Booting");
 
  initWiFi();

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

  Serial.println("Ready");
}



void reconnectMQTT() {
  // Loop until we're reconnected to mqtt broker
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
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWifi();
  }
  else {
    ArduinoOTA.handle();
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();

    ReadVEData();
    EverySecond();
  }
}

void ReadVEData() {
  while ( Serial2.available() ) {
        myve.rxData(Serial2.read());
    }
    yield();
}

void EverySecond() {
    static unsigned long prev_millis;

    if (millis() - prev_millis > 1000) {
        PublishData();
        prev_millis = millis();
    }
}


void PublishData() {
    String topic = "";
    for ( int i = 0; i < myve.veEnd; i++ ) {
      Serial.print(myve.veName[i]);
      Serial.print("= ");
      Serial.println(myve.veValue[i]);   
      topic = "Victron/";  
      topic.concat(myve.veName[i]);
      topic.replace("#",""); // # is a no go in mqtt topic
      client.publish(topic.c_str(), myve.veValue[i]); 
  }
}
