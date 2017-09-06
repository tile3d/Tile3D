#include "TSkeletonTrack.h"
#include "TSkeleton.h"
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>


TSkeletonTrack::TSkeletonTrack()
{
	m_version = 0;
	m_animStart = 0;
	m_animEnd = 0;
	m_animFPS = 0;
	m_fileNameID = 0;
}


TSkeletonTrack::~TSkeletonTrack()
{

}

bool TSkeletonTrack::Load(const char* fileName)
{
	TFileImage fi;
	if (fi.Open("", fileName, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Load,  Failed to load the skeleton track file: %s", fileName);
		return false;
	}

	if (!Load(&fi)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Load,  Failed to load the skeleton track: %s", fileName);
		fi.Close();
		return false;
	}
	fi.Close();
	return true;
}

//	Load track data from .stck file
bool TSkeletonTrack::Load(TFile * pFile)
{
	int count;
	int flag, version;

	//	Read file flag
	if (!pFile->Read(&flag, sizeof(int), &count))
		return false;

	//	Read file version
	if (!pFile->Read(&version, sizeof(int), &count))
		return false;

	if (flag != TSkeleton::TSKELETON_IDENTIFY || version > TSkeleton::TSKELETON_VERSION)
	{
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Load, file flag or version error, flag=%d, version=%d", flag, version);
		return false;
	}

	m_version = version;

	TSkeletonTrackHeader sth;
	if (!pFile->Read(&sth, sizeof(sth), &count) || count != sizeof(sth)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Load,  fail to read the skeleton track header");
		return false;
	}

	m_animStart = sth.m_animStart;
	m_animEnd = sth.m_animEnd;
	m_animFPS = sth.m_animFPS;
	m_fileName = pFile->GetRelativeFileName();
	m_fileNameID = TFileDir::GetInstance()->GetIDFromFileName(m_fileName);

	int bone_id = 0;
	for (int i = 0; i < sth.m_boneTrackNum; i++) {
		if (!pFile->Read(&bone_id, sizeof(int), &count)) {
			return false;
		}

		if (!LoadBoneTrack(pFile, bone_id)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Load,  fail to load the bone track, bone_id=%d", bone_id);
			return false;
		}
	}

	return true;
}

bool TSkeletonTrack::LoadBoneTrack(TFile * pFile, int bone_id)
{
	TSkeletonTrackDataPos * pPosTrack = new TSkeletonTrackDataPos;
	if (!pPosTrack->LoadBoneTrackPos(pFile, bone_id, m_version)) {
		delete pPosTrack;
		return false;
	}

	TSkeletonTrackDataRot * pRotTrack = new TSkeletonTrackDataRot;
	if (!pRotTrack->LoadBoneTrackPos(pFile, bone_id, m_version)) {
		delete pRotTrack;
		return false;
	}
	
	return true;
}


bool TSkeletonTrack::Save(const char* fileName)
{
	TFileImage fi;
	if (fi.Open("", fileName, TFile::TFILE_CREATENEW | TFile::TFILE_BINARY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Save,  Failed to create the new skeleton track file: %s", fileName);
		return false;
	}

	if (!Save(&fi)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrack::Save,  Failed to save the skeleton track: %s", fileName);
		fi.Close();
		return false;
	}
	fi.Close();
	return true;
}

