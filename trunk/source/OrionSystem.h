#ifndef _ORIONSYSTEM_H
#define _ORIONSYSTEM_H

//#include "TcpConnectionManager.h"
//#include "PacketFilter.h"
//#include "DataHandler.h"

#include <list>

class PacketFilter;
class DataHandler;
class TcpConnectionManager;
class PacketCaptureSource;

class OrionSystem
{
public:
	OrionSystem();
	TcpConnectionManager *connectionManager;
	std::list<PacketFilter *>packetFilters;
	std::list<DataHandler *>dataHandlers;
	std::list<PacketCaptureSource *>packetCaptureSources;

	bool addPacketFilter( PacketFilter *pf);
	bool addDataHandler( DataHandler *dh);
	bool addPacketCaptureSource( PacketCaptureSource *pcs);

	void dispatchRecognisedData( PacketFilter *recogniser, unsigned char *data, unsigned int len );

	bool start();
	void stop();
	void waitFor();
};


#endif

extern OrionSystem *orionSystem;