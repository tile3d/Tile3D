#pragma once

#include <Stat/THardwareInfoImp.h>

class THardwareInfoImpWin : public THardwareInfoImp
{
public:
	virtual void ObtainCPUInfo(TString & cpuInfo);
	virtual int ObtainMemorySize();
	virtual void ObtainVideoCardInfo(TString & cardName);


};