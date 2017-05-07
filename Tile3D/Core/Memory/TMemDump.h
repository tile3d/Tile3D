#pragma once

#include <stdio.h>
#include "TMemCommon.h"

class TMemDump
{
public:
	static FILE * GetMemoryHistoryLog();
	static void DumpAdditionalInfo(FILE* pAddiInfoFile);

	static void DumpAllocHistory(FILE* pf, const TMemSmallBlock* pBlock);
	static void DumpDeleteHistory(FILE* pf, const TMemSmallBlock* pBlock);
	static void DumpDeleteHistory(FILE* pf, const TMemLargeBlock* pBlock);

	static void DumpMemSmallBlkToLogOutput(const TMemSmallBlock* pBlock);
	static void DumpMemLargeBlkToLogOutput(const TMemLargeBlock* pBlock);

	static void ExportSymbolInfo(void * ptr);
};