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

private:
	int m_event;
};

