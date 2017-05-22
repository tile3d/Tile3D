#pragma once

#include "TMemCommon.h"
#include "TMemVec.h"


struct TMemSmallBlock;
class TMemPool
{
public:
	struct TMemPoolSlot
	{
		TMemSmallBlock * m_pool;
	};
public:
	TMemPool() {
		m_blockCount = 0;
		m_dataSize = 0;
		m_blockSize = 0;
		m_gcCount = 0;
	}

	~TMemPool() {
		for (int i = 0; i < m_pools.Size(); i++) {
			TMemCommon::RawMemFree(m_pools[i].m_pool);
		}
	}

public:
	//	Get slot number
	int	GetSlotNum() const { return m_pools.Size(); }
	//	Get pool of specified slot
	TMemSmallBlock* GetPool(int iSlot) const { return m_pools[iSlot].m_pool; }

	//  Get block count
	int	GetBlockCount() const { return m_blockCount; }
	//	Set block count 
	void SetBlockCount(int count) { m_blockCount = count; }
	//	Get block size
	int GetBlockSize() const { return m_blockSize; }
	//	Set data size
	void SetDataSize(int size)
	{
		m_dataSize = size;
		m_blockSize = CalcBlockSize();
	}
	//	Get data size
	int GetDataSize() const { return m_dataSize; }
	//	Get garbage collect counter
	int GetGCCounter() { return m_gcCount; }
	//	Inc garbage collect counter
	int IncGCCounter() { return ++m_gcCount; }
	//	Clear garbage collect counter
	void ClearGCCounter() { m_gcCount = 0; }

	//	Release pool slot
	void ReleasePool(int iSlot) {
		TMemCommon::RawMemFree(m_pools[iSlot].m_pool);
		m_pools.RemoveQuick(iSlot);
	}

	//	Allocate memory pool
	TMemSmallBlock* Allocate();

private:
	// Calculate the block size
	int CalcBlockSize();

	//	Initialize a pool
	void InitPool(TMemPoolSlot* pSlot);

private:
	int m_blockCount;
	int m_blockSize;
	int m_dataSize;
	int m_gcCount;
	TMemVec<TMemPoolSlot> m_pools;
};

