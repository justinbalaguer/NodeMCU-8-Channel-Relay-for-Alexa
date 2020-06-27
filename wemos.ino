#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// prototypes
boolean connectWifi();

//on/off callbacks 
bool officeLightsOn();
bool officeLightsOff();
bool fanOn();
bool fanOff();
bool allLightsOn();
bool allLightsOff();

// Change this before you flash
const char* ssid = "TP-LINK_E22D34";
const char* password = "";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
Switch *fan = NULL;
Switch *all = NULL;

bool isOfficeLightsOn = false;
bool isfanOn = false;
bool isAllLightstsOn = false;

#define RELAY_1 D1
#define RELAY_2 D2


void setup()
{
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    office = new Switch("office light", 80, officeLightsOn, officeLightsOff);
    fan = new Switch("fan", 81, fanOn, fanOff);
    all = new Switch("all device", 82, allLightsOn, allLightsOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*office);
    upnpBroadcastResponder.addDevice(*fan);
    upnpBroadcastResponder.addDevice(*all);

    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(RELAY_2, HIGH);
  }
}
 
void loop()
{
   if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      fan->serverLoop();
      office->serverLoop();
      all->serverLoop();
   }
}

bool officeLightsOn() {
    Serial.println("Switch 1 turn on ...");
    digitalWrite(RELAY_1, LOW);
    isOfficeLightsOn = true;    
    return isOfficeLightsOn;
}

bool officeLightsOff() {
    Serial.println("Switch 1 turn off ...");
    digitalWrite(RELAY_1, HIGH);
    isOfficeLightsOn = false;
    return isOfficeLightsOn;
}

bool fanOn() {
    Serial.println("Switch 2 turn on ...");
    digitalWrite(RELAY_2, LOW);
    isfanOn = true;
    return isfanOn;
}

bool fanOff() {
  Serial.println("Switch 2 turn off ...");
  digitalWrite(RELAY_2, HIGH);
  isfanOn = false;
  return isfanOn;
}

bool allLightsOn() {
    Serial.println("Switch 2 turn on ...");
    digitalWrite(RELAY_1, LOW);
    digitalWrite(RELAY_2, LOW);
    isAllLightstsOn = true;
    return isAllLightstsOn;
}

bool allLightsOff() {
  Serial.println("Switch 2 turn off ...");
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  isAllLightstsOn = false;
  return isAllLightstsOn;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
