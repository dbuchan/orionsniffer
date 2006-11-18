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

#include <iostream>
#include <pcap.h>

#include "Thread.h"
#include "PacketCaptureSource.h"

#define LIBPCAP_DEVICE_NAME		0x1
#define LIBPCAP_DEVICE_INDEX	0x2
#define LIBPCAP_PROMISC			0x3

// type-cast
#pragma warning(disable:4311)
#pragma warning(disable:4312)

class LibpcapPacketCaptureSource : public PacketCaptureSource
{
private:
	ThreadID captureThread;
	pcap_t *pc;
	unsigned int packetOffset; /* offset of IP packet within wire packet */
	const char *device;
	unsigned int promisc;
	bool capturing;
	
	char ebuf[PCAP_ERRBUF_SIZE];
	

	/* getLinkLevelHeaderLength:
	* Find out how long the link-level header is, based on the datalink layer
	* type. This is based on init_linktype in the libpcap distribution; I
	* don't know why libpcap doesn't expose the information directly. The
	* constants here are taken from 0.6.2, but I've added #ifdefs in the hope
	* that it will still compile with earlier versions. */
	unsigned int getLinkLevelHeaderLength(int type)
	{
		switch (type) {
			case DLT_EN10MB:		return 14;
			case DLT_SLIP:			return 16;
			case DLT_SLIP_BSDOS:	return 24;
			case DLT_NULL:
#ifdef DLT_LOOP
	        case DLT_LOOP:
#endif
				return 4;
			case DLT_PPP:				return 4;
			case DLT_PPP_BSDOS:			return 24;
			case DLT_FDDI:				return 21;
			case DLT_IEEE802:			return 22;
			case DLT_ATM_RFC1483:		return 8;
			case DLT_RAW:				return 0;
			default:;
		}
		std::cerr << "warning: unknown data link type " << type << std::endl;
		return 0;
	}

public:

	LibpcapPacketCaptureSource(void)
		: PacketCaptureSource("libpcap/win32")
	{
		promisc=1;
		device = getDeviceByIndex(0); 
		captureThread=NULL;
		capturing=false;
	}

	~LibpcapPacketCaptureSource(void)
	{
		pcap_close(pc);
	}

	bool initialise() {
		struct bpf_program filter;
		char *filterexpr;
		
		filterexpr = "tcp";
		

		pc = pcap_open_live(device, 65535 , 0, 1000, ebuf);
		if (!pc) {
			std::cerr << ": pcap_open_live: " << ebuf << std::endl;
			return false;
		} 
    
		if (pcap_compile(pc, &filter, filterexpr, 1, 0) == -1) {
			std::cerr << ": pcap_compile: " << pcap_geterr(pc) << std::endl;
			return false;
		}
    
		if (pcap_setfilter(pc, &filter) == -1) {
			std::cerr << ": pcap_setfilter: " << pcap_geterr(pc) << std::endl;
			return false;
		}

		/* Figure out the offset from the start of a returned packet to the data in it. */
		packetOffset = getLinkLevelHeaderLength(pcap_datalink(pc));
		return true;
	}

	// our thread
	static ThreadProc actualCaptureThread(void *param) {
		LibpcapPacketCaptureSource *me = (LibpcapPacketCaptureSource *)param;
 
		while (me->capturing) {
	        pcap_dispatch(me->pc, -1, me->processPacket, (u_char *)me);

		}
		return NULL;
	}
    
	static void processPacket(u_char *user, const struct pcap_pkthdr *hdr, const u_char *pkt) {
		LibpcapPacketCaptureSource *me = (LibpcapPacketCaptureSource *)user;
		unsigned char *packetStart = (unsigned char *)pkt + me->packetOffset;
		unsigned int packetLen = hdr->caplen;

		orionSystem->connectionManager->dispatchPacket( packetStart, packetLen );
	}

	bool startCapture() {
		capturing = true;
		captureThread = createThread( (ThreadProc)actualCaptureThread, this );
		return true;
	}
	
	bool stopCapture() {
		capturing = false;
		deleteThread(captureThread);
		captureThread = NULL;
	    pcap_close(pc);
		return true;
	}

	void waitFor() {
		if (!captureThread) return;
		waitOnThread(captureThread);
	}

	bool listDevices() {
		pcap_if_t *devpointer;

		if (pcap_findalldevs(&devpointer, ebuf) >= 0) {
			unsigned int i=0;
			while(devpointer) {
				std::cout << (i) << ": ";
				
				if (devpointer->description)	std::cout << devpointer->description;
				else							std::cout << devpointer->name;
				
				std::cout << std::endl;

				devpointer = devpointer->next;
				i++;
			}
			return true;
		}
		return false;
	}

	const char *getDeviceByIndex(unsigned int idx) {
		pcap_if_t *devpointer;

		if (pcap_findalldevs(&devpointer, ebuf) >= 0) {
			unsigned int i=0;
			while(devpointer) {
				if (i == idx) return devpointer->name;
				devpointer = devpointer->next;
				i++;
			}
		}
		return NULL;
	}

	bool setOption(long option, void *value) {
		if (option == LIBPCAP_DEVICE_NAME) {
			device = (const char *)value;
			return true;
		}

		if (option == LIBPCAP_DEVICE_INDEX) {
			// find the device
			device = getDeviceByIndex((int)value);
			return true;
		}

		if (option == LIBPCAP_PROMISC) {
			promisc = (int)value;
			return true;
		}

		return false; 
	}
	
	
	void* getOption(long option) {
		if (option == LIBPCAP_PROMISC) return (void *)promisc;
		if (option == LIBPCAP_DEVICE_NAME) return (void *)device;

		return NULL;
	}
};
