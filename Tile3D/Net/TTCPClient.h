#pragma once

#include <Container/TString.h>
#include "TSession.h"
#include "TProtocol.h"

class TTCPClient
{
public:
	TTCPClient(TString & serverIP, unsigned short serverPort) {
		m_serverIP = serverIP;
		m_serverPort = serverPort;
		m_pSession = nullptr;
	}

	~TTCPClient() {
		if (m_pSession != nullptr) {
			delete m_pSession;
			m_pSession = nullptr;
		}
	}

	bool Setup();

	bool SendProtocol(const TProtocol * proto) {
		m_pSession->SendProtocol(proto);
	}

	void Close();

private:
	TString m_serverIP;
	unsigned short m_serverPort;
	TSession* m_pSession;
};

