#pragma once

#include <string>
#include <vector>

//ModuleInfo
struct TCrashModuleInfo
{
public:
	std::string Name;
	//	std::string Extension;
	unsigned long long BaseOfImage;
	unsigned long SizeOfImage;
	int Major;
	int Minor;
	int Patch;
	int Revision;

	TCrashModuleInfo()
		: BaseOfImage(0)
		, SizeOfImage(0)
		, Major(0)
		, Minor(0)
		, Patch(0)
		, Revision(0)
	{
	}
};

//System info
struct TCrashSystemInfo
{
public:
	int OSMajor;
	int OSMinor;
	int OSBuild;
	int OSRevision;
	int ProcessorCount;
	std::string ProcessorArchitecture;

	TCrashSystemInfo()
		: OSMajor(0)
		, OSMinor(0)
		, OSBuild(0)
		, OSRevision(0)
		, ProcessorCount(0)

	{
	}
};

//Thread info
struct TCrashThreadInfo
{
public:
	int ThreadID;
	int SuspendCount;

	TCrashThreadInfo() : ThreadID(0), SuspendCount(0)
	{

	}
};


//Exception info
struct TCrashExceptionInfo
{
public:
	int ProcessId;
	int ThreadId;
	int Code;
	std::string ExceptionString;

	std::vector<std::string> CallStackString;

	TCrashExceptionInfo()
		: ProcessId(0)
		, ThreadId(0)
		, Code(0)
	{
	}
};


//Crash info
struct TCrashInfo
{
public:
	std::vector<TCrashModuleInfo> module_info_list;
	TCrashSystemInfo sys_info;
	TCrashExceptionInfo excep_info;
	std::vector<std::string> crash_stack_info;
};
