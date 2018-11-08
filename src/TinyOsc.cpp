/**
 * Copyright (c) 2015-2018, Martin Roth (mhroth@gmail.com)
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

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define TINYOSC_TIMETAG_IMMEDIATELY 1L


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#ifndef UTIL_H
#define UTIL_H

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

// THIS IS GUESSWORK
#define htonll(x) ntohl(x)
#define ntohll(x) ntohl(x)

//( (htonl( (uint32_t)(x) ) << 32) | (htonl( x >> 32 ) ) )
// ALTERNATIVE?
/*
#define htobe64(x)  \
    (__extension__ ({ \
        uint64_t __temp = (x); \
        uint32_t __low = htobe32((uint32_t)__temp); \
        uint32_t __high = htobe32((uint32_t)(__temp >> 32)); \
        (((uint64_t)__low) << 32) | __high; \
    }))
*/
#endif

#define tosc_strncpy(_dst, _src, _len) strncpy(_dst, _src, _len)

#else
#if _WIN32
#include <winsock2.h>
#define tosc_strncpy(_dst, _src, _len) strncpy_s(_dst, _len, _src, _TRUNCATE)
#else
#include <netinet/in.h>
#define tosc_strncpy(_dst, _src, _len) strncpy(_dst, _src, _len)
#endif
#if __unix__ && !__APPLE__
#include <endian.h>
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#endif
#endif

#include "TinyOsc.h"

#define BUNDLE_ID 0x2362756E646C6500L // "#bundle"


// always writes a multiple of 4 bytes
static uint32_t tosc_vwrite(char *buffer, const int len,
    const char *address, const char *format, va_list ap) {
  memset(buffer, 0, len); // clear the buffer
  uint32_t i = (uint32_t) strlen(address);
  if (address == NULL || i >= len) return -1;
  tosc_strncpy(buffer, address, len);
  i = (i + 4) & ~0x3;
  buffer[i++] = ',';
  int s_len = (int) strlen(format);
  if (format == NULL || (i + s_len) >= len) return -2;
  tosc_strncpy(buffer+i, format, len-i-s_len);
  i = (i + 4 + s_len) & ~0x3;

  for (int j = 0; format[j] != '\0'; ++j) {
    switch (format[j]) {
      case 'b': {
        const uint32_t n = (uint32_t) va_arg(ap, int); // length of blob
        if (i + 4 + n > len) return -3;
        char *b = (char *) va_arg(ap, void *); // pointer to binary data
        *((uint32_t *) (buffer+i)) = htonl(n); i += 4;
        memcpy(buffer+i, b, n);
        i = (i + 3 + n) & ~0x3;
        break;
      }
      case 'f': {
        if (i + 4 > len) return -3;
        const float f = (float) va_arg(ap, double);
        *((uint32_t *) (buffer+i)) = htonl(*((uint32_t *) &f));
        i += 4;
        break;
      }
      case 'd': {
        if (i + 8 > len) return -3;
        const double f = (double) va_arg(ap, double);
        *((uint64_t *) (buffer+i)) = htonll(*((uint64_t *) &f));
        i += 8;
        break;
      }
      case 'i': {
        if (i + 4 > len) return -3;
        const uint32_t k = (uint32_t) va_arg(ap, int);
        *((uint32_t *) (buffer+i)) = htonl(k);
        i += 4;
        break;
      }
      case 'm': {
        if (i + 4 > len) return -3;
        const unsigned char *const k = (unsigned char *) va_arg(ap, void *);
        memcpy(buffer+i, k, 4);
        i += 4;
        break;
      }
      case 't':
      case 'h': {
        if (i + 8 > len) return -3;
        const uint64_t k = (uint64_t) va_arg(ap, long long);
        *((uint64_t *) (buffer+i)) = htonll(k);
        i += 8;
        break;
      }
      case 's': {
        const char *str = (const char *) va_arg(ap, void *);
        s_len = (int) strlen(str);
        if (i + s_len >= len) return -3;
        tosc_strncpy(buffer+i, str, len-i-s_len);
        i = (i + 4 + s_len) & ~0x3;
        break;
      }
      case 'T': // true
      case 'F': // false
      case 'N': // nil
      case 'I': // infinitum
          break;
      default: return -4; // unknown type
    }
  }

  return i; // return the total number of bytes written
}





TinyOsc::TinyOsc(){
  b = &bundle;
  o= &message;

}



// http://opensoundcontrol.org/spec-1_0
void TinyOsc::parse(char *buffer, const int len, tOscCallbackFunction callback) {
 
  if ( callback == NULL ) return; 

  // Check for bundles
  if (isABundle(buffer)) {

    parseBundle(buffer, len);
    timetag = parseBundleTimeTag();
    isPartOfABundle = true;
    
    while ( getNextMessage()) {
      callback();
    }
  } else {
    timetag = 0;
    isPartOfABundle = false;
    if ( parseMessage(buffer,len) == 0 ) callback();
  }
  
}

int TinyOsc::parseMessage(char *buffer, const int len) {
  // NOTE(mhroth): if there's a comma in the address, that's weird
  int i = 0;
  while (buffer[i] != '\0') ++i; // find the null-terimated address
  while (buffer[i] != ',') ++i; // find the comma which starts the format string
  if (i >= len) return -1; // error while looking for format string
  // format string is null terminated
  o->format = buffer + i + 1; // format starts after comma

  while (i < len && buffer[i] != '\0') ++i;
  if (i == len) return -2; // format string not null terminated

  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  o->marker = buffer + i;

  o->buffer = buffer;
  o->len = len;

  return 0;
}


