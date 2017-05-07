#pragma once

#include <stdio.h>
#include "TMemCommon.h"

class TMemLarge
{
public:		//	Constructors and Destructors

	TMemLarge()
	{
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

protected:	//	Attributes

	static long		m_lThreadAtom;	//	Atom used to ensure thread safe
	TMemMan*		m_pMemMan;		//	Manager manager
	TMemLargeBlock*	m_blockList;	//	Block list
	int				m_blockCnt;	//	Block counter
	int				m_allocSize;	//	Total size allocated by AMemLarge

protected:	//	Operations

#ifdef _DEBUG
			//	Dump memory leak
	void Dump();
#endif
};

