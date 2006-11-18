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
    int off, len,  moff[NMEDIATYPES], dirty;
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
		
		for (unsigned int i=0; i<NMEDIATYPES; i++)
			B->moff[i]=0;

		B->off = off;
		B->len = len;
		B->dirty = 1;
		B->next = NULL;
    
		// gets rids of an int <> unsigned int conversion warning. :)
		// FIX: 9/10/06 5:05PM Jordan found a bug below, that would have resulted in certain blocks
		// not getting inserted into the block list.
		const int maxoff = off;
		/* Insert the block into the sorted block list. */
		for (b = this->blocks, bl = NULL; ; bl = b, b = b->next) {
			if ((!b || maxoff <= b->off) && (!bl || maxoff > bl->off)) {
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
	
		/* Walk through the list of blocks and try to extract media data from
		* those which have changed. */
		for (b = this->blocks; b; b = b->next) {
			if (b->len > 0 && b->dirty) {
				//for (int i = 0; i < NMEDIATYPES; i++) {
				std::list< PacketFilter *>::iterator it;
				unsigned int i=0;
				// try each filter
				for (it=orionSystem->packetFilters.begin(); it != orionSystem->packetFilters.end(); it++) {
					PacketFilter *pf = *(it);

					unsigned char *ptr, *oldptr, *media;
					int mlen;
//					printf(" V=%d\n", b->moff[i]);
					// look for all media sections in the block.

					ptr = this->data + b->off + b->moff[i];
					oldptr = NULL;
					while (ptr != oldptr && ptr < this->data + b->off + b->len) {
						oldptr = ptr;
						//ptr = driver[i].find_data(ptr, (b->off + b->len) - (ptr - c->data), &media, &mlen);
						// (b->off + b->len) - (ptr - this->data)
//						printf("b->len = %d, \n", b->len);
						media=NULL;
						ptr = pf->parseBlock( ptr, (int)((b->off + b->len) - (ptr - this->data)), &media, &mlen);
 
						if (media) {
							orionSystem->dispatchRecognisedData( pf, media, mlen);
							//driver[i].dispatch_data(driver[i].name, media, mlen);
						}
					}
					
					b->moff[i] = (int)(ptr - this->data - b->off);
					b->dirty = 0;
					i++;
				}
			}
		}
	}
};
