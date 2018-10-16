#pragma once

#include "TMinidump.h"

class TMinidumpInfoImp
{
public:
	virtual bool AnalyzeDumpFile(TCrashInfo & m_crash_info, const char* minidump_file, const char* minidump_symbol_path) { return false; }
};
