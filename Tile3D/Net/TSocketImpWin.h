#pragma once

#include "TSocketImp.h"
#include "Winsock2.h"

class TSocketImpWin : public TSocketImp
{
public:
	TSocketImpWin(TSocket * pSocket);
	~TSocketImpWin();

	virtual bool Create(TSocket::TSOCKET_TYPE type);
	virtual bool Bind(int port);
	virtual bool Listen();
	virtual TSocket* Accept();
	virtual bool Connect();
	virtual bool GetSockOpt(int option_name, int level, void * option_value, int* option_len);
	virtual bool SetSockOpt(int option_name, int level, void * option_value, int option_len);
private:
	void SetSocketAddr(TSocket* m_pSocket, SOCKET socket, sockaddr_in client_addr);

private:
	SOCKET m_socket;
	TSocket * m_pSocket;
};
