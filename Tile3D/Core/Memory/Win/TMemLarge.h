#pragma once

#include <stdio.h>
#include "TMemCommon.h"


struct TMemLargeBlock
{
#ifdef DEBUG_MEMORY
	TMemLargeBlock * m_pPrev;
	TMemLargeBlock * m_pNext;
	int	m_rawSize;
	unsigned short m_callers[MAX_CALLSTACK_LV];
#endif
	short m_flags;		//memory flag
	short m_reserved;	//reserved
	int	m_blockSize;	//memory block size

#ifdef DEBUG_MEMORY
	unsigned long m_soFlags[2];	//slop over flags
#endif
};


class TMemLarge
{
//	Constructors and Destructors
public:		
	TMemLarge()
	{
		m_lock = 0;
		m_blockList = nullptr;
		m_pMemMan = nullptr;
		m_blockCnt = 0;
		m_allocSize = 0;
	}

	virtual ~TMemLarge()
	{
#ifdef _DEBUG
		Dump();
#endif
	}

//	Attributes
public:
	friend class TMemMan;

//	Operations
public:
	void SetMemManager(TMemMan * man) {
		m_pMemMan = man;
	}

	//	Allocate memory
	void* Allocate(size_t size);
	//	Free memory
	void Free(void* p);

	//	Get total size allocated by AMemLarge
	int GetAllocSize() { return m_allocSize; }
	//	Get current block counter
	int GetBlockCnt() { return m_blockCnt; }

#ifdef _DEBUG
	void DumpMemoryBlocks(FILE* pFile);
#endif

//	Attributes
protected:	
	int				m_lock;	//	Atom used to ensure thread safe
	TMemMan*		m_pMemMan;		//	Manager manager
	TMemLargeBlock*	m_blockList;	//	Block list
	int				m_blockCnt;	//	Block counter
	int				m_allocSize;	//	Total size allocated by AMemLarge

//	Operations
protected:	
#ifdef _DEBUG
	//	Dump memory leak
	void Dump();
#endif
};
