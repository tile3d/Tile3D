#pragma once

#include <Container/THashMap.h>
#include <Container/TString.h>
#include <Core/Lock/TMutexLock.h>

class TTexture;
class TTextureMan
{
public:
	static TTextureMan* GetInstance() {
		static TTextureMan textureMan;
		return &textureMan;
	}

	TTexture* LoadTexture(TString & textureName);

private:
	TTexture* FindTexture(int textureID);

private:
	THashMap<int, TTexture*> m_textures;
	TMutexLock m_lock;

};