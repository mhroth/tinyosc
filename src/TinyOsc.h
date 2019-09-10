/**
 * By Thomas O Fredericks (tof@tofstuff.com) 
 * Base on TinyOsc by :
 * Copyright (c) 2015-2018, Martin Roth (mhroth@gmail.com)
 * https://github.com/mhroth/tinyosc
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

#include "Print.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/


typedef struct tosc_bundle {
  unsigned char* marker; // the current write head (where the next message will be written)
  unsigned char* buffer; // the original buffer
  size_t bufLen; // the byte length of the original buffer
  size_t bundleLen; // the byte length of the total bundle
} tosc_bundle;

class TinyOscMessage {
    



	public:

	char* format;  // a pointer to the format field
	unsigned char* marker;  // the current read head
	unsigned char* buffer;  // the original message data (also points to the address)
	size_t len;  // length of the buffer data



	TinyOscMessage();

	/**
	 * Returns true if the address matches.
	 */
	bool fullMatch(const char* address);

	/**
	 * Returns true if the address and type tag matches.
	 */
	bool fullMatch(const char* address, const char * typetags);


	/**
	 * Returns the next 32-bit int. Does not check buffer bounds.
	 */
	int32_t getNextInt32();


	/**
	 * Returns the next 32-bit float. Does not check buffer bounds.
	 */
	float getNextFloat();


	/**
	 * Returns the next string, or NULL if the buffer length is exceeded.
	 */
	const char* getNextString();

	/**
	 * Points the given buffer pointer to the next blob.
	 * The len pointer is set to the length of the blob.
	 * Returns NULL and 0 if the OSC buffer bounds are exceeded.
	 */
	//void getNextBlob(const unsigned char  **buffer, size_t *len);

};


class TinyOsc
{
	 public:
	 	typedef void (*tOscCallbackFunction)(TinyOscMessage&);

/*!
    @brief  Create an instance of the TinyOsc class.

    @code

    // Create an instance of the TinyOsc class.
    TinyOsc() osc;

    @endcode
*/
    TinyOsc();

 /**
 * Parse a buffer containing an OSC message or OSC bundle.
 * The contents of the buffer are NOT copied.
 * Calls the callback for every message received in a bundle or not.
 */
void parseMessages(tOscCallbackFunction callback , unsigned char *buffer, const size_t len);

 private:
     	unsigned char *buffer;
     	//int len;
     	tosc_bundle bundle;
     	//tosc_bundle* b;
     	//tosc_message* o;
     	TinyOscMessage message;
     	tOscCallbackFunction callback;
     	uint64_t timetag;
     	bool isPartOfABundle;


uint64_t parseBundleTimeTag();

void parseBundle(unsigned char  *buffer, const size_t len);

 /**
 * Parse a buffer containing an OSC message.
 * The contents of the buffer are NOT copied.
 * Returns 0 if there is no error. An error code (a negative number) otherwise.
 */
int parseMessage(unsigned char  *buffer, const size_t len);



/**
 * Returns true if the message is a bundle. False otherwise.
 */
bool isABundle(const unsigned char  *buffer);


/**
 * Parses the next message in a bundle. Returns true if successful.
 * False otherwise.
 */
bool getNextMessage();

protected :

size_t vprint(Print* output, const char *address, const char *format, va_list ap);






/**
 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
 * The entire buffer is cleared before writing.
 */
/*
size_t writeMessage(unsigned char *buffer, const size_t len, const char *address,
    const char *fmt, ...);


};

*/
/**
 * Writes an OSC packet to a buffer. Returns the total number of bytes written.
 * The entire buffer is cleared before writing.
 */
void writeMessage(Print *output, const char *address, const char *fmt, ...);


};





#endif // _TINY_OSC_
