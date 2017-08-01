#pragma once

#include "Util/TAssert.h"



//TBD
//1) support node pool? (manager the list node memory by self) [done]
//2) support iterator?
//3) support Reverse
//4) support Swap
//5) support RemoveAll
//
template<typename T> class TList
{
public:
	struct TListNode
	{
	public:
		TListNode * m_pNext;
		TListNode * m_pPrev;
		T m_value;
	};

	class TListNodePool
	{
	public:
		TListNodePool(int count) {
			m_count = count;
			m_nodes = new TListNode[count];

			for (int i = 0; i < m_count - 1; ++i) {
				m_nodes[i].m_pNext = &m_nodes[i + 1];
			}

			m_nodes[m_count - 1].m_pNext = nullptr;
			m_pNext = nullptr;
		}

		~TListNodePool() {
			if (m_nodes) {
				delete[] m_nodes;
			}
		}

	public:
		TListNodePool * m_pNext;
		TListNode * m_nodes;
		int m_count;
	};

//ctor/dtor
public:
	enum TLIST_CONST
	{
		LIST_POOL_SIZE = 12,
	};

	//ctor
	TList() {
		m_pHead = nullptr;
		m_pTail = nullptr;
		m_pools = nullptr;
		m_free = nullptr;
		m_count = 0;
		m_pool_size = LIST_POOL_SIZE;
	}


	//dtor
	~TList() {
		ReleasePool();
		m_pHead = nullptr;
		m_pTail = nullptr;
		m_pools = nullptr;
		m_free = nullptr;
		m_count = 0;
	}

//method
public:
	TListNode * GetHead() const {
		return m_pHead;
	}

	TListNode * GetTail() const {
		return m_pTail;
	}

	//Add the element to the head
	bool AddHead(const T & element) {
		TListNode * pNode = AllocNode();
		if (pNode == nullptr) return false;

		pNode->m_value = element;
		pNode->m_pNext = m_pHead;
		if (m_pHead == nullptr) {
			m_pHead = m_pTail = pNode;
		}
		else {
			m_pHead->m_pPrev = pNode;
			m_pHead = pNode;
		}
		return true;
	}

	//Add the element to the tail
	bool AddTail(const T & element) {
		TListNode * pNode = AllocNode();
		if (pNode == nullptr) return false;

		pNode->m_value = element;
		pNode->m_pPrev = m_pTail;
		if (m_pTail == nullptr) {
			m_pHead = m_pTail = pNode;
		}
		else {
			m_pTail->m_pNext = pNode;
			m_pTail = pNode;
		}
		return true;
	}

	//Insert the elment after the pNode
	bool Insert(TListNode * pOldNode, const T & element) {
		TListNode * pNewNode = AllocNode();
		if (pNewNode == nullptr) return false;

		pNewNode->value = element;

		pNewNode->m_pPrev = pOldNode;
		pNewNode->m_pNext = pOldNode->m_pNext;
		if (pOldNode == m_pTail) {
			m_pTail = pNewNode;
		}
		else {
			pOldNode->m_pNext->m_pPrev = pNewNode;
		}
		pOldNode->m_pNext = pNewNode;
		return true;
	}

	//Remove the node from the list
	void Remove(TListNode * pNode) {
		TListNode * pNext = pNode->m_pNext;
		TListNode * pPrev = pNode->m_pPrev;

		if (pNext) {
			pNext->m_pPrev = pPrev;
		}
		else {
			m_pTail = pPrev;
		}

		if (pPrev) {
			pPrev->m_pNext = pNext;
		}
		else {
			m_pHead = pNext;
		}

		FreeNode(pNode);
	}


	void RemoveHead() {
		TAssert(m_pHead);

		TListNode * pNode = m_pHead;
		m_pHead = m_pHead->m_pNext;

		if (m_pHead) {
			m_pHead->m_pPrev = nullptr;
		}
		else {
			m_pTail = nullptr;
		}
		FreeNode(pNode);
	}

	void RemoveTail() {
		TAssert(m_pTail);

		TListNode * pNode = m_pTail;
		m_pTail = m_pTail->m_pPrev;

		if (m_pTail) {
			m_pTail->m_pNext = nullptr;
		}
		else {
			m_pHead = nullptr;
		}
		FreeNode(pNode);
	}

	//Find the node corresponding to the value specified
	TListNode * Find(const T & element) {
		TListNode * pNode = m_pHead;
		while (pNode) {
			if (pNode->m_value == element) {
				return pNode;
			}
			pNode = pNode->m_pNext;
		}
		return pNode;
	}

	//Find the node by the index
	TListNode * FindByIndex(int index) {
		TAssert(index >= 0 && index < m_count);

		if (index >(int)m_count >> 1) {
			TListNode * pNode = m_pTail;
			for (int i = m_count - index - 1; i > 0; i--) {
				pNode = pNode->m_pPrev;
			}
			return pNode;
		}
		else {
			TListNode * pNode = m_pHead;
			for (int i = 0; i < index; i++) {
				pNode = pNode->m_pNext;
			}
			return pNode;
		}
	}

	int Size() {
		return m_count;
	}

	void SetPoolSize(int pool_size) {
		m_pool_size = pool_size;
	}

	bool IsEmpty() {
		return m_count != 0;
	}

	void Clear() {
		while (m_pHead != nullptr) {
			TListNode * node = m_pHead;
			m_pHead = m_pHead->m_pNext;
			delete node;
		}
	}

private:
	TListNode * AllocNode() {
		if (m_free == nullptr) {
			TListNodePool * pPool = new TListNodePool(m_pool_size);
			if (pPool == nullptr) {
				return nullptr;
			}

			m_free = pPool->m_nodes;
			pPool->m_pNext = m_pools;
			m_pools = pPool;
		}

		TListNode * pNode = m_free;
		pNode->m_pNext = nullptr;
		pNode->m_pPrev = nullptr;

		m_free = m_free->m_pNext;
		m_count++;
		return pNode;
	}


	void FreeNode(TListNode * pNode) {
		pNode->m_pNext = m_free;
		m_free = pNode;
		m_count--;
	}

	void ReleasePool() {
		while (m_pools) {
			TListNodePool * pool = m_pools;
			m_pools = pool->m_pNext;
			delete pool;
		}
	}

private:
	TList(const TList & list) {}
	TList & operator=(const TList & list) { return nullptr; }

private:
	TListNode * m_pHead;
	TListNode * m_pTail;
	
	TListNodePool * m_pools;
	TListNode * m_free;
	int m_count;
	int m_pool_size;
};
