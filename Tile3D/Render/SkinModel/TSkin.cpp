#include <Common/TLog.h>
#include <File/TFile.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Render/Material/TMaterial.h>
#include <Render/Texture/TTextureMan.h>
#include <Render/Texture/TTexture.h>
#include <Render/TEngine.h>
#include <Render/TDevice.h>
#include <Render/TCamera.h>
#include "TSkin.h"
#include "TSkinMesh.h"
#include "TSkeleton.h"
#include "TSkinModel.h"

TSkin::TSkin()
{
	m_version = 0;
	m_skinID = 0;

	m_minWeight = 0;
	m_skeBoneNum = 0;
	m_skinBoneNum = 0;
}

TSkin::~TSkin()
{
	for (int i = 0; m_textures.Size(); i++) {
		delete m_textures[i];
	}

	for (int i = 0; i < m_skinMeshs.Size(); i++) {
		delete m_skinMeshs[i];
	}

	for (int i = 0; i < m_materials.Size(); i++) {
		delete m_materials[i];
	}
}

bool TSkin::Load(const char * skinFile)
{
	TFileImage fi;
	if (!fi.Open("", skinFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load,  Failed to load the skin: [%s].", skinFile);
		return false;
	}
	if (!Load(&fi)) {
		return false;
	}
	fi.Close();
	return true;
}

bool TSkin::Load(TFile* pFile)
{
	TSkinHeader header;
	int count = 0;

	if (!pFile->Read(&header, sizeof(header), &count) || count != sizeof(header)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load,  Failed to read the skin header: [%s].", pFile->GetRelativeFileName());
		return false;
	}

	if (header.m_flags != TSKIN_IDENTIFY || header.m_version > TSKIN_VERSION) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load, incorrect skin flags or version: [%s].", pFile->GetRelativeFileName());
		return false;
	}

	m_version = header.m_version;
	m_skinFileName = pFile->GetRelativeFileName();
	m_skinID = TFileDir::GetInstance()->GetIDFromFileName(m_skinFileName);
	m_minWeight = header.m_minWeight;
	m_skeBoneNum = header.m_skeBoneNum;
	m_skinBoneNum = header.m_skinBoneNum;


	//load the bone names
	if (header.m_version >= 9 && header.m_skinBoneNum > 0) {
		m_boneNames.Reserve(header.m_skinBoneNum);
		for (int i = 0; i < header.m_skinBoneNum; i++) {
			TString boneName;
			pFile->ReadString(boneName);
			m_boneNames.Add(boneName);
		}
	}

	//load the textures
	if (header.m_textureNum > 0) {

		TString relativePath, fileTitle;
		TFileDir::GetInstance()->GetFilePath(m_skinFileName, relativePath);
		TFileDir::GetInstance()->GetFileTitle(m_skinFileName, fileTitle);
		int index = fileTitle.ReverseFind('.');
		if (index >= 0) {
			fileTitle.CutRight(fileTitle.GetLength() - index);
		}

		TString textureDir1 = relativePath + "\\Textures\\";
		TString textureDir2 = relativePath + "\\Tex_" + fileTitle + "\\";
		TString textureName;
		TString texturePath;
		for (int i = 0; i < header.m_textureNum; i++) {
			pFile->ReadString(textureName);
			texturePath = textureDir1 + textureName;
			TTexture * pTexture = TTextureMan::GetInstance()->LoadTexture(texturePath);
			if (pTexture == nullptr) {
				texturePath = textureDir2 + textureName;
				pTexture = TTextureMan::GetInstance()->LoadTexture(texturePath);
			}

			TAssert(pTexture != nullptr);
			if (pTexture) {
				m_textures.Add(pTexture);
			}
		}
	}

	//load the materials
	for (int i = 0;  i < header.m_materialNum; i++) {
		TMaterial * pMaterial = new TMaterial;
		if (!pMaterial->Load(pFile)) {
			return false;
		}

		TColor c = pMaterial->GetAmbient();
		pMaterial->SetAmbient(c.m_r, c.m_g, c.m_b, 0);

		c = pMaterial->GetEmissive();
		pMaterial->SetEmissive(c.m_r, c.m_g, c.m_b, 0);
		m_materials.Add(pMaterial);
	}


	//load the skin meshes
	for (int i = 0; i < header.m_skinMeshNum; i++) {
		TSkinMesh * pSkinMesh = new TSkinMesh();
		if (!pSkinMesh->Load(pFile, this, i, header.m_skinMeshNum, header.m_version)) {
			delete pSkinMesh;
			TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load, fail to load the skin mesh, filename=%s, index=%d, total=%d", m_skinFileName, i, header.m_skinMeshNum);
			return false;
		}
		m_skinMeshs.Add(pSkinMesh);
	}

	return true;
}

bool TSkin::Save(TFile* pFile)
{
	return true;
}

TSkin* TSkin::Clone()
{
	return this;
}

int TSkin::GetSkinBoneNum()
{
	if (m_version >= 9) {
		return m_boneNames.Size();
	}
	else {
		return m_skeBoneNum;
	}
}

bool TSkin::BindSkeleton(TSkeleton* pSkeleton)
{
	if (GetSkinBoneNum() > pSkeleton->GetBoneNum()) {
		return false;
	}

	if (m_version < 9) {
		m_boneIndex.Set(m_skeBoneNum + 1);
		for (int i = 0; i < m_skeBoneNum; i++) {
			m_boneIndex[i] = i;
		}
		m_boneIndex[m_skeBoneNum] = pSkeleton->GetBoneNum();
	}
	else {
		int boneNum = m_boneNames.Size();
		m_boneIndex.Set(boneNum + 1);

		for (int i = 0; i < boneNum; i++) {
			int index = -1;
			TSkeletonBone* pBone = pSkeleton->GetBoneByName(m_boneNames[i], index);
			if (pBone == nullptr) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkin::BindSkeleton, fail to bind the bone index, name=%s, index=%d", m_boneNames[i], i);
				return false;
			}
			m_boneIndex[i] = index;
		}
		m_boneIndex[boneNum] = pSkeleton->GetBoneNum();
	}
	return true;
}


void TSkin::Render(TSkinModel* pSkinModel)
{
	ApplyBlendMatrix(pSkinModel);

	for (int i = 0; i < m_skinMeshs.Size(); ++i) {
		TMaterial* pMaterial = m_materials[0];
		pMaterial->Render();

		TTexture* pTexture = m_textures[i];
		pTexture->Render();

		TSkinMesh* pSkinMesh = m_skinMeshs[i];
		pSkinMesh->Render();
	}
}

void TSkin::ApplyBlendMatrix(TSkinModel* pSkinModel)
{
	TCamera * pCamera = TEngine::GetInstance()->GetCamera();
	TMatrix4 matView = pCamera->GetViewMatrix();
	const TMatrix4* blendMatrixs = pSkinModel->GetBlendMatrixs();
	
	TDevice *pDevice = TEngine::GetInstance()->GetDevice();

	int numBone = GetSkinBoneNum();
	for (int i = 0; i < numBone; i++) {
		int blendIndex = GetBoneIndex(i);
		TMatrix4 matBoneView = matView * blendMatrixs[blendIndex];
		pDevice->SetVertexShaderConstantF(10+i, matBoneView.GetPointer(), 3);
	}
}


