#include <list>
#include <iostream>

#include "OrionSystem.h"
#include "PacketCaptureSource.h"
#include "TcpConnectionManager.h"
#include "DataHandler.h"
#include "PacketFilter.h"


OrionSystem::OrionSystem() {
	std::cout << "OrionSystem::OrionSystem\tcreating TcpConnectionManager" << std::endl;
	connectionManager = new TcpConnectionManager();
}
	
	bool OrionSystem::addPacketFilter( PacketFilter *pf) {
		std::list<PacketFilter *>::iterator it;
		for (it=packetFilters.begin(); it != packetFilters.end(); it++) {
			PacketFilter *pfa = *(it);
			if (stricmp( pfa->getType(), pf->getType()) == 0) return false;
		}
		std::cout << "OrionSystem::addPacketFilter\tcreating PacketFilter of type " << pf->getType() << std::endl;

		packetFilters.push_back( pf );
		return true;
	}

	bool OrionSystem::addDataHandler( DataHandler *dh) {
		std::list<DataHandler *>::iterator it;
		for (it=dataHandlers.begin(); it != dataHandlers.end(); it++) {
			DataHandler *dha = *(it);
			if (stricmp( dha->getType(), dh->getType()) == 0) return false;
		}
		
		std::cout << "OrionSystem::addDataHandler\tcreating DataHandler of type " << dh->getType() << std::endl;

		dataHandlers.push_back(dh);
		return true; 
	}

	bool OrionSystem::addPacketCaptureSource( PacketCaptureSource *pcs) {
		packetCaptureSources.push_back(pcs);
		pcs->initialise();

		std::cout << "OrionSystem::addPacketCaptureSource\tcreating PacketCaptureSource of type " << pcs->getType() << std::endl;

		return true;
	}

	bool OrionSystem::start() {
		std::list<PacketCaptureSource *>::iterator it;
		for (it = packetCaptureSources.begin(); it != packetCaptureSources.end();it++) {
			PacketCaptureSource *pcs = *(it);
			pcs->startCapture();
		}
		return true;
	}

	void OrionSystem::waitFor() {
		std::list<PacketCaptureSource *>::iterator it;
		for (it = packetCaptureSources.begin(); it != packetCaptureSources.end();it++) {
			PacketCaptureSource *pcs = *(it);
			pcs->waitFor();
		}
	}

	void OrionSystem::stop() {
		std::list<PacketCaptureSource *>::iterator it;
		for (it = packetCaptureSources.begin(); it != packetCaptureSources.end();it++) {
			PacketCaptureSource *pcs = *(it);
			pcs->stopCapture();
		}
	}

	void OrionSystem::dispatchRecognisedData( PacketFilter *recogniser, unsigned char *data, unsigned int len ) {
		std::list<DataHandler *>::iterator it;
		for (it = dataHandlers.begin(); it != dataHandlers.end(); it++) {
			DataHandler *dh = *(it);
			
			if (!dh->dispatch( recogniser, data, len ) ) {
				std::cerr << "dispatchRecognisedData failed for " << dh->getType() << std::endl;

			}
		}
	}


