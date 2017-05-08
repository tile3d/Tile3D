#pragma once

class TMemSmall;
class TMemLarge;
class TMemDump;

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
	TMemSmall* GetMemSmall() { return m_pMemSmall; }
	TMemLarge* GetMemLarge() { return m_pMemLarge; }

	//	Get next memory block's ID
	int GetNextID();
	//	Get current memory block ID
	int GetID() { return m_idCnt; }
	//	Get peak size of memory
	int	GetPeakSize() { return m_peakSize; }
	//	Get allocation counter
	int GetAllocSize() { return m_sizeCnt; }
	//	Get allocated raw size
	int GetAllocRawSize() { return m_rawSizeCnt; }
	//	Add total allocated size
	void AddAllocSize(int size);
	//	Add total allocated raw size
	void AddAllocRawSize(int size);

	//	Get a memory block's usable size
	int CalMemUseableSize(void* pMem);

	void GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt);

	void GetLargeMemInfo(int* piBlkCnt, int* piAllocSize);

	void GarbageCollect();

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
	TMemSmall*		m_pMemSmall;
	TMemLarge*		m_pMemLarge;

	int	m_lock;			//	Atom used to ensure thread safe
	int	m_idCnt;		//	Allocate ID counter
	int	m_peakSize;		//	Peak size (maximum size of memory)
	int	m_sizeCnt;		//	Allocate size counter
	int	m_rawSizeCnt;	//	Raw size counter

	static TMemMan	gMemMan;
};


