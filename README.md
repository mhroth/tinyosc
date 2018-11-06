# TinyOSC

TinyOSC is a minimal [Open Sound Control](http://opensoundcontrol.org/) (OSC) library for Arduino based on TinyOsc by Martin Roth (https://github.com/mhroth/tinyosc). The typical use case is to parse a raw buffer received directly from a socket. Given the limited nature of the library it also tends to be quite fast. It doesn't hold on to much state and it doesn't do much error checking. If you have a good idea of what OSC packets you will receive and need to process them quickly, this library might be for you.

## Supported Features
Due to its *tiny* nature, TinyOSC does not support all standard OSC features. Currently it supports:
* message parsing
* message writing
* bundle parsing
* ~~bundle writing~~
* ~~timetags~~
* ~~matching~~
* Types
  * `b`: binary blob
  * `f`: float
  * `d`: double
  * `i`: int32
  * `h`: int64
  * `s`: string
  * `m`: midi
  * `t`: timetag
  * `T`: true
  * `F`: false
  * `I`: infinitum
  * `N`: nil

## Code Examples
### Reading UDP OSC Messages and Bundles
```C

#define UDP_RX_BUFFER_MAX_SIZE 256
char udpRxBuffer[UDP_RX_BUFFER_MAX_SIZE];

[...setup UDP and WIFI or ETHERNET...]

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
        Serial.print("Yes, this message has the address /test");
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

void loop() {




if ( udp.parsePacket() ) {
    
    // COPY THE PACKET INTO A BUFFER THAT WILL THEN BE USED BY TinyOsc
    // udp.read() RETURNS THE NUMBER OF chars THAT WERE RECEIVED 
    int packetSize = udp.read(udpRxBuffer, UDP_RECEIVE_BUFFER_MAX_SIZE);

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = udp.remoteIP();
    Serial.println(remoteIp);

    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // <TinyOsc>.parse( buffer name, size of the data, callback function)
    // FOR EVERY PARSED OSC MESSAGE IN udpRxBuffer THE receivedOscMessage WILL BE CALLED
    osc.parse( udpRxBuffer, packetSize, receivedOscMessage);

  }
}



```


### Writing UDP OSC Messages
```C
#define UDP_TX_BUFFER_MAX_SIZE 256
char udpTxBuffer[UDP_TX_BUFFER_MAX_SIZE];
```

```C
   // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
    // THE FORMAT STRING MUST MATCH THE DATA
    // 'f':32-bit float, 's':ascii string, 'i':32-bit integer
    // IN THIS CASE, THE DATA IS 1.0 (float), "hello" (string) AND millis() (int)
    int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/ping",  "fsi",   1.0, "hello", millis() );

    // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
    udp.beginPacket( udpTxIp , udpTxPort );
    udp.write( udpTxBuffer ,  udpTxBufferLength );
    udp.endPacket();
```



## Tests
Meh. Not really. But it works with [TouchOSC](http://hexler.net/software/touchosc)!

## License
TinyOSC is published under the [ISC license](http://opensource.org/licenses/ISC). Please see the `LICENSE` file included in this repository, also reproduced below. In short, you are welcome to use this code for any purpose, including commercial and closed-source use.

Original TinyOsc copyright:
```
Copyright (c) 2015, Martin Roth <mhroth@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```
