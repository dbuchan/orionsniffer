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
