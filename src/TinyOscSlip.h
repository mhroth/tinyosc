#include <TinySlip.h>
#include <TinyOsc.h>

#ifndef TINY_OSC_SLIP_BUFFER_SIZE
#define TINY_OSC_SLIP_BUFFER_SIZE 256
#endif

class TinyOscSlip : public TinyOsc {

    TinySlip stream;
    unsigned char inputBuffer[TINY_OSC_SLIP_BUFFER_SIZE];

  public:
    TinyOscSlip(Stream * stream) : stream(stream) {

    }

    void receiveMessages(tOscCallbackFunction callback) {

      size_t packetLength = stream.parsePacket(inputBuffer, TINY_OSC_SLIP_BUFFER_SIZE );
      if ( packetLength > 0 ) {
        TinyOsc::parseMessages( callback , inputBuffer , TINY_OSC_SLIP_BUFFER_SIZE );
      }

    }

    void sendMessage(const char *address, const char *format, ...) {
      stream.beginPacket();
      va_list ap;
      va_start(ap, format);
      vprint(&stream, address, format, ap);
      va_end(ap);
      stream.endPacket(); 
    }



};
