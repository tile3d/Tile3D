#include "TVertexShader.h"
#include <Common/TLog.h>
#include <File/TFileDir.h>
#include <Render/TEngine.h>
#include <Render/Windows/TD3D9Device.h>
#include <d3d9.h>
#include <d3dx9.h>

TVertexShader* TVertexShader::m_pCurShader = nullptr;

TVertexShader::TVertexShader()
{
	m_pD3DXShaderBuf = nullptr;
	m_pD3DShader = nullptr;
	m_pDecl = nullptr;
	m_shaderID = 0;
}

TVertexShader::~TVertexShader()
{

}

void TVertexShader::Release()
{
	if (m_pD3DXShaderBuf != nullptr) {
		m_pD3DXShaderBuf->Release();
	}

	if (m_pD3DShader != nullptr) {
		m_pD3DShader->Release();
	}
}


bool TVertexShader::Load(const char* fileName, D3DVERTEXELEMENT9* pDecl)
{
	HRESULT hr;
	DWORD flags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

	ID3DXBuffer*		pVertexShaderErr = nullptr;
	ID3DXConstantTable* pVertexShaderConstantTable = nullptr;

	hr = D3DXCompileShaderFromFile(fileName, nullptr, nullptr, "vs_main", "vs_2_0", flags, &m_pD3DXShaderBuf, &pVertexShaderErr, &pVertexShaderConstantTable);
	if (FAILED(hr)) {
		if (pVertexShaderErr != nullptr) {
			TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to compile the shader file:%s, error:%s", fileName, pVertexShaderErr->GetBufferPointer());
		}
		else {
			TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to compile the shader file:%s", fileName);
		}
		return false;
	}

	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	hr = pDevice->GetDirect3DDevice()->CreateVertexShader((DWORD*)m_pD3DXShaderBuf->GetBufferPointer(), &m_pD3DShader);
	if(hr != D3D_OK){
		TLog::Log(LOG_ERR, "Shader", "TVertexShader::Load,  fail to create the shader file:%s", fileName);
		return false;
	}

	m_pD3DXShaderBuf->Release();
	m_pD3DXShaderBuf = nullptr;

	m_pDecl = new TVertexDecl();
	if (!m_pDecl->Init(pDecl)) {
		delete m_pDecl;
		m_pDecl = nullptr;
		return false;	
	}

	return true;
}

//
//TBD: add curShader optimaztion
bool TVertexShader::Appear()
{
	if (m_pD3DShader == nullptr) {
		return false;
	}

	if (m_pCurShader == this) {
		return true;
	}

	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	if (!pDevice->GetDirect3DDevice()->SetVertexShader(m_pD3DShader)) {
		TLog::Log(LOG_ERR, "Shader", "TVertexShader::Appear,  fail to set the shader file:%s");
		return false;
	}

	m_pCurShader = this;
	return true;
}


bool TVertexShader::Disappear()
{
	if (m_pCurShader == this) {
		TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
		pDevice->GetDirect3DDevice()->SetVertexShader(nullptr);
		m_pCurShader = nullptr;
	}
	return true;
}
