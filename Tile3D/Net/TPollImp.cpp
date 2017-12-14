#include "TPollImp.h"
#include "TPollMan.h"
#include "TPollIO.h"

TPollIO* TPollImp::FindPollIO(int fd)
{
	return TPollMan::GetInstance()->FindPollIO(fd);
}


TPollWithSelect::TPollWithSelect()
{
	m_maxfd = 0;
	FD_ZERO(&m_rfd);
	FD_ZERO(&m_wfd);
	FD_ZERO(&m_efd);

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

	//0 means nothing intersting happens
	//-1 means something bad happens
	//TBD: handle -1 case
	if (active_fds <= 0) {
		return;
	}

	for (int i = 0; i < m_fdset.Size(); i++) {
		TPollIO* pPollIO = FindPollIO(m_fdset[i]);
		if (pPollIO == nullptr) continue;
		if (FD_ISSET(m_fdset[i], &m_rfd)) {
			pPollIO->PollIn();
		}

		if (FD_ISSET(m_fdset[i], &m_wfd)) {
			pPollIO->PollOut();
		}

		//TBD
		if (FD_ISSET(m_fdset[i], &m_efd)) {
					
		}
	}
}

