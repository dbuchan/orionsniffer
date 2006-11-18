#pragma once
#include "DataHandler.h"
#include "sha1.h"

class FileOutputHandler :
	public DataHandler
{
public:

	bool dispatch(PacketFilter *recogniser, unsigned char *data, unsigned int len) {
		char fn[256];
		unsigned char digest[20];

		SHA_CTX sha;
		SHAInit(&sha);
		SHAUpdate(&sha, (BYTE *)data, len);
		SHAFinal(digest, &sha);

		char hash[255]; *hash=0;
		for (unsigned int i = 0; i < 20; i++)
		{
			char buf[3];
			sprintf(buf,"%02x", digest[i]);
			strcat(hash, buf);
		}

		sprintf(fn, "images/%s.%s", hash, recogniser->getType());
		FILE *f;
		f = fopen(fn, "rb");
		if (f) {
			fclose(f); return true;
		}

		printf("%s\t%d\n", fn, len);
		f=fopen(fn,"wb");
		if (!f) return false;
		fwrite(data, 1, len, f);
		fclose(f);

		return true;
	}

	FileOutputHandler(void)
		: DataHandler("file")
	{
	}

	~FileOutputHandler(void)
	{
	}
};
