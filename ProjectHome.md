## Overview ##
Orion Sniffer (or simply, 'Orion') is capable of sniffing GIF, JPEG and PNG images from unencrypted network traffic and saving these images to disk on the fly. Written in C++, with a heavily object oriented design, Orion aims to be an easily extendable data sniffing system by providing a data filtering mechanism. A Filter recognizes specific data (eg: A GIF image) received from a Source (eg: TCP/IP packet), and an Output Handler 'does something' with the data (eg: write it to a file). Orion uses libpcap, so it can fully support the Windows 9x/NT platform. Orion was originally based on the Driftnet C GPL source code, but was ported to Windows and rewritten in C++, although parts of the Driftnet code have been reused.

## Dependencies ##
libpcap (for low level network support)

## Platform ##
Win32 (should run on Win9x and Windows NT (XP, Vista))

Compiles under MSVC 7 (includes workspace) (should compile under other versions of MSVC)