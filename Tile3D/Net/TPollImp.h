#pragma once

#include <Container/TArray.h>

#ifdef PLATFORM_WIN
	#include <WinSock2.h>
#else
	#include <sys/select.h>
#endif

class TPollIO;
class TPollImp
{
public:
	TPollImp() {}
	virtual ~TPollImp() {}

	TPollIO * FindPollIO(int fd);

	virtual void Poll(int timeout) = 0;
};

//
//select problem
//1) on windows, select only support 64 fd by default (if need support more, try this: 
//	#undef FD_SETSIZE
//	#define FD_SETSIZE xxxx
//2) on linux, select only support 1024 fds, and only suport the fds number in the scope [1, 1024] (bitmask implementaiton)
//      fd_set rfd;
//      FD_ZERO(&rfd);
//		FD_SET(23, &rfd);
//		FD_SET(2256, &rfd);	 //wrong, fd must in the scope [1, 1024]
//3) performance consideration
// -> rfd/wfd/efd must reset every time [install before select, clean after select]
//
//Summary: windows client could choose select, server should not use "select"(performace problem + fd number problem)
//
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


private:

};



#endif