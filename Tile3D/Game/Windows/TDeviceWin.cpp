#include "TDeviceWin.h"
#include <Util/TLog.h>

bool TDeviceWin::Init()
{
	if (Direct3DCreate9Ex(D3D_SDK_VERSION, &m_d3dObject) != S_OK)
	{
		TLog::Log(LOG_ERR, "Application", "TDeviceWin::Init,  Failed to create the d3d driver");
		MessageBox(0, "Direct3DCreate9 FAILED", 0, 0);
		PostQuitMessage(0);
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.BackBufferWidth = m_pAppWin->GetWidth();
	d3dpp.BackBufferHeight = m_pAppWin->GetHeight();
	d3dpp.Windowed = !m_pAppWin->IsFullScreen();
	d3dpp.hDeviceWindow = m_pAppWin->GetWnd();
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	if (!m_pAppWin->IsFullScreen()) {
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else {
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	DWORD flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	if (m_d3dObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_pAppWin->GetWnd(), flags, &d3dpp, nullptr, &m_d3dDevice) != S_OK) {
		TLog::Log(LOG_ERR, "Application", "TDeviceWin::Init,  Failed to create the device");
		return false;
	}

	return true;
}

