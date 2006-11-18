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

class PacketSource
{
public:

	PacketSource(void)
	{

	}

	~PacketSource(void)
	{

	}

	virtual bool beginCapture()=0;
	virtual bool endCapture()=0;
};
