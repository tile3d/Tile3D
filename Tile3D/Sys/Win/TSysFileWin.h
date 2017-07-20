#pragma once

#include <stdio.h>

class TSysFile
{
public:
	static int FileNo(FILE * fp);
	static int SetFileSize(int fd, int size);
	static int StrCmpNoCase(const char * str1, const char * str2);
};
