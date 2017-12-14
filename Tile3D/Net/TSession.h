#pragma once

#include <Common/TTypes.h>
#include <Common/TOctets.h>
#include <Container/TQueue.h>
#include <Container/TDeque.h>
#include <Core/Lock/TAtomicInt.h>
#include <Core/Lock/TMutexLock.h>
#include "TSocket.h"

class TSocket;
class TProtocol;
class TSessionMan;
class TPollIO;
class TSession
{
public:
	enum
	{
		SESSION_STATUS_NONE = 0x0,
		SESSION_STATUS_NORMAL = 0x01,
		SESSION_STATUS_CLOSING = 0x02,
		SESSION_STATUS_SENDING = 0x04
	};

public:
	TSession(TSocket* pSocket) {
		m_pSocket = pSocket;
		m_sessionID = NextSessionID();
		m_status = SESSION_STATE_NORMAL;
	}

	~TSession() {
		if (m_pSocket != nullptr) {
			delete m_pSocket;
			m_pSocket = nullptr;
		}
	}

	int NextSessionID() {
		static TAtomicInt session_id = 0;
		return session_id.Increment();
	}

	int GetSessionID() {
		return m_sessionID;
	}

	bool Send(const TProtocol* proto, bool urgent);

	void SendReady();

private:
	int m_sessionID;
	int m_status;
	TSocket * m_pSocket;
	TPollIO * m_pPollIO;
	TDeque<TOctets> m_output;
	TSessionMan* m_pSessionMan;
	TMutexLock m_lock;
};

