#pragma once

#include "TMinidump.h"

class TMinidumpInfoImp;
class TMinidumpInfo
{
public:
	TMinidumpInfo(const char* minidump_file, const char* minidump_symbol_file);

	bool AnalyzeDumpFile(TCrashInfo & m_crash_info);

private:
	const char* m_minidump_file;
	const char* m_minidump_symbol_file;
	TCrashInfo m_crash_info;
	TMinidumpInfoImp * pMinidumpInfoImp;
};

