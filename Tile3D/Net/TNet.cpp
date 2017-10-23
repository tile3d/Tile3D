#include "TNet.h"

#ifdef PLATFORM_WIN
#include "Winsock2.h"
#endif

bool TNet::Init()
{
#ifdef PLATFORM_WIN
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return false;
	}
	return true;
#endif
	return true;
}

bool TNet::Cleanup()
{
#ifdef PLATFORM_WIN
	WSACleanup();
#endif
	return true;
}


