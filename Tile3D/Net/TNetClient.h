#pragma once

#include <Container/TString.h>
#include "TSessionMan.h"


//
//1) keepalive implementation
//
class TProtocol;
class TNetClient : public TSessionMan
{
public:
	TNetClient() {
		m_sid = 0;
		m_ping = 0;
	}

	bool Init();

	bool SendData(const TProtocol *protocol) {
		return Send(m_sid, protocol, false);
	}

	bool UrgentSend(const TProtocol *protocol) {
		return Send(m_sid, protocol, true);
	}

private:
	int m_sid;
	int m_ping;
	TString m_name;

};

