#pragma once

#include <Common/TTypes.h>
#include <Common/TOctets.h>
#include <Container/TQueue.h>
#include <Core/Lock/TAtomicInt.h>
#include <Core/Lock/TMutexLock.h>
#include "TSocket.h"

class TSocket;
class TProtocol;
class TSession
{
public:
	TSession(TSocket* pSocket) {
		m_pSocket = pSocket;
		m_sessionID = NextSessionID();
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

	bool SendProtocol(const TProtocol* proto);

	void SendReady();

private:
	int m_sessionID;
	TSocket * m_pSocket;
	TQueue<TOctets> m_queue;
	TMutexLock m_lock;
};

