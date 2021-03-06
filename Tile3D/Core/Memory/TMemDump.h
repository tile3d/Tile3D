#pragma once

#include <map>

struct TMemSmallBlock;
struct TMemLargeBlock;

struct SymbloInfo
{
	const void*		pAddr;
	char			name[128];
	char			fileName[128];
	unsigned long   lineNumber;
};


//TBD: replace std::map to TMap or THashMap
typedef std::map<void*, SymbloInfo> SymbloInfoMap;


class TMemDump
{
public:
	static void DumpAllocHistory(FILE* pf, const TMemSmallBlock* pBlock);

	static void DumpDeleteHistory(FILE* pf, const TMemSmallBlock* pBlock);
	static void DumpDeleteHistory(FILE* pf, const TMemLargeBlock* pBlock);

	static void DumpMemSmallBlkToLogOutput(const TMemSmallBlock* pBlock);
	static void DumpMemLargeBlkToLogOutput(const TMemLargeBlock* pBlock);
	template<typename TBlk>
	static void DumpBlockInfo(const TBlk* pBlock);

	static void ExportSymbolInfo(void * ptr);
	static void TouchToLogAddressSymbol(void* pAddress);

	static void DumpAdditionalInfoMap(FILE* pAddiInfoFile);
	static void DumpAdditionalInfo(FILE* pAddiInfoFile);
	static void DumpMemoryBlocks(FILE* pFile, FILE* pAddiInfoFile);

	static void ExportMemLog(const char* szPath);

	static void SetMemoryHistoryLog(FILE * logFile) { m_pLogFile = logFile; }
	static FILE * GetMemoryHistoryLog() { return m_pLogFile; }

private:
	static FILE * m_pLogFile;
	static SymbloInfoMap addr_info_map;
};



