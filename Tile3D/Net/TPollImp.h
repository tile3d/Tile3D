#pragma once

#include <Container/TArray.h>

#ifdef PLATFORM_WIN
	#include <WinSock2.h>
#else
	#include <sys/select.h>
#endif


class TPollImp
{
public:
	TPollImp() {}
	virtual ~TPollImp() {}

	virtual void Poll(int timeout) = 0;
};


class TPollWithSelect : public TPollImp
{
public:
	TPollWithSelect();
	~TPollWithSelect();
	virtual void Poll(int timeout);


private:
	fd_set m_rfd;
	fd_set m_wfd;
	fd_set m_efd;
	int m_maxfd;
	TArray<int> m_fdset;
};


#ifndef PLATFORM_WIN

class TPollWithEPoll : public TPollImp
{
public:
	TPollWithEPoll();
	~TPollWithEPoll();
	virtual void Poll(int timeout);
};



#endif