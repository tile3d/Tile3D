#include "TMemMan.h"
#include "TMemSmall.h"
#include "TMemLarge.h"
#include "TMemDump.h"
#include <Core/Lock/TInterlocked.h>
#include <Util/TLog.h>

#pragma warning (disable: 4996)
#pragma warning (disable: 4073)


/*	init_seg (lib) ensure memoryMan to be constructed before and
destructed after all other application objects. This is very
important, because some application's global or static objects may allocate
or free memory in their constructor or destructors.

init_seg (lib) will cause a VC compiling warning 4073, disable it.
*/

#pragma init_seg (lib)


static TMemMan	gMemMan;


TMemMan::TMemMan() {
	m_pMemSmall = new TMemSmall();
	m_pMemLarge = new TMemLarge();

	m_idCnt = 0;
	m_peakSize = 0;
	m_sizeCnt = 0;
	m_lock = 0;
	m_rawSizeCnt = 0;
}

TMemMan::~TMemMan() {
	delete m_pMemSmall;
	delete m_pMemLarge;

#ifdef DEBUG_MEMORY
		TLog::Log(LOG_INFO, "MEMORY", "Maximum memory used: %d (K)", (m_peakSize + 1023) / 1024);
#endif
}

int TMemMan::GetNextID() 
{ 
	return TInterlocked::InterlockedIncrement(&m_idCnt); 
}

void TMemMan::AddAllocSize(int size)
{
	TInterlocked::Lock(&m_lock);
	if ((m_sizeCnt += size) > m_peakSize) {
		m_peakSize = m_sizeCnt;
	}
	TInterlocked::Unlock(&m_lock);
}

//	Add total allocated raw size
void TMemMan::AddAllocRawSize(int size)
{
	TInterlocked::InterlockedAdd(&m_rawSizeCnt, size);
}


void TMemMan::GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt)
{
	*piBlkSize = m_pMemSmall->GetBlockSize(iSlot);
	*piBlkCnt = m_pMemSmall->GetTotalBlockCnt(iSlot);
	*piFreeCnt = m_pMemSmall->GetFreeBlockCnt(iSlot);
}

void TMemMan::GetLargeMemInfo(int* piBlkCnt, int* piAllocSize)
{
	*piBlkCnt = m_pMemLarge->GetBlockCnt();
	*piAllocSize = m_pMemLarge->GetAllocSize();
}

void TMemMan::GarbageCollect()
{
	m_pMemSmall->GarbageCollect();
}


/*	Get a memory block's usable size. This function is only used internally.

pMem: point to a memory allocated by _a_New.
*/
int TMemMan::CalMemUseableSize(void* pMem)
{
	TMemSmallBlock* pBlock = TMemSmall::GetMemSmallBlockInfo(pMem);
	int maxSize = 0;

	if (pBlock->m_flag <= MEM_ALLOC_FLAG_S)
	{
		int dataSize = GetMemSmall()->GetDataSize(pBlock->m_poolSlot);

#ifdef DEBUG_MEMORY
		maxSize = dataSize - SLOPOVER_SIZE;	//	Slop-over checking flags
#else
		iMaxSize = iDataSize;
#endif
	}
	else if (pBlock->m_flag <= MEM_ALLOC_FLAG_L)
	{
		TMemLargeBlock* pLargeBlock = (TMemLargeBlock*)((char*)pMem - sizeof(TMemLargeBlock));
#ifdef _DEBUG
		maxSize = pLargeBlock->m_blockSize - sizeof(TMemLargeBlock) - SLOPOVER_SIZE;	//	Slop-over checking flags
#else
		iMaxSize = pLargeBlock->iBlkSize - sizeof(s_MEMLARGEBLK);
#endif
	}
	else
	{
		TAssert(0);
		return 0;
	}

	return maxSize;
}

void* TMemMan::Malloc(size_t size)
{
#ifdef DEBUG_MEMORY
	return MallocDebug(size);
#else
	return MallocRelease(size);
#endif
}

void* TMemMan::Realloc(void* pMem, size_t size)
{
#ifdef DEBUG_MEMORY
	return ReallocDebug(pMem, size);
#else
	return RealloRelease(pMem, size);
#endif
}

void* TMemMan::MallocAlign(size_t size, int align)
{
#ifdef DEBUG_MEMORY
	return MallocAlignDebug(size, align);
#else
	return MallocAlignRelease(size, align);
#endif

}

void TMemMan::Free(void *p)
{
	GetMemSmall()->Free(p);
}

//	Free a aligned memory block allocated by _a_NewAlign
void TMemMan::FreeAlign(void* pMem)
{
	char* data2;
	data2 = (char*)pMem;
	data2 -= *(int*)(data2 - sizeof(int));
	Free(data2);
}

#ifdef DEBUG_MEMORY

