#include <Util/TLog.h>
#include <Sys/TSysFile.h>
#include <Core/TMemory.h>
#include "TPackageMan.h"
#include "TPackage.h"
#include "TFileDir.h"

bool TPackageMan::CreateFilePackage(const char* pckFile, const char* folder, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackage * pPackage = new TPackage();
	if (!pPackage->Create(pckFile, folder, encrypt))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "AFilePackMan::CreateFilePackage(), Can not open package [%s]", pckFile);
		return false;
	}

	m_packages.Add(pPackage);
	return true;
}


bool TPackageMan::OpenFilePackage(const char * pckFile, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackage * pPackage = new TPackage();
	if (!pPackage->Open(pckFile,  encrypt))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "TPackageMan::OpenFilePackage(), Can not open package [%s]", pckFile);
		return false;
	}

	m_packages.Add(pPackage);
	return true;
}

bool TPackageMan::OpenFilePackage(const char* pckFile, const char* folder, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackage * pPackage = new TPackage();
	if (!pPackage->Open(pckFile, folder,  encrypt, true))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "TPackageMan::OpenFilePackage(), Can not open package [%s]", pckFile);
		return false;
	}

	m_packages.Add(pPackage);
	return true;
}


TPackage* TPackageMan::GetPackage(const char* path)
{
	//	Normalize file name
	char lowPath[MAX_PATH];
	strncpy(lowPath, path, MAX_PATH);
	TSysFile::StrToLower(lowPath);
	TFileDir::GetInstance()->NormalizeFileName(lowPath);

	//	Check other packages through file path
	int count = m_packages.Size();
	for (int i = 0; i < count; i++)
	{
		TPackage* pPackage = m_packages[i];
		if (strstr(lowPath, pPackage->GetFolder()) == lowPath)
		{
			return pPackage;
		}
	}

	return NULL;
}

void TPackageMan::CloseAllPackages()
{
	int count = m_packages.Size();
	for (int i = 0; i < count; ++i) {
		TPackage * pPackage = m_packages[i];
		pPackage->Close();
		delete pPackage;
	}
	m_packages.Clear();
}

bool TPackageMan::ClosePackage(TPackage * pPackage)
{
	int count = m_packages.Size();
	for (int i = 0; i < count; ++i) {
		if (pPackage == m_packages[i]) {
			m_packages.Remove(&pPackage);
			pPackage->Close();
			delete pPackage;
			return true;
		}
	}
	return false;
}





