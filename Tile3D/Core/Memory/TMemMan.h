#pragma once

#include "TMemSmall.h"
#include "TMemLarge.h"
#include <Core/Lock/TAtomicInt.h>
#include <Core/Lock/TMutexLock.h>


//TBD
//1) Change AllocSize from int to int64
//
class TMemMan
{

//singleton
public:
	static TMemMan * GetInstance() {
		return &gMemMan;
	}

//Constructors and Destructors
public:		
	TMemMan();
	~TMemMan();

//Methods
public:
	TMemSmall* GetMemSmall() { return &m_memSmall; }
	TMemLarge* GetMemLarge() { return &m_memLarge; }

	//	Get next memory block's ID
	int GetNextID();
	//	Get current memory block ID
	int GetID() { return m_idCnt.Get(); }
	//	Get peak size of memory
	int	GetPeakSize() { return m_peakSize.Get(); }
	//	Get allocation counter
	int GetAllocSize() { return m_sizeCnt.Get(); }
	//	Get allocated raw size
	int GetAllocRawSize() { return m_rawSizeCnt.Get(); }
	//	Add total allocated size
	void AddAllocSize(int size);
	//	Add total allocated raw size
	void AddAllocRawSize(int size);

	//	Get a memory block's usable size
	int CalMemUseableSize(void* pMem);

	void GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt);

	void GetLargeMemInfo(int* piBlkCnt, int* piAllocSize);

	void GarbageCollect();

	void ExportMemoryInfo(const char * file);
public:
	void * Malloc(size_t size);
	void * Realloc(void * pMem, size_t size);
	void * MallocAlign(size_t size, int align);
	void Free(void * p);
	void FreeAlign(void * p);

private:
#ifdef DEBUG_MEMORY
	void * MallocDebug(size_t size);
	void * ReallocDebug(void * pMem, size_t size);
	void * MallocAlignDebug(size_t size, int align);
#endif

	void * MallocRelease(size_t size);
	void * RealloRelease(void * pMem, size_t size);
	void * MallocAlignRelease(size_t size, int align);


protected:	//	Attribute
	TMemSmall		m_memSmall;
	TMemLarge		m_memLarge;

	TAtomicInt	m_idCnt;		//	Allocate ID counter
	TAtomicInt	m_peakSize;		//	Peak size (maximum size of memory)
	TAtomicInt	m_sizeCnt;		//	Allocate size counter
	TAtomicInt	m_rawSizeCnt;	//	Raw size counter
	TMutexLock	m_lock;	//	Lock to ensure thread safe

	static TMemMan	gMemMan;
};


