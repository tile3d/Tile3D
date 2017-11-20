#pragma once

#include <Stat/TNetInfoImp.h>

class TNetInfoImpWin : public TNetInfoImp
{
public:
	virtual void ObtainNetInfo(TString & macAddr, TString & ipAddr);
};
