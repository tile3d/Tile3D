#pragma once

#include "TSocketAddr.h"

enum TSOCKET_TYPE
{
	TSOCKET_TYPE_TCP,
	TSOCKET_TYPE_UDP
};

class TSocketImp;
class TSocket
{
public:
	TSocket(TSOCKET_TYPE type, bool isServer, bool isListen);
	~TSocket();

	bool IsServer() { return m_bServer; }
	bool IsListenSocket() { return m_bListenSocket; }
	int GetSocketType() const { return m_socketType; }
	int GetSocketfd() const { return m_socketfd; }

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

	bool Init();

	bool Bind();

	bool Listen();

	TSocket* Accept();

	bool Connect();

	bool GetSockOpt(int option_name, int level, void * option_value, int* option_len);
	bool SetSockOpt(int option_name, int level, void * option_value, int option_len);

	void SetNonBlock();

	int Recv(int fd, char * buf, int len, int flags);

private:
	bool m_bServer;
	bool m_bListenSocket;
	int  m_socketfd;
	int m_socketType;

	TSocketAddr m_clientAddr;
	TSocketAddr m_serverAddr;	
	
	TSocketImp * m_pSocketImp;
};


