#pragma once

#include <Core/TObject.h>
#include <Core/Lock/TMutexLock.h>
#include <Container/THashMap.h>
#include <Render/Shader/TVertexShader.h>
#include <Render/Shader/TPixelShader.h>

class TFile;
class TSkinModel;

class TSkinModelMan : public TObject
{
public:
	static TSkinModelMan * GetInstance() {
		static TSkinModelMan skinModelMan;
		return &skinModelMan;
	}

	TSkinModelMan();
	~TSkinModelMan();

	bool LoadSkinMeshShader();

	TSkinModel * LoadSkinModel(const char * pFile, int skinFlag);
	TSkinModel * LoadSkinModel(TFile * pFile, int skinFlag);
	bool ReleaseSkinModel(int skinModelID);
	bool ReleaseSkinModel(TSkinModel * pSkinModel);

	TVertexShader* GetSkinMeshVertexShader() { return pSkinMeshVertexShader; }
	TPixelShader* GetSkinMeshPixelShader() { return pSkinMeshPixelShader;}

private:
	TSkinModel* FindSkinModelByFile(const char * skinFile);
	TSkinModel* FindSkinModelByID(int skinModelID);
	TSkinModel * CreateSkinModel(TFile * pFile, int skinFlag);

private:
	TVertexShader * pSkinMeshVertexShader;
	TPixelShader * pSkinMeshPixelShader;
	THashMap<int, TSkinModel*> m_skinModels;
	TMutexLock m_lock;
};

