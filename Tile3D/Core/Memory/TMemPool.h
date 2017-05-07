#pragma once

#include "TMemCommon.h"
#include "TMemVec.h"

class TMemPool
{
public:
	struct TMemPoolSlot
	{
		TMemSmallBlock * m_pool;
	};
public:
	TMemPool() {
		m_blockCount = 256;
		m_dataSize = 16;
		m_blockSize = CalcBlockSize();
		m_gcCount = 0;
	}

	~TMemPool() {
		for (int i = 0; i < m_pools.Size(); i++) {
			RawMemFree(m_pools[i].m_pool);
		}
	}

public:
	//	Get slot number
	int	GetSlotNum() const { return m_pools.Size(); }
	//	Get pool of specified slot
	TMemSmallBlock* GetPool(int iSlot) const { return m_pools[iSlot].m_pool; }

	//  Get block count
	int	GetBlockCount() const { return m_blockCount; }
	//	Set block count 
	void SetBlockCount(int count) { m_blockCount = count; }
	//	Get block size
	int GetBlockSize() const { return m_blockSize; }
	//	Set data size
	void SetDataSize(int size)
	{
		m_dataSize = size;
		m_blockSize = CalcBlockSize();
	}
	//	Get data size
	int GetDataSize() const { return m_dataSize; }
	//	Get garbage collect counter
	int GetGCCounter() { return m_gcCount; }
	//	Inc garbage collect counter
	int IncGCCounter() { return ++m_gcCount; }
	//	Clear garbage collect counter
	void ClearGCCounter() { m_gcCount = 0; }

	//	Release pool slot
	void ReleasePool(int iSlot) {
		RawMemFree(m_pools[iSlot].m_pool);
		m_pools.RemoveQuick(iSlot);
	}

private:
	int CalcBlockSize() { return m_dataSize + sizeof(TMemSmallBlock); }

	//	Initialize a pool
	void InitPool(TMemPoolSlot* pSlot);

	//	Allocate memory pool
	TMemSmallBlock* Allocate();

private:
	int m_blockCount;
	int m_blockSize;
	int m_dataSize;
	int m_gcCount;
	TMemoryVec<TMemPoolSlot> m_pools;
};

/*	Initialize memory pool buffer

pSlot: pool slot's address
*/
void TMemPool::InitPool(TMemPoolSlot* pSlot)
{
	char* pNext, *pByte = (char*)pSlot->m_pool;
	for (int i = 0; i<m_blockCount - 1; i++, pByte = pNext)
	{
		pNext = pByte + m_blockSize;
		((TMemSmallBlock*)pByte)->m_pNext = (TMemSmallBlock*)pNext;
	}

	//	The last block
	((TMemSmallBlock*)pByte)->m_pNext = NULL;
}

/*	Create memory pool

iBlkSize: size of eack block in pool
*/
TMemSmallBlock* TMemPool::Allocate()
{
	//	Allocate pool buffer
	TMemPoolSlot slot;
	//	Slot.pPool = (s_MEMSMALLBLK*)_RawMemAlloc(m_iBlkCount * m_iBlkSize);
	slot.m_pool = (TMemSmallBlock*)RawMemAlloc(MEM_POOL_DATA_SIZE);
	if (!slot.m_pool)
		return NULL;

	//	Initialize memory pool
	InitPool(&slot);

	m_pools.Push(slot);
	return slot.m_pool;
}
