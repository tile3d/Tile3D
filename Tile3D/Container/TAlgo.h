#pragma once


template<typename T> void TSwap(T & t1, T & t2)
{
	T t = t1;
	t1 = t2;
	t2 = t;
}