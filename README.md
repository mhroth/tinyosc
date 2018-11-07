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

```C
// Create a TinyOsc instance
TinyOsc osc;
```

### Receiving UDP OSC Messages

```C
// Parsing an OSC message from a UDP buffer
#define UDP_RX_BUFFER_MAX_SIZE 256
char udpRxBuffer[UDP_RX_BUFFER_MAX_SIZE];

[...setup UDP and WIFI or ETHERNET...]

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {

  // AN OSC MESSAGE IS RECEIVED
  // PARSE THE MESSAGE HERE WITH 
  // osc.fullMatch() TO CHECK THE ADDRESS
  // osc.getNextInt32(), osc.getNextFloat(), osc.getNextString(), etc TO GET THE ARGUMENTS
}

void loop() {


if ( udp.parsePacket() ) {
    
    // COPY THE PACKET INTO A BUFFER THAT WILL THEN BE USED BY TinyOsc
    // udp.read() RETURNS THE NUMBER OF chars THAT WERE RECEIVED 
    int packetSize = udp.read(udpRxBuffer, UDP_RECEIVE_BUFFER_MAX_SIZE);

    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // osc.parse( buffer name, size of the data, callback function)
    // FOR EVERY PARSED OSC MESSAGE IN udpRxBuffer THE receivedOscMessage WILL BE CALLED
    osc.parse( udpRxBuffer, packetSize, receivedOscMessage);

  }
}

```

### PARSING OSC MESSAGES

```C
/**
 * Returns true if the address matches "/led".
 */
if (  osc.fullMatch("/led") ) {
  // Get the first argument as an Arduino int
  int argument = getNextInt32();
}
```

```C
/**
 * Returns true if the address matches "/led" and 
 the type tag string matches "i" (one OSC integers)
 */
if (  osc.fullMatch("/led","i") ) {
  // Get the first argument as a long (Arduino long == OSC int)
  long argument = osc.getNextInt32();
}
```

```C
/**
 * Returns true if the address matches "/motor" and 
 the type tag string matches "ii" (two OSC integers)
 */
if (  osc.fullMatch("/motor","ii") ) {
  int firstArgument  = osc.getNextInt32();
  int secondArgument = osc.getNextInt32();
}
```

```C
/**
 * Returns true if the address matches "/rgb" and 
 the type tag string matches "iii" (three OSC integers)
 */
if (  osc.fullMatch("/rgb","iii") ) {
  int r  = osc.getNextInt32();
  int g = osc.getNextInt32();
  int b = osc.getNextInt32();
}
```

```C
/**
 * Returns true if the address matches "/touch" and 
 the type tag string matches "f" (one OSC float)
 */
if (  osc.fullMatch("/touch","f") ) {
  int f  = osc.getNextFloat();
}
```

### Debuging OSC Messages
```C
/**
 * Returns a pointer to the address block of the OSC buffer.
 */
char * address = osc.getAddress();
Serial.println(address);
```
```C
/**
 * Returns a pointer to the type tag block of the OSC buffer.
 */
char * typetags = osc.getTypeTags();
Serial.println(typetags);
```

### Writing OSC Messages
```C
/**
 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
 * The entire buffer is cleared before writing.
 */
uint32_t writeMessage(char *buffer, const int len, const char *address,
    const char *fmt, ...);


};
```
### Sending UDP OSC Messages
```C
#define TX_BUFFER_MAX_SIZE 256
char txBuffer[TX_BUFFER_MAX_SIZE];
```

```C
   // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
    // THE TYPE TAG STRING MUST MATCH THE DATA
    // 'f':32-bit float, 's':ascii string, 'i':32-bit integer
    // IN THIS CASE, THE DATA IS 1.0 (float), "hello" (string) AND millis() (int)
    int txBufferLength = osc.writeMessage( txBuffer, TX_BUFFER_MAX_SIZE ,  "/ping",  "fsi",   1.0, "hello", millis() );

    // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
    udp.beginPacket( targetIp , targetPort );
    udp.write( txBuffer ,  txBufferLength );
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
