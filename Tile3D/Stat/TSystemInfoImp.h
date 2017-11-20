#pragma once

#include <Container/TString.h>

class TSystemInfoImp
{
public:
	virtual void ObtainOSInfo(TString & osName, bool & is64Bit) {}
};