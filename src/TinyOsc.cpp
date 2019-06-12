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


#include "Arduino.h"

/*
 based on http://stackoverflow.com/questions/809902/64-bit-ntohl-in-c
 
 if the system is little endian, it will flip the bits
 if the system is big endian, it'll do nothing
 */
template<typename T> 
static inline T tosc_bigEndian(const T& x)
{
    const int one = 1;
    const char sig = *(char*)&one;
    if (sig == 0) return x; // for big endian machine just return the input
    T ret;
    int size = sizeof(T);
    char* src = (char*)&x + sizeof(T) - 1;
    char* dst = (char*)&ret;
    while (size-- > 0){
        *dst++ = *src--;
    }
    return ret;
}




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
#endif // UTIL_H

#define tosc_strncpy(_dst, _src, _len) strncpy((char *)_dst, _src, _len)


#include "TinyOsc.h"
/*
#define BUNDLE_ID 0x2362756E646C6500L // "#bundle"
*/





// always writes a multiple of 4 bytes
/*
static size_t tosc_vwrite(unsigned char *buffer, const size_t len,
    const char *address, const char *format, va_list ap) {
  memset(buffer, 0, len); // clear the buffer
  size_t i = (size_t) strlen(address);
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
        const size_t n = (size_t) va_arg(ap, int); // length of blob
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
      case 'i': {
        if (i + 4 > len) return -3;
        const uint32_t k = (uint32_t) va_arg(ap, int);
        *((uint32_t *) (buffer+i)) = htonl(k);
        i += 4;
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

size_t TinyOsc::writeMessage(unsigned char *buffer, const size_t len, const char *address, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  const size_t  i = tosc_vwrite(buffer, len, address, format, ap);
  va_end(ap);
  return i; // return the total number of bytes written
}
*/
// always writes a multiple of 4 bytes


size_t TinyOsc::vprint(Print* output, const char *address, const char *format, va_list ap) {
  
  uint8_t nullChar = '\0';

  size_t i = strlen(address);
  if (address == NULL ) return -1;
  output->print(address);
  output->write(nullChar);
  i++; 

  // pad the size
  while ( (i % 4 ) ) {
      output->write(nullChar);
      i++; 
  }
  
  output->write(',');
  i++; 

  size_t s_len = strlen(format);
  if (format == NULL ) return -2;
  output->print(format);
  output->write(nullChar);
  i += s_len + 1;

   // pad the size
  while ( (i % 4 ) ) {
      output->write(nullChar);
      i++; 
  }


  for (int j = 0; format[j] != '\0'; ++j) {
    switch (format[j]) {
      case 'b': {
        const int n = va_arg(ap, int); // length of blob
        
        uint32_t n32 = tosc_bigEndian(n);
        uint8_t * ptr = (uint8_t *) &n32;
        output->write(ptr, 4);

        unsigned char *b = (unsigned char *) va_arg(ap, void *); // pointer to binary data
        output->write(b, n);

        i += (4 + n);
        // pad the size
        while ( (i % 4 ) ) {
            output->write(nullChar);
            i++; 
        }

        break;
      }
      case 'f': {
        double  d = (double) va_arg(ap, double);//const float v = (float) va_arg(ap, double);
        float v = d; // double might be 64 bites
        float v32 = tosc_bigEndian(v);
        uint8_t * ptr = (uint8_t *) &v32;
        //uint8_t * ptr = (uint8_t *) &v;
        output->write(ptr, 4);
        i+=4;
        break;
      }
      case 'i': {
        const uint32_t  v = (uint32_t ) va_arg(ap, int);
        uint32_t v32 = tosc_bigEndian(v);
        uint8_t * ptr = (uint8_t *) &v32;
        output->write(ptr, 4);
        i+=4;
        break;
      }
      case 's': {
        const char *str = (const char *) va_arg(ap, void *);
        s_len = strlen(str);
        output->print(str);
          output->write(nullChar);
         i += s_len + 1;

         // pad the size
        while ( (i % 4 ) ) {
            output->write(nullChar);
            i++; 
        }

        break;
      }
      case 'T': // true
      case 'F': // false
      case 'N': // nil
      case 'I': // infinitum
          break;
      default: return -4; // unsupported type
    }
  }

  return i; // return the total number of bytes written
}


void TinyOsc::writeMessage(Print* output, const char *address, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vprint(output, address, format, ap);
  va_end(ap);
}



TinyOsc::TinyOsc(){
  b = &bundle;
  o= &message;

}



// http://opensoundcontrol.org/spec-1_0
void TinyOsc::parseMessages(tOscCallbackFunction callback, unsigned char *buffer, const size_t len) {
 
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

int TinyOsc::parseMessage(unsigned char  *buffer, const size_t len) {
  // NOTE(mhroth): if there's a comma in the address, that's weird
  size_t i = 0;
  while (buffer[i] != '\0') ++i; // find the null-terimated address
  while (buffer[i] != ',') ++i; // find the comma which starts the format string
  if (i >= len) return -1; // error while looking for format string
  // format string is null terminated
  o->format = (char*)(buffer + i + 1); // format starts after comma

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
bool TinyOsc::isABundle(const unsigned char  *buffer) {
  return (strcmp( (const char*)buffer, "#bundle") == 0); //return ((*(const int64_t *) buffer) == htonll(BUNDLE_ID));
}


void TinyOsc::parseBundle(unsigned char  *buffer, const size_t len) {
  b->buffer =  buffer;
  b->marker = buffer + 16; // move past '#bundle ' and timetag fields
  b->bufLen = len;
  b->bundleLen = len;
}





bool TinyOsc::getNextMessage() {
  if ((b->marker - b->buffer) >= b->bundleLen) return false;
  size_t len = (size_t) ntohl(*((size_t *) b->marker));
  parseMessage(b->marker+4, len);
  b->marker += (4 + len); // move marker to next bundle element
  return true;
}

int32_t TinyOsc::getNextInt32() {
  // convert from big-endian (network btye order)
  const int32_t i = (int32_t) ntohl(*((uint32_t *) o->marker));
  o->marker += 4;
  return i;
}


float TinyOsc::getNextFloat() {
  // convert from big-endian (network btye order)
  const uint32_t i = ntohl(*((uint32_t *) o->marker));
  o->marker += 4;
  return *((float *) (&i));
}


const char *TinyOsc::getNextString() {
  int i = (int) strlen(o->marker);
  if (o->marker + i >= o->buffer + o->len) return NULL;
  const char *s = o->marker;
  i = (i + 4) & ~0x3; // advance to next multiple of 4 after trailing '\0'
  o->marker += i;
  return s;
}

void TinyOsc::getNextBlob( const unsigned char  **buffer, size_t *len) {
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



void TinyOsc::reset() {
  int i = 0;
  while (o->format[i] != '\0') ++i;
  i = (i + 4) & ~0x3; // advance to the next multiple of 4 after trailing '\0'
  o->marker = o->format + i - 1; // -1 to account for ',' format prefix
}




bool TinyOsc::fullMatch(const char* address) {

    return (strcmp( (const char *) o->buffer, address) == 0);
}

bool TinyOsc::fullMatch(const char* address, const char * typetags){
   return (strcmp( o->buffer, address) == 0) && (strcmp( o->format, typetags) == 0) ;
}




