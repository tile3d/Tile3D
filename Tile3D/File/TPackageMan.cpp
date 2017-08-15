#include "TPackageMan.h"


bool TPackageMan::OpenFilePackage(const char * pckFile, int flags)
{
	bool encrypt = (flags & OPEN_ENCRYPT) ? true : false;

	TPackageFile * pPackage = new TPackageFile();
}


void TPackageMan::CloseAllPackages()
{
	int count = m_filePackages.Size();
	for (int i = 0; i < count; ++i) {
		TPackageFile * pPackage = m_filePackages[i];
		pPackage->Close();
		delete pPackage;
	}
	m_filePackages.Clear();
}

bool TPackageMan::ClosePackageFile(TPackageFile * pPackage)
{
	int count = m_filePackages.Size();
	for (int i = 0; i < count; ++i) {
		if (pPackage == m_filePackages[i]) {
			m_filePackages.Remove(&pPackage);
			pPackage->Close();
			delete pPackage;
			return true;
		}
	}
	return false;
}

