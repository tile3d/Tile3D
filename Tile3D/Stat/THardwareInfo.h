#pragma once

#include <Container/TString.h>

class THardwareInfoImp;
class THardwareInfo
{
public:
	static THardwareInfo* GetInstance() {
		static THardwareInfo hardwareInfo;
		return &hardwareInfo;
	}

	THardwareInfo();
	~THardwareInfo();

	TString& GetCPUInfo() { return m_cpuInfo; }

	int GetMemorySize() { return m_memorySize; }

	TString& GetVideoCardName() { return m_videoCardName; }

private:
	void RetrieveHardwareInfo();

private:	
	TString m_cpuInfo;
	int m_memorySize;		//MB
	TString m_videoCardName;
	
	THardwareInfoImp * pHardwareInfoImp;
};

