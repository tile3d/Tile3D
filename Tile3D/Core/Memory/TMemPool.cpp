#include "TMemPool.h"
#include "TMemSmall.h"

int TMemPool::CalcBlockSize()
{ 
	return m_dataSize + sizeof(TMemSmallBlock); 
}

/*	Initialize memory pool buffer

pSlot: pool slot's address
*/
void TMemPool::InitPool(TMemPoolSlot* pSlot)
{
	char* pNext, *pByte = (char*)pSlot->m_pool;
	for (int i = 0; i<m_blockCount - 1; i++, pByte = pNext)
	{
		pNext = pByte + m_blockSize;
		((TMemSmallBlock*)pByte)->m_pNext = (TMemSmallBlock*)pNext;
	}

	//	The last block
	((TMemSmallBlock*)pByte)->m_pNext = NULL;
}

/*	Create memory pool

iBlkSize: size of eack block in pool
*/
TMemSmallBlock* TMemPool::Allocate()
{
	//	Allocate pool buffer
	TMemPoolSlot slot;
	//	Slot.pPool = (s_MEMSMALLBLK*)_RawMemAlloc(m_iBlkCount * m_iBlkSize);
	slot.m_pool = (TMemSmallBlock*)TMemCommon::RawMemAlloc(MEM_POOL_DATA_SIZE);
	if (!slot.m_pool)
		return NULL;

	//	Initialize memory pool
	InitPool(&slot);

	m_pools.Push(slot);
	return slot.m_pool;
}
