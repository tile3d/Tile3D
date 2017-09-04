#include "TBone.h"
#include "TSkeleton.h"
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>

//TBD: init the property
TBone::TBone()
{
	m_pSkeleton = nullptr;
}

TBone::~TBone()
{
	Release();
}

void TBone::Release()
{

}

bool TBone::Load(TFile * pFile)
{
	if (!pFile->ReadString(m_boneName)) {
		TLog::Log(LOG_ERR, "SkinModel", "TBone::Load, failed to read the bone name filename=%s, bonename=%s", pFile->GetRelativeFileName(), m_boneName);
		return false;
	}

	int count;
	TBoneData data;
	if (!pFile->Read(&data, sizeof(data), &count) || count != sizeof(data)) {
		TLog::Log(LOG_ERR, "SkinModel", "TBone::Load, failed to read the bone data, count=%d, data=%d", count, sizeof(data));
		return false;
	}

	m_boneID = TFileDir::GetInstance()->GetIDFromFileName(m_boneName);
	m_flags = data.m_flags;
	m_parentIndex = data.m_parentIndex;
	m_firstJointIndex = data.m_firstJointIndex;
	m_originRelMat = data.m_relativeMat;
	m_boneInitMat = data.m_boneInitMat;

	ResetRelativeTM();

	m_childrens.Reserve(data.m_childNum);
	for (int i = 0; i < data.m_childNum; i++) {
		int child;
		pFile->Read(&child, sizeof(int), &count);
		m_childrens.Add(child);
	}
	return true;
}

bool TBone::Save(TFile * pFile)
{
	if (!pFile->WriteString(m_boneName)) {
		TLog::Log(LOG_ERR, "SkinModel", "TBone::Save, failed to write the bone name filename=%s, bonename=%s", pFile->GetRelativeFileName(), m_boneName);
		return false;
	}

	TBoneData data;
	memset(&data, 0, sizeof(data));
	data.m_flags = m_flags;
	data.m_parentIndex = m_parentIndex;
	data.m_firstJointIndex = m_firstJointIndex;
	data.m_childNum = m_childrens.Size();
	data.m_relativeMat = m_relativeMat;
	data.m_boneInitMat = m_boneInitMat;

	int count;
	if (!pFile->Write(&data, sizeof(data), &count) || count != sizeof(data)) {
		TLog::Log(LOG_ERR, "SkinModel", "TBone::Save, failed to read the bone data, count=%d, data=%d", count, sizeof(data));
		return false;
	}


	for (int i = 0; i < m_childrens.Size(); i++) {
		pFile->Write(&m_childrens[i], sizeof(int), &count);
	}
	return true;
}

void TBone::SetRelativeTM(const TMatrix4 & mat)
{
	m_relativeMat = mat;
	if (IsFlipped()) {
		TMatrix4 tempMat = m_relativeMat;
		tempMat.m_mat[0][0] = -tempMat.m_mat[0][0];
		tempMat.m_mat[0][1] = -tempMat.m_mat[0][1];
		tempMat.m_mat[0][2] = -tempMat.m_mat[0][2];
		m_relativeQu.ConvertFromMatrix(tempMat);
	}
	else {
		m_relativeQu.ConvertFromMatrix(m_relativeMat);
	}
}

