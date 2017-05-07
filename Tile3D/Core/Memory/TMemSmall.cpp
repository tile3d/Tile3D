#include "TMemSmall.h"
#include "TMemMan.h"

//	Allocate memory
void* TMemSmall::Allocate(size_t size)
{
#ifdef DEBUG_MEMORY
	//	Add 2 DWORDs to do slop-over checking
	int iDataSize = TMemMan::RoundUp(size + SLOPOVER_SIZE);
#else
	int iDataSize = AMemoryMan::RoundUp(size);
#endif

	//	Block size
	if (iDataSize > MEM_SMALLSIZE)
		return m_pMemMan->GetMemLarge()->Allocate(size);

	int iPoolSlot = (iDataSize - 1) / MEM_ALIGN;

	//	Record allocated size
#ifdef DEBUG_MEMORY
	m_pMemMan->AddAllocSize(GetBlockSize(iPoolSlot));
	m_pMemMan->AddAllocRawSize((int)size);
#endif

	//	------ Get free block from manager ------ 
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_aThreadAtoms[iPoolSlot]);

	//	Get proper memory pool slot
	TMemSmallSlot& slot = m_pools[iPoolSlot];

	if (!slot.m_pFreeBlks)
	{
		slot.m_pFreeBlks = m_poolMans[iPoolSlot].Allocate();
		int iBlkCount = m_poolMans[iPoolSlot].GetBlockCount();
		slot.m_blockCnt += iBlkCount;
		slot.m_freeCnt += iBlkCount;
	}

	TMemSmallBlock* pFreeBlk = slot.m_pFreeBlks;
	slot.m_pFreeBlks = pFreeBlk->m_pNext;
	slot.m_freeCnt--;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_aThreadAtoms[iPoolSlot]);

	//	------ Initialize block ------ 
	char* pData = (char*)pFreeBlk + sizeof(TMemSmallBlock);

#ifdef DEBUG_MEMORY

	//	Fill data with special code
	memset(pData, MEM_ALLOC_FILL, size);
	pFreeBlk->iRawSize = size;
	//	Fill slop-over checking flags
	_FillSlopOverFlags(pFreeBlk->aSOFlags);
	_FillSlopOverFlags(pData + size);

#endif

	pFreeBlk->sPoolSlot = (short)iPoolSlot;
	pFreeBlk->pNext = NULL;
	pFreeBlk->sFlag = MEM_ALLOC_FLAG_S;
	return pData;
}

//	Free memory
void AMemSmall::Free(void *p)
{
	if (!p)
		return;

	s_MEMSMALLBLK* pBlock = _GetMemBlockInfo(p);

#ifdef _DEBUG

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

	if (pBlock->sFlag == MEM_FREE_FLAG_S)
	{
		//	Memory has been freed
		assert(pBlock->sFlag != MEM_FREE_FLAG_S);
		return;
	}
	else if (pBlock->sFlag == MEM_ALLOC_FLAG_L || pBlock->sFlag == MEM_FREE_FLAG_L)
	{
		m_pMemMan->GetMemLarge()->Free(p);
		return;
	}
	else if (pBlock->sFlag != MEM_ALLOC_FLAG_S)
	{
		assert(pBlock->sFlag == MEM_ALLOC_FLAG_S);
		return;
	}

	int iSlot = pBlock->sPoolSlot;
	if (iSlot < 0 || iSlot >= MEM_SLOTNUM)
	{
		assert(iSlot >= 0 && iSlot < MEM_SLOTNUM);
		return;
	}

	//	Get proper memory pool
	POOLSLOT& Slot = m_aPools[iSlot];

#ifdef _DEBUG

	DumpDeleteHistory(GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!_CheckSlopOver(pBlock->aSOFlags) ||
		!_CheckSlopOver((char*)p + pBlock->iRawSize))
	{
		assert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	memset(p, MEM_FREE_FILL, Slot.iDataSize);

	m_pMemMan->AddAllocSize(-Slot.iBlockSize);
	m_pMemMan->AddAllocRawSize(-pBlock->iRawSize);

#endif

	//	Set freed flag
	pBlock->sFlag = MEM_FREE_FLAG_S;

	//	------- Return free block to manager --------
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_aThreadAtoms[iSlot]);

	s_MEMSMALLBLK** ppFreeList = &Slot.pFreeBlks;
	pBlock->pNext = *ppFreeList;
	*ppFreeList = pBlock;
	Slot.iFreeCnt++;

	m_aPoolMans[iSlot].IncGCCounter();

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_aThreadAtoms[iSlot]);
}

