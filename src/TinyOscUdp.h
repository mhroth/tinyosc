#include <TinyOsc.h>
#include <Udp.h>

#ifndef TINY_OSC_UDP_BUFFER_SIZE
#define TINY_OSC_UDP_BUFFER_SIZE 1024
#endif

class TinyOscUdp : public TinyOsc {

    UDP* udp;
    unsigned char inputBuffer[TINY_OSC_UDP_BUFFER_SIZE];
    IPAddress destinationIp;
    unsigned int destinationPort;

  public:
    TinyOscUdp(UDP * udp, IPAddress destinationIp, unsigned int destinationPort) {
    	this->udp = udp;
      this->destinationIp = destinationIp;
      this->destinationPort = destinationPort;
    }

    void receiveMessages(tOscCallbackFunction callback) {

      size_t packetLength = udp->parsePacket();
      if ( packetLength > 0 ) {
        packetLength = udp->read(inputBuffer, TINY_OSC_UDP_BUFFER_SIZE);
        #ifdef TINY_OSC_UDP_DEBUG
        
        Serial.print("Got UDP bytes: ");
        Serial.println(packetLength);
        #endif
      	
        TinyOsc::parseMessages( callback , inputBuffer , packetLength);
      }

    }

    void sendMessage(const char *address, const char *format, ...) {
      udp->beginPacket(destinationIp, destinationPort);
      va_list ap;
      va_start(ap, format);
      vprint(udp, address, format, ap);
      va_end(ap);
      udp->endPacket(); 
    }

    void setDestination(IPAddress destinationIp, unsigned int destinationPort) {
      this->destinationIp = destinationIp;
      this->destinationPort = destinationPort;
    }



};
