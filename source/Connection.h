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
