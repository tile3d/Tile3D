#pragma once

#include <atomic>
#include <Common/TTypes.h>

class TAtomicLong
{
public:
	TAtomicLong(int64 value) : m_value(value){
	}

	TAtomicLong() : m_value(0) {

	}

	int64 Get() {
		return m_value.load();
	}

	void Set(int64 value) {
		m_value.store(value);
	}

	int64 Increment() {
		return m_value++;
	}

	int64 Decrement() {
		return m_value--;
	}

	int64 Add(int value) {
		return m_value.fetch_add(value);
	}

	int64 Sub(int value) {
		return m_value.fetch_sub(value);
	}

	bool CompareAndExchange(int64 & expect, int64 value) {
		return m_value.compare_exchange_weak(expect, value);
	}

	int64 Exchange(int64 value) {
		return m_value.exchange(value);
	}

	bool IsLockFree() {
		m_value.is_lock_free();
	}
private:
	std::atomic<int64> m_value;
};








