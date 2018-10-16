#include "TMinidumpInfoImpWin.h"
#include <Windows.h>
#include <DbgHelp.h>
#include <dbgeng.h>

bool TMinidumpInfoImpWin::AnalyzeDumpFile(TCrashInfo & crash_info, const char* minidump_file, const char* minidump_symbol_path)
{
	HRESULT hr = ::CoInitialize(NULL);
	if (hr != S_OK) {
		printf("Coinitialize fail to init");
		return false;
	}

	if (DebugCreate(__uuidof(IDebugClient5), (void**)&Client) != S_OK) {
		printf("fail to create the Client");
		return false;
	}

	if (Client->QueryInterface(__uuidof(IDebugControl4), (void**)&Control) != S_OK) {
		printf("fail to query the Control\n");
		return false;
	}

	if (Client->QueryInterface(__uuidof(IDebugSymbols3), (void**)&Symbol) != S_OK) {
		printf("fail to query the symbol\n");
		return false;
	}

	if (Client->QueryInterface(__uuidof(IDebugAdvanced3), (void**)&Advanced) != S_OK) {
		printf("fail to query the advanced\n");
		return false;
	}

	hr = Client->OpenDumpFile(minidump_file);
	if (FAILED(hr))
	{
		printf("Failed to open minidump file: %s", minidump_file);
		return false;
	}

	if (Control->WaitForEvent(0, INFINITE) != S_OK)
	{
		printf("Failed while waiting for minidump to load: %s", minidump_file);
		return false;
	}

	//Init the symbol option and path
	InitSymbol(minidump_symbol_path);

	//Load the moudle symbol
	LoadModule(crash_info);

	//Get System info
	GetSystemInfo(crash_info);

	//Get Thread info
	GetThreadInfo();

	//Get Exception info
	GetExceptionInfo(crash_info);

	//Get the Callstack info
	GetCallstackInfo(crash_info);

	//release
	Advanced->Release();
	Symbol->Release();
	Control->Release();

	Client->Release();

	::CoUninitialize();
	return true;
}


void TMinidumpInfoImpWin::InitSymbol(const char* symbol_path)
{
	ULONG SymOpts = 0;

	// Load line information
	SymOpts |= SYMOPT_LOAD_LINES;
	SymOpts |= SYMOPT_OMAP_FIND_NEAREST;
	// Fail if a critical error is encountered
	SymOpts |= SYMOPT_FAIL_CRITICAL_ERRORS;
	// Always load immediately; no deferred loading
	SymOpts |= SYMOPT_DEFERRED_LOADS;
	// Require an exact symbol match
	SymOpts |= SYMOPT_EXACT_SYMBOLS;
	// This option allows for undecorated names to be handled by the symbol engine.
	SymOpts |= SYMOPT_UNDNAME;

	Symbol->SetSymbolOptions(SymOpts);
	Symbol->SetImagePath(symbol_path);
	Symbol->SetSymbolPath(symbol_path);
}

void TMinidumpInfoImpWin::LoadModule(TCrashInfo & m_crash_info)
{
	// The the number of loaded modules
	ULONG LoadedModuleCount = 0;
	ULONG UnloadedModuleCount = 0;

	Symbol->GetNumberModules(&LoadedModuleCount, &UnloadedModuleCount);

	// Find the relative names of all the modules so we know which files to sync
	int ExecutableIndex = -1;
	for (ULONG ModuleIndex = 0; ModuleIndex < LoadedModuleCount; ModuleIndex++)
	{
		TCrashModuleInfo module_info;

		ULONG64 ModuleBase = 0;
		Symbol->GetModuleByIndex(ModuleIndex, &ModuleBase);

		// Get the full path of the module name
		char ModuleName[MAX_PATH] = { 0 };
		Symbol->GetModuleNameString(DEBUG_MODNAME_IMAGE, ModuleIndex, ModuleBase, ModuleName, MAX_PATH, NULL);
		module_info.Name = ModuleName;
		module_info.BaseOfImage = ModuleBase;

		VS_FIXEDFILEINFO VersionInfo = { 0 };
		Symbol->GetModuleVersionInformation(ModuleIndex, ModuleBase, TEXT("\\"), &VersionInfo, sizeof(VS_FIXEDFILEINFO), NULL);
		module_info.Major = HIWORD(VersionInfo.dwProductVersionMS);
		module_info.Minor = LOWORD(VersionInfo.dwProductVersionMS);
		module_info.Patch = HIWORD(VersionInfo.dwProductVersionLS);
		module_info.Revision = LOWORD(VersionInfo.dwProductVersionLS);

		DEBUG_MODULE_PARAMETERS ModuleParameters = { 0 };
		Symbol->GetModuleParameters(1, NULL, ModuleIndex, &ModuleParameters);
		module_info.SizeOfImage = ModuleParameters.Size;

		Symbol->Reload(ModuleName);
		m_crash_info.module_info_list.push_back(module_info);

		HRESULT hr = Symbol->Reload(ModuleName);

		/*
		if (FAILED(hr)) {
		printf("Fail to load the module: %s\n", ModuleName);
		}
		else {
		printf("success to load the module: %s\n", ModuleName);
		}
		*/
	}
}

