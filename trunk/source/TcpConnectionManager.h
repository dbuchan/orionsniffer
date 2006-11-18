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

#include <list>

#include "TcpConnection.h"
#include "BlockParser.h" //debug
#include "proto.h"

#define SNAPLEN 262144      /* largest chunk of data we accept from pcap */
#define WRAPLEN 262144      /* out-of-order packet margin */


class TcpConnectionManager
{
private:
	//std::list<TcpConnection *>connections;	
	unsigned int slotsalloc;
	TcpConnection **connections;

public:

	TcpConnectionManager(void)
	{
		slotsalloc=64;
		connections = (TcpConnection **)calloc(slotsalloc, sizeof(TcpConnection *));
	}

	~TcpConnectionManager(void)
	{
	}

	TcpConnection *allocConnection(const struct in_addr *src, const struct in_addr *dst, const short int sport, const short int dport) {
		TcpConnection *ret = new TcpConnection(src,dst,sport,dport);
		for (unsigned int i=0; i<slotsalloc; i++) {
			if (connections[i] == NULL) {
				connections[i] = ret;

				//printf("alloc-free@%d  %d\n",  i,slotsalloc);
				//printf("ALLOC %s@%d ", BlockParser::cS( *src, sport, *dst, dport),i ); fflush(stdout);
				return ret;
			}
		}

		//printf("alloc-new %d\n", slotsalloc);
//		printf("ALLOC NEW %s@%d ", BlockParser::cS( *src, sport, *dst, dport),slotsalloc ); fflush(stdout);
		connections = (TcpConnection **)realloc(connections, slotsalloc * 2 * sizeof(TcpConnection *));
		memset(connections + slotsalloc, 0, slotsalloc * sizeof(TcpConnection *));
		//C = slots + slotsalloc;
		connections[ slotsalloc ] = ret;
		slotsalloc *= 2;
		
		return ret;
	
   
	}

	TcpConnection *findConnection(const struct in_addr *src, const struct in_addr *dst, const short int sport, const short int dport) {
		for (unsigned int i=0; i<slotsalloc; i++) {
			TcpConnection *c = connections[i];
			if (c) {
				if (c->sport == sport && c->dport == dport
					&& memcmp(&(c->src), src, sizeof(struct in_addr)) == 0
					&& memcmp(&(c->dst), dst, sizeof(struct in_addr)) == 0)
					//printf("FOUND %s ", BlockParser::cS( *src, sport, *dst, dport) ); fflush(stdout);
				
					return c;
			}
		}
//		printf("NOT-FOUND %s ", BlockParser::cS( *src, sport, *dst, dport) );  fflush(stdout);
				
		return NULL;
	}

	void deleteConnection(Connection *cc) {
		if (!cc) return;

		for (unsigned int i=0; i<slotsalloc; i++) {
			TcpConnection *c = connections[i];
			
			if ( c == cc) {
				delete c;
				connections[i]=NULL;
//				printf("deleted conn @ %d\n", i);
				return;
			}	
		}
	}

	#define TIMEOUT 5

	void finaliseConnections() {  
		time_t now;
		//connection *C;
		now = time(NULL);
		for (unsigned int i=0; i<slotsalloc; i++) {
			TcpConnection *c = connections[i];

			if (c) {
				/* We discard connections which have seen no activity for TIMEOUT
				* or for which a FIN has been seen and for which there are no
				* gaps in the stream. */ 
			
				if ((now - c->last) > TIMEOUT || (c->fin && (!c->blocks || !c->blocks->next))) {
					c->parseBuffer();
//					printf("DELETE %d ", (now - c->last) > TIMEOUT);
					deleteConnection(c); 
				}
			}
		}
	}

	void dispatchPacket( unsigned char *packet /* = pkt + pkt_offset */, int packetLen  /* = hdr->caplen */) {
		struct ip ip;
		struct tcphdr tcp;
		struct in_addr s, d;
		int off, len, delta;
		
		memcpy(&ip, packet, sizeof(ip));

		memcpy(&s, &ip.ip_src, sizeof(ip.ip_src));
		memcpy(&d, &ip.ip_dst, sizeof(ip.ip_dst));
 
		memcpy(&tcp, packet + (ip.ip_hl << 2), sizeof(tcp));
		off = (ip.ip_hl << 2) + (tcp.th_off << 2);
		len = packetLen - off;

		/* XXX fragmented packets and other nasties. */
    
		/* try to find the connection slot associated with this. */
		TcpConnection *conn = findConnection( &s, &d,  ntohs(tcp.th_sport), ntohs(tcp.th_dport));
		
		/* no connection at all, so we need to allocate one. */
		if (!conn) {
			//C = alloc_connection();
			//conn = new TcpConnection( &s, &d, ntohs(tcp.th_sport), ntohs(tcp.th_dport) );

			//connections.push_back(conn);
			conn = allocConnection( &s, &d, ntohs(tcp.th_sport), ntohs(tcp.th_dport) );

			/* This might or might not be an entirely new connection (SYN flag
			* set). Either way we need a sequence number to start at. */
			conn->setSequenceNumber( ntohl(tcp.th_seq) );
		}

	    delta = 0;/*tcp.syn ? 1 : 0;*/

		/* NB (STD0007):
		*    SEG.LEN = the number of octets occupied by the data in the
		*    segment (counting SYN and FIN) */
	#if 0
		if (tcp.syn)
			/* getting a new isn. */
			c->isn = htonl(tcp.seq);
	#endif

		if (tcp.th_flags & TH_RST) {
			/* Looks like this connection is bogus, and so might be a
			* connection going the other way. */
			deleteConnection(conn);
			//delete conn;

			if ((conn = findConnection(&d, &s, ntohs(tcp.th_dport), ntohs(tcp.th_sport)))) {
				//delete conn;
				deleteConnection(conn);
			}
			return;
		}

		if (len > 0) {
			/* We have some data in the packet. If this data occurred after
			* the first data we collected for this connection, then save it
			* so that we can look for images. Otherwise, discard it. */
			unsigned int offset;
        
			offset = ntohl(tcp.th_seq);

			/* Modulo 2**32 arithmetic; offset = seq - isn + delta. */
			if (offset < (conn->isn + delta))
				offset = 0xffffffff - (conn->isn + delta - offset);
			else
				offset -= conn->isn + delta;
        
			if ((int)offset > conn->len + WRAPLEN) {
				/* Out-of-order packet. */
			} else {
//				printf("offset = %ld, off=%ld, len=%d\n", offset,off,len);
				// off = start of data, offset=overall offset in connection, len=len of actual data.
				conn->pushData(  packet + off, offset, len );
				conn->parseBuffer();
				//connection_push(c, pkt + off, offset, len);
				//connection_extract_data(c);//, extract_type);
				// TODO
			}
		}

		if (tcp.th_flags & TH_FIN) {
			/* Connection closing; mark it as closed, but let sweep_connections
			* free it if appropriate. */
			conn->setFin(1);
			//c->fin = 1;
		}

		/* sweep out old connections */
		//sweep_connections();
		finaliseConnections();
	}
};
