#pragma once

#include "PacketFilter.h"
#include "BlockParser.h"

#define PNG_HDR_BIN "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"
#define PNG_HDR_LEN 8

#define PNG_FTR_BIN "\x49\x45\x4E\x44\xAE\x42\x60\x82"
#define PNG_FTR_LEN 8

class PngFilter :
	public PacketFilter
{
public:

	PngFilter(void) : PacketFilter("PNG")
	{
	}

	~PngFilter(void)
	{
	}

	unsigned char *parseBlock(const unsigned char *data, const int len, unsigned char **pngdata, int *pnglen) {
		unsigned char *pnghdr, *pngend=NULL;
		*pngdata = NULL;
		if (len < PNG_HDR_LEN) return (unsigned char*)data;
		pnghdr = BlockParser::memstr(data, len, (unsigned char *)PNG_HDR_BIN, PNG_HDR_LEN);	
		if (!pnghdr) {
			//return (unsigned char*)(data+len);
			return PF_NOTFOUND;
		}
	
		pngend = BlockParser::memstr_full(pnghdr, len - (pnghdr-data), (unsigned char *)PNG_FTR_BIN,PNG_FTR_LEN);
		
		if (!pngend) { 
			return pnghdr;
		}
	
		pngend += PNG_FTR_LEN;

		*pngdata = pnghdr; 
		*pnglen = (int)(pngend - pnghdr);

		return pngend;
	}
};
