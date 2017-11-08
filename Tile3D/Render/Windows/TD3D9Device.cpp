#include "TD3D9Device.h"
#include "TD3D9Stream.h"
#include <Common/TLog.h>
#include <Render/Material/TMaterial.h>

bool TD3D9Device::Init()
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

	m_d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_d3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);
	m_d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	/*
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);


	m_d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
	m_d3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_d3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);


	m_d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	*/
	return true;
}

void TD3D9Device::Close()
{

}

TStream* TD3D9Device::CreateStream()
{
	return new TD3D9Stream();
}

void TD3D9Device::Clear()
{
	m_d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
}

void TD3D9Device::BeginRender()
{
	m_d3dDevice->BeginScene();
}

void TD3D9Device::EndRender()
{
	m_d3dDevice->EndScene();
}

void TD3D9Device::Present()
{
	m_d3dDevice->Present(0, 0, 0, 0);
}

void TD3D9Device::SetTransform(TRANSFORMATION_STATE state, const TMatrix4 & mat)
{
	switch (state)
	{
	case TS_VIEW:
		m_d3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)mat.GetPointer()));
		break;

	case TS_WORLD:
		m_d3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)mat.GetPointer()));
		break;

	case TS_PROJECTION:
		m_d3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)mat.GetPointer()));
		break;
	};
}


void SetColor(D3DCOLORVALUE& color, TColor & tcolor)
{
	color.r = tcolor.m_r;
	color.g = tcolor.m_g;
	color.b = tcolor.m_b;
	color.a = tcolor.m_a;

}

void TD3D9Device::SetMaterial(TMaterial* pMaterial)
{

	D3DMATERIAL9 mat;
	SetColor(mat.Ambient, pMaterial->GetAmbient());
	SetColor(mat.Diffuse, pMaterial->GetDiffuse());
	SetColor(mat.Emissive, pMaterial->GetEmissive());
	SetColor(mat.Specular, pMaterial->GetSpecular());
	mat.Power = pMaterial->GetPower();

	HRESULT hresult = m_d3dDevice->SetMaterial((D3DMATERIAL9 *)this);
	if (hresult != S_OK) {
		TLog::Log(LOG_ERR, "Texture", "TTexture::Render,  failed to set the texture");
	}
}
