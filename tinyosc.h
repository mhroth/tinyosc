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

#ifndef _TINY_OSC_
#define _TINY_OSC_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tosc_tinyosc {
  char *format;  // a pointer to the format field
  char *marker;  // the current read head
  char *buffer;  // the original message data (also points to the address)
  uint32_t len;  // length of the buffer data
} tosc_tinyosc;

typedef struct tosc_bundle {
  char *marker;
  char *buffer;
  uint32_t len;
} tosc_bundle;


// parse a buffer containing an OSC message.
// The contents of the buffer are NOT copied.
// The tosc_tinyosc struct only points at relevant parts of the original buffer.
// Returns 0 if there is no error. An error code (a negative number) otherwise.
int tosc_read(tosc_tinyosc *o, char *buffer, const int len);

// reads a buffer containing a bundle of OSC messages
void tosc_readBundle(tosc_bundle *b, char *buffer, const int len);

// returns true if the buffer refers to a bundle of OSC messages. False otherwise.
bool tosc_isBundle(const char *buffer);

// returns the timetag of an OSC bundle
uint64_t tosc_getTimetag(tosc_bundle *b);

bool tosc_getNextMessage(tosc_bundle *b, tosc_tinyosc *o);

// returns a point to the address block of the OSC buffer.
// This is also the start of the buffer.
char *tosc_getAddress(tosc_tinyosc *o);

// returns a point to the format block of the OSC buffer
char *tosc_getFormat(tosc_tinyosc *o);

// returns the next 32-bit int. Does not check buffer bounds.
int32_t tosc_getNextInt32(tosc_tinyosc *o);

// returns the next 32-bit float. Does not check buffer bounds.
float tosc_getNextFloat(tosc_tinyosc *o);

// returns the next string, or NULL if the buffer length is exceeded.
const char *tosc_getNextString(tosc_tinyosc *o);

// points the given buffer pointer to the next blob.
// The len pointer is set to the length of the blob.
// Returns NULL and 0 if the OSC buffer bounds are exceeded.
void tosc_getNextBlob(tosc_tinyosc *o, const char **buffer, int *len);

// starts writing a bundle to the given buffer with length
void tosc_writeBundle(tosc_bundle *b, uint32_t timetag, char *buffer, const int len);

// write a message to a bundle buffer
int tosc_writeNextMessage(tosc_bundle *b,
    const char *address, const char *format, ...);

// writes an OSC packet to a buffer. Returns the total number of bytes written.
// The entire buffer is cleared before writing.
int tosc_write(char *buffer, const int len, const char *address,
    const char *fmt, ...);

// a convenience function to (non-destructively) print a buffer containing
// an OSC message to stdout.
void tosc_printOscBuffer(char *buffer, const int len);

#ifdef __cplusplus
}
#endif

#endif // _TINY_OSC_
