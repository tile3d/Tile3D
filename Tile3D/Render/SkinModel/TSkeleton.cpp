#include "TSkeleton.h"
#include "TBone.h"
#include "TJoint.h"
#include "THook.h"
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>

TSkeleton::TSkeleton() : TObject(CLASSID_SKELETON)
{
	m_version = 0;
	m_animFPS = 0;
	m_skeletonID = 0;
	m_pSkinModel = nullptr;
}

TSkeleton::~TSkeleton()
{
	Release();
}

void TSkeleton::Release()
{

}

bool TSkeleton::Save(TFile * pFile)
{

}

bool TSkeleton::Load(TFile * pFile)
{
	TSkeletonFileHeader header;
	int count;

	if (!pFile->Read(&header, sizeof(header), &count) || count != sizeof(header)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load,  Failed to read the skeleton header: [%s].", pFile->GetRelativeFileName());
		return false;
	}

	if (header.m_flags != TSKELETON_IDENTIFY || header.m_version > TSKELETON_VERSION) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, incorrect skelethon flags or version: [%s].", pFile->GetRelativeFileName());
		return false;
	}

	m_version = header.m_version;
	m_animFPS = header.m_animFPS;
	m_filename = pFile->GetRelativeFileName();
	m_skeletonID = TFileDir::GetInstance()->GetIDFromFileName(m_filename);

	//Load the bones
	for (int i = 0; i < header.m_boneNum; i++) {
		TBone * pBone = new TBone();
		pBone->Init(this);

		if (!pBone->Load(pFile)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the bone, filename=%s, boneindex=%d", pFile->GetRelativeFileName(), i);
			delete pBone;
			return false;
		}
		AddBone(pBone);
	}

	//Load the joints
	if (header.m_jointNum > 0) {
		int* ids = new int[header.m_jointNum];

		if (!pFile->Read(ids, header.m_jointNum*sizeof(int), &count) || count != header.m_jointNum*sizeof(int)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the join ids, filename=%s", pFile->GetRelativeFileName());
			delete[] ids;
			return false;
		}

		if (header.m_version < 6) {

		}
		else {
			for (int i = 0; i < header.m_jointNum; i++) {
				TJoint * pJoint = TJoint::CreateJointByClassID(ids[i]);
				pJoint->Init(this);

				if (pJoint->Load(pFile)) {
					return false;
				}
			}
		}

	}

	TLog::Log(LOG_DEBUG, "SkinModel", "TSkeleton::Load, load the skelethon from file=%s", pFile->GetRelativeFileName());
}

int TSkeleton::AddBone(TBone* pBone)
{
	int index = m_bones.Add(pBone);
	if (pBone->GetParentIndex() < 0) {
		m_rootBones.Add(index);
	}
	return index;
}



