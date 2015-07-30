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

#include <netinet/in.h>
#include <stddef.h>
#include "tinyosc.h"

// http://opensoundcontrol.org/spec-1_0
int tosc_init(tosc_tinyosc *o, const char *buffer, const int len) {
  // extract the address
  o->address = buffer; // address string is null terminated
  // NOTE(mhroth): if there's a comma in the address, that's weird

  int i = 0;
  while (i < len && buffer[i] != ',') ++i; // find the format comma
  // TODO(mhroth): does not check for null terminated address string
  if (i == len) return -1; // error while looking for format string
  // format string is null terminated
  o->format = buffer + i + 1; // format starts after comma

  while (i < len && buffer[i] != '\0') ++i;
  if (i == len) return -2; // format string not null terminated

  ++i; while (i & 0x3) ++i; // advance to the next multiple of 4
  o->marker = buffer + i;

  o->buffer = buffer;
  o->len = len;

  return 0;
}

int32_t tosc_getNextInt32(tosc_tinyosc *o) {
  const int32_t i = (int32_t) ntohl(*((uint32_t *) o->marker)); // convert from big-endian
  o->marker += 4;
  return i;
}

float tosc_getNextFloat(tosc_tinyosc *o) {
  const uint32_t i = ntohl(*((uint32_t *) o->marker)); // convert from big-endian
  o->marker += 4;
  return *((float *) (&i));
}

const char *tosc_getNextString(tosc_tinyosc *o) {
  // TODO(mhroth): test this
  int i = o->marker - o->buffer; // offset
  const char *s = o->marker;
  while (i < o->len && s[i] != '\0') ++i;
  if (i == o->len) return NULL;
  ++i; while (i & 0x3) ++i; // advance to next multiple of 4
  o->marker = o->buffer + i;
  return s;
}
