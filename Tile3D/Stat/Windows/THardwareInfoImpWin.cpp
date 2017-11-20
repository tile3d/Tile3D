#include "THardwareInfoImpWin.h"
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>


void THardwareInfoImpWin::ObtainCPUInfo(TString & cpuInfo)
{

}

int THardwareInfoImpWin::ObtainMemorySize()
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	if (GlobalMemoryStatusEx(&memStatus)) {
		int totalMem = (int)(memStatus.ullTotalPhys / (1024 * 1024));
		return totalMem;
	}
	return 0;
}

void THardwareInfoImpWin::ObtainVideoCardInfo(TString & cardName)
{
	IDirect3D9* m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr) return;

	D3DADAPTER_IDENTIFIER9 adapter;
	if (m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapter) != D3D_OK) {
		m_pD3D->Release();
		return;
	}

	cardName = adapter.Description;
	m_pD3D->Release();
	return;
}

