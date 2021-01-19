#include <ESP8266WebServer-impl.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <Parsing-impl.h>
#include <Uri.h>

#include <SoftwareSerial.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>

#define BAUD_RATE 9600

SoftwareSerial softSerial(14, 16, false, 32);

const char* ssid = "test.";
const char* password = "test1234567890";

IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

int values[12];
char cvalue='=';
char cindex='~';

int m=0;

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

int i;
String str;

void setup(void){
  for(i=0;i<=10;i++){
    values[i]=-2018;
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  softSerial.begin(BAUD_RATE);

  Serial.print("Setting ip config...");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(ssid, password);
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  Serial.println("");

  // Wait for connection
  /*while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }*/

  delay(5000);
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  /*server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });*/
  server.on("/",[](){
    str="<html><head><meta http-equiv=\"refresh\" content=\"1\"></head><body><hr>START<hr><pre>";
    for(i=0;i<=10;i++){
      if(values[i]!=-2018){
        str+=i;
        str+=" = ";
        str+=values[i];
        str+="\n";
      }
    }
    str+="</pre><hr>END<hr></body></html>";
    server.send(200,"text/html",str);    
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

char c;
int in;
int ibuffer=0;
int selected=0;
void loop(void){
  server.handleClient();
  if (Serial.available() > 0) {
      c = Serial.read();
      if(c==cvalue){  /* = */
        m=1;
      }else if(c==cindex){ /* ~ */
        m=0;
      } else {
        in=c-'0';
        if(in>=0 && in<=9){
          if(m==0){ /* selecting values [ in ] */
            selected=in;
            values[selected]=0;
            Serial.print("H s "); Serial.println(selected);
          }else{
            values[selected]=values[selected]*10+in;
            Serial.print("H s "); Serial.print(selected); Serial.print(" "); Serial.println(values[selected]);
          }
        }
      }
  }

  if (softSerial.available() > 0) {
      c = softSerial.read();
      if(c==cvalue){  /* = */
        m=1;
      }else if(c==cindex){ /* ~ */
        m=0;
      } else {
        in=c-'0';
        if(in>=0 && in<=9){
          if(m==0){ /* selecting values [ in ] */
            selected=in;
            values[selected]=0;
            Serial.print("S s "); Serial.println(selected);
          }else{
            values[selected]=values[selected]*10+in;
            Serial.print("S s "); Serial.print(selected); Serial.print(" "); Serial.println(values[selected]);
          }
        }
      }
  }
}
