#ifdef PLATFORM_WIN

#include <Sys/TSysFile.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <sys/stat.h>

int TSysFile::FileNo(FILE * fp)
{
	return _fileno(fp);
}


int TSysFile::SetFileSize(int fd, int size)
{
	return _chsize(fd, size);
}


int TSysFile::GetFileTimeStamp(const char* szFileName)
{
	struct stat fileStat;
	stat(szFileName, &fileStat);
	return (int)fileStat.st_mtime;
}

int TSysFile::StrCmpNoCase(const char * str1, const char * str2)
{
	return _stricmp(str1, str2);
}

char * TSysFile::StrToLower(char * str)
{
	return _strlwr(str);
}

char * TSysFile::StrToUpper(char * str)
{
	return _strupr(str);
}






#endif