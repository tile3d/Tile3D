#pragma once

#include <Container/TArray.h>
#include "TPackageFile.h"

class TPackageMan
{
public:
	enum
	{
		OPEN_ENCRYPT	= 0x0001
	};

	static TPackageMan * GetInstance() {
		static TPackageMan pm;
		return &pm;
	}

	TPackageMan() {}

	//close all the pck file
	~TPackageMan() {
		CloseAllPackages();
	}

	bool OpenFilePackage(const char * pckFile, int flags);
	bool OpenFilePackage(const char * pckFile, const char * folder, int flags);
	bool OpenFilePackageInGame(const char * pckFile, int flags);
	bool OpenFilePackageInGame(const char * pckFile, const char * folder, int flags);
	bool CreateFilePackage(const char * pckFile, const char * folder, int flags);

	void CloseAllPackages();
	bool ClosePackageFile(TPackageFile * pPackage);

	int GetPackageNum() { return m_filePackages.Size(); }
	TPackageFile * GetPackageFile(int index) { return m_filePackages[index]; }

	//Get the package file using a path
	TPackageFile * GetPackageFile(const char * path);

private:
	TArray<TPackageFile*> m_filePackages;
};

