#pragma once

#include <Render/TDevice.h>
#include <Render/TStream.h>
#include <Util/TTypes.h>
#include "TApplicationWin.h"
#include <d3d9.h>

class TD3D9Stream;
class TD3D9Device : public TDevice
{
public:
	TD3D9Device(TApplicationWin * pAppWin) {
		m_d3dObject = nullptr;
		m_d3dDevice = nullptr;
		m_pAppWin = pAppWin;
	}

	virtual TStream* CreateStream();

	virtual bool Init();
	virtual void Close();

	virtual void Clear();
	virtual void BeginRender();
	virtual void EndRender();
	virtual void Present();
	
	virtual void SetTransform(TRANSFORMATION_STATE state, const TMatrix4 & mat);

	IDirect3DDevice9Ex* GetDirect3DDevice() { return m_d3dDevice; }

private:
	IDirect3D9Ex*           m_d3dObject;
	IDirect3DDevice9Ex*	  m_d3dDevice;
	TApplicationWin*	m_pAppWin;
};

