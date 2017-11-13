#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "TVertexDecl.h"
#include <Container/TString.h>


class TVertexShader
{
public:
	TVertexShader();
	~TVertexShader();

	void Release();

	bool Load(const char* fileName, D3DVERTEXELEMENT9* pDecl);

	bool Appear();
	bool Disappear();

	TString& GetFileName() { return m_fileName; }
	void SetShaderID(int shaderID) {
		m_shaderID = shaderID;
	}

private:
	static TVertexShader* m_pCurShader;
	TString m_fileName;
	ID3DXBuffer*  m_pD3DXShaderBuf;
	IDirect3DVertexShader9* m_pD3DShader;
	TVertexDecl* m_pDecl;
	int m_shaderID;
};

