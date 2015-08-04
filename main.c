/**
 * Copyright (c) 2015, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "tinyosc.h"

static volatile bool keepRunning = true;

// handle Ctrl+C
static void sigintHandler(int x) {
  keepRunning = false;
}

/**
 * A basic program to listen to port 9000 and print received OSC packets.
 */
int main(int argc, char *argv[]) {

  // register the SIGINT handler (Ctrl+C)
  signal(SIGINT, &sigintHandler);

  // open a socket to listen for datagrams (i.e. UDP packets) on port 9000
  const int fd = socket(AF_INET, SOCK_DGRAM, 0);
  fcntl(fd, F_SETFL, O_NONBLOCK); // set the socket to non-blocking
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(9000);
  sin.sin_addr.s_addr = INADDR_ANY;
  bind(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  printf("tinyosc is now listening on port 9000.\n");
  printf("Press Ctrl+C to stop.\n");

  tosc_tinyosc osc;
  char buffer[2048]; // declare a 2Kb buffer to read packet data into

  while (keepRunning) {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(fd, &readSet);
    struct timeval timeout = {1, 0}; // select times out after 1 second
    if (select(fd+1, &readSet, NULL, NULL, &timeout) > 0) {
      struct sockaddr sa; // can be safely cast to sockaddr_in
      socklen_t sa_len = sizeof(struct sockaddr_in);
      int len = 0;
      while ((len = recvfrom(fd, buffer, sizeof(buffer), 0, &sa, &sa_len)) > 0) {
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
                for (int j = 0; j < n; j++) printf("%X", b[j]); // print blob bytes
                printf(" ");
                break;
              }
              case 'f': printf("%g ", tosc_getNextFloat(&osc)); break;
              case 'i': printf("%i ", tosc_getNextInt32(&osc)); break;
              // returns NULL if the buffer length is exceeded
              case 's': printf("%s ", tosc_getNextString(&osc)); break;
              case 'F': printf("false "); break;
              case 'I': printf("inf "); break;
              case 'N': printf("nil "); break;
              case 'T': printf("true "); break;
              default: printf("Unknown format: '%c' ", osc.format[i]); break;
            }
          }
          printf("\n");
        }
      }
    }
  }

  // close the UDP socket
  close(fd);

  return 0;
}
