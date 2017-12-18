#pragma once

#include <Container/TString.h>
#include <Common/TAssert.h>
#include "TSessionMan.h"


//
//1) keepalive implementation
//2) replace TConfFile to TConf
//3) implement reconnect
//
class TProtocol;
class TConfFile;
class TSocket;
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
		STATE_CONNECTERR
	};

	TNetClient(TString & name) {
		m_sid = 0;
		m_ping = 0;
		m_status = 0;
		m_connectTimes = 0;
		m_name = name;
		m_pSocket = nullptr;
		m_pConf = nullptr;
	}

	~TNetClient() {
		//tbd
		if (m_pConf) {
			delete m_pConf;
		}
	}

	bool Init(const char* conf_file);	
	void ShutDown() {
		m_status = STATE_CLOSING;
	}

	bool SendProtocol(const TProtocol *protocol) {
		if (m_sid > 0 && m_status == STATE_CONNECTED) {
			return Send(m_sid, protocol, false);
		}
		return false;
	}

	bool UrgentSendProtocol(const TProtocol *protocol) {
		if (m_sid > 0 && m_status == STATE_CONNECTED) {
			return Send(m_sid, protocol, true);
		}
		return false;
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

	virtual void OnConnect(TSocket * pSocket, bool success);
	void OnClose();
	void ReConnect();

private:
	int m_sid;
	int m_ping;
	int m_status;
	int m_connectTimes;
	TString m_name;
	TSocket* m_pSocket;
	TConfFile * m_pConf;
};

