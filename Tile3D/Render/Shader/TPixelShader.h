#pragma once

#include <Container/TString.h>

struct ID3DXBuffer;
struct IDirect3DPixelShader9;
class TPixelShader
{
public:
	TPixelShader();
	~TPixelShader();

	void Release();

	bool Load(const char* fileName);

	bool Appear();
	bool Disappear();

	TString& GetFileName() { return m_fileName; }

private:
	static TPixelShader* m_pCurShader;
	TString m_fileName;
	ID3DXBuffer*  m_pD3DXShaderBuf;
	IDirect3DPixelShader9* m_pD3DShader;
	int m_shaderID;
};

