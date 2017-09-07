#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>
#include "TSkeleton.h"
#include "TSkeletonBone.h"
#include "TSkeletonJoint.h"
#include "TSkeletonHook.h"
#include "TSkeletonTrack.h"
#include "TSkeletonTrackMan.h"

TSkeleton::TSkeleton()
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
	return true;
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
		TSkeletonBone * pBone = new TSkeletonBone();
		pBone->Init(this);

		if (!pBone->Load(pFile)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the bone, filename=%s, boneindex=%d", m_filename, i);
			delete pBone;
			return false;
		}
		AddBone(pBone);
	}

	//Load the joints
	if (header.m_jointNum > 0) {
		int* ids = new int[header.m_jointNum];
		if (!pFile->Read(ids, header.m_jointNum*sizeof(int), &count) || count != header.m_jointNum*sizeof(int)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the join ids, filename=%s", m_filename);
			delete[] ids;
			return false;
		}

		//old version, load the skeleton track data from the bon file directly, new version store the track data under stck dir
		TSkeletonTrack *pSkeletonTrack = nullptr;
		if (header.m_version < 6) {
			pSkeletonTrack = TSkeletonTrackMan::GetInstance()->LoadSkeletonTrack(m_skeletonID, m_filename, true);
			if (pSkeletonTrack == nullptr) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the skeleton track, old filename=%s", m_filename);
				return false;
			}

			pSkeletonTrack->SetAnimFrameRange(header.m_animStart, header.m_animEnd);
			pSkeletonTrack->SetAnimFPS(header.m_animFPS);
		}
		
		for (int i = 0; i < header.m_jointNum; i++) {
			TSkeletonJoint * pJoint = TSkeletonJoint::CreateJointByClassID(ids[i]);
			pJoint->Init(this);

			if (pJoint->Load(pFile)) {
				delete pJoint;
				TLog::Log(LOG_ERR, "SkinModel", "TSkeleton::Load, failed to load the join from the filename=%s", m_filename);
				return false;
			}
			AddJoint(pJoint);
			if (header.m_version < 6) {
				pSkeletonTrack->LoadBoneTrack(pFile, i);
			}
		}
		
		delete[] ids;
	}

	//load the hooks

	TLog::Log(LOG_DEBUG, "SkinModel", "TSkeleton::Load, load the skelethon from file=%s", m_filename);
	return true;
}

int TSkeleton::AddBone(TSkeletonBone* pBone)
{
	int index = m_bones.Add(pBone);
	if (pBone->GetParentIndex() < 0) {
		m_rootBones.Add(index);
	}
	return index;
}

int TSkeleton::AddJoint(TSkeletonJoint * pJoint)
{
	int index = m_joints.Add(pJoint);
	return index;
}



