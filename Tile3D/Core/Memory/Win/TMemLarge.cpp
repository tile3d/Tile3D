#include <Core/Lock/TInterlocked.h>
#include <Util/TAssert.h>
#include <string.h>
#include "TMemLarge.h"
#include "TMemMan.h"
#include "TMemDump.h"


//	Allocate memory
void* TMemLarge::Allocate(size_t size)
{
#ifdef DEBUG_MEMORY
	//	Add 2 DWORDs to do slop-over checking
	int blkSize = size + sizeof(TMemLargeBlock) + SLOPOVER_SIZE;
#else
	int blkSize = size + sizeof(TMemLargeBlock);
#endif

	TMemLargeBlock* p = (TMemLargeBlock*)TMemCommon::RawMemAlloc(blkSize);
	if (!p)
		return NULL;

	p->m_blockSize = blkSize;
	p->m_reserved = 0;
	p->m_flags = MEM_ALLOC_FLAG_L;

#ifdef _DEBUG

	p->m_rawSize = size;

	//	Fill data with special code
	char* pData = (char*)p + sizeof(TMemLargeBlock);
	memset(pData, MEM_ALLOC_FILL, size);

	//	Fill slop-over checking flags
	TMemCommon::FillSlopOverFlags(p->m_soFlags);
	TMemCommon::FillSlopOverFlags(pData + size);

	//	Record allocated size
	m_pMemMan->AddAllocSize(blkSize);
	m_pMemMan->AddAllocRawSize((int)size);

	//	Get free block from manager
	TInterlocked::Lock(&m_lock);
	m_allocSize += blkSize;
	m_blockCnt++;

	p->m_pPrev = NULL;
	p->m_pNext = m_blockList;

	if (m_blockList)
		m_blockList->m_pPrev = p;

	m_blockList = p;
	TInterlocked::Unlock(&m_lock);
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

	TMemDump::DumpDeleteHistory(TMemDump::GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!TMemCommon::CheckSlopOver(pBlock->m_soFlags) ||
		!TMemCommon::CheckSlopOver((char*)p + pBlock->m_rawSize))
	{
		TAssert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	int iUseableSize = pBlock->m_blockSize - sizeof(TMemLargeBlock);
	memset(p, MEM_FREE_FILL, iUseableSize);

	m_pMemMan->AddAllocSize(-pBlock->m_blockSize);
	m_pMemMan->AddAllocRawSize(-pBlock->m_rawSize);

	//	------- Unlink free block from manager -------
	TInterlocked::Lock(&m_lock);

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

	TInterlocked::Unlock(&m_lock);
#endif

	TMemCommon::RawMemFree(pBlock);
}

#ifdef DEBUG_MEMORY

void TMemLarge::DumpMemoryBlocks(FILE* pFile)
{
	TMemLargeBlock* pBlock = m_blockList;
	while (pBlock)
	{
		for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
		{
			fprintf(pFile, "0x%d ", pBlock->m_callers[i]);
			TMemDump::TouchToLogAddressSymbol(reinterpret_cast<void*>(pBlock->m_callers[i]));
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
		TMemDump::DumpMemLargeBlkToLogOutput(m_blockList);
		m_blockList = m_blockList->m_pNext;
	}
}


#endif	//	DEBUG_MEMORY

