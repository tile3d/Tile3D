#include "TPixelShader.h"
#include <Common/TLog.h>
#include <File/TFileDir.h>
#include <Render/TEngine.h>
#include <Render/Windows/TD3D9Device.h>
#include <d3d9.h>
#include <d3dx9.h>

TPixelShader* TPixelShader::m_pCurShader = nullptr;


TPixelShader::TPixelShader()
{
	m_pD3DXShaderBuf = nullptr;
	m_pD3DShader = nullptr;
	m_shaderID = 0;
}

TPixelShader::~TPixelShader()
{

}

void TPixelShader::Release()
{
	if (m_pD3DXShaderBuf != nullptr) {
		m_pD3DXShaderBuf->Release();
	}

	if (m_pD3DShader != nullptr) {
		m_pD3DShader->Release();
	}
}


bool TPixelShader::Load(const char* fileName)
{
	HRESULT hr;
	DWORD flags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

	ID3DXBuffer*		pPixelShaderErr = nullptr;
	ID3DXConstantTable* pPixelShaderConstantTable = nullptr;

	hr = D3DXCompileShaderFromFile(fileName, nullptr, nullptr, "ps_main", "ps_2_0", flags, &m_pD3DXShaderBuf, &pPixelShaderErr, &pPixelShaderConstantTable);
	if (FAILED(hr)) {
		if (pPixelShaderErr != nullptr) {
			TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to compile the shader file:%s, error:%s", fileName, pPixelShaderErr->GetBufferPointer());
		}
		else {
			TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to compile the shader file:%s", fileName);
		}
		return false;
	}

	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	hr = pDevice->GetDirect3DDevice()->CreatePixelShader((DWORD*)m_pD3DXShaderBuf->GetBufferPointer(), &m_pD3DShader);
	if (hr != D3D_OK) {
		TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to create the shader file:%s", fileName);
		return false;
	}

	m_pD3DXShaderBuf->Release();
	m_pD3DXShaderBuf = nullptr;

	return true;
}

//
//TBD: add curShader optimaztion
bool TPixelShader::Appear()
{
	if (m_pD3DShader == nullptr) {
		return false;
	}

	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	if (!pDevice->GetDirect3DDevice()->SetPixelShader(m_pD3DShader)) {
		TLog::Log(LOG_ERR, "Shader", "TVertexShader::Appear,  fail to set the shader file:%s");
		return false;
	}

	return true;
}


bool TPixelShader::Disappear()
{
	if (m_pCurShader == this) {
		TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
		pDevice->GetDirect3DDevice()->SetPixelShader(nullptr);
		m_pCurShader = nullptr;
	}
	return true;
}
