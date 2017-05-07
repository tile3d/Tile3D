#include "TMemCommon.h"
#include "TMemSmall.h"
#include "TMemLarge.h"

class TMemMan
{
public:		//	Types

public:		//	Constructors and Destructors

	TMemMan();
	virtual ~TMemMan();

public:		//	Attributes

public:		//	Operations

	TMemSmall* GetMemSamll() { return &m_memSmall; }
	TMemLarge* GetMemLarge() { return &m_memLarge; }
	ATempMemMan* GetTempMemMan() { return &m_tempMemMan; }

	//	Get next memory block's ID
	long GetNextID() { return ::InterlockedIncrement(&m_lIDCnt); }
	//	Get current memory block ID
	long GetID() { return m_lIDCnt; }
	//	Get peak size of memory
	int	GetPeakSize() { return m_iPeakSize; }
	//	Get allocation counter
	int GetAllocSize() { return m_iSizeCnt; }
	//	Get allocated raw size
	int GetAllocRawSize() { return m_lRawSizeCnt; }

	//	Add total allocated size
	void AddAllocSize(int iSize)
	{
		_MemThreadLock(&m_lThreadAtom);

		if ((m_iSizeCnt += iSize) > m_iPeakSize)
			m_iPeakSize = m_iSizeCnt;

		_MemThreadUnlock(&m_lThreadAtom);
	}

	//	Add total allocated raw size
	void AddAllocRawSize(int iSize)
	{
		::InterlockedExchangeAdd(&m_lRawSizeCnt, iSize);
	}

	//	Round up memory size
	static int RoundUp(int iSize)
	{
		if (0 == iSize)
			return MEM_ALIGN;
		else
			return (iSize + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	}

protected:	//	Attributes

	TMemSmall		m_memSmall;
	TMemLarge		m_memLarge;
	TTempMemMan		m_tempMemMan;

	long	m_lThreadAtom;	//	Atom used to ensure thread safe
	long	m_lIDCnt;		//	Allocate ID counter
	int		m_iPeakSize;	//	Peak size (maximum size of memory)
	int		m_iSizeCnt;		//	Allocate size counter
	long	m_lRawSizeCnt;	//	Raw size counter

protected:	//	Operations

};


TMemMan::TMemMan() :
	m_TempMemMan(20 * 1024 * 1024)
{
	m_memSmall.m_pMemMan = this;
	m_memLarge.m_pMemMan = this;

	m_lIDCnt = 0;
	m_iPeakSize = 0;
	m_iSizeCnt = 0;
	m_lThreadAtom = 0;
	m_lRawSizeCnt = 0;
}

TMemMan::~TMemMan()
{
	m_TempMemMan.Release();
#ifdef _DEBUG
	a_LogOutput(0, "Maximum memory used: %d (K)", (m_iPeakSize + 1023) / 1024);
#endif
}

