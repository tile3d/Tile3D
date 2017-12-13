#pragma once

#include "TMutexLock.h"

class TScopeLock
{
public:
	TScopeLock(TMutexLock & lock) : m_lock(&lock) {
		m_lock->Lock();
	}

	~TScopeLock() {
		m_lock->Unlock();
	}

private:
	TMutexLock* m_lock;
};


