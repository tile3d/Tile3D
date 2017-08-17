#include "TFileDir.h"
#include <string.h>
#include <stdio.h>
#include <Container/TString.h>

#ifdef PLATFORM_WIN
	#include <io.h>
	#include <sys/stat.h>
#endif



bool TFileDir::Initialize(const char* baseDir, const char* documentDir, const char* libraryDir, const char* tempDir)
{
	strncpy(m_baseDir, baseDir, MAX_PATH);
	RemoveLastDirSlash(m_baseDir);

	strncpy(m_documentDir, documentDir, MAX_PATH);
	RemoveLastDirSlash(m_documentDir);

	strncpy(m_libraryDir, libraryDir, MAX_PATH);
	RemoveLastDirSlash(m_libraryDir);

	strncpy(m_tempDir, tempDir, MAX_PATH);
	RemoveLastDirSlash(m_tempDir);
	return true;
}


inline void TFileDir::RemoveLastDirSlash(char* pDir) {
	// Get rid of last '\\'
	int nLength = (int)strlen(pDir);
	if (pDir[0] && (pDir[nLength - 1] == '\\' || pDir[nLength - 1] == '/'))
		pDir[nLength - 1] = '\0';
}

void TFileDir::GetFullPath(char * fullPath, const char * folderName, const char * fileName)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetFullPathNoBase(fullPath, baseDir, fileName);
}

void TFileDir::GetFullPath(char * fullPath, const char* fileName)
{
	GetFullPath(fullPath, m_baseDir, fileName);
}

void TFileDir::GetFullPath(TString& fullPath, const char* folderName, const char* fileName)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetFullPathNoBase(fullPath, baseDir, fileName);
}

void TFileDir::GetFullPath(TString& fullPath, const char* fileName)
{
	GetFullPathNoBase(fullPath, m_baseDir, fileName);
}

bool TFileDir::IsFileExist(const char* szFileName)
{
	if (_access(szFileName, 0) == 0)
		return true;
	return false;
}

int TFileDir::GetFileTimeStamp(const char * fileName)
{	
	struct stat fileStat;
	stat(fileName, &fileStat);
	return (int)(fileStat.st_mtime);
}

void TFileDir::GetFullPathWithUpdate(TString& fullPath, const char* fileName, bool noCheckFileExist)
{
	TString fname = fileName;
	fname.ToLower();
	if (*m_libraryDir != '\0')
	{
		GetFullPathNoBase(fullPath, m_libraryDir, (const char*)fname);
		if (noCheckFileExist)
			return;
		if (IsFileExist(fullPath))
			return;
	}
	GetFullPathNoBase(fullPath, m_baseDir, (const char*)fname);
}

void TFileDir::GetFullPathWithDocument(TString& fullPath, const char* fileName, bool noCheckFileExist)
{
	TString strfilename = fileName;
	strfilename.ToLower();
	if (*m_documentDir != '\0')
	{
		GetFullPathNoBase(fullPath, m_documentDir, (const char*)strfilename);
		if (noCheckFileExist)
			return;
		if (IsFileExist(fullPath))
			return;
	}
	GetFullPathNoBase(fullPath, m_baseDir, (const char*)strfilename);
}

void TFileDir::GetFullPathNoBase(char* fullPath, const char* baseDir, const char* filename)
{
	fullPath[0] = '\0';

	int nStrLenName = (int)strlen(filename);
	if (nStrLenName == 0)
		return;

	//See if it is a absolute path;
	if (nStrLenName > 3)
	{
		if ((filename[1] == ':' && (filename[2] == '\\' || filename[2] == '/')) || (filename[0] == '\\' && filename[1] == '\\'))
		{
			strcpy(fullPath, filename);
			return;
		}
	}
	else
	{
		if (nStrLenName > 1 && (filename[0] == '\\' || filename[0] == '/'))
			return;
	}

	const char* pszRealfile = filename;
	// Get rid of prefix .\, so to make a clean relative file path
	if (nStrLenName > 2 && filename[0] == '.' && (filename[1] == '\\' || filename[1] == '/'))
		pszRealfile = filename + 2;

	if (baseDir[0] == '\0')
		strcpy(fullPath, pszRealfile);
	else if ((baseDir[strlen(baseDir) - 1] == '\\') || (baseDir[strlen(baseDir) - 1] == '/'))
		sprintf(fullPath, "%s%s", baseDir, pszRealfile);
	else
		sprintf(fullPath, "%s/%s", baseDir, pszRealfile);
	return;
}

