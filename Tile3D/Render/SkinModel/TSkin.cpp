#include "TSkin.h"
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>

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
	m_skinBoneNum = header.m_boneNum;

	//load the bone names
	if (header.m_version >= 9 && header.m_boneNum > 0) {
		m_boneNames.Reserve(header.m_boneNum);
		for (int i = 0; i < header.m_boneNum; i++) {
			TString boneName;
			pFile->ReadString(boneName);
			m_boneNames.Add(boneName);
		}
	}

	//load the textures
	if (header.m_textureNum > 0) {

	}

	//load the materials
	if (header.m_materialNum > 0) {

	}


	//load the skin meshes
	if (header.m_skinMeshNum > 0) {
		for (int i = 0; i < header.m_skinMeshNum; i++) {
			TSkinMesh * pMesh = TSkinMeshMan::GetInstance()->LoadSkinMesh(pFile, this, TSKINMESH_CID_SKINMESH, i, header.m_skinMeshNum, header.m_version);
			if (pMesh == nullptr) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkin::Load, fail to load the skin mesh, filename=%s, index=%d, total=%d", m_skinFileName, i, header.m_skinMeshNum);
				return false;
			}
		}
	}
}

bool TSkin::Save(TFile* pFile)
{

}

TSkin* TSkin::Clone()
{
	return this;
}

