#pragma once

#include <mutex>

class TMutexLock
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
	std::mutex m_lock;
};


