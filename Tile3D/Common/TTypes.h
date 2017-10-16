#pragma once

#ifdef      MAX_PATH
#undef      MAX_PATH
#endif
#define     MAX_PATH        260


#define MAX_FILE_PACKAGE	0x7fffff00U

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uint64;


#define PI 3.14159265359f


enum TRANSFORMATION_STATE
{
	TS_VIEW,
	TS_WORLD,
	TS_PROJECTION,
	TS_TEXTURE0,
	TS_TEXTURE1,
	TS_TEXTURE2,
	TS_TEXTURE3,
	TS_TEXTURE4,
	TS_TEXTURE5,
	TS_TEXTURE6,
	TS_TEXTURE7
};
