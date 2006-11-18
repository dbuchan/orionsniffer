/*
 * This is part of the Orion project.
 * Copyright (c) 2006 Jordan Bayliss-McCulloch.
 * Email: j.baylissmcculloch@gmail.com
 *
 * Portions are derived from the Driftnet project:
 *    Copyright (c) 2002 Chris Lightfoot. All rights reserved.
 *    Email: chris@ex-parrot.com; WWW: http://www.ex-parrot.com/~chris/
 */

#include "Platform.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <iostream>

#include "OrionSystem.h"
#include "TcpConnection.h"
#include "TcpConnectionManager.h"
#include "JpegFilter.h"
#include "PngFilter.h"
#include "GifFilter.h"
#include "ConsoleDataHandler.h"
#include "FileOutputHandler.h"
#include "LibpcapPacketCaptureSource.h"

// This shold really be a singleton
OrionSystem *orionSystem = new OrionSystem();


int main(int argc, char *argv[]) {

	LibpcapPacketCaptureSource *libpcap = new LibpcapPacketCaptureSource();
 
	if (argc != 2) {
		std::cout << "usage: orion DEVICE_INDEX (see below:)" << std::endl;
		libpcap->listDevices();
		return 0;
	}
	
   	orionSystem->addPacketFilter( new JpegFilter() );
	orionSystem->addPacketFilter( new GifFilter() );
	orionSystem->addPacketFilter( new PngFilter() );
	
	orionSystem->addDataHandler( new ConsoleDataHandler() );
	orionSystem->addDataHandler( new FileOutputHandler() );

	
	const char *device = libpcap->getDeviceByIndex( atoi(argv[1]) );
	if (!device) {
		std::cerr << "Could not find device index " << atoi(argv[1]) << "!" << std::endl;
		return 1;
	}
	std::cout << "listening on " << device << std::endl;
	libpcap->setOption( LIBPCAP_DEVICE_NAME, (void *)device );
	orionSystem->addPacketCaptureSource( libpcap );

	orionSystem->start();
	orionSystem->waitFor();
	orionSystem->stop();
  
    return 0;
}
