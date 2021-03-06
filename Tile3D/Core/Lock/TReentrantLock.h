#pragma once

#include <mutex>

class TReentrantLock
{
public:
	void Lock() {
		m_lock.lock();
	}

	void Unlock() {
		m_lock.unlock();
	}

	bool TryLock() {
		m_lock.try_lock();
	}

private:
	std::recursive_mutex m_lock;
};


