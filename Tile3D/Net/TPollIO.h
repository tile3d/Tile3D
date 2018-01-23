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
	TPollIO(int fd, TSocket* pSocket, TSessionMan* sessionman) {
		m_event = 0; 
		m_fd = fd;
		m_pSocket = pSocket;
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
	TSocket * m_pSocket;
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
	int m_socktype;
};

class TPassiveIO : public TPollIO
{
public:
	TPassiveIO(TSocket * pSocket, TSessionMan * sessionman);

	virtual void PollIn();

private:
	int m_socktype;

};

class TSession;
class TNetIO : public TPollIO
{
public:
	TNetIO(int fd, TSocket* pSocket, TSessionMan* sessionman) : TPollIO(fd, pSocket, sessionman) {

	}
	virtual ~TNetIO() {

	}

protected:
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