void TMinidumpInfoImpWin::GetSystemInfo(TCrashInfo & crash_info)
{
	TCrashSystemInfo & info = crash_info.sys_info;
	ULONG PlatformId = 0;
	ULONG Major = 0;
	ULONG Minor = 0;
	ULONG Build = 0;
	ULONG Revision = 0;
	Control->GetSystemVersionValues(&PlatformId, &Major, &Minor, &Build, &Revision);

	info.OSMajor = Major;
	info.OSMinor = Minor;
	info.OSBuild = Build;
	info.OSRevision = Revision;

	ULONG ProcessorType = 0;
	Control->GetActualProcessorType(&ProcessorType);

	switch (ProcessorType)
	{
	case IMAGE_FILE_MACHINE_I386:
		// x86
		info.ProcessorArchitecture = "PA_X86";
		break;

	case IMAGE_FILE_MACHINE_ARM:
		// ARM
		info.ProcessorArchitecture = "PA_ARM";
		break;

	case IMAGE_FILE_MACHINE_AMD64:
		// x64
		info.ProcessorArchitecture = "PA_X64";
		break;

	default:
		break;
	};

	ULONG ProcessorCount = 0;
	Control->GetNumberProcessors(&ProcessorCount);
	info.ProcessorCount = ProcessorCount;
}

void TMinidumpInfoImpWin::GetThreadInfo()
{

}


void TMinidumpInfoImpWin::GetExceptionInfo(TCrashInfo & crash_info)
{
	TCrashExceptionInfo & info = crash_info.excep_info;
	ULONG ExceptionType = 0;
	ULONG ProcessID = 0;
	ULONG ThreadId = 0;
	char Description[MAX_PATH] = { 0 };
	Control->GetLastEventInformation(&ExceptionType, &ProcessID, &ThreadId, NULL, 0, NULL, Description, MAX_PATH, NULL);

	info.Code = ExceptionType;
	info.ProcessId = ProcessID;
	info.ThreadId = ThreadId;
	info.ExceptionString = Description;
}


bool IsOffsetWithinModules(ULONG64 Offset, TCrashInfo & crash_info)
{
	for (size_t module_index = 0; module_index < crash_info.module_info_list.size(); module_index++)
	{
		TCrashModuleInfo& module_info = crash_info.module_info_list[module_index];
		if (Offset >= module_info.BaseOfImage && Offset < module_info.BaseOfImage + module_info.SizeOfImage)
		{
			return true;
		}
	}

	return false;
}

int TMinidumpInfoImpWin::GetCallstackInfo(TCrashInfo & crash_info)
{
	int NumValidFunctionNames = 0;

	//const float int int32 FString
	const int ContextSize = 4096;
	byte* Context = new byte[ContextSize];
	ULONG DebugEvent = 0;
	ULONG ProcessID = 0;
	ULONG ThreadID = 0;
	ULONG ContextUsed = 0;

	// Get the context of the crashed thread
	HRESULT hr = Control->GetStoredEventInformation(&DebugEvent, &ProcessID, &ThreadID, Context, ContextSize, &ContextUsed, NULL, 0, 0);
	if (FAILED(hr))
	{
		return NumValidFunctionNames;
	}

	// Some magic number checks
	if (ContextUsed == 716)
	{
		printf("Context size matches x86 sizeof( CONTEXT )\n");
	}
	else if (ContextUsed == 1232)
	{
		printf("Context size matches x64 sizeof( CONTEXT )\n");
	}

	// Get the entire stack trace
	const int MaxFrames = 8192;
	const int MaxFramesSize = MaxFrames * ContextUsed;

	DEBUG_STACK_FRAME* StackFrames = new DEBUG_STACK_FRAME[MaxFrames];
	ULONG Count = 0;
	bool bFoundSourceFile = false;
	byte* ContextData = new byte[MaxFramesSize];
	memset(ContextData, 0, MaxFramesSize);

	HRESULT HR = Control->GetContextStackTrace(Context, ContextUsed, StackFrames, MaxFrames, ContextData, MaxFramesSize, ContextUsed, &Count);
	printf("GetContextStackTrace() got %d frames\n", Count);


	int AssertOrEnsureIndex = -1;
	for (ULONG StackIndex = 0; StackIndex < Count; StackIndex++)
	{
		const ULONG64 Offset = StackFrames[StackIndex].InstructionOffset;

		if (IsOffsetWithinModules(Offset, crash_info))
		{
			// Get the module, function, and offset
			ULONG64 Displacement = 0;
			char NameByOffset[MAX_PATH] = { 0 };
			Symbol->GetNameByOffset(Offset, NameByOffset, ARRAYSIZE(NameByOffset) - 1, NULL, &Displacement);
			std::string ModuleAndFunction = NameByOffset;

			if (ModuleAndFunction.find('!') != std::string::npos)
			{
				NumValidFunctionNames++;
			}

			// Look for source file name and line number
			char SourceName[MAX_PATH] = { 0 };
			ULONG LineNumber = 0;
			Symbol->GetLineByOffset(Offset, &LineNumber, SourceName, ARRAYSIZE(SourceName) - 1, NULL, NULL);

			char crash_string[2048];
			memset(crash_string, 0, sizeof(crash_string));
			if (strlen(SourceName) > 0) {
				sprintf_s(crash_string, "%s [%s:%d]", NameByOffset, SourceName, LineNumber);
			}
			else {
				strcpy_s(crash_string, NameByOffset);
			}
			crash_info.crash_stack_info.push_back(crash_string);
		}
	}

	return NumValidFunctionNames;
}