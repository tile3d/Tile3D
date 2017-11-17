#include "TSkinModelMan.h"
#include "TSkinModel.h"
#include <Common/TLog.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Render/Shader/TShaderMan.h>
#include <Render/TEngine.h>
#include <Render/TCamera.h>
#include <Render/Windows/TD3D9Device.h>
#include <d3d9.h>
#include <d3dx9.h>


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

void TSkinModelMan::Render()
{
	m_lock.Lock();

	pSkinMeshVertexShader->Appear();
	pSkinMeshPixelShader->Appear();

	//set the project and view matrix
	TCamera* pCamera = TEngine::GetInstance()->GetCamera();
	TMatrix4 matProject = pCamera->GetProjectionMatrix();
	TMatrix4 matView = pCamera->GetViewMatrix();
	matView.Inverse();

	TDevice * pDevice = TEngine::GetInstance()->GetDevice();

	pDevice->SetVertexShaderConstantF(0, matProject.GetPointer(), 4);
	pDevice->SetVertexShaderConstantF(4, matView.GetPointer(), 4);
	
	//Set the dir light register
	TVector3 dirLight(1.0, 1.0, 1.0);
	dirLight.Normalize();
	dirLight = dirLight*pCamera->GetViewMatrix();
	float dirLight2[4] = { dirLight.m_x, dirLight.m_y, dirLight.m_z, 0 };
	pDevice->SetVertexShaderConstantF(8, dirLight2, 1);

	float   g_fExtend[4] = { 0, 0, 0, 0 };
	pDevice->SetVertexShaderConstantF(9, g_fExtend, 1);

	THashNode<int, TSkinModel*>* pNode = m_skinModels.GetHead();
	while (pNode != nullptr) {
		TSkinModel* pSkinModel = pNode->m_value;
		pSkinModel->Render();
		pNode = m_skinModels.GetNext(pNode);
	}

	pSkinMeshVertexShader->Disappear();
	pSkinMeshPixelShader->Disappear();
	
	m_lock.Unlock();
}

void TSkinModelMan::ReleaseSkinModel()
{
	m_lock.Lock();
	THashNode<int, TSkinModel*>* pNode = m_skinModels.GetHead();
	while (pNode != nullptr) {
		TSkinModel* pSkinModel = pNode->m_value;
		pSkinModel->Release();
		pNode = m_skinModels.GetNext(pNode);
	}
	m_skinModels.Clear();
	m_lock.Unlock();
}