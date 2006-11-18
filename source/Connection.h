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

class Connection
{
private:
	char *connType;
public:
	Connection(char *type) 	{
		connType = type;
	}

	virtual void pushData( const unsigned char *data, unsigned int off, unsigned int len)=0;

	virtual void parseBuffer()=0;

	virtual ~Connection() {};
};
