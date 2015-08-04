# TinyOSC

TinyOSC is a minimal [Open Sound Control](http://opensoundcontrol.org/) (OSC) library written in C. The typical use case is to parse a raw buffer received directly from a socket. Given the limited nature of the library it also tends to be quite fast. It doesn't hold on to much state and it doesn't do much error checking. If you have a good idea of what OSC packets you will receive and need to process them quickly, this library might be for you.

## Supported Features
Due to its *tiny* nature, TinyOSC does not support all standard OSC features. Currently it supports:
* packet parsing
* packet writing
* ~~bundles~~
* ~~timetag~~
* ~~matching~~
* Types
  * `b`: binary blob
  * `f`: float
  * `i`: int32
  * `s`: string

## Code Example
### Reading
```C
#include "tinyosc.h"

tosc_tinyosc osc; // declare the TinyOSC structure
char buffer[1024]; // declare a buffer into which to read the socket contents
int len = 0; // the number of bytes read from the socket

while ((len = READ_BYTES_FROM_SOCKET(buffer)) > 0) {
  // parse the buffer contents (the raw OSC bytes)
  // a return value of 0 indicates no error
  if (!tosc_read(&osc, buffer, len)) {
    printf("Received OSC message: [%i bytes] %s %s ",
        len, // the number of bytes in the OSC message
        osc.address, // the OSC address string, e.g. "/button1"
        osc.format); // the OSC format string, e.g. "f"
    for (int i = 0; osc.format[i] != '\0'; i++) {
      switch (osc.format[i]) {
        case 'b': {
          const char *b = NULL; // will point to binary data
          int n = 0; // takes the length of the blob
          tosc_getNextBlob(&osc, &b, &n);
          printf("[%i]", n); // print length of blob
          for (int i = 0; i < n; i++) printf("%X", b[i]); // print blob bytes
          printf(" ");
          break;
        }
        case 'f': printf("%g ", tosc_getNextFloat(&osc)); break;
        case 'i': printf("%i ", tosc_getNextInt32(&osc)); break;
        // returns NULL if the buffer length is exceeded
        case 's': printf("%s ", tosc_getNextString(&osc)); break;
        default: continue;
      }
    }
    printf("\n");
  }
}
```

### Writing
```C
// declare a buffer for writing the OSC packet into
char buffer[1024];

// write the OSC packet to the buffer
// returns the number of bytes written to the buffer, negative on error
// note that tosc_write will clear the entire buffer before writing to it
int len = tosc_write(
    buffer, sizeof(buffer),
    "/ping", // the address
    "fsi",   // the format; 'f':32-bit float, 's':ascii string, 'i':32-bit integer
    1.0f, "hello", 2);

// send the data out of the socket
send(socket_fd, buffer, len, 0);
```

### main.c
A small example program is included in `main.c`. Build it using the included shell script `build.sh`, and run it with `tinyosc`. The program simply opens a UDP socket on port 9000 and prints out received OSC messages. Press Ctrl+C to stop. Try it with any OSC client, such as TouchOSC. This program is also an example for how TinyOSC is expected to be used.


## Tests
Meh. Not really. But it works with [TouchOSC](http://hexler.net/software/touchosc)!

## License
TinyOSC is published under the [ISC license](http://opensource.org/licenses/ISC). Please see the `LICENSE` file included in this repository, also reproduced below. In short, you are welcome to use this code for any purpose, including commercial and closed-source use.

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
