#include "TSystemInfoImpWin.h"
#include <Windows.h>

#pragma warning (disable: 4996)

/*****************************************************************************
Operating System             Version     PlatformID
Windows 10                  10.0         VER_PLATFORM_WIN32_NT
Windows Server 2016         10.0         VER_PLATFORM_WIN32_NT
Windows 10 Preview           6.4         VER_PLATFORM_WIN32_NT
Windows 8.1                  6.3         VER_PLATFORM_WIN32_NT
Windows Server 2012 R2       6.3         VER_PLATFORM_WIN32_NT
Windows 8                    6.2         VER_PLATFORM_WIN32_NT (=2)
Windows Server 2012          6.2         VER_PLATFORM_WIN32_NT
Windows 7                    6.1         VER_PLATFORM_WIN32_NT
Windows Server 2008 R2       6.1         VER_PLATFORM_WIN32_NT
Windows Server 2008          6.0         VER_PLATFORM_WIN32_NT
Windows Vista                6.0         VER_PLATFORM_WIN32_NT
Windows Server 2003 R2       5.2         VER_PLATFORM_WIN32_NT
Windows Server 2003          5.2         VER_PLATFORM_WIN32_NT
Windows XP 64-Bit Edition    5.2         VER_PLATFORM_WIN32_NT
Windows XP                   5.1         VER_PLATFORM_WIN32_NT
Windows 2000                 5.0         VER_PLATFORM_WIN32_NT
Windows NT 4.0               4.0         VER_PLATFORM_WIN32_NT
Windows NT 3.51              3.51 ?      VER_PLATFORM_WIN32_NT
Windows Millennium Edition   4.90        VER_PLATFORM_WIN32_WINDOWS (=1)
Windows 98                   4.10        VER_PLATFORM_WIN32_WINDOWS
Windows 95                   4.0         VER_PLATFORM_WIN32_WINDOWS
Windows 3.1                  3.1 ?       VER_PLATFORM_WIN32s (=0)
*****************************************************************************/

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);


BOOL GetVersionEx2(LPOSVERSIONINFOW lpVersionInformation)
{
	HMODULE hNtDll = GetModuleHandleW(L"NTDLL");
	typedef NTSTATUS(NTAPI*tRtlGetVersion)(PRTL_OSVERSIONINFOW povi); // RtlGetVersion prototype
	tRtlGetVersion pRtlGetVersion = NULL;
	if (hNtDll)
	{
		pRtlGetVersion = (tRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
	}
	if (pRtlGetVersion)
	{
		return pRtlGetVersion((PRTL_OSVERSIONINFOW)lpVersionInformation) >= 0;
	}
	return FALSE;
}

void TSystemInfoImpWin::ObtainOSInfo(TString & osName, bool & is64Bit)
{
	//detect os version is 32 or 64 bit
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		//fail
		if (fnIsWow64Process(GetCurrentProcess(), &bIsWow64) == 0)
		{
			is64Bit = false;
		}
		else {
			is64Bit = true;
		}
	}
	else {
		is64Bit = false;
	}

	//obtain the os info
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);
	if (bOsVersionInfoEx == false) return;

	PGNSI pGNSI;
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"GetNativeSystemInfo");

	if (NULL != pGNSI) {
		pGNSI(&si);
	}
	else {
		GetSystemInfo(&si);
	}

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
	{
		// after Windows 8.1 or later, GetVersionEx function cannot get correct version
		if (osvi.dwMajorVersion >= 6) {
			OSVERSIONINFOEXW ovi;
			ZeroMemory(&ovi, sizeof(OSVERSIONINFOEXW));
			if (!GetVersionEx2((LPOSVERSIONINFOW)&ovi)) {
				osName = "Windows Unknown";
				return;
			}
			osvi.dwMajorVersion = ovi.dwMajorVersion;
			osvi.dwMinorVersion = ovi.dwMinorVersion;
			osvi.dwBuildNumber = ovi.dwBuildNumber;
		}

		if (osvi.dwMajorVersion > 10) {
			osName = "Windows Unknown";
		}
		else if (osvi.dwMajorVersion == 10) {
			if (osvi.wProductType == VER_NT_WORKSTATION) {
				osName = "Windows 10";
			}
			else {
				osName = "Windows Server 2016";
			}
		}
		else if (osvi.dwMajorVersion == 6) {
			if (osvi.dwMinorVersion == 0) {
				if (osvi.wProductType == VER_NT_WORKSTATION) {
					osName = "Windows Vista";
				}
				else {
					osName = "Windows Server 2008";
				}
			}
			else if (osvi.dwMinorVersion == 1) {
				if (osvi.wProductType == VER_NT_WORKSTATION) {
					osName = "Windows 7";
				}
				else {
					osName = "Windows Server 2008";
				}
			}
			else if (osvi.dwMinorVersion == 2 || osvi.dwMinorVersion == 3) {
				if (osvi.wProductType == VER_NT_WORKSTATION) {
					osName = "Windows 8";
				}
				else {
					osName = "Windows Server 2012";
				}
			}
		}
		else if (osvi.dwMajorVersion == 5) {
			if (osvi.dwMinorVersion == 1) {
				osName = "Windows XP";
			}
			else if (osvi.dwMinorVersion == 0) {
				osName = "Windows 2000";
			}
		}
	}

	if (VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId && osvi.dwMajorVersion == 4) {
		if (osvi.dwMinorVersion == 0 || osvi.dwMinorVersion == 3) {
			osName = "Windows 95";
		}
		else if (osvi.dwMinorVersion == 10) {
			osName = "Windows 98";
		}
		else if (osvi.dwMinorVersion == 90) {
			osName = "Windows ME";
		}
	}

	if (osName.IsEmpty()) {
		osName = "Windows Unknown";
	}
}


