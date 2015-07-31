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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tosc_tinyosc {
  const char *address; // a pointer to the OSC address field
  const char *format; // a pointer to the format field
  const char *marker; // the current read head
  const char *buffer; // the original message data
  int len; // length of the buffer data
} tosc_tinyosc;


int tosc_init(tosc_tinyosc *o, const char *buffer, const int len);

// returns the next 32-bit int. Does not check buffer bounds.
int32_t tosc_getNextInt32(tosc_tinyosc *o);

// returns the next 32-bit float. Does not check buffer bounds.
float tosc_getNextFloat(tosc_tinyosc *o);

// returns the next string, or NULL if the buffer length is exceeded.
const char *tosc_getNextString(tosc_tinyosc *o);

#ifdef __cplusplus
}
#endif

#endif // _TINY_OSC_