#pragma warning (disable: 6011)
void* TMemMan::MallocDebug(size_t size)
{
	char* pData = (char*)(GetMemSmall()->Allocate(size));
	TMemSmallBlock* pBlock = TMemSmall::GetMemSmallBlockInfo(pData);

	// now do a call stack backtrace
	DWORD frame_cur = 0;
	int c = 0;
	const int skip = 1;

	__asm
	{
		mov frame_cur, ebp
	}

	memset(pBlock->m_callers, 0, sizeof(pBlock->m_callers));
	while (!IsBadReadPtr((LPVOID)(4 + frame_cur), 4))
	{
		DWORD thisCall = *(DWORD*)(frame_cur + 4);
		if (c >= skip)
			pBlock->m_callers[c - skip] = thisCall;

		c++;

		if (c >= MAX_CALLSTACK_LV + skip || thisCall == 0)
			break;

		DWORD frame_up = *(DWORD *)frame_cur;
		if (frame_up <= frame_cur)
			break;

		frame_cur = frame_up;
	}

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
	it->second->call_count++;
	it->second->total_size += (DWORD)pBlock->iRawSize;
	}
	else
	{
	s_MEM_CALLER_INFO* pInfo = (s_MEM_CALLER_INFO*)malloc(sizeof(s_MEM_CALLER_INFO));
	memcpy(pInfo->callers, pBlock->callers, 32);
	pInfo->call_count = 1;
	pInfo->total_size = pBlock->iRawSize;
	(l_MemCallersInfo.GetMap())[hash_val] = pInfo;
	}

	UnlockMemCallersInfoModule();
	*/

	TMemDump::DumpAllocHistory(TMemDump::GetMemoryHistoryLog(), pBlock);
	return pData;
}


//	Reallocate, pMem must point to a mmeory block that is allocated by
//	a_malloc or a_realloc.
void* TMemMan::ReallocDebug(void* pMem, size_t size)
{
	if (!size && pMem)
	{
		Free(pMem);
		return NULL;
	}

	if (!pMem)	//	Act as malloc
		return MallocDebug(size);

	//	Get old memory block
	DWORD dwMaxSize = (DWORD)CalMemUseableSize(pMem);

	//	If old memory block is big enough, return memory block unchanged.
	if (size <= dwMaxSize)
	{
		TMemSmallBlock* pBlock = TMemSmall::GetMemSmallBlockInfo(pMem);

		int iDelta = (int)size - pBlock->m_rawSize;
		AddAllocRawSize(iDelta);

		pBlock->m_rawSize = size;

		//	Re-fill slop-over checking flag at data tail
		char* pData = (char*)pMem;
		TMemCommon::FillSlopOverFlags(pData + size);
		return pMem;
	}

	//	Allocate new memory
	void* pNewMem = MallocDebug(size);
	if (!pNewMem)
		return NULL;

	if (pNewMem != pMem)
	{
		//	Duplicate data
		memcpy(pNewMem, pMem, dwMaxSize);
		Free(pMem);
	}

	return pNewMem;
}


//	Allocate a aligned memory block.
void* TMemMan::MallocAlignDebug(size_t size, int align)
{
	int tsize = size + align + sizeof(int);
	char* data = (char*)MallocDebug(tsize);
	char* data2;

	if (data)
	{
		if ((unsigned int)(data) % align)
			data2 = (char*)(((unsigned int)data & (~(align - 1))) + align);
		else
			data2 = data + ((align > sizeof(int)) ? align : sizeof(int));

		if (data2 - data < sizeof(int))
			data2 += align;

		*(int*)(data2 - sizeof(int)) = data2 - data;

		return data2;
	}

	return NULL;
}

#endif

void* TMemMan::MallocRelease(size_t size)
{
	return GetMemSmall()->Allocate(size);
}

//	Reallocate, pMem must point to a mmeory block that is allocated by
//	a_malloc or a_realloc.
void* TMemMan::RealloRelease(void* pMem, size_t size)
{
	if (!size && pMem)
	{
		Free(pMem);
		return NULL;
	}

	if (!pMem)	//	Act as malloc
		return MallocRelease(size);

	//	Get old memory block
	int maxSize = CalMemUseableSize(pMem);

	//	If old memory block is big enough, return memory block unchanged.
	if ((int)size <= maxSize)
		return pMem;

	//	Allocate new memory
	void* pNewMem = MallocRelease(size);
	if (!pNewMem)
		return NULL;

	if (pNewMem != pMem)
	{
		//	Duplicate data
		memcpy(pNewMem, pMem, maxSize);
		Free(pMem);
	}

	return pNewMem;
}

//	Allocate a aligned memory block.
void* TMemMan::MallocAlignRelease(size_t size, int align)
{
	int tsize = size + align + sizeof(int);
	char* data = (char*)MallocRelease(tsize);
	char* data2;

	if (data)
	{
		if ((unsigned int)(data) % align)
			data2 = (char*)(((unsigned int)data & (~(align - 1))) + align);
		else
			data2 = data + ((align > sizeof(int)) ? align : sizeof(int));

		if (data2 - data < sizeof(int))
			data2 += align;

		*(int*)(data2 - sizeof(int)) = data2 - data;

		return data2;
	}

	return NULL;
}

