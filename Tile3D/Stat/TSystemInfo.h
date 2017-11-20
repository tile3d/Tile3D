#pragma once

#include <Container/TString.h>

class TSystemInfoImp;
class TSystemInfo
{
public:
	static TSystemInfo* GetInstance() {
		static TSystemInfo systemInfo;
		return &systemInfo;
	}

	TSystemInfo();
	~TSystemInfo();

	TString& GetOSName() { return m_osName; }
	bool Is64Bit() { return m_is64Bit; }
private:
	void RetrieveSystemInfo();

private:
	TString m_osName;
	bool m_is64Bit;
	TSystemInfoImp* pSystemInfoImp;
};

