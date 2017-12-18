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


TActiveIO::TActiveIO(TSocket * pSocket, TSessionMan * sessionman) : TPollIO(pSocket->GetSocketfd(), sessionman) {
	m_pSocket = pSocket;
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

void TPassiveIO::PollIn()
{

}


TStreamIO::TStreamIO(TSession * pSession, TSocket* pSocket, TSessionMan* sessionman) : TNetIO(pSocket->GetSocketfd(), sessionman)
{

}


void TStreamIO::PollIn()
{

}

void TStreamIO::PollOut()
{

}


void TStreamIO::PollClose()
{

}

