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

class PacketCaptureSource
{
private:
	char *type;
	
public:

	PacketCaptureSource(char *type)
	{
		this->type=type;
	}

	~PacketCaptureSource(void)
	{
	}

	const char *getType() {
		return type;
	}

	virtual bool initialise()=0;
	virtual bool startCapture()=0;
	virtual bool stopCapture()=0;
	virtual void waitFor()=0;

	virtual bool setOption(long option, void *value) { return false; }
	virtual void* getOption(long option) { return NULL; }


};
