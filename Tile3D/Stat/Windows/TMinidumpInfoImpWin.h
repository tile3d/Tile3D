#pragma once

#include <Stat/TMinidumpInfoImp.h>

struct IDebugClient5;
struct IDebugControl4;
struct IDebugSymbols3;
struct IDebugAdvanced3;
class TMinidumpInfoImpWin : public TMinidumpInfoImp
{
public:
	TMinidumpInfoImpWin() {
		Client = nullptr;
		Control = nullptr;
		Symbol = nullptr;
		Advanced = nullptr;
	}

	virtual bool AnalyzeDumpFile(TCrashInfo & m_crash_info, const char* minidump_file, const char* minidump_symbol_path);


private:
	void InitSymbol(const char* symbol_path);

	void LoadModule(TCrashInfo & m_crash_info);
	void GetSystemInfo(TCrashInfo & m_crash_info);
	void GetThreadInfo();
	void GetExceptionInfo(TCrashInfo & m_crash_info);
	int GetCallstackInfo(TCrashInfo & crash_info);
private:
	IDebugClient5* Client;
	IDebugControl4* Control;
	IDebugSymbols3* Symbol;
	IDebugAdvanced3* Advanced;
};

