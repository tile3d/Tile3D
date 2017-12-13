#include "TPollIO.h"

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
