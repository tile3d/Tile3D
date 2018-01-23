#include "TPollIO.h"
#include "TSocket.h"
#include "TSession.h"
#include "TSessionMan.h"
#include "TPollMan.h"

void TPollIO::PermitSend()
{
	if (m_event & EVENT_POLLOUT) {
		return;
	}

	m_event |= EVENT_POLLOUT;
}


void TPollIO::ForbidSend()
{
	if ( (m_event & EVENT_POLLOUT) == 0) {
		return;
	}

	m_event &= ~EVENT_POLLOUT;
}


void TPollIO::PermitRecv()
{
	if (m_event & EVENT_POLLIN) {
		return;
	}

	m_event |= EVENT_POLLIN;
}

void TPollIO::ForbidRecv()
{
	if ((m_event & EVENT_POLLIN) == 0) {
		return;
	}

	m_event &= ~EVENT_POLLIN;
}


TActiveIO::TActiveIO(TSocket * pSocket, TSessionMan * sessionman) : TPollIO(pSocket->GetSocketfd(), pSocket, sessionman) {
	m_socktype = m_pSocket->GetSocketType();
	m_event |= (EVENT_POLLIN | EVENT_POLLOUT | EVENT_POLLERR);
}


TActiveIO::~TActiveIO() {

}

void TActiveIO::PollIn()
{

}

void TActiveIO::PollOut()
{
	if (m_socktype == TSOCKET_TYPE_TCP) {
		TSession * pSession = new TSession(m_pSocket);
		TStreamIO * pStreamIO = new TStreamIO(pSession, m_pSocket, m_pSessionMan);
		pStreamIO->PermitRecv();
		TPollMan::GetInstance()->RemovePollIO(m_fd, this);
		TPollMan::GetInstance()->AddPollIO(m_fd, pStreamIO);
		m_pSessionMan->AddSession(pSession->GetSessionID(), pSession);
		m_pSessionMan->OnConnect(m_pSocket, true);
		delete this;
	}
	//TBD
	else if (m_socktype == TSOCKET_TYPE_UDP) {

	}
}

void TActiveIO::PollError()
{
	if (m_pSocket != nullptr) {
		delete m_pSocket;
	}

	m_pSessionMan->OnConnect(m_pSocket, false);
	delete this;
}


TPassiveIO::TPassiveIO(TSocket * pSocket, TSessionMan * sessionman) : TPollIO(pSocket->GetSocketfd(), pSocket, sessionman)
{

}

void TPassiveIO::PollIn()
{

}


TStreamIO::TStreamIO(TSession * pSession, TSocket* pSocket, TSessionMan* sessionman) : TNetIO(pSocket->GetSocketfd(), pSocket, sessionman)
{

}

TStreamIO::~TStreamIO()
{

}

void TStreamIO::PollIn()
{
	TOctets & ibuf = m_pSession->GetInputBuffer();
	int recv_bytes = m_pSocket->Recv(m_fd, (char*)ibuf.End(), ibuf.Capacity() - ibuf.Size(), 0);
	if (recv_bytes > 0) {
		ibuf.Resize(ibuf.Size() + recv_bytes);
		m_pSession->OnRecv();
		if (ibuf.Size() == ibuf.Capacity()) {
			ForbidRecv();
		}
	}
	else if (recv_bytes == 0) {

	}
	else if (recv_bytes == -1) {

	}

}

void TStreamIO::PollOut()
{

}


void TStreamIO::PollClose()
{

}

