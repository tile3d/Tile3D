#pragma warning (disable: 4996)
#pragma warning (disable: 4091)

#include <Windows.h>
#include <DbgHelp.h>
#include <Shlwapi.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <map>
#include <Util/TLog.h>
#include "TMemDump.h"
#include "TMemCommon.h"
#include "TMemSmall.h"
#include "TMemLarge.h"
#include "TMemMan.h"

FILE * TMemDump::m_pLogFile = nullptr;

struct SymbloInfo
{
	const void*		pAddr;
	char			name[128];
	char			fileName[128];
	unsigned long   lineNumber;
};

typedef std::map<void*, SymbloInfo> SymbloInfoMap;
static SymbloInfoMap addr_info_map;



void TMemDump::DumpAdditionalInfoMap(FILE* pAddiInfoFile)
{
	for (SymbloInfoMap::iterator itr = addr_info_map.begin(); itr != addr_info_map.end(); ++itr)
	{
		fprintf(pAddiInfoFile, "0x%p %s\n(%d): %s\n", itr->first, itr->second.fileName, itr->second.lineNumber, itr->second.name);
	}
}
void TMemDump::DumpAdditionalInfo(FILE* pAddiInfoFile)
{
	//	Additional information
	int iTotalFreeBytes = 0;
	for (int i = 0; i < MEM_SLOTNUM; ++i)
	{
		int iBlkSize, iBlkCnt, iFreeCnt;
		TMemMan::GetInstance()->GetSmallMemInfo(i, &iBlkSize, &iBlkCnt, &iFreeCnt);
		fprintf(pAddiInfoFile, "%d slot has %d free bytes\n", iBlkSize, iBlkSize * iFreeCnt);
		iTotalFreeBytes += iBlkSize * iFreeCnt;
	}

	fprintf(pAddiInfoFile, "Totally %d free bytes\n", iTotalFreeBytes);
	fprintf(pAddiInfoFile, "----- ----- ----- -----\n");
	DumpAdditionalInfoMap(pAddiInfoFile);
}


void TMemDump::DumpMemoryBlocks(FILE* pFile, FILE* pAddiInfoFile)
{
	if (!pFile)
		return;

	TMemMan::GetInstance()->GetMemLarge()->DumpMemoryBlocks(pFile);
	TMemMan::GetInstance()->GetMemSmall()->DumpMemoryBlocks(pFile);

	DumpAdditionalInfo(pAddiInfoFile);
}


template<typename TBlk>
void TMemDump::DumpBlockInfo(const TBlk* pBlock)
{
	char buffer[1024];
	_snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "------------------------------------\n" 
		"Leak %d byte(s):\n", pBlock->m_rawSize);
	buffer[1023] = '\0';
	OutputDebugStringA(buffer);

	for (int i = 0; i < MAX_SYM_CALLSTACK_LV; ++i)
	{
		if (pBlock->m_callers[i] < 0xffff)
			break;

		ExportSymbolInfo((void*)pBlock->m_callers[i]);
	}
}

void TMemDump::DumpMemSmallBlkToLogOutput(const TMemSmallBlock* pBlock)
{
	char szMsg[1024] = { 0 };
	sprintf(szMsg, "Memory [%d bytes@%p] leak at (", pBlock->m_rawSize, (char*)pBlock + sizeof(TMemSmallBlock));
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		if (i == MAX_CALLSTACK_LV - 1 || (!pBlock->m_callers[i] && i >= 7))
		{
			sprintf(szPart, "0x%d)", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%d <- ", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
		}
	}

	TLog::Log(LOG_INFO, "MEMORY", szMsg);
	DumpBlockInfo(pBlock);
}

void TMemDump::DumpMemLargeBlkToLogOutput(const TMemLargeBlock* pBlock)
{
	char szMsg[1024] = { 0 };
	sprintf(szMsg, "Memory [%d bytes@%p] leak at (", pBlock->m_rawSize, (char*)pBlock + sizeof(TMemLargeBlock));
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		if (i == MAX_CALLSTACK_LV - 1 || (!pBlock->m_callers[i] && i >= 7))
		{
			sprintf(szPart, "0x%d)", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%d <- ", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
		}
	}

	TLog::Log(LOG_INFO, "MEMORY", szMsg);
	DumpBlockInfo(pBlock);
}


