#include <string.h>
#include "TMemLarge.h"
#include "TMemMan.h"
#include <Util/TAssert.h>

//	Allocate memory
void* TMemLarge::Allocate(size_t size)
{
#ifdef DEBUG_MEMORY
	//	Add 2 DWORDs to do slop-over checking
	int iBlkSize = size + sizeof(TMemLargeBlock) + SLOPOVER_SIZE;
#else
	int iBlkSize = size + sizeof(s_MEMLARGEBLK);
#endif

	TMemLargeBlock* p = (TMemLargeBlock*)RawMemAlloc(iBlkSize);
	if (!p)
		return NULL;

	p->m_blockSize = iBlkSize;
	p->m_reserved = 0;
	p->m_flags = MEM_ALLOC_FLAG_L;

#ifdef _DEBUG

	p->m_rawSize = size;

	//	Fill data with special code
	char* pData = (char*)p + sizeof(TMemLargeBlock);
	memset(pData, MEM_ALLOC_FILL, size);

	//	Fill slop-over checking flags
	FillSlopOverFlags(p->m_soFlags);
	FillSlopOverFlags(pData + size);

	//	Record allocated size
	m_pMemMan->AddAllocSize(iBlkSize);
	m_pMemMan->AddAllocRawSize((int)size);

	//	Get free block from manager
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_lThreadAtom);

	m_iAllocSize += iBlkSize;
	m_iBlockCnt++;

	p->m_pPrev = NULL;
	p->m_pNext = m_BlockList;

	if (m_BlockList)
		m_BlockList->pPrev = p;

	m_BlockList = p;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_lThreadAtom);

#endif
	return ++p;
}

//	Free memory
void TMemLarge::Free(void* p)
{
	if (!p)
		return;

	TMemLargeBlock* pBlock = (TMemLargeBlock*)((char*)(p)-sizeof(TMemLargeBlock));

#ifdef DEBUG_MEMORY

	if (pBlock->m_flags != MEM_ALLOC_FLAG_L)
	{
		TAssert(pBlock->m_flags == MEM_ALLOC_FLAG_L);
		return;
	}

	DumpDeleteHistory(GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!CheckSlopOver(pBlock->m_soFlags) ||
		!CheckSlopOver((char*)p + pBlock->m_rawSize))
	{
		TAssert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	int iUseableSize = pBlock->m_blockSize - sizeof(TMemLargeBlock);
	memset(p, MEM_FREE_FILL, iUseableSize);

	m_pMemMan->AddAllocSize(-pBlock->m_blockSize);
	m_pMemMan->AddAllocRawSize(-pBlock->m_rawSize);

	//	------- Unlink free block from manager -------
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_lThreadAtom);

	m_allocSize -= pBlock->m_blockSize;
	m_blockCnt--;

	TMemLargeBlock* pp = pBlock->m_pPrev;
	TMemLargeBlock* pn = pBlock->m_pNext;

	if (pp)
		pp->m_pNext = pn;
	else
		m_blockList = pn;

	if (pn)
		pn->m_pPrev = pp;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_lThreadAtom);

#endif

	RawMemFree(pBlock);
}

#ifdef DEBUG_MEMORY

void TMemLarge::DumpMemoryBlocks(FILE* pFile)
{
	TMemLargeBlock* pBlock = m_blockList;
	while (pBlock)
	{
		for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
		{
			fprintf(pFile, "0x%p ", pBlock->callers[i]);
				TouchToLogAddressSymbol(reinterpret_cast<void*>(pBlock->callers[i]));
		}
		fprintf(pFile, "%d\n", pBlock->m_rawSize);
		pBlock = pBlock->m_pNext;
	}
}

//	Dump memory leak
void TMemLarge::Dump()
{
	while (m_blockList)
	{
		DumpMemLargeBlkToALogOutput(m_BlockList);

		m_BlockList = m_BlockList->pNext;
	}
}


void DumpMemLargeBlkToLogOutput(const TMemLargeBlock* pBlock)
{
	char szMsg[1024] = { 0 };
	sprintf(szMsg, "Memory [%d bytes@%p] leak at (", pBlock->m_rawSize, (char*)pBlock + sizeof(TMemLargeBlock));
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		if (i == MAX_CALLSTACK_LV - 1 || (!pBlock->m_callers[i] && i >= 7))
		{
			sprintf(szPart, "0x%p)", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%p <- ", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
		}
	}

	a_LogOutput(1, szMsg);
	DumpBlockInfo(pBlock);
}



#endif	//	DEBUG_MEMORY

