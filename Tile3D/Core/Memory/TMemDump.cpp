#include "TMemDump.h"
#include <string.h>

FILE * TMemDump::GetMemoryHistoryLog()
{
	return g_pLogFile;
}

template<typename _TBlk>
static void TMemDump::DumpBlockInfo(const _TBlk* pBlock)
{
	char buffer[1024];

	_snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]),
		"------------------------------------\n"
		"Leak %d byte(s):\n", pBlock->iRawSize);
	OutputDebugStringA(buffer);

	for (int i = 0; i < MAX_SYM_CALLSTACK_LV; ++i)
	{
		if (pBlock->callers[i] < 0xffff)
			break;

		ExportSymbolInfo((void*)pBlock->callers[i]);
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
			sprintf(szPart, "0x%p)", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%p <- ", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
		}
	}

	a_LogOutput(1, szMsg);

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
			sprintf(szPart, "0x%p)", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%p <- ", pBlock->m_callers[i]);
			strcat(szMsg, szPart);
		}
	}

	a_LogOutput(1, szMsg);

	DumpBlockInfo(pBlock);
}


void TMemDump::DumpAllocHistory(FILE* pf, const TMemSmallBlock* pBlock)
{
#ifdef _DEBUG
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "A %d %d ", timeGetTime(), pBlock->m_rawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%p ", pBlock->m_callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
#endif
}

void TMemDump::DumpDeleteHistory(FILE* pf, const TMemSmallBlock* pBlock)
{
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "D %d %d ", timeGetTime(), pBlock->iRawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%p ", pBlock->callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
}

void TMemDump::DumpDeleteHistory(FILE* pf, const TMemLargeBlock* pBlock)
{
	if (!pf)
		return;

	char szMsg[1024] = { 0 };
	sprintf(szMsg, "D %d %d ", timeGetTime(), pBlock->m_rawSize);
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = { 0 };
		sprintf(szPart, "0x%p ", pBlock->m_callers[i]);
		strcat(szMsg, szPart);
	}

	fprintf(pf, "%s\n", szMsg);
}

void TMemDump::DumpBlockInfo(const _TBlk* pBlock)
{
	char buffer[1024];

	_snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]),
		"------------------------------------\n"
		"Leak %d byte(s):\n", pBlock->iRawSize);
	OutputDebugStringA(buffer);

	for (int i = 0; i < MAX_SYM_CALLSTACK_LV; ++i)
	{
		if (pBlock->callers[i] < 0xffff)
			break;

		ExportSymbolInfo((void*)pBlock->callers[i]);
	}
}

void TMemDump::ExportSymbolInfo(void * ptr)
{
	char szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
	if (LPSTR pStr = PathFindFileNameA(szFileName))
		*pStr = NULL;
	HANDLE hProcess = _LoadSymForCurrentProcess(szFileName);
	SymbloInfo si;
	si = _AddressToSymbol(ptr, hProcess);

	char szMsg[1024];
	if (si.LineNumber == 0)
	{
		_snprintf(szMsg, sizeof(szMsg), " %s -- 0x%p\n", si.Name, si.pAddr);
	}
	else
	{
		_snprintf(szMsg, sizeof(szMsg), " %s (%d): %s -- 0x%p\n", si.FileName, si.LineNumber, si.Name, si.pAddr);
	}
	OutputDebugStringA(szMsg);
}

