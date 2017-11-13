#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>
#include <d3d9.h>

class TVertexShader;
class TPixelShader;
class TShaderMan
{
public:
	static TShaderMan * GetInstance() {
		static TShaderMan shaderMan;
		return &shaderMan;
	}

	TVertexShader* LoadVertexShader(const char* fileName, D3DVERTEXELEMENT9* pDecl);
	TPixelShader* LoadPixelShader(const char* fileName);

	bool ReleaseVertexShader(int shaderID);
	bool ReleasePixelShader(int shaderID);

private:
	TVertexShader* FindVertexShaderByID(int shaderID);
	TPixelShader* FindPixelShaderByID(int shaderID);

private:
	TMutexLock m_lock;
	THashMap<int, TVertexShader*> m_vertexShaders;
	THashMap<int, TPixelShader*> m_pixelShaders;
};

