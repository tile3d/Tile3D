#include "TSession.h"
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