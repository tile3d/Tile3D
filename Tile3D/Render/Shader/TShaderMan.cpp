#include <File/TFileDir.h>
#include "TShaderMan.h"
#include "TVertexShader.h"
#include "TPixelShader.h"

TVertexShader* TShaderMan::LoadVertexShader(const char* fileName, D3DVERTEXELEMENT9* pDecl)
{
	int shaderID = TFileDir::GetInstance()->GetIDFromFileName(fileName);
	TVertexShader * pVertexShader = FindVertexShaderByID(shaderID);
	if (pVertexShader != nullptr) {
		return pVertexShader;
	}

	pVertexShader = new TVertexShader();
	if (!pVertexShader->Load(fileName, pDecl)) {
		delete pVertexShader;
		return nullptr;
	}
	pVertexShader->SetShaderID(shaderID);

	m_lock.Lock();
	m_vertexShaders.Put(shaderID, pVertexShader);
	m_lock.Unlock();
	return pVertexShader;
}


TPixelShader* TShaderMan::LoadPixelShader(const char* fileName)
{
	int shaderID = TFileDir::GetInstance()->GetIDFromFileName(fileName);
	TPixelShader * pPixelShader = FindPixelShaderByID(shaderID);
	if (pPixelShader != nullptr) {
		return pPixelShader;
	}

	pPixelShader = new TPixelShader();
	if (!pPixelShader->Load(fileName)) {
		delete pPixelShader;
		return nullptr;
	}
	pPixelShader->SetShaderID(shaderID);

	m_lock.Lock();
	m_pixelShaders.Put(shaderID, pPixelShader);
	m_lock.Unlock();
	return pPixelShader;
}

bool TShaderMan::ReleaseVertexShader(int shaderID)
{
	TVertexShader * pVertexShader = FindVertexShaderByID(shaderID);
	if (pVertexShader != nullptr) {
		pVertexShader->Release();
		m_lock.Lock();
		m_vertexShaders.Remove(shaderID);
		m_lock.Unlock();
	}
	return true;
}

bool TShaderMan::ReleasePixelShader(int shaderID)
{
	TPixelShader * pPixelShader = FindPixelShaderByID(shaderID);
	if (pPixelShader != nullptr) {
		pPixelShader->Release();
		m_lock.Lock();
		m_pixelShaders.Remove(shaderID);
		m_lock.Unlock();
	}
	return true;
}

TVertexShader* TShaderMan::FindVertexShaderByID(int shaderID)
{
	m_lock.Lock();
	TVertexShader ** pVertexShader = m_vertexShaders.Find(shaderID);
	m_lock.Unlock();
	if (pVertexShader == nullptr) {
		return nullptr;
	}
	return (*pVertexShader);
}


TPixelShader* TShaderMan::FindPixelShaderByID(int shaderID)
{
	m_lock.Lock();
	TPixelShader ** pPixelShader = m_pixelShaders.Find(shaderID);
	m_lock.Unlock();
	if (pPixelShader == nullptr) {
		return nullptr;
	}
	return (*pPixelShader);
}

