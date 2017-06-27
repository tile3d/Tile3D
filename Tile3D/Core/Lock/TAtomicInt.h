#pragma once

#include <atomic>

class TAtomicInt
{
public:
	TAtomicInt(int value) : m_value(value){
	}

	TAtomicInt() : m_value(0) {

	}

	int Get() {
		return m_value.load();
	}

	void Set(int value) {
		m_value.store(value);
	}

	int Increment() {
		return m_value++;
	}

	int Decrement() {
		return m_value--;
	}

	int Add(int value) {
		return m_value.fetch_add(value);
	}

	int Sub(int value) {
		return m_value.fetch_sub(value);
	}

	int CompareAndExchange(int & expect, int value) {
		return m_value.compare_exchange_weak(expect, value);
	}

	int Exchange(int value) {
		return m_value.exchange(value);
	}

	bool IsLockFree() {
		m_value.is_lock_free();
	}
private:
	std::atomic<int> m_value;
};








