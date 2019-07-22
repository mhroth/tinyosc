#include <TinyOsc.h>
#include <Udp.h>

#ifndef TINY_OSC_UDP_BUFFER_SIZE
#define TINY_OSC_UDP_BUFFER_SIZE 1024
#endif

class TinyOscUdp : public TinyOsc {

    UDP* udp;
    unsigned char inputBuffer[TINY_OSC_UDP_BUFFER_SIZE];

  public:
    TinyOscUdp(UDP * udp) {
    	this->udp = udp;
    }

    void receiveMessages(tOscCallbackFunction callback) {

      size_t packetLength = udp->parsePacket();
      if ( packetLength > 0 ) {
      	packetLength = udp->read(inputBuffer, TINY_OSC_UDP_BUFFER_SIZE);
        TinyOsc::parseMessages( callback , inputBuffer , packetLength);
      }

    }

    void sendMessage(IPAddress ip, unsigned int port, const char *address, const char *format, ...) {
      udp->beginPacket(ip, port);
      va_list ap;
      va_start(ap, format);
      vprint(udp, address, format, ap);
      va_end(ap);
      udp->endPacket(); 
    }



};
