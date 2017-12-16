#ifdef PLATFORM_ANDROID

#include <Sys/TSysFile.h>
#include <stdio.h>
#include <string.h>

int TSysFile::FileNo(FILE * fp)
{
	return fileno(_File);
}

int TSysFile::GetFileTimeStamp(const char* szFileName)
{
	struct stat fileStat;
	stat(szFileName, &fileStat);
	return fileStat.st_mtime;
}

int TSysFile::SetFileSize(int fd, int size)
{
	return ftruncate(fd, size);
}

int TSysFile::StrCmpNoCase(const char * str1, const char * str2)
{
	return strcasecmp(sz1, sz2);
}

char * TSysFile::StrToLower(char * str)
{
	char* prt = str;
	while (*prt != '\0') {
		if (isupper(*prt))
		{
			*prt = tolower(*prt);
		}
		prt++;
	}
	return str;
}

char * TSysFile::StrToUpper(char * str)
{
	char* prt = str;
	while (*prt != '\0') {
		if (islower(*prt))
		{
			*prt = toupper(*prt);
		}
		prt++;
	}
	return str;
}

int TSysFile::Access(const char* filename, int mode)
{
	return access(filename, mode);
}

#endif