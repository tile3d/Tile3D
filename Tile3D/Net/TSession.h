#pragma once

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

	int64 NextSessionID() {
		static int session_id = 0;
		return ++session_id;
	}

	int64 GetSessionID() {
		return m_sessionID;
	}

	bool SendProtocol(const TProtocol* proto);

private:
	int64 m_sessionID;
	TSocket * m_pSocket;
};

