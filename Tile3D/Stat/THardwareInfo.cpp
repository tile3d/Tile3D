#include "THardwareInfo.h"

#ifdef PLATFORM_WIN
#include <Stat/Windows/THardwareInfoImpWin.h>
#endif


THardwareInfo::THardwareInfo()
{
	RetrieveHardwareInfo();
}


THardwareInfo::~THardwareInfo()
{
	if (pHardwareInfoImp != nullptr) {
		delete pHardwareInfoImp;
		pHardwareInfoImp = nullptr;
	}
}

void THardwareInfo::RetrieveHardwareInfo()
{
#ifdef PLATFORM_WIN
	pHardwareInfoImp = new THardwareInfoImpWin();
#endif;
	m_memorySize = pHardwareInfoImp->ObtainMemorySize();
	pHardwareInfoImp->ObtainVideoCardInfo(m_videoCardName);
}

