#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3d9types.h>
#include <d3dx9math.h>
#include "TTexture.h"
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Common/TLog.h>
#include <Render/TEngine.h>
#include <Render/TDevice.h>
#include <Render/Windows/TD3D9Device.h>


TTexture::~TTexture()
{
	if (m_pDXTexture) {

	}
}

void TTexture::Render()
{
	if (m_pDXTexture) {
		TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
		HRESULT hresult = pDevice->GetDirect3DDevice()->SetTexture(0, m_pDXTexture);
		if (hresult != S_OK) {
			TLog::Log(LOG_ERR, "Texture", "TTexture::Render,  failed to set the texture");
		}
	}
}

bool TTexture::Load(TString & textureName)
{
	if (TFileDir::GetInstance()->CheckFileExt(textureName, ".dds", 4, -1) == true) {
		m_ddsTexture = true;
	}
	else if (TFileDir::GetInstance()->CheckFileExt(textureName, ".tga", 4, -1) == true) {
		m_alphaTexture = true;
	}

	TFileImage fileImage;
	if (!fileImage.Open(textureName, TFile::TFILE_OPENEXIST| TFile::TFILE_TEMPMEMORY)) {
		TLog::Log(LOG_ERR, "Texture", "TTexture::Load,  failed to open the texture file: [%s].", textureName);
		fileImage.Close();
		return false;
	}

	if (!LoadTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), textureName)) {
		TLog::Log(LOG_ERR, "Texture", "TTexture::Load,  failed to load the texture: [%s].", textureName);
		return false;
	}

	fileImage.Close();
	return true;
}

bool TTexture::LoadTexture(unsigned char* pBuf, int len, const char* relativePath)
{
	D3DXIMAGE_INFO imageInfo;
	int mipLevel = 0;
	HRESULT hval;
	D3DPOOL pool = D3DPOOL_DEFAULT;
	int m_usage = D3DUSAGE_DYNAMIC;

	hval = D3DXGetImageInfoFromFileInMemory(pBuf, len, &imageInfo);
	if (hval != D3D_OK) {
		TLog::Log(LOG_ERR, "Texture", "TTexture::LoadTexture,  failed to get the image info");
		return true;
	}

	if (m_ddsTexture) {
		if (imageInfo.Format == D3DFMT_A8B8G8R8 || imageInfo.Format == D3DFMT_A4R4G4B4 || imageInfo.Format == D3DFMT_DXT3 || imageInfo.Format == D3DFMT_DXT5) {
			m_alphaTexture = true;
		}
		mipLevel = imageInfo.MipLevels;
	}

	int width = imageInfo.Width < 1 ? 1 : imageInfo.Width;
	int height = imageInfo.Height < 1 ? 1 : imageInfo.Height;

	TDevice * pDevice = TEngine::GetInstance()->GetDevice();
	hval = D3DXCreateTextureFromFileInMemoryEx(((TD3D9Device*)pDevice)->GetDirect3DDevice(), pBuf, len, width, height, mipLevel, m_usage, D3DFMT_UNKNOWN,
		pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imageInfo, nullptr, &m_pDXTexture);
	if (D3D_OK != hval) {
		m_pDXTexture = nullptr;
		TLog::Log(LOG_ERR, "Texture", "TTexture::LoadTexture,  failed to create the texture");
		return true;
	}

	return true;
}

