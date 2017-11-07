#pragma once

#include <Container/TString.h>

struct IDirect3DTexture9;
class TTexture
{
public:
	TTexture(int textureID) {
		m_textureID = textureID;
		m_ddsTexture = false;
		m_alphaTexture = false;
		m_pDXTexture = nullptr;
	}

	~TTexture();

	bool Load(TString & textureName);

	void Render(int stage);

	IDirect3DTexture9* GetD3DTexture() {
		return m_pDXTexture;
	}

private:
	bool LoadTexture(unsigned char* pBuf, int len, const char* relativePath);

private:
	bool m_ddsTexture;
	bool m_alphaTexture;
	int m_textureID;
	int m_usage;
	TString m_textureName;
	IDirect3DTexture9 * m_pDXTexture;
};

