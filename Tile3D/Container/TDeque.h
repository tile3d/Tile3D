#pragma once

#pragma once

#include "TList.h"


//TBD
//1) Implement Enqueue(const T && value) version
//2) Consider Dequeue version
//
//
template<typename T> class TDeque
{
public:
	TDeque() {}

	bool AddHead(const T & value) {
		return m_list.AddHead(value);
	}


	bool AddTail(const T & value) {
		return m_list.AddTail(value);
	}


	bool PopHead(const T & value) {
		TListNode * pNode = m_list.GetHead();
		if (pNode == nullptr) {
			return false;
		}

		value = pNode->m_value;
		m_list.RemoveHead();
		return true;
	}


	bool Peek(T & value) const {
		TListNode * pNode = m_list.GetHead();
		if (pNode == nullptr) {
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	bool IsEmpty() {
		return m_list.IsEmpty();
	}

	void Clear() {
		return m_list.Clear();
	}

private:
	/** Hidden copy constructor. */
	TDeque(const TDeque&) = delete;

	/** Hidden assignment operator. */
	TDeque& operator=(const TDeque&) = delete;


private:

	TList<T> m_list;

};

