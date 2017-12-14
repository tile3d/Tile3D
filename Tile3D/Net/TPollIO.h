#pragma once


enum EVENT_TYPE
{
	EVENT_POLLIN = 0x01,
	EVENT_POLLOUT = 0x02,
	EVENT_POLLERR = 0x04,
	EVENT_POLLCLOSE = 0x08,
	EVENT_POLLHUP = 0x10
};

class TPollIO
{
public:
	TPollIO() { m_event = 0; }
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
};

class TActiveIO : public TPollIO
{
public:

};

class TPassiveIO : public TPollIO
{
public:

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

