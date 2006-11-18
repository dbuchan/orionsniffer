#pragma once

#include <list>
#include <malloc.h>
#include <time.h>

#include "OrionSystem.h"
#include "Connection.h"
#include "PacketFilter.h"

#include "proto.h"
#define NMEDIATYPES 4

 
struct datablock {
    int off, len,  dirty;
    struct datablock *next;
};

class TcpConnection :
	public Connection
{
public: // todo: add accessor methods
    struct in_addr src, dst;
    short int sport, dport;
    uint32_t isn;
    unsigned int len, off, alloc;
	unsigned char *data;
    int fin;
    time_t last;
    struct datablock *blocks;

private:
	int moff[NMEDIATYPES];
public:

	void setFin(unsigned int fin) {
		this->fin = fin;
	}

	unsigned int getFin() {
		return fin;
	}

	void setSequenceNumber( uint32_t isn) {
		this->isn = isn;
	}

	uint32_t getSequenceNumber() {
		return isn;
	}

	TcpConnection(const struct in_addr *src, const struct in_addr *dst, const short int sport, const short int dport)
		: Connection("tcp") 
	{
		memcpy(&this->src, src, sizeof(in_addr));
	    memcpy(&this->dst, dst, sizeof(in_addr));
		this->sport = sport;
		this->dport = dport;
		this->alloc = 16384;
		this->data = (unsigned char *)malloc(this->alloc);
		this->last = time(NULL);
		this->blocks = NULL;
		this->fin = 0;
		this->isn = 0;
		this->len = 0;
		this->off = 0;

		for (unsigned int i=0; i<NMEDIATYPES; i++)
			moff[i]=0;

		

	}

	~TcpConnection(void) 	{
		if (data) {
			free(data);
			data=NULL;
		}
	}

	void pushData( const unsigned char *data, unsigned int off, unsigned int len) {
		struct datablock *B, *b, *bl;//, BZ = {0};
		int a; 

		
//		assert(this->alloc > 0);
		if (off + len > this->alloc) {
			while (off + len > this->alloc) {
				this->alloc *= 2;
			}
//			printf("alloc = %d mb, off=%d, len=%d\n", this->alloc / (1024*1024), off, len );

			this->data = (unsigned char*)realloc(this->data, this->alloc);
//			printf("realloc data=%ld\n", this->data);
		}

		if (this->data == 0) {
			printf("ABORT: data ptr = 0! This is impossible!");
		}

		memcpy(this->data + off, data, len);

		if (off + len > this->len) this->len = off + len;
		this->last = time(NULL);
    
		B = new datablock;
		
		B->off = off;
		B->len = len;
		B->dirty = 1;
		B->next = NULL;
    
		/* Insert the block into the sorted block list. */
		for (b = this->blocks, bl = NULL; ; bl = b, b = b->next) {
			if ((!b || off <= b->off) && (!bl || off > bl->off)) {
				B->next = b;
				if (bl)
					bl->next = B;
				else
					this->blocks = B;
				break;
			}
		}

		/* Now go through the list combining adjacent blocks. */
		
		do {
			a = 0;
			for (b = this->blocks; b; b = b->next) {
				if (b->next && b->off + b->len >= b->next->off) {
					struct datablock *bb;
					bb = b->next;
					b->len = (bb->off + bb->len) - b->off;
					b->next = bb->next;
					b->dirty = 1;
					//todo: should realloc/copy b->data
					delete bb;
				    ++a;
			    }
		    }
	    } while (a);
		
	}

	void parseBuffer() {
		struct datablock *b;
		bool datachanged = false;
		/* Walk through the list of blocks and try to extract media data from
		* those which have changed. */
		for (b = this->blocks; b; b = b->next) {
			if (b->len > 0 && b->dirty) {
				datachanged = true;
				b->dirty = 0;
			}
		}

		std::list< PacketFilter *>::iterator it;
		unsigned int i=0;
		// try each filter
		for (it=orionSystem->packetFilters.begin(); it != orionSystem->packetFilters.end(); it++) {
			PacketFilter *pf = *(it);

			unsigned char *ptr, *oldptr, *media;
			int mlen; 
//			printf(" V=%d\n", moff[i]);

			// look for all media sections in the block.
			ptr = this->data + moff[i];
			oldptr = NULL;
			while (ptr != oldptr && ptr < this->data + this->len) {
				oldptr = ptr;
				
				media=NULL;
				int bitlen=len;
				bitlen = (len - moff[i]);
				ptr = pf->parseBlock( ptr, bitlen, &media, &mlen);
 
				if (media) {
					orionSystem->dispatchRecognisedData( pf, media, mlen);
				}
				moff[i] = (int)(ptr - this->data);
			}
					
			i++;
		}
	}
};
