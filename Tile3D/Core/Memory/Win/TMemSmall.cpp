#include <Core/Lock/TInterlocked.h>
#include "TMemSmall.h"
#include "TMemLarge.h"
#include "TMemMan.h"
#include "TMemVec.h"
#include "TMemDump.h"
#include <string.h>
#include <Util/TAssert.h>



//	Get block address from a allocated memory address
TMemSmallBlock* TMemSmall::GetMemSmallBlockInfo(void* pMem)
{
	//	In both debug and release modes, sizeof (TMemSmallBlock) <= sizeof (TMemLargeBlock).
	return (TMemSmallBlock*)((char*)pMem - sizeof(TMemSmallBlock));
}

//	Allocate memory
void* TMemSmall::Allocate(size_t size)
{
#ifdef DEBUG_MEMORY
	//	Add 2 DWORDs to do slop-over checking
	int dataSize = TMemCommon::RoundUp(size + SLOPOVER_SIZE);
#else
	int iDataSize = AMemoryMan::RoundUp(size);
#endif

	//	Block size
	if (dataSize > MEM_SMALLSIZE)
		return m_pMemMan->GetMemLarge()->Allocate(size);

	int poolSlot = (dataSize - 1) / MEM_ALIGN;

	//	Record allocated size
#ifdef DEBUG_MEMORY
	m_pMemMan->AddAllocSize(GetBlockSize(poolSlot));
	m_pMemMan->AddAllocRawSize((int)size);
#endif

	//	------ Get free block from manager ------ 
	TInterlocked::Lock(&m_locks[poolSlot]);

	//	Get proper memory pool slot
	TMemSmallSlot& slot = m_pools[poolSlot];
	if (!slot.m_pFreeBlks)
	{
		slot.m_pFreeBlks = m_poolMans[poolSlot].Allocate();
		int iBlkCount = m_poolMans[poolSlot].GetBlockCount();
		slot.m_blockCnt += iBlkCount;
		slot.m_freeCnt += iBlkCount;
	}

	TMemSmallBlock* pFreeBlk = slot.m_pFreeBlks;
	slot.m_pFreeBlks = pFreeBlk->m_pNext;
	slot.m_freeCnt--;

	TInterlocked::Unlock(&m_locks[poolSlot]);

	//	------ Initialize block ------ 
	char* pData = (char*)pFreeBlk + sizeof(TMemSmallBlock);

#ifdef DEBUG_MEMORY
	//	Fill data with special code
	memset(pData, MEM_ALLOC_FILL, size);
	pFreeBlk->m_rawSize = size;
	//	Fill slop-over checking flags
	TMemCommon::FillSlopOverFlags(pFreeBlk->m_soFlags);
	TMemCommon::FillSlopOverFlags(pData + size);
#endif

	pFreeBlk->m_poolSlot = (short)poolSlot;
	pFreeBlk->m_pNext = NULL;
	pFreeBlk->m_flag = MEM_ALLOC_FLAG_S;
	return pData;
}



