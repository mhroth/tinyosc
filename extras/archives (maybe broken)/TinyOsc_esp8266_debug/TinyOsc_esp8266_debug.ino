// ECHO TEST FOR ESP8266

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
IPAddress udpTxIp = IPAddress(192, 168, 25, 125);
int udpTxPort = 7890;

// INCLUDE ESP8266 UDP
#include <WiFiUdp.h>
WiFiUDP udp;

// UDP BUFFERS 
#define UDP_RX_BUFFER_MAX_SIZE 256
char udpRxBuffer[UDP_RX_BUFFER_MAX_SIZE];

#define UDP_TX_BUFFER_MAX_SIZE 256
char udpTxBuffer[UDP_TX_BUFFER_MAX_SIZE];

// https://github.com/mhroth/tinyosc
// OSC PARSER AND PACKER
#include <TinyOsc.h>

TinyOsc osc;


#include <Chrono.h>
Chrono sendOSCMessageChrono;

//===========
//== SETUP ==
//===========
void setup() {

  // INITIATE SERIAL COMMUNICATION FOR DEBUGGING.
  Serial.begin(57600);

  Serial.println("***STARTING WIFI***");

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

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {

    // IS IT PART OF A BUNDLE?
    bool isBundled = osc.isBundled();

    // GET THE FORMAT C STRING
    char * format = osc.getFormat();

     Serial.println("***OSC***");

      if ( isBundled ) Serial.println("This message is part of a bundle");
      else Serial.println("This message is not part of a bundle");
      
      Serial.print("Address: ");
      Serial.println(osc.getAddress());
      Serial.print("Type tags: ");
      Serial.println(format);

      if ( osc.fullMatch("/test") ) {
        Serial.println("Yes, this message has the address /test");
      }

      Serial.print("Arguments : ");

      // LOOP THROUGH THE FORMAT STRING (IT ENDS WITH A 0)
      for (int i = 0; format[i] != '\0'; i++) {
        switch (format[i]) {
          case 'f': Serial.print( osc.getNextFloat() ); break;
          case 'i': Serial.print( osc.getNextInt32() ); break;
          // returns NULL if the buffer length is exceeded
          case 's': Serial.print( osc.getNextString() ); break;
          default: continue;
        }
        Serial.print(" ");
      }
      Serial.println();  
}


//==========
//== LOOP ==
//==========
void loop() {

  // CHECK FOR OSC MESSAGES OVER UDP
  // ===============================

  // CHECK IF AN UDP PACKET WAS RECEIVED:
  // udp.parsePacket() RETURNS ture IF IT AN UDP PACKET WAS RECEIVED
  if ( udp.parsePacket() ) {
    
    // COPY THE PACKET INTO A BUFFER THAT WILL THEN BE USED BY TinyOsc
    // udp.read() RETURNS THE NUMBER OF chars THAT WERE RECEIVED 
    int packetSize = udp.read(udpRxBuffer, UDP_RX_BUFFER_MAX_SIZE);

    Serial.println("***UDP***");
    Serial.print("Received packet of size: ");
    Serial.println(packetSize);
    Serial.print("From: ");
    IPAddress remoteIp = udp.remoteIP();
    Serial.println(remoteIp);

    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // <TinyOsc>.parse( buffer name, size of the data, callback function)
    // FOR EVERY PARSED OSC MESSAGE IN  udpRxBuffer THE receivedOscMessage WILL BE CALLED
    osc.parse( udpRxBuffer, packetSize, receivedOscMessage);

  }

  // SEND OSC MESSAGES OVER UDP
  // ===============================

  // SEND A MESSAGE EVERY SECOND (1000 ms)
  if ( sendOSCMessageChrono.hasPassed( 1000 ) ) {
    sendOSCMessageChrono.restart();

    // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
    // THE FORMAT STRING MUST MATCH THE DATA
    // 'f':32-bit float, 's':ascii string, 'i':32-bit integer
    // IN THIS CASE, THE DATA IS 1.0 (float), "hello" (string) AND millis() (int)
    int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/ping",  "fsi",   1.0, "hello", millis() );

    // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
    udp.beginPacket( udpTxIp , udpTxPort );
    udp.write( udpTxBuffer ,  udpTxBufferLength );
    udp.endPacket();
    
  }
  

}
