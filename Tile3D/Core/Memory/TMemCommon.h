#pragma once

#include <stdlib.h>


#define MAX_CALLSTACK_LV	8											//	Max call stack level
#define MAX_SYM_CALLSTACK_LV	256
#define MEM_POOL_DATA_SIZE			512 * 1024
#define MEM_ALIGN					16
#define MEM_SMALLSIZE				1024
#define MEM_SLOTNUM					(MEM_SMALLSIZE / MEM_ALIGN)
#define SLOPOVER_SIZE				(sizeof (unsigned long) * 2)       //	Size used for slop-over checking

#define MEM_GC_COUNTER				1000

#define MEM_ALLOC_FLAG_S			0x0100
#define MEM_FREE_FLAG_S				0x0101
#define MEM_ALLOC_FLAG_L			0x0200
#define MEM_FREE_FLAG_L				0x0201

#define MEM_ALLOC_FILL				0xcd
#define MEM_FREE_FILL				0xfe
#define MEM_SLOP_OVER				0x98989898

#define DEBUG_MEMORY

struct TMemSmallBlock
{
#ifdef DEBUG_MEMORY
	int m_rawSize;								//raw memory size
	unsigned long	m_callers[MAX_CALLSTACK_LV];	//	max 4 level call stack
#endif
	short m_flag;				//memmory flag
	short m_poolSlot;			//pool slot index
	TMemSmallBlock * m_pNext;	//next free block
#ifdef DEBUG_MEMORY
	unsigned long m_soFlags[2];	//slop over flags
#endif
};


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

void* RawMemAlloc(size_t size) {
	return malloc(size);
}

void RawMemFree(void* p) {
	free(p);
}

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