void TFileDir::GetFullPathNoBase(TString& fullpath, const char* baseDir, const char* filename)
{
	char szFullPath[MAX_PATH];
	GetFullPathNoBase(szFullPath, baseDir, filename);
	fullpath = szFullPath;
}

void TFileDir::GetRelativePath(const char* fullPath, const char* folderName, char* relativePath)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileDir::GetRelativePath(const char* fullPath, char* relativePath)
{
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileDir::GetRelativePath(const char* fullPath, const char* folderName, TString& relativePath)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileDir::GetRelativePath(const char* fullPath, TString& relativePath)
{
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}


void TFileDir::GetRelativePathNoBase(const char* fullpath, const char* parentPath, TString& relativePath)
{
	char rPath[MAX_PATH];
	GetRelativePathNoBase(fullpath, parentPath, rPath);
	relativePath = rPath;
}

void TFileDir::GetRelativePathNoBase(const char* fullpath, const char* parentPath, char* relativepath)
{
	if (!fullpath || !parentPath || !relativepath)
		return;

	const char* p1 = parentPath;
	const char* p2 = fullpath;

	while (*p1 && *p2 && // Not null
		(
			(*p1 == *p2) || // Character is identical
			(*p1 >= 'A' && *p1 <= 'Z' && *p1 + 0x20 == *p2) || (*p2 >= 'A' && *p2 <= 'Z' && *p1 == *p2 + 0x20) || // Compare English character without regard to case.
			(*p1 == '\\' && (*p2 == '/' || *p2 == '\\')) || (*p1 == '/' && (*p2 == '/' || *p2 == '\\'))		// Both are / or \;
			)
		)
	{
		++p1;
		++p2;
	}

	if (*p1) // not found;
	{
		strcpy(relativepath, fullpath);
		return;
	}

	if ((*p2 == '\\') || (*p2 == '/'))
		p2++;

	strcpy(relativepath, p2);
}

bool TFileDir::GetFileTitle(const char* pFile, char* pTitle)
{
	if (!pFile || !pTitle)
		return false;

	pTitle[0] = '\0';
	if (pFile[0] == '\0')
		return true;

	const char* pTemp = pFile + strlen(pFile);
	--pTemp;
	if ('\\' == (*pTemp) || '/' == (*pTemp)) {
		return false;
	}

	while (true) {
		if ('\\' == (*pTemp) || '/' == (*pTemp))
		{
			++pTemp;
			break;
		}
		if (pTemp == pFile) break;
		--pTemp;
	}
	strcpy(pTitle, pTemp);
	return true;
}

bool TFileDir::GetFileTitle(const char* pFile, TString& title)
{
	char titleBuf[MAX_PATH];
	bool bRet = GetFileTitle(pFile, titleBuf);
	if (bRet) {
		title = titleBuf;
	}
	return bRet;
}

bool TFileDir::GetFilePath(const char* pFile, char* pPath, unsigned short cbBuf)
{
	if (!pFile || !pPath)
		return false;

	pPath[0] = '\0';
	if (pFile[0] == '\0')
		return true;

	strncpy(pPath, pFile, cbBuf);
	char* pTemp = (char *)pPath + strlen(pPath);

	--pTemp;
	while (true) {
		if ('\\' == (*pTemp) || '/' == (*pTemp))
		{
			break;
		}
		if (pTemp == pPath) break;
		--pTemp;
	}
	*pTemp = '\0';
	return true;
}


bool TFileDir::GetFilePath(const char* pFile, TString& path)
{
	char pathBuf[MAX_PATH];
	bool bRet = GetFilePath(pFile, pathBuf, MAX_PATH);
	if (bRet) {
		path = pathBuf;
	}

	return bRet;
}

//	Check file extension
bool TFileDir::CheckFileExt(const char* fileName, const char* extName, int extLen, int fileNameLen)
{
	TAssert(fileName && extName);

	if (fileNameLen < 0)
		fileNameLen = (int)strlen(fileName);

	if (extLen < 0)
		extLen = (int)strlen(extName);

	const char* p1 = fileName + fileNameLen - 1;
	const char* p2 = extName + extLen - 1;

	bool bMatch = fileNameLen > 0 && extLen > 0;

	while (bMatch && p2 >= extName && p1 >= fileName)
	{
		if (*p1 != *p2 && !(*p1 >= 'A' && *p1 <= 'Z' && *p2 == *p1 + 32) &&
			!(*p1 >= 'a' && *p1 <= 'z' && *p2 == *p1 - 32))
		{
			bMatch = false;
			break;
		}

		p1--;
		p2--;
	}

	return bMatch;
}

//	Change file extension
bool TFileDir::ChangeFileExt(char* fileNameBuf, int bufLen, const char* newExt)
{
	char szFile[MAX_PATH];
	strcpy(szFile, fileNameBuf);

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
		strcpy(pTemp, newExt);
	else
		strcat(szFile, newExt);

	int iLen = (int)strlen(szFile);
	if (iLen >= bufLen)
	{
		TAssert(iLen < bufLen);
		return false;
	}

	strcpy(fileNameBuf, szFile);
	return true;
}

bool TFileDir::ChangeFileExt(TString& fileName, const char* newExt)
{
	char fileBuf[MAX_PATH];
	strcpy(fileBuf, fileName);

	char* pTemp = strrchr(fileBuf, '.');
	if (pTemp)
		strcpy(pTemp, newExt);
	else
		strcat(fileBuf, newExt);

	fileName = fileBuf;
	return true;
}

void TFileDir::RemoveExtName(TString& fileName)
{
	int iPos = fileName.ReverseFind('.');
	if (iPos >= 0)
	{
		fileName = fileName.Left(iPos);
	}
}

bool TFileDir::ContainFilePath(const char* szFileName)
{
	return strchr(szFileName, '\\') || strchr(szFileName, '/');
}


void TFileDir::ConvertFileName(char* fileName)
{
	if (fileName)
	{
		char* p = fileName;
		while (*p)
		{
			if (*p >= 'A' && *p <= 'Z')
				*p += 32;
			else if (*p == '\\')
				*p = '/';
			p++;
		}
	}
}

void TFileDir::ConvertFileName(const char* srcFileName, char* dstFileName)
{
	if (!srcFileName || !dstFileName)
	{
		TAssert(srcFileName && dstFileName);
		return;
	}

	const char* ps = srcFileName;
	char* pd = dstFileName;
	while (*ps)
	{
		char ch = *ps++;
		if (ch >= 'A' && ch <= 'Z')
			*pd = ch + 32;
		else if (ch == '\\')
			*pd = '/';
		else
			*pd = ch;

		pd++;
	}

	*pd = '\0';
}

bool TFileDir::NormalizeFileName(char* szFileName)
{
	int i, nLength;

	nLength = strlen(szFileName);

	//	First we should unite the path seperator to '\'
	for (i = 0; i < nLength; i++)
	{
		if (szFileName[i] == '/')
			szFileName[i] = '\\';
	}

	//	Remove multi '\'
	for (i = 0; i < nLength - 1; )
	{
		if (szFileName[i] == '\\' && szFileName[i + 1] == '\\')
		{
			int j;
			for (j = i; j < nLength - 1; j++)
				szFileName[j] = szFileName[j + 1];

			szFileName[j] = '\0';
		}
		else
		{
			i++;
		}
	}

	//	Get rid of the preceding .\ string
	if (nLength > 2 && szFileName[0] == '.' && szFileName[1] == '\\')
	{
		for (i = 0; i < nLength - 2; i++)
			szFileName[i] = szFileName[i + 2];

		szFileName[i] = '\0';
	}

	//	Get rid of extra space at the tail of the string;
	nLength = strlen(szFileName);

	for (i = nLength - 1; i >= 0; i--)
	{
		if (szFileName[i] != ' ')
			break;
		else
			szFileName[i] = '\0';
	}
	return true;
}

static const unsigned long CRC32Table[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

int TFileDir::GetIDFromFileName(const char* fileName)
{
	TAssert(fileName);
	int c = 0xffffffff; /* shift register contents */
	const unsigned char* p = (const unsigned char*)fileName;

	while (*p)
	{
		unsigned char ch = *p++;
		if (ch >= 'A' && ch <= 'Z')
			ch += 32;
		else if (ch == '/')
			ch = '\\';

		c = CRC32Table[((int)c ^ (ch)) & 0xff] ^ (c >> 8);
	}
	return c ^ 0xffffffffL;
}

