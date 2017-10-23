#include "TSocketImpWin.h"
#pragma warning(disable:4996)

TSocketImpWin::TSocketImpWin(TSocket * pSocket)
{
	m_socket = INVALID_SOCKET;
	m_pSocket = pSocket;
}

TSocketImpWin::~TSocketImpWin()
{

}

bool TSocketImpWin::Create(TSocket::TSOCKET_TYPE type)
{
	if (type == TSocket::TSOCKET_TYPE_TCP) {
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
	}
	else if (type == TSocket::TSOCKET_TYPE_UDP) {
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	}
	else {
		return false;
	}

	if (m_socket == INVALID_SOCKET) {
		return false;
	}
	return true;
}

bool TSocketImpWin::Bind(int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_socket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		return false;
	}
	return true;
}

bool TSocketImpWin::Listen()
{
	if (listen(m_socket, 5) == SOCKET_ERROR) {
		return false;
	}
	return true;
}

TSocket* TSocketImpWin::Accept()
{
	SOCKET socket;
	struct sockaddr_in client_addr;
	int len;
	socket = accept(m_socket, (struct sockaddr*)&client_addr, &len);
	if (socket == INVALID_SOCKET) {
		return nullptr;
	}

	TSocket* pSocket = new TSocket(m_pSocket->GetSocketType(), m_pSocket->IsServer(), false);
	TSocketImpWin * pWinImp = new TSocketImpWin(pSocket);
	pWinImp->m_socket = socket;
	pSocket->SetSocketImp(pWinImp);
	SetSocketAddr(pSocket, socket, client_addr);
	return pSocket;
}

void TSocketImpWin::SetSocketAddr(TSocket* m_pSocket, SOCKET socket, sockaddr_in client_addr)
{
	sockaddr_in sockaddr;
	int socklen;
	getsockname(socket, (struct sockaddr*)&sockaddr, &socklen);
	int so_port = ntohs(sockaddr.sin_port);
	TString so_ipAddr = inet_ntoa(sockaddr.sin_addr);
	TSocketAddr soaddr(so_ipAddr, so_port);
	m_pSocket->SetServerAddr(soaddr);

	int client_port = ntohs(client_addr.sin_port);
	TString client_ipAddr = inet_ntoa(client_addr.sin_addr);
	TSocketAddr clientaddr(so_ipAddr, so_port);
	m_pSocket->SetClientAddr(clientaddr);
}

bool TSocketImpWin::Connect()
{
	TSocketAddr sockAddr = m_pSocket->GetServerAddr();
	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(sockAddr.GetPort());
	serverAddr.sin_addr.s_addr = htonl(inet_addr(sockAddr.GetIPAddr()));

	if (connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		return false;
	}
	return true;
}

