#pragma once

#include "TList.h"


//TBD
//1) Implement Enqueue(const T && value) version
//
//
//
template<typename T> class TQueue
{
public:		
	bool Enqueue(const T & value) {
		return m_list.AddTail(value);
	}


	bool Dequeue(const T & value) {
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
	TQueue(const TQueue&) = delete;

	/** Hidden assignment operator. */
	TQueue& operator=(const TQueue&) = delete;


private:

	TList<T> m_list;

};