//	Free memory
void TMemSmall::Free(void *p)
{
	if (!p)
		return;

	TMemSmallBlock* pBlock = GetMemSmallBlockInfo(p);

#ifdef DEBUG_MEMORY

	/*
	BYTE* pCallers = (BYTE*)pBlock->callers;
	DWORD hash_val = 0;

	for (int i = 0; i < sizeof(pBlock->callers); i++)
	{
	hash_val = hash_val * 31 + pCallers[i];
	}

	LockMemCallersInfoModule();

	MemCallersInfoMap::iterator it = l_MemCallersInfo.GetMap().find(hash_val);

	if (it != l_MemCallersInfo.GetMap().end())
	{
	if (it->second->call_count)
	{
	it->second->call_count--;
	it->second->total_size -= (DWORD)pBlock->iRawSize;
	}
	else
	assert(false);
	}
	else
	{
	assert(false);
	}

	UnlockMemCallersInfoModule();
	*/

#endif

	if (pBlock->m_flag == MEM_FREE_FLAG_S)
	{
		//	Memory has been freed
		TAssert(pBlock->m_flag != MEM_FREE_FLAG_S);
		return;
	}
	else if (pBlock->m_flag == MEM_ALLOC_FLAG_L || pBlock->m_flag == MEM_FREE_FLAG_L)
	{
		m_pMemMan->GetMemLarge()->Free(p);
		return;
	}
	else if (pBlock->m_flag != MEM_ALLOC_FLAG_S)
	{
		TAssert(pBlock->m_flag == MEM_ALLOC_FLAG_S);
		return;
	}

	int iSlot = pBlock->m_poolSlot;
	if (iSlot < 0 || iSlot >= MEM_SLOTNUM)
	{
		TAssert(iSlot >= 0 && iSlot < MEM_SLOTNUM);
		return;
	}

	//	Get proper memory pool
	TMemSmallSlot& slot = m_pools[iSlot];

#ifdef DEBUG_MEMORY
	TMemDump::DumpDeleteHistory(TMemDump::GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!TMemCommon::CheckSlopOver(pBlock->m_soFlags) || !TMemCommon::CheckSlopOver((char*)p + pBlock->m_rawSize))
	{
		TAssert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	memset(p, MEM_FREE_FILL, slot.m_dataSize);

	m_pMemMan->AddAllocSize(-slot.m_blockSize);
	m_pMemMan->AddAllocRawSize(-pBlock->m_rawSize);

#endif

	//	Set freed flag
	pBlock->m_flag = MEM_FREE_FLAG_S;

	//	------- Return free block to manager --------
	TInterlocked::Lock(&m_locks[iSlot]);

	TMemSmallBlock** ppFreeList = &slot.m_pFreeBlks;
	pBlock->m_pNext = *ppFreeList;
	*ppFreeList = pBlock;
	slot.m_freeCnt++;

	m_poolMans[iSlot].IncGCCounter();

	TInterlocked::Unlock(&m_locks[iSlot]);
}

//	Garbage collect
void TMemSmall::GarbageCollect()
{
	int iSlot = m_gcCounter % MEM_SLOTNUM;
	m_gcCounter++;

	TMemPool& poolMan = m_poolMans[iSlot];
	TMemSmallSlot& poolSlot = m_pools[iSlot];
	int blkCount = poolMan.GetBlockCount();

	//	Do garbage collection only when free block number is enough
	if (poolMan.GetGCCounter() < MEM_GC_COUNTER || poolSlot.m_freeCnt < blkCount * 15)
		return;

	poolMan.ClearGCCounter();

	//	Free block record
	struct FREEBLKLIST
	{
		TMemSmallBlock* pHead;
		TMemSmallBlock* pTail;
	};

	int i, iNumPoolSlot = poolMan.GetSlotNum();
	int iTotalFree = poolSlot.m_freeCnt;
	FREEBLKLIST tempFreeList = { NULL, NULL };
	TMemVec<int> remPools;

	for (i = 0; i < iNumPoolSlot; i++)
	{
		if (iTotalFree <= blkCount * 5)
			break;

		TMemSmallBlock* pStart = poolMan.GetPool(i);
		TMemSmallBlock* pEnd = (TMemSmallBlock*)((char*)pStart + blkCount * poolMan.GetBlockSize());

		TMemSmallBlock* pCur = poolSlot.m_pFreeBlks;
		TMemSmallBlock* pPrev = NULL;
		FREEBLKLIST poolFreeList = { NULL, NULL };
		int blkCnt = 0;

		while (pCur)
		{
			if (pCur >= pStart && pCur < pEnd)
			{
				TAssert(blkCnt < blkCount);
				blkCnt++;

				//	Remove block from total free list and add it to pool free list
				TMemSmallBlock* pNext = pCur->m_pNext;

				if (!poolFreeList.pHead)
				{
					pCur->m_pNext = NULL;
					poolFreeList.pHead = pCur;
					poolFreeList.pTail = pCur;
				}
				else
				{
					pCur->m_pNext = poolFreeList.pHead;
					poolFreeList.pHead = pCur;
				}

				if (pPrev)
					pPrev->m_pNext = pNext;
				else
					poolSlot.m_pFreeBlks = pNext;

				pCur = pNext;
			}
			else
			{
				pPrev = pCur;
				pCur = pCur->m_pNext;
			}
		}

		if (blkCnt == blkCount)
		{
			//	Ok, all blocks belong to this pool are free now.
			remPools.Push(i);
			iTotalFree -= blkCnt;
		}
		else if (poolFreeList.pHead)
		{
			//	Add whole pool free list to temp free list
			if (!tempFreeList.pHead)
			{
				tempFreeList.pHead = poolFreeList.pHead;
				tempFreeList.pTail = poolFreeList.pTail;
			}
			else
			{
				poolFreeList.pTail->m_pNext = tempFreeList.pHead;
				tempFreeList.pHead = poolFreeList.pHead;
			}
		}
	}

	//	Return temp free list
	if (tempFreeList.pHead)
	{
		tempFreeList.pTail->m_pNext = poolSlot.m_pFreeBlks;
		poolSlot.m_pFreeBlks = tempFreeList.pHead;
	}

	//	Remove all pools in remove array. 
	//	Note: we must remove from tail to head !!!
	for (i = remPools.Size() - 1; i >= 0; i--)
	{
		poolMan.ReleasePool(remPools[i]);
		poolSlot.m_blockCnt -= blkCount;
		poolSlot.m_freeCnt -= blkCount;
	}
}


#ifdef DEBUG_MEMORY

void TMemSmall::DumpMemoryBlocks(FILE* pFile)
{
	struct __X__ {
		struct DumpSmallBlkToFile
		{
			DumpSmallBlkToFile(FILE * pOutFile) : m_pOutFile(pOutFile) {}
			FILE * m_pOutFile;
			void operator () (const TMemSmallBlock* pBlock) const
			{
				for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
				{
					fprintf(m_pOutFile, "0x%d", pBlock->m_callers[i]);
					TMemDump::TouchToLogAddressSymbol(reinterpret_cast<void*>(pBlock->m_callers[i]));
				}
				fprintf(m_pOutFile, "%d\n", pBlock->m_rawSize);
			}
		};
	};

	__X__::DumpSmallBlkToFile dumpFunc(pFile);
	for (int i = 0; i < MEM_SLOTNUM; i++)
	{
		const TMemPool& poolMan = m_poolMans[i];
		int num = poolMan.GetSlotNum();
		for (int j = 0; j < num; j++)
			Dump(i, poolMan.GetPool(j), poolMan.GetBlockCount(), poolMan.GetBlockSize(), dumpFunc);
	}
}

//	Dump memory leak
void TMemSmall::Dump()
{
	for (int i = 0; i < MEM_SLOTNUM; i++)
	{
		const TMemPool& poolMan = m_poolMans[i];
		int num = poolMan.GetSlotNum();
		for (int j = 0; j < num; j++)
			Dump(i, poolMan.GetPool(j), poolMan.GetBlockCount(), poolMan.GetBlockSize(), TMemDump::DumpMemSmallBlkToLogOutput);
	}
}


//	Dump memory leak
template<typename T>
void TMemSmall::Dump(int iSlot, TMemSmallBlock* p, int blkCount, int blkSize, T dumpFunction)
{
	TMemSmallBlock** blocks = new TMemSmallBlock*[MEM_POOL_MAX_BLOCK_COUNT];

	char* pBegin = (char*)p;
	for (int i = 0; i < blkCount; i++, pBegin += blkSize)
		blocks[i] = (TMemSmallBlock*)pBegin;

	pBegin = (char*)p;
	TMemSmallBlock* pCur = m_pools[iSlot].m_pFreeBlks;
	while (pCur)
	{
		int n = ((char*)pCur - pBegin) / blkSize;
		if (n >= 0 && n < blkCount)
			blocks[n] = NULL;

		pCur = pCur->m_pNext;
	}

	for (int i = 0; i < blkCount; i++)
	{
		if (blocks[i])
		{
			dumpFunction(blocks[i]);
		}
	}
	delete[] blocks;
}

#endif