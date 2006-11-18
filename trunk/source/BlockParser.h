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

class BlockParser
{
public:

	static unsigned char *memstr(const unsigned char *haystack, const size_t hlen,
                             const unsigned char *needle, const size_t nlen) {
								 /*
		int skip[256], k;

		if (nlen == 0) return (unsigned char*)haystack;

		// Set up the finite state machine we use.
		for (k = 0; k < 255; ++k) skip[k] = nlen;
		for (k = 0; k < nlen - 1; ++k) skip[needle[k]] = nlen - k - 1;

		// Do the search. 
		for (k = nlen - 1; k < hlen; k += skip[haystack[k]]) {
			int i, j;
			for (j = nlen - 1, i = k; j >= 0 && haystack[i] == needle[j]; j--) i--;
			if (j == -1) return (unsigned char*)(haystack + i + 1);
		}

		return 0;
		*/
		return memstr_full( haystack, hlen, needle, nlen );
	}

	static unsigned char *memstr_full(const unsigned char *haystack, const size_t hlen, const unsigned char *needle, const size_t nlen) {
		for (size_t i=0; i<hlen; i++) {
			size_t j=0;
			while (needle[j] == haystack[i] && i<hlen && j<nlen) {
				j++;
				i++;
			} 
			if ( j == nlen ) return (unsigned char *)(haystack + i - j);
		}
		return 0;
	}
	
	static void debug(const unsigned char *data, const unsigned int len) {
		printf("===================== %ld =====================\n", len);
		for (unsigned int i=0; i<len; i++) {
			fputc( data[i], stdout );
		}
		printf("=============================================\n");
		
	}

	static void debugHex(const unsigned char *data, const unsigned int len) {
		printf("===================== %ld =====================\n", len);
		for (unsigned int i=0; i<len; i++) {
			printf("%X ", data[i]);
			if (i % 25 == 0) printf("\n");

		}
		printf("=============================================\n");
		
	}

	static char *cS(const struct in_addr s, const unsigned short s_port, const struct in_addr d, const unsigned short d_port) {
    static char buf[50] = {0};
    sprintf(buf, "%s:%d -> ", inet_ntoa(s), (int)s_port);
    sprintf(buf + strlen(buf), "%s:%d", inet_ntoa(d), (int)d_port);
    return buf;
}

};
