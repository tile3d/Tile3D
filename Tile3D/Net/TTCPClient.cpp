#include "TTCPClient.h"
#include "TSocket.h"
#include <Common/TLog.h>


bool TTCPClient::Setup()
{
	TSocket * pSocket = new TSocket(TSocket::TSOCKET_TYPE_TCP, false, false);
	if (!pSocket->Init()) {
		TLog::Log(LOG_ERR, "NET", "TTCPClient::Setup: failed to init the tcp client socket");
		delete pSocket;
		return false;
	}

	TSocketAddr serverAddr(m_serverIP, m_serverPort);
	pSocket->SetServerAddr(serverAddr);

	if (!pSocket->Connect()) {
		TLog::Log(LOG_ERR, "NET", "TTCPClient::Setup: failed to connect the server");
		return false;
	}

	m_pSession = new TSession(pSocket);
}

