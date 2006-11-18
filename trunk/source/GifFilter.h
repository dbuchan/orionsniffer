#pragma once

#include "packetfilter.h"
#include "BlockParser.h"

#define spaceleft       if (block > data + len) return gifhdr

class GifFilter :
	public PacketFilter
{
private:
	

public:

	GifFilter(void) : PacketFilter("GIF")
	{
	}

	~GifFilter(void)
	{
	}

	unsigned char *parseBlock(const unsigned char *data, const int len, unsigned char **gifdata, int *giflen) {
		    unsigned char *gifhdr, *block;

//			BlockParser::debug(data,len);
    unsigned int gotimgblock = 0;
    unsigned int ncolours;
 
	*gifdata = NULL;

    if (len < 6) return (unsigned char*)(data + len);

	gifhdr = BlockParser::memstr_full(data, len, (unsigned char *)"GIF89a", 6);
    if (!gifhdr) gifhdr = BlockParser::memstr_full(data, len,  (unsigned char *)"GIF87a", 6);
    if (!gifhdr) return (unsigned char*)(data + len);
	
    if (data + len - gifhdr < 14) return gifhdr; /* no space for header */

    ncolours = (1 << ((gifhdr[10] & 0x7) + 1));
    /* printf("gif header %d colours\n", ncolours); */
    block = gifhdr + 13;
    if (gifhdr[10] & 0x80) block += 3 * ncolours; /* global colour table */
    spaceleft;

    do {
         /* printf("gifhdr = %p block = %p off = %u %02x\n", gifhdr, block, block - gifhdr, (unsigned int)*block); */
        switch (*block) {
            case 0x2c:
                /* image block */
                /* printf("image data\n"); */
                if (block[9] & 0x80)
                    /* local colour table */
                    block += 3 * ((1 << ((gifhdr[9] & 0x7) + 1)));
                block += 10;
                ++block;        /* lzw code size */
                do {
                    spaceleft;
                    block += *block + 1;
                    spaceleft;
                } while (*block);
                ++block;
                spaceleft;
                gotimgblock = 1;

                break;

            case 0x21:
                /* extension */
                ++block;
                spaceleft;
                if (*block == 0xf9) {
                    /* graphic control */
                    /* printf("graphic control\n"); */
                    ++block;
                    spaceleft;
                    block += *block + 2;
                    spaceleft;
                    break;
                } else if (*block == 0xfe) {
                    /* comment */
                    /* printf("comment\n"); */
                    ++block;
                    do {
                        spaceleft;
                        block += *block + 1;
                        spaceleft;
                    } while (*block);
                    ++block;
                    spaceleft;
                } else if (*block == 0x01) {
                    /* text label */
                    /* printf("text label\n"); */
                    ++block;
                    spaceleft;
                    if (*block != 12) return gifhdr + 6;
                    block += 13;
                    do {
                        spaceleft;
                        block += *block + 1;
                        spaceleft;
                    } while (*block);
                    ++block;
                    spaceleft;
                } else if (*block == 0xff) {
                    /* printf("application extension\n"); */
                    ++block;
                    spaceleft;
                    if (*block != 11) return gifhdr + 6;
                    block += 12;
                    do {
                        spaceleft;
                        /* printf("app extension data %d bytes\n", (int)*block); */
                        block += *block + 1;
                        spaceleft;
                    } while (*block);
                    ++block;
                    spaceleft;
                } else {
                    /* printf("unknown extension block\n"); */
                    return gifhdr + 6;
                }
                break;
                
            case 0x3b:
                /* end of file block: we win. */
                /* printf("gif data from %p to %p\n", gifhdr, block); */
                *gifdata = gifhdr;
                *giflen = (int)(block - gifhdr + 1);
                return block + 1;
                break;

            default:
                /* printf("unknown block %02x\n", *block); */
                return gifhdr + 6;
        }
    } while (1);

	}
};
