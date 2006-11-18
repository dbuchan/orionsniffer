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
