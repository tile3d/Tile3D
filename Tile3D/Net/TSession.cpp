#include "TSession.h"
#include "TSessionMan.h"
#include "TSocket.h"
#include "TProtocol.h"
#include "TPollIO.h"

bool TSession::Send(const TProtocol* proto, bool urgent)
{
	TOctetsStream os;
	proto->Encode(os);

	m_lock.Lock();
	if (urgent) {
		m_output.AddHead(os);
	}
	else {
		m_output.AddTail(os);
	}
	SendReady();
	m_lock.Unlock();
	return true;
}


void TSession::SendReady()
{
	if (m_status & (SESSION_STATUS_SENDING | SESSION_STATUS_CLOSING)) {
		return;
	}

	if (m_pPollIO != nullptr) {
		m_pPollIO->PermitSend();
	}
}

void TSession::OnOpen() {
	m_pSessionMan->OnAddSession(m_sid);
}

void TSession::OnClose() {
	m_pSessionMan->OnAddSession(m_sid);
}

void TSession::OnAbort() {
	m_pSessionMan->OnAbortSession(m_sid);
}


void TSession::OnRecv()
{
	m_isecbuf.Push(m_ibuffer.Begin(), m_ibuffer.Size());
	m_ibuffer.Clear();
	m_pPollIO->PermitRecv();
}

void TSession::OnSend()
{

}

