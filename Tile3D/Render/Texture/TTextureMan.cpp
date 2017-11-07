#include "TTextureMan.h"
#include <Render/Texture/TTexture.h>
#include <Common/TLog.h>
#include <File/TFileDir.h>

TTexture* TTextureMan::LoadTexture(TString & textureName)
{
	int textureID = TFileDir::GetInstance()->GetIDFromFileName(textureName);
	TTexture* pTexture = FindTexture(textureID);
	if (pTexture != nullptr) {
		return pTexture;
	}

	pTexture = new TTexture(textureID);
	if (!pTexture->Load(textureName)) {
		TLog::Log(LOG_ERR, "Texture", "TTextureMan::LoadTexture,  failed to load the texture: [%s].", textureName);
		delete pTexture;
		return nullptr;
	}
	m_textures.Put(textureID, pTexture);
	return pTexture;
}


TTexture* TTextureMan::FindTexture(int textureID)
{
	m_lock.Lock();
	TTexture ** pTexture = m_textures.Find(textureID);
	m_lock.Unlock();
	if (pTexture == nullptr) {
		return nullptr;
	}
	return (*pTexture);
}

