#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>

WebSocketsServer webSocket = WebSocketsServer(81);

#define SOFTAP

#ifndef SOFTAP
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWD";
#endif

#define ALI 5
#define AHI 2
#define BLI 4
#define BHI 0
#define DIS 16

void disableMotor()
{
  digitalWrite(AHI, 0);
  digitalWrite(BHI, 0);
  digitalWrite(BLI, 0);
  digitalWrite(ALI, 0);
  digitalWrite(DIS, 1);
}

void enableMotor()
{
  digitalWrite(AHI, 1);
  digitalWrite(BHI, 1);
  digitalWrite(BLI, 0);
  digitalWrite(ALI, 0);
  digitalWrite(DIS, 0);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * command, size_t lenght) {
  switch(type) {
      case WStype_DISCONNECTED:
        // stop the tank when connection drops
        disableMotor();
        break;

      case WStype_CONNECTED: {
        // send message to client
        webSocket.sendTXT(num, "Connected");
        // enable motor
        enableMotor();
        }
        break;

      case WStype_TEXT:
        int r = atoi((char *) command);
        analogWrite(ALI, r);
        break;
  }
}


void setup()
{
  Serial.begin(115200);

  // setup pins as output
  pinMode(ALI, OUTPUT);
  pinMode(AHI, OUTPUT);
  pinMode(BLI, OUTPUT);
  pinMode(AHI, OUTPUT);
  pinMode(DIS, OUTPUT);

  // initalize with motor off
  disableMotor();

  #ifndef SOFTAP
  WiFi.begin(ssid, password);
  #else
  WiFi.softAP("OpenSkate");
  #endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  #ifndef SOFTAP
  IPAddress myIP = WiFi.localIP();
  #else
  IPAddress myIP = WiFi.softAPIP();
  #endif

	Serial.print("AP IP address: ");
	Serial.println(myIP);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  MDNS.addService("ws", "tcp", 81);
}

void loop()
{
  webSocket.loop();
}
