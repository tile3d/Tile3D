#pragma once

#include <Container/TArray.h>


class TPackage;
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

	TPackageMan() {
		m_guardByte0 = 0xfdfdfeee;
		m_guardByte1 = 0xf00dbeef;
		m_maskPasswd = 0xa8937462;
		m_checkMask = 0x59374231;
	}

	//close all the pck file
	~TPackageMan() {
		CloseAllPackages();
	}

	bool CreateFilePackage(const char * pckFile, const char * folder, int flags);

	bool OpenFilePackage(const char * pckFile, int flags);
	bool OpenFilePackage(const char * pckFile, const char * folder, int flags);
	bool OpenFilePackageInGame(const char * pckFile, int flags);
	bool OpenFilePackageInGame(const char * pckFile, const char * folder, int flags);

	int GetPackageNum() { return m_packages.Size(); }
	TPackage * GetPackage(int index) { return m_packages[index]; }

	//Get the package file using a path
	TPackage * GetPackage(const char * path);

	void CloseAllPackages();
	bool ClosePackage(TPackage * pPackage);

private:
	TArray<TPackage*> m_packages;
};


