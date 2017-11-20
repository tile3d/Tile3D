#include <Stat/THardwareInfo.h>
#include <Stat/TSystemInfo.h>
#include <Stat/TNetInfo.h>
#include <stdio.h>

int main()
{
	int memorySize = THardwareInfo::GetInstance()->GetMemorySize();
	TString videoCardName = THardwareInfo::GetInstance()->GetVideoCardName();

	TString osName = TSystemInfo::GetInstance()->GetOSName();
	bool is64Bit = TSystemInfo::GetInstance()->Is64Bit();

	TString macAddr = TNetInfo::GetInstance()->GetMacAddr();
	TString ipAddr = TNetInfo::GetInstance()->GetIPAddr();


	printf("OS name:%s\n", osName.ToString());
	printf("64bit:%d\n", is64Bit);
	printf("video card name:%s\n", videoCardName.ToString());
	printf("memory size:%d\n", memorySize);
	printf("mac addr:%s\n", macAddr.ToString());
	printf("ip addr:%s\n", ipAddr.ToString());

	return 0;
}