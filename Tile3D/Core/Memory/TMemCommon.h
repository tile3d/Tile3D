#pragma once

#include <stdlib.h>

#define MAX_CALLSTACK_LV	8											//	Max call stack level
#define MAX_SYM_CALLSTACK_LV	256
#define MEM_POOL_DATA_SIZE			512 * 1024
#define MEM_ALIGN					16
#define MEM_SMALLSIZE				1024
#define MEM_SLOTNUM					(MEM_SMALLSIZE / MEM_ALIGN)

#define MEM_POOL_MAX_BLOCK_COUNT	MEM_POOL_DATA_SIZE / 16
#define MEM_GC_COUNTER				1000

#define MEM_ALLOC_FLAG_S			0x0100
#define MEM_FREE_FLAG_S				0x0101
#define MEM_ALLOC_FLAG_L			0x0200
#define MEM_FREE_FLAG_L				0x0201

#define MEM_ALLOC_FILL				0xcd
#define MEM_FREE_FILL				0xfe
#define SLOPOVER_SIZE				(sizeof (unsigned long) * 2)       //	Size used for slop-over checking
#define MEM_SLOP_OVER				0x98989898


struct TMemSmallBlock
{
	TMemSmallBlock * m_pNext;	//next free block
	short m_flag;				//memory flag
	short m_poolSlot;			//pool slot index
#ifdef DEBUG_MEMORY
	int m_rawSize;								//raw memory size
	unsigned long	m_callers[MAX_CALLSTACK_LV];	//	max 4 level call stack
	unsigned long m_soFlags[2];	//slop over flags
#endif
};



struct TMemLargeBlock
{
#ifdef DEBUG_MEMORY
	TMemLargeBlock * m_pPrev;
	TMemLargeBlock * m_pNext;
#endif
	int	m_blockSize;	//memory block size
	short m_flags;		//memory flag
	short m_reserved;	//reserved
#ifdef DEBUG_MEMORY
	int	m_rawSize;
	unsigned long m_callers[MAX_CALLSTACK_LV];
	unsigned long m_soFlags[2];	//slop over flags
#endif
};


class TMemCommon
{
public:
	//	Fill slop over flags
	static void FillSlopOverFlags(void* pBuf)
	{
		unsigned long * pdw = (unsigned long*)pBuf;
		*(pdw + 0) = MEM_SLOP_OVER;
		*(pdw + 1) = MEM_SLOP_OVER;
	}

	//	Check slop over flags
	static bool CheckSlopOver(const void* pBuf)
	{
		const unsigned long * pdw = (const unsigned long*)pBuf;
		if (*(pdw + 0) != MEM_SLOP_OVER || *(pdw + 1) != MEM_SLOP_OVER)
			return false;
		else
			return true;
	}

	//	Round up memory size
	static int RoundUp(int size)
	{
		if (0 == size)
			return MEM_ALIGN;
		else
			return (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	}

	static void* RawMemAlloc(size_t size) {
		return malloc(size);
	}

	static void RawMemFree(void* p) {
		free(p);
	}

};

