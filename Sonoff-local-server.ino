#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

IPAddress ip(10, 2, 50, 11);
IPAddress dns(10, 0, 1, 10);
IPAddress gateway(10, 2, 50, 1);
IPAddress subnet(255, 255, 255, 0);

const int RELAY_PIN = 12; //RELAY
const int STAT_PIN = 13; // status
int state = 0;
 
const char* ssid = "SSID";
const char* password = "P@ssw0rd";
MDNSResponder mdns;
ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>SONOFF Wireless Switch</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a></p>");
  //server.serveStatic("/img", SPIFFS, "/img");
  //server.serveStatic("/", SPIFFS, "/index.htm");
  //server.send(200, SPIFFS, "/index.htm");
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
  SPIFFS.begin();
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    digitalWrite(STAT_PIN, 1);
  }

  digitalWrite(STAT_PIN, 0);
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  
  server.on("/on", [](){
  server.send(200, "text/plain", "Okay -- Light is ON!");
  
  state = 1;
  File f = SPIFFS.open("/state.txt", "w"); 

    if (!f) {
      Serial.println("file open failed on update.");
    } else {
      f.println(state); 
      f.close();
    }
  });
  
  server.on("/off", [](){
  server.send(200, "text/plain", "Okay -- Light is OFF!");
  
  state = 0;
  File f = SPIFFS.open("/state.txt", "w"); 

    if (!f) {
      Serial.println("file open failed on update.");
    } else {
      f.println(state); 
      f.close();
    }
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  File f = SPIFFS.open("/state.txt", "r");
  if (!f) {
      Serial.println("File open failed on read.");
    } else {
      while(f.available()) {
        //Lets read line by line from the file
        String line = f.readStringUntil('\n');
        state = line.toInt();
        break; //if left in, we'll just read the first line then break out of the while.
      } 
      f.close();
    }

    if (state == 1) {
  digitalWrite(RELAY_PIN, 1);
  }
  else {
    if (state == 0) {
    digitalWrite(RELAY_PIN, 0);
    }
  }
  server.handleClient();
}
