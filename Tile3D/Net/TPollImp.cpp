#include "TPollImp.h"

TPollWithSelect::TPollWithSelect()
{
	m_maxfd = 0;
}

TPollWithSelect::~TPollWithSelect()
{

}


void TPollWithSelect::Poll(int timeout)
{
	int active_fds = 0;

	if (timeout < 0) {
		active_fds = select(m_maxfd + 1, &m_rfd, &m_wfd, &m_efd, nullptr);
	}
	else {
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;
		active_fds = select(m_maxfd + 1, &m_rfd, &m_wfd, &m_efd, &tv);
	}


}
