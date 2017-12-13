#include "TSession.h"
#include "TSocket.h"
#include "TProtocol.h"

bool TSession::SendProtocol(const TProtocol* proto)
{
	TOctetsStream os;
	proto->Encode(os);

	m_lock.Lock();
	m_queue.Enqueue(os);
	m_lock.Unlock();
	return true;
}


void TSession::SendReady()
{

}