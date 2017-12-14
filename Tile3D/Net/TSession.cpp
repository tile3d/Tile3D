#include "TSession.h"
#include "TSocket.h"
#include "TProtocol.h"

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

}