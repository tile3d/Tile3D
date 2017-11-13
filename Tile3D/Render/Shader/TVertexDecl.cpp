#include <d3d9.h>
#include "TVertexDecl.h"
#include <Render/TEngine.h>
#include <Render/Windows/TD3D9Device.h>

bool TVertexDecl::Init(const D3DVERTEXELEMENT9* decl)
{
	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();

	if (pDevice->GetDirect3DDevice()->CreateVertexDeclaration(decl, &m_pVertDecl) != D3D_OK) {
		return false;
	}

	return true;
}

void TVertexDecl::Release()
{
	if (m_pVertDecl) {
		TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
		m_pVertDecl->Release();
		m_pVertDecl = nullptr;
	}
}

bool TVertexDecl::Appear()
{
	if (!m_pVertDecl) {
		return false;
	}
	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	return pDevice->GetDirect3DDevice()->SetVertexDeclaration(m_pVertDecl) == D3D_OK;
}

