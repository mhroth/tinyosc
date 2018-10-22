// EXAMPLE FOR ESP8266

// INCLUDE ESP8266WiFi:
#include <ESP8266WiFi.h>

// Please change the following values with your network settings:
const char* ssid     = "******";
const char* password = "*****";
IPAddress ip(10, 5, 0, 120);
IPAddress gateway(10, 5, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// INCLUDE ESP8266 UDP
#include <WiFiUdp.h>
WiFiUDP udpReceive;

#define RECEIVE_BUFFER_MAX_SIZE 256 
uint8_t receiveBuffer[RECEIVE_BUFFER_MAX_SIZE];


//===========
//== SETUP ==
//===========
void setup() {

  // INITIATE SERIAL COMMUNICATION FOR DEBUGGING.
  Serial.begin(57600);

  // BEGIN WIFI
  WiFi.config(ip , gateway , subnet );
  WiFi.begin(ssid, password);


  // WAIT UNTIL CONNECTED
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }
  //

  // PRINT CONNECTION SETTINGS
  Serial.println();
  Serial.println("WiFi connected, IP address: ");
  Serial.println( WiFi.localIP() );
  // 


  udpReceive.begin(7777); // BEGIN LISTENING ON UDP PORT 7777.

}


//==========
//== LOOP ==
//==========
void loop() {

 // REVEIVE MASSAGES
 // ================

  // RECEIVE PACKETS OVER UDP
  if ( udpReceive.parsePacket() ) {
    // AN UDP PACKET WAS RECEIVED
    // COPY PACKET INTO BUFFER
    udpReceive.read(receiveBuffer, RECEIVE_BUFFER_MAX_SIZE);
    
  }


/*
 // SEND MASSAGES
 // ================

  // [START] SEND PACKET OVER UDP EVERY 50 ms:
  if ( sendMassageChrono.hasPassed(50) ) { // THE CHRONO PASSED 50 ms.
    sendMassageChrono.restart(); // RESTART THE CHRONO.

    // [START] PREPARE ASCII PACKET
    outbound.beginPacket("address");
    outbound.addByte(195); // ADD WHAT YOU WANT.
    outbound.endPacket();
    // [END] PREPARE ASCII PACKET.

    // [START] SEND PACKET OVER UDP
    IPAddress targetIp = IPAddress(10, 5, 0, 141);
    int targetPort = 7890;
    udp.beginPacket( targetIp , targetPort );
    outbound.streamPacket(&udp);
    udp.endPacket();
    // [END] SEND PACKETS OVER UDP
  }
  // [END] SEND PACKET OVER UDP EVERY 50 ms
  */
}