void TMemDump::DumpAllocHistory(FILE* pf, const TMemSmallBlock* pBlock)
{
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "A %lld %d ", time(NULL), pBlock->m_rawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%d ", pBlock->m_callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
}

void TMemDump::DumpDeleteHistory(FILE* pf, const TMemSmallBlock* pBlock)
{
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "D %lld %d ", time(NULL), pBlock->m_rawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%d ", pBlock->m_callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
}

void TMemDump::DumpDeleteHistory(FILE* pf, const TMemLargeBlock* pBlock)
{
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "D %lld %d ", time(NULL), pBlock->m_rawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%d ", pBlock->m_callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
}

static HANDLE LoadSymForCurrentProcess(const char* UserSearchPath)
{
	static HANDLE hProcess;
	static const  int bufsz = 256;
	static char   szLastPath[bufsz];
	szLastPath[bufsz - 1] = '\0';

	if (!hProcess || (UserSearchPath && strncmp(szLastPath, UserSearchPath, bufsz - 1)))
	{
		if (UserSearchPath)
		{
			strncpy(szLastPath, UserSearchPath, bufsz - 1);
		}
		else {
			szLastPath[0] = 0;
		}

		szLastPath[bufsz - 1] = 0;

		hProcess = GetCurrentProcess();
		DWORD oldFlag = SymGetOptions();
		DWORD flags = SymSetOptions(oldFlag | SYMOPT_LOAD_LINES);
		bool bSuccess = SymInitialize(hProcess, (char*)UserSearchPath, TRUE) ? true : false;
	}

	return hProcess;
}

