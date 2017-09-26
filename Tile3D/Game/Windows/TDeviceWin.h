#pragma once

#include <Game/TDevice.h>
#include <d3d9.h>
#include "TApplicationWin.h"

class TDeviceWin : public TDevice
{
public:
	TDeviceWin(TApplicationWin * pAppWin) {
		m_d3dObject = nullptr;
		m_d3dDevice = nullptr;
		m_pAppWin = pAppWin;
	}

	virtual bool Init();

private:
	IDirect3D9Ex*           m_d3dObject;
	IDirect3DDevice9Ex*	  m_d3dDevice;
	TApplicationWin*	m_pAppWin;
};

