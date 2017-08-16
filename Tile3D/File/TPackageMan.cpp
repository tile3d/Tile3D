#include "TPackageMan.h"
#include "TPackage.h"
#include "Util/TLog.h"

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
	if (!pPackage->Open(pckFile, folder,  encrypt))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "TPackageMan::OpenFilePackage(), Can not open package [%s]", pckFile);
		return false;
	}

	m_packages.Add(pPackage);
	return true;
}

bool TPackageMan::OpenFilePackageInGame(const char* pckFile, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackage * pPackage = new TPackage();
	if (!pPackage->Open(pckFile, encrypt))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "TPackageMan::OpenFilePackageInGame(), Can not open package [%s]", pckFile);
		return false;
	}

	m_packages.Add(pPackage);
	return true;
}


bool TPackageMan::OpenFilePackageInGame(const char* pckFile, const char* folder, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackage * pPackage = new TPackage();
	if (!pPackage->Open(pckFile, folder, encrypt))
	{
		delete pPackage;
		TLog::Log(LOG_ERR, "FILE", "TPackageMan::OpenFilePackageInGame(), Can not open package [%s]", pckFile);
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
	_strlwr(lowPath);
	TPackage::NormalizeFileName(lowPath);

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


bool TPackageMan::SetAlgorithmID(int id)
{
	switch (id)
	{
	case 111:
		m_guardByte0 = 0xab12908f;
		m_guardByte1 = 0xb3231902;
		m_maskPasswd = 0x2a63810e;
		m_checkMask = 0x18734563;
		break;

	default:
		m_guardByte0 = 0xfdfdfeee + id * 0x72341f2;
		m_guardByte1 = 0xf00dbeef + id * 0x1237a73;
		m_maskPasswd = 0xa8937462 + id * 0xab2321f;
		m_checkMask = 0x59374231 + id * 0x987a223;
		break;
	}

	return true;
}


