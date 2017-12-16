#pragma once


#if PLATFORM_WIN
	#include "Win/TSysFileWin.h"
#elif PLATFORM_IOS
	#include "IOS/TSysFileIOS.h"
#elif PLATFORM_ANDROID
	#include "Android/TSysFileAndroid.h"
#endif


/*
#include <stdio.h>

class TSysFile
{
public:
	static int FileNo(FILE * fp);
	static int GetFileTimeStamp(const char* szFileName);
	static int SetFileSize(int fd, int size);
	static int StrCmpNoCase(const char * str1, const char * str2);
	static char * StrToLower(char * str);
	static char * StrToUpper(char * str);
	static int Access(const char* filename, int mode);
};

*/