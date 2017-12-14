#pragma once


enum EVENT_TYPE
{
	EVENT_POLLIN = 0x01,
	EVENT_POLLOUT = 0x02,
	EVENT_POLLERR = 0x04,
	EVENT_POLLCLOSE = 0x08,
	EVENT_POLLHUP = 0x10
};

enum SOCK_TYPE
{
	SOCK_TYPE_NONE = 0,
	SOCK_TYPE_TCP = 1,
	SOCK_TYPE_UDP = 2,
};

class TPollIO
{
public:
	TPollIO() { 
		m_event = 0; 
		m_fd = 0;
	}

	virtual ~TPollIO() {}

	void PermitSend();
	void ForbidSend();

	void PermitRecv();
	void ForbidRecv();

	virtual void PollIn() = 0;
	virtual void PollOut() { }
	virtual void PollClose() { }

private:
	int m_event;
	int m_fd;
};

class TActiveIO : public TPollIO
{
public:
	TActiveIO(int socktype) {
		m_socktype = socktype;
	}

	virtual void PollIn();

private:
	int m_socktype;
};

class TPassiveIO : public TPollIO
{
public:
	virtual void PollIn();
};

class TSession;
class TNetIO : public TPollIO
{
public:


private:
	TSession * m_pSession;
};


class TStreamIO : public TNetIO
{
public:
	virtual void PollIn();
	virtual void PollOut();
	virtual void PollClose();
};

