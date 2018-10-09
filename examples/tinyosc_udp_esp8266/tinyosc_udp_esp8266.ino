// EXAMPLE FOR ESP8266

// INCLUDE ESP8266WiFi:
#include <ESP8266WiFi.h>

// Please change the following values with your network settings:
const char* ssid     = "PetitPet";
const char* password = "freedomostie";
IPAddress ip(192, 168, 25, 10);
IPAddress gateway(192, 168, 25, 1);
IPAddress subnet(255, 255, 255, 0);

// UDP
int udpReceivePort = 7777;
IPAddress udpSendIp = IPAddress(192, 168, 25, 125);
int udpSendPort = 7890;

// INCLUDE ESP8266 UDP
#include <WiFiUdp.h>
WiFiUDP udp;

// UDP BUFFERS 
#define UDP_RECEIVE_BUFFER_MAX_SIZE 256
char udpReceiveBuffer[UDP_RECEIVE_BUFFER_MAX_SIZE];

#define UDP_SEND_BUFFER_MAX_SIZE 256
char udpSendBuffer[UDP_SEND_BUFFER_MAX_SIZE];

// https://github.com/mhroth/tinyosc
// OSC PARSER
#include <tinyosc.h>
tosc_message oscMessage; // declare the TinyOSC structure


#include <Chrono.h>
Chrono sendOSCMessageChrono;

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


  udp.begin(udpReceivePort); // BEGIN LISTENING ON UDP PORT udpReceivePort

}


//==========
//== LOOP ==
//==========
void loop() {

  // REVEIVE MASSAGES
  // ================

  // RECEIVE PACKETS OVER UDP
  if ( udp.parsePacket() ) {
    // AN UDP PACKET WAS RECEIVED
    // COPY PACKET INTO BUFFER
    int packetSize = udp.read(udpReceiveBuffer, UDP_RECEIVE_BUFFER_MAX_SIZE);

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = udp.remoteIP();

    if ( tosc_isBundle(udpReceiveBuffer) ) Serial.println("Is a bundle");

    // parse the buffer contents (the raw OSC bytes)
    // a return value of 0 indicates no error
    if ( tosc_parseMessage(&oscMessage, udpReceiveBuffer, packetSize) == 0 ) {
      Serial.println("Received OSC message:");
      Serial.print(" Address: ");
      Serial.println(tosc_getAddress(&oscMessage));
      Serial.print(" Type tags: ");
      Serial.println(tosc_getFormat(&oscMessage));

      Serial.println("Arguments :");

      for (int i = 0; oscMessage.format[i] != '\0'; i++) {
        switch (oscMessage.format[i]) {
          case 'f': Serial.println( tosc_getNextFloat(&oscMessage) ); break;
          case 'i': Serial.println( tosc_getNextInt32(&oscMessage) ); break;
          // returns NULL if the buffer length is exceeded
          case 's': Serial.println( tosc_getNextString(&oscMessage) ); break;
          default: continue;
        }
      }
      
    }
  }


  if ( sendOSCMessageChrono.hasPassed( 1000 ) ) {
    sendOSCMessageChrono.restart();

    int udpSendBufferLength = tosc_writeMessage(
    udpSendBuffer, UDP_SEND_BUFFER_MAX_SIZE , // buffer and its size
    "/ping", // the address
    "fsi",   // the format; 'f':32-bit float, 's':ascii string, 'i':32-bit integer
    1.0, "hello", millis() );
    
    udp.beginPacket( udpSendIp , udpSendPort );
    udp.write( udpSendBuffer ,  udpSendBufferLength );
    udp.endPacket();
    
  }

}
