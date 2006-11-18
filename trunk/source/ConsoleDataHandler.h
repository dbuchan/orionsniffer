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
#include "datahandler.h"

class ConsoleDataHandler :
	public DataHandler
{
public:

	bool dispatch(PacketFilter *recogniser, unsigned char *data, unsigned int len) {
		printf("Recognised data: type=%s, len=%d\n", recogniser->getType(), len);
		return true;
	}

	ConsoleDataHandler(void)
		: DataHandler("console")
	{
	}

	~ConsoleDataHandler(void)
	{
	}
};
