#include "TMemMan.h"
#include "TMemSmall.h"
#include "TMemLarge.h"
#include "TMemDump.h"
#include <Core/Lock/TMutexLock.h>
#include <Common/TLog.h>

#pragma warning (disable: 4996)
#pragma warning (disable: 4073)


/*	init_seg (lib) ensure memoryMan to be constructed before and
destructed after all other application objects. This is very
important, because some application's global or static objects may allocate
or free memory in their constructor or destructors.

init_seg (lib) will cause a VC compiling warning 4073, disable it.
*/

#pragma init_seg (lib)


TMemMan TMemMan::gMemMan;


TMemMan::TMemMan() {
	m_memSmall.SetMemManager(this);
	m_memLarge.SetMemManager(this);
}

TMemMan::~TMemMan() {

#ifdef DEBUG_MEMORY
		TLog::Log(LOG_INFO, "MEMORY", "Maximum memory used: %d (K)", (m_peakSize.Get() + 1023) / 1024);
#endif
}

int TMemMan::GetNextID() 
{ 
	return m_idCnt.Increment();
}

void TMemMan::AddAllocSize(int size)
{
	m_lock.Lock();
	if (m_sizeCnt.Add(size) > m_peakSize.Get()) {
		m_peakSize.Set(m_sizeCnt.Get());
	}
	m_lock.Unlock();
}

//	Add total allocated raw size
void TMemMan::AddAllocRawSize(int size)
{
	m_rawSizeCnt.Add(size);
}


void TMemMan::GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt)
{
	*piBlkSize = m_memSmall.GetBlockSize(iSlot);
	*piBlkCnt = m_memSmall.GetTotalBlockCnt(iSlot);
	*piFreeCnt = m_memSmall.GetFreeBlockCnt(iSlot);
}

void TMemMan::GetLargeMemInfo(int* piBlkCnt, int* piAllocSize)
{
	*piBlkCnt = m_memLarge.GetBlockCnt();
	*piAllocSize = m_memLarge.GetAllocSize();
}

void TMemMan::GarbageCollect()
{
	m_memSmall.GarbageCollect();
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
#ifdef DEBUG_MEMORY
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
	int frame_cur = 0;
	int c = 0;
	const int skip = 1;

	__asm
	{
		mov frame_cur, ebp
	}

	memset(pBlock->m_callers, 0, sizeof(pBlock->m_callers));
	while (true)
	{
		int thisCall = *(int*)(frame_cur + 4);
		if (c >= skip)
			pBlock->m_callers[c - skip] = thisCall;

		c++;

		if (c >= MAX_CALLSTACK_LV + skip || thisCall == 0)
			break;

		int frame_up = *(int *)frame_cur;
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
	int dwMaxSize = CalMemUseableSize(pMem);

	//	If old memory block is big enough, return memory block unchanged.
	if (size <= (size_t)dwMaxSize)
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

void TMemMan::ExportMemoryInfo(const char * file)
{
	TMemDump::ExportMemLog(file);
}