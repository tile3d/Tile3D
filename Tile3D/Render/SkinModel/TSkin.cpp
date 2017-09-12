#include "TSkin.h"
#include "TSkinMeshMan.h"
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>
#include <Render/Material/TMaterial.h>

//TBD
TSkin::TSkin()
{

}

TSkin::~TSkin()
{

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
		TSkinMesh * pMesh = TSkinMeshMan::GetInstance()->LoadSkinMesh(pFile, this,  i, header.m_skinMeshNum, header.m_version);
		if (pMesh == nullptr) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load, fail to load the skin mesh, filename=%s, index=%d, total=%d", m_skinFileName, i, header.m_skinMeshNum);
			return false;
		}
		m_skinMeshs.Add(pMesh);
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

