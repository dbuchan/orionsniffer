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

#define PF_NOTFOUND (unsigned char *)(data+len);

class PacketFilter
{
private:
	char *pfType;

protected:

	PacketFilter(char *type) {
		pfType=type;
	}

	~PacketFilter(void)
	{
	}
public:

	/* input: block of data @ data, length len bytes
	   output: return the next possible location for data. ie: a pointer to the possible start of
	           the next data block. if header found, but data incompleted, return the start of the header.
			   if no header found, return data+len (PF_NOTFOUND).

			   *filterData should contain a pointer to the recognised data block, or NULL.
			   *filterDataLen should contain the length of the recognised data, or 0.
   */
	virtual unsigned char *parseBlock(	const unsigned char *data, const int len,
										unsigned char **filterData, int *filterDataLen) = 0;


	const char *getType() { 
		return pfType;
	}
};
