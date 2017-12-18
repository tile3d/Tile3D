#pragma once

enum EVENT_TYPE
{
	EVENT_POLLIN = 0x01,
	EVENT_POLLOUT = 0x02,
	EVENT_POLLERR = 0x04,
	EVENT_POLLCLOSE = 0x08,
	EVENT_POLLHUP = 0x10
};

class TSocket;
class TSessionMan;
class TPollIO
{
public:
	TPollIO(int fd, TSessionMan* sessionman) { 
		m_event = 0; 
		m_fd = fd;
		m_pSessionMan = sessionman;
	}

	virtual ~TPollIO() {}

	void PermitSend();
	void ForbidSend();

	void PermitRecv();
	void ForbidRecv();

	virtual void PollIn() = 0;
	virtual void PollOut() { }
	virtual void PollError() { }
	virtual void PollClose() { }

protected:
	int m_event;
	int m_fd;
	TSessionMan * m_pSessionMan;
};

class TActiveIO : public TPollIO
{
public:
	TActiveIO(TSocket * pSocket, TSessionMan * sessionman);
	virtual ~TActiveIO();

	virtual void PollIn();
	virtual void PollOut();
	virtual void PollError();
private:
	TSocket * m_pSocket;
	int m_socktype;
};

class TPassiveIO : public TPollIO
{
public:
	TPassiveIO(TSocket * pSocket, int fd);

	virtual void PollIn();

private:
	int m_socktype;

};

class TSession;
class TNetIO : public TPollIO
{
public:
	TNetIO(int fd, TSessionMan* sessionman) : TPollIO(fd, sessionman) {

	}
	virtual ~TNetIO() {

	}

private:
	TSession * m_pSession;
};


class TStreamIO : public TNetIO
{
public:
	TStreamIO(TSession * pSession, TSocket* pSocket, TSessionMan* sessionman);
	~TStreamIO();

	virtual void PollIn();
	virtual void PollOut();
	virtual void PollClose();


};

