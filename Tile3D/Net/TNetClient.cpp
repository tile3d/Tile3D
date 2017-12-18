#include "TNetClient.h"
#include "TSocket.h"
#include "TPollIO.h"
#include "TPollMan.h"
#include <File/TConfFile.h>
#include <Sys/TSysFile.h>
#include <Common/TLog.h>

#ifdef PLATFORM_WIN
	#include "Winsock2.h"
#endif

bool TNetClient::Init(const char* conf_file)
{
	m_pConf = new TConfFile();
	if (!m_pConf->Open(conf_file)) {
		return false;
	}

	TString type = m_pConf->GetValueAsString(m_name.ToString(), "type");
	TString address = m_pConf->GetValueAsString(m_name.ToString(), "address");
	int port = m_pConf->GetValueAsInt(m_name.ToString(), "port", 0);
	int sndbuf = m_pConf->GetValueAsInt(m_name.ToString(), "so_sndbuf", 0);
	int rcvbuf = m_pConf->GetValueAsInt(m_name.ToString(), "so_revbuf", 0);
	int tcp_nodelay = m_pConf->GetValueAsInt(m_name.ToString(), "tcp_nodelay", 0);
	int so_broadcast = m_pConf->GetValueAsInt(m_name.ToString(), "so_broadcast", 0);

	if (TSysFile::StrCmpNoCase(type.ToString(), "tcp")) {
		m_pSocket = new TSocket(TSOCKET_TYPE_TCP, false, false);
	}
	else if (TSysFile::StrCmpNoCase(type.ToString(), "udp")) {
		m_pSocket = new TSocket(TSOCKET_TYPE_UDP, false, false);
	}

	if (m_pSocket == nullptr || !m_pSocket->Init()) {
		return false;
	}

	TSocketAddr sockaddr(address, port);
	m_pSocket->SetServerAddr(sockaddr);

	if (sndbuf > 0) {
		m_pSocket->SetSockOpt(SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
	}
	if (rcvbuf > 0) {
		m_pSocket->SetSockOpt(SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
	}
	
	m_pSocket->SetSockOpt(IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(tcp_nodelay));

	if (m_pSocket->GetSocketType() == TSOCKET_TYPE_UDP && so_broadcast > 0) {
		m_pSocket->SetSockOpt(SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
	}
	m_pSocket->SetNonBlock();

	m_pSocket->Connect();
	m_connectTimes++;
	m_status = STATE_CONNECTING;
	TActiveIO * pActiveIO = new TActiveIO(m_pSocket, this);
	TPollMan::GetInstance()->AddPollIO(m_pSocket->GetSocketfd(), pActiveIO);
	return true;
}


void TNetClient::OnConnect(TSocket * m_pSocket, bool success)
{
	TSocketAddr serverAddr = m_pSocket->GetServerAddr();

	TLog::Log(LOG_INFO, "Net", "TNetClient::OnConnect: IP=%s  Port=%d, fd=%d, result=%d", serverAddr.GetIPAddr().ToString(), serverAddr.GetPort(), m_pSocket->GetSocketfd(), success);
	if (m_connectTimes < 10) {
		ReConnect();
	}
	//TBD
	else {
		m_status = STATE_CONNECTERR;
	}

}

void TNetClient::ReConnect()
{
	m_pSocket->Connect();
	m_connectTimes++;
	m_status = STATE_CONNECTING;
	TActiveIO * pActiveIO = new TActiveIO(m_pSocket, this);
	TPollMan::GetInstance()->AddPollIO(m_pSocket->GetSocketfd(), pActiveIO);
}

