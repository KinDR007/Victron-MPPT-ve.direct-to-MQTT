/* 
 * copy to config.h and do your settings.
 */

const char* ssid = "SSID";
const char* password = "hidden_password_to_my_wifi";  //password to wifi
const char* mqtt_server = "192.168.0.10";             //ip to mqtt server
const char* mqtt_user = "";                           //mqtt user name
const char* mqtt_pass = "";                           //mqtt password

const int rxPin = 22;                      // RX using Software Serial so we can use the hardware UART to check the ouput
const int txPin = 21;                      // TX Not used

#define OTA_HOSTNAME                    "VictronMPPT"
#define MQTT_ROOT                       "Victron"
