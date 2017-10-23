#pragma once

#include "TSocketAddr.h"

class TSocketImp;
class TSocket
{
public:
	enum TSOCKET_TYPE
	{
		TSOCKET_TYPE_TCP,
		TSOCKET_TYPE_UDP
	};

	TSocket(TSOCKET_TYPE type, bool isServer, bool isListen);
	~TSocket();


	bool IsServer() { return m_bServer; }
	bool IsListenSocket() { return m_bListenSocket; }
	TSOCKET_TYPE GetSocketType() const { return m_socketType; }

	TSocketAddr& GetClientAddr() { return m_clientAddr; }
	TSocketAddr& GetServerAddr() { return m_serverAddr; }

	void SetClientAddr(TSocketAddr & socketAddr) {
		m_clientAddr = socketAddr;
	}

	void SetServerAddr(TSocketAddr & socketAddr) {
		m_serverAddr = socketAddr;
	}

	void SetSocketImp(TSocketImp* imp) {
		m_pSocketImp = imp;
	}

	bool Create();

	bool Bind();

	bool Listen();

	TSocket* Accept();

	bool Connect();

private:
	bool m_bServer;
	bool m_bListenSocket;
	TSOCKET_TYPE m_socketType;

	TSocketAddr m_clientAddr;
	TSocketAddr m_serverAddr;	
	
	TSocketImp * m_pSocketImp;
};


