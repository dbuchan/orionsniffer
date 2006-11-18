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
