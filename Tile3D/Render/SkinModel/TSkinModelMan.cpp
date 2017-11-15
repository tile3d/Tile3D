#include "TSkinModelMan.h"
#include "TSkinModel.h"
#include <Common/TLog.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Render/Shader/TShaderMan.h>


TSkinModelMan::TSkinModelMan()
{
	pSkinMeshVertexShader = nullptr;
	pSkinMeshPixelShader = nullptr;
}


TSkinModelMan::~TSkinModelMan()
{
	if (pSkinMeshVertexShader) {
		pSkinMeshVertexShader->Release();
		delete pSkinMeshVertexShader;
		pSkinMeshVertexShader = nullptr;
	}

	if (pSkinMeshPixelShader) {
		pSkinMeshPixelShader->Release();
		delete pSkinMeshPixelShader;
		pSkinMeshPixelShader = nullptr;
	}
}


bool TSkinModelMan::LoadSkinMeshShader()
{
	D3DVERTEXELEMENT9 skinMeshDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
		{ 0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		D3DDECL_END()
	};

	pSkinMeshVertexShader = TShaderMan::GetInstance()->LoadVertexShader("D:\\engine\\Tile3D\\Shaders\\HLSL\\SkinModel\\skinmodel_vs.hlsl", skinMeshDecl);
	if (pSkinMeshVertexShader == nullptr) return false;


	pSkinMeshPixelShader = TShaderMan::GetInstance()->LoadPixelShader("D:\\engine\\Tile3D\\Shaders\\HLSL\\SkinModel\\skinmodel_ps.hlsl");
	if (pSkinMeshPixelShader == nullptr) return false;

	return true;
}


TSkinModel* TSkinModelMan::LoadSkinModel(const char * SkinModelFile, int skinFlag)
{
	TSkinModel * pSkinModel = FindSkinModelByFile(SkinModelFile);
	if (pSkinModel != nullptr) {
		return pSkinModel->Clone();
	}

	TFileImage fi;
	if (!fi.Open("", SkinModelFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModelMan::LoadSkinModel,  failed to load the SkinModel: [%s].", SkinModelFile);
		return nullptr;
	}
	TSkinModel* pNewSkinModel = CreateSkinModel(&fi, skinFlag);
	fi.Close();
	return pNewSkinModel;
}

TSkinModel* TSkinModelMan::LoadSkinModel(TFile * pFile, int skinFlag)
{
	TSkinModel * pSkinModel = FindSkinModelByFile(pFile->GetRelativeFileName());
	if (pSkinModel != nullptr) {
		return pSkinModel->Clone();
	}
	return CreateSkinModel(pFile, skinFlag);
}

bool TSkinModelMan::ReleaseSkinModel(int skinModelID)
{
	TSkinModel * pSkinModel = FindSkinModelByID(skinModelID);
	if (pSkinModel == nullptr) return false;
	pSkinModel->Release();
	return true;
}

bool TSkinModelMan::ReleaseSkinModel(TSkinModel * pSkinModel)
{
	return ReleaseSkinModel(pSkinModel->GetSkinModelID());
}

TSkinModel * TSkinModelMan::FindSkinModelByFile(const char * SkinModelFile)
{
	char relFile[MAX_PATH];
	relFile[0] = '\0';

	TFileDir::GetInstance()->GetRelativePath(SkinModelFile, relFile);
	int SkinModelFileID = TFileDir::GetInstance()->GetIDFromFileName(relFile);
	return FindSkinModelByID(SkinModelFileID);
}

TSkinModel* TSkinModelMan::FindSkinModelByID(int SkinModelFileID)
{
	m_lock.Lock();
	TSkinModel ** pSkinModel = m_skinModels.Find(SkinModelFileID);
	m_lock.Unlock();
	if (pSkinModel == nullptr) {
		return nullptr;
	}
	return (*pSkinModel);
}

TSkinModel * TSkinModelMan::CreateSkinModel(TFile * pFile, int skinFlag)
{
	TSkinModel * pSkinModel = new TSkinModel();

	if (!pSkinModel->Load(pFile, skinFlag)) {
		delete pSkinModel;
		TLog::Log(LOG_ERR, "SkinModelModel", "TSkinModelMan::CreateSkinModel,  Failed to create the SkinModel: [%s].", pFile->GetRelativeFileName());
		return nullptr;
	}

	m_lock.Lock();
	m_skinModels.Put(pSkinModel->GetSkinModelID(), pSkinModel);
	m_lock.Unlock();
	return pSkinModel;
}
