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

class DataHandler
{
private:
	char *type;
public:
	const char *getType() {
		return type;
	}

	DataHandler(char *type)	{
		this->type=type;
	}


	/* return true if processed data, false if you want to continue processing */
	virtual bool dispatch(PacketFilter *recogniser, unsigned char *data, unsigned int len)=0;

	~DataHandler(void)
	{
	}
};
