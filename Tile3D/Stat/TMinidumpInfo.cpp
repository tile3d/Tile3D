#include "TMinidumpInfo.h"
#include "TMinidumpInfoImp.h"

#ifdef PLATFORM_WIN
#include <Stat/Windows/TMinidumpInfoImpWin.h>
#endif

TMinidumpInfo::TMinidumpInfo(const char* minidump_file, const char* minidump_symbol_file)
{
	m_minidump_file = minidump_file;
	m_minidump_symbol_file = minidump_symbol_file;

#ifdef PLATFORM_WIN
	pMinidumpInfoImp = new TMinidumpInfoImpWin();
#endif
}

bool TMinidumpInfo::AnalyzeDumpFile(TCrashInfo & m_crash_info)
{
	return pMinidumpInfoImp->AnalyzeDumpFile(m_crash_info, m_minidump_file, m_minidump_symbol_file);
}