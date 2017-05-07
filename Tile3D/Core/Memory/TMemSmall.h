#pragma once

#include "TMemCommon.h"
#include "TMemPool.h"


class TMemMan;
class TMemSmall
{
public:
	struct TMemSmallSlot
	{
		TMemSmallBlock*	m_pFreeBlks;	//	Free blocks
		int				m_dataSize;	//	Data size
		int				m_blockSize;	//	Block size
		int				m_blockCnt;	//	Total allocated block counter
		int				m_freeCnt;	//	Free block counter
	};

	//ctor/dtor
public:
	TMemSmall() : m_pMemMan(NULL)
	{
		for (int i = 0; i < MEM_SLOTNUM; i++)
		{
			//	Set data size first, so that block size can be calculated.
			int iSize = (i + 1) * MEM_ALIGN;
			m_poolMans[i].SetDataSize(iSize);

			int iCount = MEM_POOL_DATA_SIZE / m_poolMans[i].GetBlockSize();
			m_poolMans[i].SetBlockCount(iCount);

			m_pools[i].m_pFreeBlks = NULL;
			m_pools[i].m_dataSize = m_poolMans[i].GetDataSize();
			m_pools[i].m_blockSize = m_poolMans[i].GetBlockSize();
			m_pools[i].m_blockCnt = 0;
			m_pools[i].m_freeCnt = 0;

			m_locks[i] = 0;
			m_gcCounter = 0;
		}
	}

	~TMemSmall() {
#ifdef DEBUG_MEMORY
		Dump();
#endif
	}

public:		//	Operations
			//	Allocate memory
	void* Allocate(size_t size);
	//	Free memory
	void Free(void *p);

	//  Get pool slot num
	int GetPoolSlotNum(int iSlot) { return m_poolMans[iSlot].GetSlotNum(); }
	//	Get pool slot block num
	int GetPoolSlotBlockNum(int iSlot) { return m_poolMans[iSlot].GetBlockCount(); }
	//	Get block size of specified pool slot
	int GetBlockSize(int iSlot) { return m_pools[iSlot].m_blockSize; }
	//	Get data size of specified pool slot
	int GetDataSize(int iSlot) { return m_pools[iSlot].m_dataSize; }
	//	Get total allocated block counter of specified pool slot
	int GetTotalBlockCnt(int iSlot) { return m_pools[iSlot].m_blockCnt; }
	//	Get free block counter of specified pool slot
	int GetFreeBlockCnt(int iSlot) { return m_pools[iSlot].m_freeCnt; }
	//	Garbage collect
	void GarbageCollect();

private:
#ifdef DEBUG_MEMORY
	void Dump();

	//	Dump memory leak of one pool slot
	template < class T >
	void Dump(int iSlot, TMemSmallBlock *p, int blockCount, int blkSize, T dumpFunction);
#endif

private:
	TMemMan * m_pMemMan;
	int		m_locks[MEM_SLOTNUM];	//	Atom used to ensure thread safe
	int		m_gcCounter;
	TMemPool m_poolMans[MEM_SLOTNUM];
	TMemSmallSlot m_pools[MEM_SLOTNUM];
};


