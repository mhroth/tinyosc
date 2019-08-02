/////////////
// IMPORTS //
/////////////

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#include "WiFiCredentials.h"

#include <TinyOscUdp.h>

#include <Chrono.h>

///////////////////
// CONFIGURATION //
///////////////////

IPAddress localIp(192, 168, 1, 31);
IPAddress networkGateway(192, 168, 1, 31);
IPAddress networkSubnet(255, 255, 255, 1);
IPAddress broadcastIp(255, 255, 255, 255);

unsigned int localPort = 8888;
unsigned int remotePort = 7777;
IPAddress remoteIp(192, 168, 1, 111);

//////////
// CODE //
//////////


WiFiUDP udp;

TinyOscUdp osc(&udp);

Chrono messageOutputChrono;


///////////
// SETUP //
///////////
void setup() {

   Serial.begin(115200);


  WiFi.config(localIp, networkGateway, networkSubnet);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }
  Serial.println();

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  udp.begin(localPort);


}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {


}


//////////
// LOOP //
//////////
void loop() {


  osc.receiveMessages(receivedOscMessage);

  if ( messageOutputChrono.hasPassed(1000) ){
    
      messageOutputChrono.restart();
  

    
    // STEP 2 : STREAM THE OSC MESSAGE THROUGH SLIP
    osc.sendMessage(broadcastIp, remotePort,  "/ms",  "i",   millis() );
    

    
  }

}
