#include "TNet.h"
#include <Common/TLog.h>

#ifdef PLATFORM_WIN
#include "Winsock2.h"
#endif

bool TNet::Init()
{
#ifdef PLATFORM_WIN
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		TLog::Log(LOG_ERR, "Net", " TNet::Init,  fail to init the network");
		return false;
	}
	return true;
#endif
	TLog::Log(LOG_INFO, "Net", " TNet::Init,  success to init the network");
	return true;
}

bool TNet::Cleanup()
{
#ifdef PLATFORM_WIN
	WSACleanup();
#endif
	return true;
}


