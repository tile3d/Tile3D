#pragma once

#include <Container/TString.h>

class THardwareInfoImp
{
public:
	virtual void ObtainCPUInfo(TString & cpuInfo) {}
	virtual int ObtainMemorySize() { return 0; }
	virtual void ObtainVideoCardInfo(TString & cardName) {}

};