#pragma once

#include <Container/TString.h>
#include <Common/TAssert.h>
#include "TSessionMan.h"


//
//1) keepalive implementation
//
class TProtocol;
class TNetClient : public TSessionMan
{
public:
	enum
	{
		STATE_NONE,
		STATE_CONNECTING,
		STATE_CONNECTED,
		STATE_CLOSING,
		STATE_CLOSE,
	};


	TNetClient(TString & name) {
		m_sid = 0;
		m_ping = 0;
		m_status = 0;
		m_name = name;

		Init();
	}

	bool Init();

	bool SendData(const TProtocol *protocol) {
		return Send(m_sid, protocol, false);
	}

	bool UrgentSend(const TProtocol *protocol) {
		return Send(m_sid, protocol, true);
	}

	virtual void OnAddSession(int sid) {
		m_sid = sid;
		m_status = STATE_CONNECTED;

	}

	virtual void OnDelSession(int sid) {
		TAssert(m_sid == sid);
		m_sid = 0;
		m_status = STATE_CLOSE;
	}

	void Close() {
		m_status = STATE_CLOSING;
	}

private:
	int m_sid;
	int m_ping;
	int m_status;
	TString m_name;
};

