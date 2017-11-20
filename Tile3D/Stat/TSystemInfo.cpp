#include "TSystemInfo.h"
#include "TSystemInfoImp.h"

#ifdef PLATFORM_WIN
#include <Stat/Windows/TSystemInfoImpWin.h>
#endif

TSystemInfo::TSystemInfo()
{
	m_is64Bit = false;
	RetrieveSystemInfo();
}

TSystemInfo::~TSystemInfo()
{
	if (pSystemInfoImp != nullptr) {
		delete pSystemInfoImp;
		pSystemInfoImp = nullptr;
	}
}

void TSystemInfo::RetrieveSystemInfo()
{
#ifdef PLATFORM_WIN
	pSystemInfoImp = new TSystemInfoImpWin();
#endif
	pSystemInfoImp->ObtainOSInfo(m_osName, m_is64Bit);
}

