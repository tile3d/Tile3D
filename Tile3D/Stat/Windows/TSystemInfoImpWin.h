#pragma once

#include <Stat/TSystemInfoImp.h>
#include <Container/TString.h>


class TSystemInfoImpWin : public TSystemInfoImp
{
public:
	virtual void ObtainOSInfo(TString & osName, bool & is64Bit);
};