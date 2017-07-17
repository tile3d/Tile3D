#include "TFileSys.h"
#include <string.h>
#include <stdio.h>
#include <Container/TString.h>

#ifdef PLATFORM_WIN
	#include <io.h>
	#include <sys/stat.h>
#endif



bool TFileSys::Initialize(const char* baseDir, const char* documentDir, const char* libraryDir, const char* tempDir)
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


inline void TFileSys::RemoveLastDirSlash(char* pDir) {
	// Get rid of last '\\'
	int nLength = (int)strlen(pDir);
	if (pDir[0] && (pDir[nLength - 1] == '\\' || pDir[nLength - 1] == '/'))
		pDir[nLength - 1] = '\0';
}

void TFileSys::GetFullPath(char * fullPath, const char * folderName, const char * fileName)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetFullPathNoBase(fullPath, baseDir, fileName);
}

void TFileSys::GetFullPath(char * fullPath, const char* fileName)
{
	GetFullPath(fullPath, m_baseDir, fileName);
}

void TFileSys::GetFullPath(TString& fullPath, const char* folderName, const char* fileName)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetFullPathNoBase(fullPath, baseDir, fileName);
}

void TFileSys::GetFullPath(TString& fullPath, const char* fileName)
{
	GetFullPathNoBase(fullPath, m_baseDir, fileName);
}

bool TFileSys::IsFileExist(const char* szFileName)
{
	if (_access(szFileName, 0) == 0)
		return true;
	return false;
}

int TFileSys::GetFileTimeStamp(const char * fileName)
{	
	struct stat fileStat;
	stat(fileName, &fileStat);
	return (int)(fileStat.st_mtime);
}

void TFileSys::GetFullPathWithUpdate(TString& fullPath, const char* fileName, bool noCheckFileExist)
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

void TFileSys::GetFullPathWithDocument(TString& fullPath, const char* fileName, bool noCheckFileExist)
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

void TFileSys::GetFullPathNoBase(char* fullPath, const char* baseDir, const char* filename)
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

void TFileSys::GetFullPathNoBase(TString& fullpath, const char* baseDir, const char* filename)
{
	char szFullPath[MAX_PATH];
	GetFullPathNoBase(szFullPath, baseDir, filename);
	fullpath = szFullPath;
}

void TFileSys::GetRelativePath(const char* fullPath, const char* folderName, char* relativePath)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileSys::GetRelativePath(const char* fullPath, char* relativePath)
{
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileSys::GetRelativePath(const char* fullPath, const char* folderName, TString& relativePath)
{
	char baseDir[MAX_PATH];
	sprintf(baseDir, "%s/%s", m_baseDir, folderName);
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}

void TFileSys::GetRelativePath(const char* fullPath, TString& relativePath)
{
	GetRelativePathNoBase(fullPath, m_baseDir, relativePath);
}


void TFileSys::GetRelativePathNoBase(const char* fullpath, const char* parentPath, TString& relativePath)
{
	char rPath[MAX_PATH];
	GetRelativePathNoBase(fullpath, parentPath, rPath);
	relativePath = rPath;
}

void TFileSys::GetRelativePathNoBase(const char* fullpath, const char* parentPath, char* relativepath)
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

bool TFileSys::GetFileTitle(const char* pFile, char* pTitle)
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

bool TFileSys::GetFileTitle(const char* pFile, TString& title)
{
	char titleBuf[MAX_PATH];
	bool bRet = GetFileTitle(pFile, titleBuf);
	if (bRet) {
		title = titleBuf;
	}
	return bRet;
}

bool TFileSys::GetFilePath(const char* pFile, char* pPath, unsigned short cbBuf)
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


bool TFileSys::GetFilePath(const char* pFile, TString& path)
{
	char pathBuf[MAX_PATH];
	bool bRet = GetFilePath(pFile, pathBuf, MAX_PATH);
	if (bRet) {
		path = pathBuf;
	}

	return bRet;
}

//	Check file extension
bool TFileSys::CheckFileExt(const char* fileName, const char* extName, int extLen, int fileNameLen)
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
bool TFileSys::ChangeFileExt(char* fileNameBuf, int bufLen, const char* newExt)
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

bool TFileSys::ChangeFileExt(TString& fileName, const char* newExt)
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

void TFileSys::RemoveExtName(TString& fileName)
{
	int iPos = fileName.ReverseFind('.');
	if (iPos >= 0)
	{
		fileName = fileName.Left(iPos);
	}
}

bool TFileSys::ContainFilePath(const char* szFileName)
{
	return strchr(szFileName, '\\') || strchr(szFileName, '/');
}


void TFileSys::NormalizeFileName(char* fileName)
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

void TFileSys::NormalizeFileName(const char* srcFileName, char* dstFileName)
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

