#include "TSocket.h"
#include "TSocketImp.h"

#ifdef PLATFORM_WIN
#include "TSocketImpWin.h"
#elif PLATFORM_LINUX
#include "TSocketImpLinux.h"
#endif



TSocket::TSocket(TSOCKET_TYPE type, bool isServer, bool isListen) {
	m_socketType = type;
	m_bServer = isServer;
	m_bListenSocket = isListen;
	m_pSocketImp = nullptr;
}

TSocket::~TSocket() {
	if (m_pSocketImp != nullptr) {
		delete m_pSocketImp;
		m_pSocketImp = nullptr;
	}
}


bool TSocket::Init()
{
#ifdef PLATFORM_WIN
	m_pSocketImp = new TSocketImpWin(this);
#else PLATFORM_LINUX
	m_pSocketImp = new TSocketImpLinux(this);
#endif

	return m_pSocketImp->Create(m_socketType);
}


bool TSocket::Bind()
{
	return m_pSocketImp->Bind(m_clientAddr.GetPort());
}


bool TSocket::Listen()
{
	return m_pSocketImp->Listen();
}


TSocket* TSocket::Accept()
{
	return m_pSocketImp->Accept();
}


bool TSocket::Connect()
{
	return m_pSocketImp->Connect();
}

bool TSocket::SetSockOpt(int option_name, int level, void * option_value, int option_len)
{
	return m_pSocketImp->SetSockOpt(option_name, level, option_value, option_len);
}

bool TSocket::GetSockOpt(int option_name, int level, void * option_value, int* option_len)
{
	return m_pSocketImp->GetSockOpt(option_name, level, option_value, option_len);
}