static SymbloInfo AddressToSymbol(const void* pAddr, HANDLE hProcess)
{
	SymbloInfo si;
	memset(&si, 0, sizeof(si));
	si.pAddr = pAddr;

	DWORD  error;
	DWORD  dwAddress = (DWORD)pAddr;
	ULONG64 buffer[(sizeof(SYMBOL_INFO) +
		MAX_SYM_NAME * sizeof(TCHAR) +
		sizeof(ULONG64) - 1) /
		sizeof(ULONG64)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;
	DWORD64 dw64Offset;
	if (SymFromAddr(hProcess, dwAddress, &dw64Offset, pSymbol))
	{
		strncpy(si.name, pSymbol->Name, sizeof(si.name));
	}
	else
	{
		error = GetLastError();
	}

	IMAGEHLP_LINE line;
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
	DWORD dwOffset = (DWORD)dw64Offset;
	if (SymGetLineFromAddr(hProcess, dwAddress, &dwOffset, &line))
	{
		strncpy(si.fileName, line.FileName, sizeof(si.fileName));
		si.lineNumber = line.LineNumber;
	}
	else
	{
		error = GetLastError();
	}

	return si;
}


void TMemDump::ExportSymbolInfo(void * ptr)
{
	char szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
	if (LPSTR pStr = PathFindFileNameA(szFileName))
		*pStr = NULL;
	HANDLE hProcess = LoadSymForCurrentProcess(szFileName);
	SymbloInfo si;
	si = AddressToSymbol(ptr, hProcess);

	char szMsg[1024];
	if (si.lineNumber == 0)
	{
		_snprintf(szMsg, sizeof(szMsg), " %s -- 0x%p\n", si.name, si.pAddr);
	}
	else
	{
		_snprintf(szMsg, sizeof(szMsg), " %s (%d): %s -- 0x%p\n", si.fileName, si.lineNumber, si.name, si.pAddr);
	}
	szMsg[1023] = '\0';
	OutputDebugStringA(szMsg);
}

void TMemDump::TouchToLogAddressSymbol(void* pAddress)
{
	if (pAddress == NULL)
		return;

	static HANDLE hProcess = NULL;
	if (hProcess == NULL)
	{
		char szFileName[MAX_PATH];
		GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
		if (LPSTR pStr = PathFindFileNameA(szFileName))
			*pStr = NULL;
		hProcess = LoadSymForCurrentProcess(szFileName);
	}

	if (addr_info_map.find(pAddress) == addr_info_map.end())
	{
		addr_info_map[pAddress] = AddressToSymbol(pAddress, hProcess);
	}
}

void TMemDump::ExportMemLog(const char* szPath)
{
	FILE* pFile = fopen(szPath, "wt");
	if (!pFile)
		return;

	int nSize = 0;
	char szBuf[256];
	memset(szBuf, 0, 256);

	nSize = sprintf_s(szBuf, 256, "////////////////////MemLog//////////////////////\n");
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "PeakSize=\t%d\n", TMemMan::GetInstance()->GetPeakSize());
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "AllocCounter=\t%d\n", TMemMan::GetInstance()->GetID());
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "CurSize=\t%d\n", TMemMan::GetInstance()->GetAllocSize());
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "RawSize=\t%d\n", TMemMan::GetInstance()->GetAllocRawSize());
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "\n");
	fwrite(szBuf, sizeof(char), nSize, pFile);


	////////////////////MemLarge////////////////////
	TMemLarge* pMemLarge = TMemMan::GetInstance()->GetMemLarge();
	nSize = sprintf_s(szBuf, 256, "////////////////////MemLarge////////////////////\n");
	fwrite(szBuf, sizeof(char), nSize, pFile);

	int iLMemAllocSize = pMemLarge->GetAllocSize();
	int iLMemBlkCnt = pMemLarge->GetBlockCnt();
	nSize = sprintf_s(szBuf, 256, "LargeTotalBlkSize= %-20d, LargeTotalBlkCount= %-12d\n", iLMemAllocSize, iLMemBlkCnt);
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "\n");
	fwrite(szBuf, sizeof(char), nSize, pFile);


	////////////////////MemSmall////////////////////
	TMemSmall* pMemSmall = TMemMan::GetInstance()->GetMemSmall();
	nSize = sprintf_s(szBuf, 256, "////////////////////MemSmall////////////////////\n");
	fwrite(szBuf, sizeof(char), nSize, pFile);

	int iSMemAllocSize = 0;
	int iSMemBlkCnt = 0;
	int iSMemAllocFreeSize = 0;
	int iSMemBlkCntFree = 0;
	for (int i = 0; i < MEM_SLOTNUM; i++)
	{
		int iPoolSlotNum = pMemSmall->GetPoolSlotNum(i);
		int iPoolSlotBlkCount = pMemSmall->GetPoolSlotBlockNum(i);
		int iBlkSize = pMemSmall->GetBlockSize(i);
		int iDataSize = pMemSmall->GetDataSize(i);
		int iBlkCount = pMemSmall->GetTotalBlockCnt(i);
		int iBlkFree = pMemSmall->GetFreeBlockCnt(i);
		int iTotal = iBlkSize * iBlkCount;
		iSMemAllocSize += iTotal;
		iSMemBlkCnt += iBlkCount;

		iSMemAllocFreeSize += iBlkSize * iBlkFree;
		iSMemBlkCntFree += iBlkFree;

		nSize = sprintf_s(szBuf, 256, "PoolSlotNum= %-8d, PoolSlotBlkCount= %-8d, PoolSlotDataSize= %-4d, BlkSize= %-4d, BlkCount= %-12d, BlkTotalSize= %-12d, BlkFree= %-12d\n", iPoolSlotNum, iPoolSlotBlkCount, iDataSize, iBlkSize, iBlkCount, iTotal, iBlkFree);
		fwrite(szBuf, sizeof(char), nSize, pFile);
	}

	nSize = sprintf_s(szBuf, 256, "SmallTotalBlkSize= %-20d, SmallTotalBlkCount= %-12d\n", iSMemAllocSize, iSMemBlkCnt);
	fwrite(szBuf, sizeof(char), nSize, pFile);

	nSize = sprintf_s(szBuf, 256, "SmallFreeTotalBlkSize= %-20d, SmallFreeTotalBlkCount= %-12d\n", iSMemAllocFreeSize, iSMemBlkCntFree);
	fwrite(szBuf, sizeof(char), nSize, pFile);

	fclose(pFile);
}


