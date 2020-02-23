/*
 * This is a simple no frills switch firmware for the Simple basic which will allow you to 
 * interface with the switch using a simple HTTP interface.  You can then use the Simple with your 
 * own home automation hub by creating a dummy switch that calls either "/on" or "/off".
 * 
 * Written By: John Rogers
 * Date: 9/14/17
 * 
 * Revision: 2/23/2020 - to include the use of wifimanager for easy wifi setup without needing to hard code
 *                       wifi creds and IP.
 * 
 * License: GPLv3
 */


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <EEPROM.h>
/*
IPAddress ip(10, 2, 50, 11);        // This section if you want hard-coded IP
IPAddress dns(10, 0, 1, 10);
IPAddress gateway(10, 2, 50, 1);
IPAddress subnet(255, 255, 255, 0);
*/
const int RELAY_PIN = 12; //RELAY on D6 or GPIO 12
const int STAT_PIN = 13; // status on D7 or GPIO 13   status indicates whether or not wifi is connected
int state = 0;
int lastState;
String response;  // we build the string that creates the web UI based on selection
String mac;
 
//const char* ssid = "SSID";                 // This section if you want hard-coded
//const char* password = "P@ssword";         // This section if you want hard-coded

MDNSResponder mdns;
ESP8266WebServer server(80);

void handleRoot() {
  showstate();
  server.send(200, "text/html", response);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void){
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STAT_PIN, OUTPUT);
  Serial.begin(115200);
  //WiFi.mode(WIFI_STA);
  //WiFi.config(ip, dns, gateway, subnet);    // This section if you want hard-coded IP
  //WiFi.begin(ssid, password);                // This section if you want hard-coded creds
  WiFiManager wifiManager;
  wifiManager.autoConnect("SimpleSwitch");
  Serial.println("");
  Serial.print("MAC: ");
  mac = WiFi.macAddress();
  Serial.println(mac);
  EEPROM.begin(8);

  // Wait for connection
  //while (WiFi.status() != WL_CONNECTED) {    // This section if you want hard-coded
   // delay(500);
   // Serial.print(".");
  //}

  if (WiFi.status() == WL_CONNECTED) {
    delay(500);
    digitalWrite(STAT_PIN, LOW);  // reverse the HIGH and LOW on these if using a regular ESP8266 module - GPIO 13 is pulled UP on the Simple (the green LED)
  } else {
    delay(500);
    digitalWrite(STAT_PIN, HIGH);
  }

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  
  server.on("/on", [](){
  state = 1;
  setting();
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
  });
  
  server.on("/off", [](){
  state = 0;
  setting();
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void setting(){
  if( EEPROM.read(0) != state ){
    EEPROM.write(0, state);
    EEPROM.commit();
  }
}

void showstate(){
  if( EEPROM.read(0) == 1 ){
    response = "<center>"
               "<h1>Simple Wireless Switch</h1>"
               "<table>"
               "<tr>"
               "<td colspan='2'>Device MAC: " + mac + "</td>"
               "</tr><tr>"
               "<td><a href=\"on\"><button>ON</button></a></td>"
               "<td><a href=\"off\"><button>OFF</button></a></td>"
               "</tr><tr>"
               "<td align='center' style='color:green'><b>SWITCH IS ON</b></td>"
               "</tr>"
               "</table>"
               "</center>";
  } else if ( EEPROM.read(0) == 0 ){
    response = "<center>"
               "<h1>Simple Wireless Switch</h1>"
               "<table>"
               "<tr>"
               "<td colspan='2'>Device MAC: " + mac + "</td>"
               "</tr><tr>"
               "<td><a href=\"on\"><button>ON</button></a></td>"
               "<td><a href=\"off\"><button>OFF</button></a></td>"
               "</tr><tr>"
               "<td align='center' style='color:red'><b>SWITCH IS OFF</b></td>"
               "</tr>"
               "</table>"
               "</center>";
  }
}

//void startWifi(void) {
//  WiFi.begin(ssid, password);       // Use these if not using wifimanager and hard coding everything
//}
 
void loop(void){

  showstate();
  lastState = EEPROM.read(0);

    if (lastState == 1) {
      digitalWrite(RELAY_PIN, HIGH);
    } else if (lastState == 0) {
      digitalWrite(RELAY_PIN, LOW);
    }

  //connect wifi if not connected
    if (WiFi.status() != WL_CONNECTED) {
      delay(1);
      //startWifi();             // This section if you want hard-coded
    return;
    }
    
  server.handleClient();
}
