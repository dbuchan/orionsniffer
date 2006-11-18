/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

// To make MSVC 2005 happy :)
#if _MSC_VER >= 1400
#pragma warning (disable: 4996)
#endif

// Include some standard files
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define _WIN32_WINNT 0x0500
#  ifndef WINVER
#     define WINVER 0x0500
#  endif
#  include <windows.h>
#else
#  include <stdio.h>
#endif

//#include <limits.h>
#include <float.h>


// Define some sized types
typedef unsigned char uint8;
typedef   signed char  int8;

typedef unsigned short uint16;
typedef   signed short  int16;

typedef unsigned int uint32;
typedef   signed int  int32;

#include <stddef.h>
typedef ptrdiff_t intptr;

#ifdef _WIN32
	typedef   signed __int64  int64;
	typedef unsigned __int64 uint64;
#else
	typedef   signed long long  int64;
	typedef unsigned long long uint64;
#endif

typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;


#ifdef _DEBUG
#  define DEBUG
#endif


// Define some useful macros
#define MCHAR2(a, b) (a | (b << 8))
#define MCHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

#define _STR_(x) #x
#define DEFINE_STR(x) "#define " #x " " _STR_(x) "\n"

#define elementsOf(x) (sizeof(x) / sizeof(x[0]))
#define offsetOf(strct, member) uint(((strct *) NULL)->member)

#ifdef _WIN32
#define forceinline __forceinline
#define alignment(x) __declspec(align(x))
#else
#define forceinline inline
#define alignment(x) __attribute__ ((aligned(x)))
#endif


#ifdef _WIN32

#ifdef _MSC_VER
// Ensure proper handling of for-scope
#  if (_MSC_VER <= 1200)
#    define for if(0); else for
#  else
#    pragma conform(forScope, on)
#    pragma warning(disable: 4258)
#  endif
#endif

#else

#define stricmp(a, b) strcasecmp(a, b)

#endif // LINUX

/* TODO: Replace #pragma once with standard '#ifndef' */

#endif // _PLATFORM_H_
