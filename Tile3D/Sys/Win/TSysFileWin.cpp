#ifdef PLATFORM_WIN

#include "TSysFileWin.h"
#include <stdio.h>
#include <string.h>
#include <io.h>

int TSysFile::FileNo(FILE * fp)
{
	return _fileno(fp);
}

int TSysFile::SetFileSize(int fd, int size)
{
	return _chsize(fd, size);
}

static int StrCmpNoCase(const char * str1, const char * str2)
{
	return _stricmp(str1, str2);
}








#endif