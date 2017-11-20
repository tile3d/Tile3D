#include "TNetInfo.h"

#ifdef PLATFORM_WIN
#include <Stat/Windows/TNetInfoImpWin.h>
#endif

TNetInfo::TNetInfo()
{
	RetrieveNetInfo();
}

TNetInfo::~TNetInfo()
{
	if (pNetInfoImp != nullptr) {
		delete pNetInfoImp;
		pNetInfoImp = nullptr;
	}
}

void TNetInfo::RetrieveNetInfo()
{
#ifdef PLATFORM_WIN
	pNetInfoImp = new TNetInfoImpWin();
#endif
	pNetInfoImp->ObtainNetInfo(m_macAddr, m_ipAddr);
}

