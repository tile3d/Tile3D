#pragma once

#include <Util/TTypes.h>

class TString;
class TFileDir
{
public:
	static TFileDir * GetInstance() {
		static TFileDir fs;
		return &fs;
	}

	bool Init(const char* pszBaseDir, const char* pszDocumentDir, const char* pszLibraryDir, const char* pszTempDir);

	
	const char * GetBaseDir() { return m_baseDir; }	
	const char * GetDocumentDir() { return m_documentDir; }
	const char * GetLibraryDir() { return m_libraryDir; }
	const char * GetTempDir() { return m_tempDir; }

	void GetFullPath(char * fullPath, const char * folderName, const char * fileName);
	void GetFullPath(char * fullPath, const char* fileName);
	void GetFullPath(TString& fullPath, const char* folderName, const char* fileName);
	void GetFullPath(TString& fullPath, const char* fileName);

	void GetFullPathWithUpdate(TString& fullPath, const char* fileName, bool noCheckFileExist);
	void GetFullPathWithDocument(TString& fullPath, const char* fileName, bool noCheckFileExist);

	void GetFullPathNoBase(char* fullPath, const char* baseDir, const char* filename);
	void GetFullPathNoBase(TString& fullPath, const char* baseDir, const char* filename);

	void GetRelativePath(const char* fullPath, const char* folderName, char* relativePath);
	void GetRelativePath(const char* fullPath, char* relativePath);
	void GetRelativePath(const char* fullPath, const char* folderName, TString& relativePath);
	void GetRelativePath(const char* fullPath, TString& relativePath);
	void GetRelativePathNoBase(const char* fullPath, const char* parentPath, TString& strRelativePath);
	void GetRelativePathNoBase(const char* fullPath, const char* parentPath, char* relativepath);

	bool GetFileTitle(const char* pFile, char* pTitle);
	bool GetFileTitle(const char* pFile, TString& title);

	bool GetFilePath(const char* pFile, char* pPath, unsigned short cbBuf);
	bool GetFilePath(const char* pFile, TString& path);

	bool CheckFileExt(const char* fileName, const char* extName, int extLen, int fileNameLen);
	bool ChangeFileExt(char* fileNameBuf, int bufLen, const char* newExt);
	bool ChangeFileExt(TString& fileName, const char* newExt);
	void RemoveExtName(TString& fileName);

	bool ContainFilePath(const char* fileName);

	void ConvertFileName(char* fileName);
	void ConvertFileName(const char* srcFileName, char* dstFileName);

	bool NormalizeFileName(char* szFileName);

	void RemoveLastDirSlash(char* pDir);

	bool IsFileExist(const char* fileName);

	int GetFileTimeStamp(const char * fileName);

	int GetIDFromFileName(const char * fileName);
private:
	// BaseDir is the working directory we load resource files from, and it's read-only.
	//
	// Under iOS system:
	//
	// These four directories are independent, and have different access authorities.
	// We write files into Document directory or Library directory which could be synchronized automatically by system.
	// We write temporary files like preprocessed .glsl files to TempDir.
	//
	// Under Windows system:
	//
	// BaseDir, DocumentDir and LibraryDir are same, they all pointed to the current working directory of the program.
	// And the TempDir is a sub-dir under current working directory named Temp.

	char m_baseDir[MAX_PATH];
	char m_documentDir[MAX_PATH];
	char m_libraryDir[MAX_PATH];
	char m_tempDir[MAX_PATH];
};



