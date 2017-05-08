#pragma once

//#include "intrin.h"
//#include "synchapi.h"
#include <Windows.h>


//TBD
//1) need wrap to support different platforms
//2) need support int64
class TInterlocked
{
public:
	static int InterlockedIncrement(volatile int * value) {
		return (int)::_InterlockedIncrement((long*)value);
	}

	static int InterlockedDecrement(volatile int * value) {
		return (int)::_InterlockedDecrement((long*)value);
	}

	static int InterlockedAdd(volatile int * value, int amount) {
		return (int)::_InterlockedExchangeAdd((long*)value, amount);
	}

	static int InterlockedExchange(volatile int * value, int exchange) {
		return (int)::_InterlockedExchange((long*)value, (long)exchange);
	}

	static int InterlockedCompareExchange(volatile int * dest, int exchange, int comparand) {
		return (int)::_InterlockedCompareExchange((long*)dest, (long)exchange, (long)comparand);
	}

	static void Lock(volatile int * value) {
		while (InterlockedExchange(value, 1)) {
			Sleep(0);
		}
	}

	static void Unlock(volatile int * value) {
		InterlockedExchange(value, 0);
	}
};

