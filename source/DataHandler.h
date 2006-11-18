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
