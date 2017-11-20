#pragma once

#include <Container/TString.h>

class TNetInfoImp;
class TNetInfo
{
public:
	static TNetInfo* GetInstance() {
		static TNetInfo netInfo;
		return &netInfo;
	}

	TNetInfo();
	~TNetInfo();

	TString& GetMacAddr() { return m_macAddr; }
	TString& GetIPAddr() { return m_ipAddr; }

private:
	void RetrieveNetInfo();

private:
	TString m_macAddr;
	TString m_ipAddr;
	TNetInfoImp * pNetInfoImp;
};

