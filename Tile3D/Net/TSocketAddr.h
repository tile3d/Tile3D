#pragma once

#include <Container/TString.h>

class TSocketAddr
{
public:
	TSocketAddr() {
		m_port = 0;
	}

	TSocketAddr(TString & ipAddr, int port) {
		m_ipAddr = ipAddr;
		m_port = port;
	}

	int GetPort() const { return m_port; }
	const TString & GetIPAddr() const { return m_ipAddr; }


private:
	TString m_ipAddr;
	int m_port;
	TString m_hostname;
};

