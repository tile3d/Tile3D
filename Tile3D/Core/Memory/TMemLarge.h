#pragma once

#include <stdio.h>
#include "TMemCommon.h"
#include "Core/Lock/TMutexLock.h"


class TMemLarge
{
//	Constructors and Destructors
public:		
	TMemLarge()
	{
		m_blockList = nullptr;
		m_pMemMan = nullptr;
		m_blockCnt = 0;
		m_allocSize = 0;
	}

	virtual ~TMemLarge()
	{
#ifdef DEBUG_MEMORY
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

#ifdef DEBUG_MEMORY
	void DumpMemoryBlocks(FILE* pFile);
#endif

//	Attributes
protected:	
	TMutexLock				m_lock;	//	Atom used to ensure thread safe
	TMemMan*		m_pMemMan;		//	Manager manager
	TMemLargeBlock*	m_blockList;	//	Block list
	int				m_blockCnt;	//	Block counter
	int				m_allocSize;	//	Total size allocated by AMemLarge

//	Operations
protected:	
#ifdef DEBUG_MEMORY
	//	Dump memory leak
	void Dump();
#endif
};