uint64_t TinyOsc::parseBundleTimeTag() {
  return ntohll(*((uint64_t *) (b->buffer+8)));
}

// check if first eight bytes are '#bundle '
bool TinyOsc::isABundle(const char *buffer) {
  return (strcmp( buffer, "#bundle") == 0); //return ((*(const int64_t *) buffer) == htonll(BUNDLE_ID));
}

bool TinyOsc::isBundled() {
  return isPartOfABundle;
}

void TinyOsc::parseBundle(char *buffer, const int len) {
  b->buffer = (char *) buffer;
  b->marker = buffer + 16; // move past '#bundle ' and timetag fields
  b->bufLen = len;
  b->bundleLen = len;
}





bool TinyOsc::getNextMessage() {
  if ((b->marker - b->buffer) >= b->bundleLen) return false;
  uint32_t len = (uint32_t) ntohl(*((int32_t *) b->marker));
  parseMessage(b->marker+4, len);
  b->marker += (4 + len); // move marker to next bundle element
  return true;
}

char* TinyOsc::getAddress() {
  return o->buffer;
}

char* TinyOsc::getTypeTags() {
  return o->format;
}

uint32_t TinyOsc::getLength() {
  return o->len;
}

int32_t TinyOsc::getNextInt32() {
  // convert from big-endian (network btye order)
  const int32_t i = (int32_t) ntohl(*((uint32_t *) o->marker));
  o->marker += 4;
  return i;
}

int64_t TinyOsc::getNextInt64() {
  const int64_t i = (int64_t) ntohll(*((uint64_t *) o->marker));
  o->marker += 8;
  return i;
}

uint64_t TinyOsc::getNextTimetag() {
  return (uint64_t) getNextInt64();
}

float TinyOsc::getNextFloat() {
  // convert from big-endian (network btye order)
  const uint32_t i = ntohl(*((uint32_t *) o->marker));
  o->marker += 4;
  return *((float *) (&i));
}

double TinyOsc::getNextDouble() {
  const uint64_t i = ntohll(*((uint64_t *) o->marker));
  o->marker += 8;
  return *((double *) (&i));
}

const char *TinyOsc::getNextString() {
  int i = (int) strlen(o->marker);
  if (o->marker + i >= o->buffer + o->len) return NULL;
  const char *s = o->marker;
  i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
  o->marker += i;
  return s;
}

void TinyOsc::getNextBlob( const char **buffer, int *len) {
  int i = (int) ntohl(*((uint32_t *) o->marker)); // get the blob length
  if (o->marker + 4 + i <= o->buffer + o->len) {
    *len = i; // length of blob
    *buffer = o->marker + 4;
    i = (i + 7) & ~0x3;
    o->marker += i;
  } else {
    *len = 0;
    *buffer = NULL;
  }
}

unsigned char *TinyOsc::getNextMidi() {
  unsigned char *m = (unsigned char *) o->marker;
  o->marker += 4;
  return m;
}

void TinyOsc::reset() {
  int i = 0;
  while (o->format[i] != '\0') ++i;
  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  o->marker = o->format + i - 1; // -1 to account for ',' format prefix
}




bool TinyOsc::fullMatch(const char* address) {

    return (strcmp( o->buffer, address) == 0);
}

bool TinyOsc::fullMatch(const char* address, const char * typetags){
   return (strcmp( o->buffer, address) == 0) && (strcmp( o->format, typetags) == 0) ;
}

uint32_t TinyOsc::writeMessage(char *buffer, const int len,
    const char *address, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  const uint32_t i = tosc_vwrite(buffer, len, address, format, ap);
  va_end(ap);
  return i; // return the total number of bytes written
}

/*
uint64_t TinyOsc::getBundleTimetag() {
  return timetag;
}
*/

/*

void TinyOsc::startBundle(tosc_bundle *b, uint64_t timetag, char *buffer, const int len) {

  //*((uint64_t *) buffer) = htonll(BUNDLE_ID);
   //static uint8_t header[] = {'#', 'b', 'u', 'n', 'd', 'l', 'e', 0};
   buffer[0] = '#';
   buffer[1] = 'b';
   buffer[2] = 'u';
   buffer[3] = 'n';
   buffer[4] = 'd';
   buffer[5] = 'l';
   buffer[6] = 'e';
   buffer[7] = 0;

  *((uint64_t *) (buffer + 8)) = htonll(timetag);

  b->buffer = buffer;
  b->marker = buffer + 16;
  b->bufLen = len;
  b->bundleLen = 16;
}

uint32_t TinyOsc::addMessageToBundle(tosc_bundle *b,
    const char *address, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  if (b->bundleLen >= b->bufLen) return 0;
  const uint32_t i = tosc_vwrite( b->marker+4, b->bufLen-b->bundleLen-4, address, format, ap);
  va_end(ap);
  *((uint32_t *) b->marker) = htonl(i); // write the length of the message
  b->marker += (4 + i);
  b->bundleLen += (4 + i);
  return i;
}

uint32_t TinyOsc::getBundleLength(tosc_bundle *newBundle) {
  return newBundle->bundleLen;
}

*/