//	Garbage collect
void AMemSmall::GarbageCollect()
{
	int iSlot = m_dwGCCounter % MEM_SLOTNUM;
	m_dwGCCounter++;

	AMemPool& PoolMan = m_aPoolMans[iSlot];
	POOLSLOT& PoolSlot = m_aPools[iSlot];
	int iBlkCount = PoolMan.GetBlockCount();

	//	Do garbage collection only when free block number is enough
	if (PoolMan.GetGCCounter() < MEM_GC_COUNTER ||
		PoolSlot.iFreeCnt < iBlkCount * 15)
		return;

	PoolMan.ClearGCCounter();

	//	Free block record
	struct FREEBLKLIST
	{
		s_MEMSMALLBLK* pHead;
		s_MEMSMALLBLK* pTail;
	};

	int i, iNumPoolSlot = PoolMan.GetSlotNum();
	int iTotalFree = PoolSlot.iFreeCnt;
	FREEBLKLIST TempFreeList = { NULL, NULL };
	AMemVector<int> aRemPools;

	for (i = 0; i < iNumPoolSlot; i++)
	{
		if (iTotalFree <= iBlkCount * 5)
			break;

		s_MEMSMALLBLK* pStart = PoolMan.GetPool(i);
		s_MEMSMALLBLK* pEnd = (s_MEMSMALLBLK*)((BYTE*)pStart + iBlkCount * PoolMan.GetBlockSize());

		s_MEMSMALLBLK* pCur = PoolSlot.pFreeBlks;
		s_MEMSMALLBLK* pPrev = NULL;
		FREEBLKLIST PoolFreeList = { NULL, NULL };
		int iBlkCnt = 0;

		while (pCur)
		{
			if (pCur >= pStart && pCur < pEnd)
			{
				assert(iBlkCnt < iBlkCount);
				iBlkCnt++;

				//	Remove block from total free list and add it to pool free list
				s_MEMSMALLBLK* pNext = pCur->pNext;

				if (!PoolFreeList.pHead)
				{
					pCur->pNext = NULL;
					PoolFreeList.pHead = pCur;
					PoolFreeList.pTail = pCur;
				}
				else
				{
					pCur->pNext = PoolFreeList.pHead;
					PoolFreeList.pHead = pCur;
				}

				if (pPrev)
					pPrev->pNext = pNext;
				else
					PoolSlot.pFreeBlks = pNext;

				pCur = pNext;
			}
			else
			{
				pPrev = pCur;
				pCur = pCur->pNext;
			}
		}

		if (iBlkCnt == iBlkCount)
		{
			//	Ok, all blocks belong to this pool are free now.
			aRemPools.push_back(i);
			iTotalFree -= iBlkCnt;
		}
		else if (PoolFreeList.pHead)
		{
			//	Add whole pool free list to temp free list
			if (!TempFreeList.pHead)
			{
				TempFreeList.pHead = PoolFreeList.pHead;
				TempFreeList.pTail = PoolFreeList.pTail;
			}
			else
			{
				PoolFreeList.pTail->pNext = TempFreeList.pHead;
				TempFreeList.pHead = PoolFreeList.pHead;
			}
		}
	}

	//	Return temp free list
	if (TempFreeList.pHead)
	{
		TempFreeList.pTail->pNext = PoolSlot.pFreeBlks;
		PoolSlot.pFreeBlks = TempFreeList.pHead;
	}

	//	Remove all pools in remove array. 
	//	Note: we must remove from tail to head !!!
	for (i = aRemPools.size() - 1; i >= 0; i--)
	{
		PoolMan.ReleasePool(aRemPools[i]);
		PoolSlot.iBlockCnt -= iBlkCount;
		PoolSlot.iFreeCnt -= iBlkCount;
	}
}


#ifdef DEBUG_MEMORY
//	Dump memory leak
void TMemSmall::Dump()
{
	int i, j;

	for (j = 0; j < MEM_SLOTNUM; j++)
	{
		const TMemPool& poolMan = m_poolMans[j];

		int num = poolMan.GetSlotNum();
		for (i = 0; i < num; i++)
			Dump(j, poolMan.GetPool(i), poolMan.GetBlockCount(), poolMan.GetBlockSize(), DumpMemSmallBlkToALogOutput);
	}
}


//	Dump memory leak
template<class T>
void TMemSmall::Dump(int iSlot, TMemSmallBlock* p, int blkCount, int blkSize, T dumpFunction)
{
	TMemSmallBlock* blocks[MEM_POOL_MAX_BLOCK_COUNT];
	char* pBegin = (char*)p;
	int i;

	for (i = 0; i < blkCount; i++, pBegin += blkSize)
		blocks[i] = (TMemSmallBlock*)pBegin;

	pBegin = (char*)p;
	TMemSmallBlock* pCur = m_aPools[iSlot].m_pFreeBlks;

	while (pCur)
	{
		int n = ((char*)pCur - pBegin) / blkSize;
		if (n >= 0 && n < blkCount)
			aBlocks[n] = NULL;

		pCur = pCur->pNext;
	}

	for (i = 0; i < blkCount; i++)
	{
		if (blocks[i])
		{
			dumpFunction(aBlocks[i]);
		}
	}
}

#endif