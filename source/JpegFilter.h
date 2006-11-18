/*
 * This is part of the Orion project.
 * Copyright (c) 2006 Jordan Bayliss-McCulloch.
 * Email: j.baylissmcculloch@gmail.com
 *
 * Portions are derived from the Driftnet project:
 *    Copyright (c) 2002 Chris Lightfoot. All rights reserved.
 *    Email: chris@ex-parrot.com; WWW: http://www.ex-parrot.com/~chris/
 */

#pragma once

#include "packetfilter.h"
#include "BlockParser.h"

#define jpegcount(c)    ((*(c) << 8) | *((c) + 1))

class JpegFilter :
	public PacketFilter
{
private:
	unsigned char *findNextMarker(unsigned char *d, size_t len) {
		unsigned char *end = d + len;
		while (d < end && *d != 0xff) ++d;
		if (d == end) return NULL;
		while (d < end && *d == 0xff) ++d; /* skip 0xff padding */
		if (d == end) return NULL;

		return d;
	}

	unsigned char *skipBlock(unsigned char *d, size_t len) {
		unsigned int l;
		if (len < 2) return NULL;
		l = jpegcount(d);
		if (l > len) return NULL; 
		return d + l;
	}



public:

	JpegFilter(void) : PacketFilter("JPEG")
	{
	}

	~JpegFilter(void)
	{
	}

	unsigned char *parseBlock(const unsigned char *data, const int len, unsigned char **jpegdata, int *jpeglen) {
		unsigned char *jpeghdr, *block;

		*jpegdata = NULL;

		//BlockParser::debugHex(data,len);
		if (len < 2) return (unsigned char *)data;
		jpeghdr = BlockParser::memstr_full(data, len, (unsigned char*)"\xff\xd8", 2); /* JPEG SOI marker */
		 //printf("check %p\n", jpeghdr);
		if (!jpeghdr) return (unsigned char*)(data);//+ len - 2);

		// printf("SOI marker at %p\n", jpeghdr);
    
		if (jpeghdr + 2 > data + len) return jpeghdr;
		block = findNextMarker(jpeghdr + 2, len - 2 - (jpeghdr - data));
		//printf("next block at %p\n", block);
		if (!block || (block - data) >= len) return jpeghdr;

		/* now we need to find the onward count from this place */
		while ((block = skipBlock(block + 1, len - (block - data)))) {
			/* printf("data = %p block = %p\n", data, block); */

			block = findNextMarker(block, len - (block - data));
			if (!block || (block - data) >= len) return jpeghdr;

			/* printf("got block of type %02x\n", *block); */

			if (*block == 0xda) {
				/* start of scan; dunno how to parse this but just look for end of
				* image marker. XXX this is broken, fix it! */
				block = BlockParser::memstr(block, len - (block - data), (unsigned char*)"\xff\xd9", 2);
				if (block) {
					*jpegdata = jpeghdr;
					*jpeglen = (int)(block + 2 - jpeghdr);
					return block + 2;
				} else break;
			}
		}
		/* printf("nope, no complete JPEG here\n"); */
		return jpeghdr;
	}
};
