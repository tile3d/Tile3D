#pragma once

#include "TSocketAddr.h"
#include "TSocket.h"

class TSocketImp
{
public:
	virtual bool Create(TSocket::TSOCKET_TYPE type) { return false; }
	virtual bool Bind(int port) { return false; }
	virtual bool Listen() { return false; }
	virtual TSocket* Accept() { return nullptr; }
	virtual bool Connect() { return false; }
};

