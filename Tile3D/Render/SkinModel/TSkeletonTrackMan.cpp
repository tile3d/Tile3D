#include "TSkeletonTrackMan.h"
#include <Common/TLog.h>

TSkeletonTrack * TSkeletonTrackMan::LoadSkeletonTrack(int fileNameID, const char * fileName, bool oldBoneFile)
{
	TSkeletonTrack *pSkeletonTrack = FindSkeletonTrack(fileNameID, fileName);
	//TBD: need use reference count or clone?
	if (pSkeletonTrack) {
		return pSkeletonTrack;
	}

	pSkeletonTrack = new TSkeletonTrack();
	pSkeletonTrack->SetBoneFile(oldBoneFile);

	//use the old bone file, all the track data in the .bon file
	if (oldBoneFile) {
		pSkeletonTrack->SetFileNameAndID(fileName, fileNameID);
	}
	//all the track data under the stcks files
	else{
		if (!pSkeletonTrack->Load(fileName)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackMan::LoadSkeletonTrack,  Failed to load the skeleton track: [%s].", fileName);
			return nullptr;
		}
	}

	m_lock.Lock();
	m_skeletonTracks.Put(fileNameID, pSkeletonTrack);
	m_lock.Unlock();
	return pSkeletonTrack;
}

TSkeletonTrack * TSkeletonTrackMan::FindSkeletonTrack(int fileNameID, const char * fileName)
{
	m_lock.Lock();
	TSkeletonTrack ** pSkeletonTrack = m_skeletonTracks.Find(fileNameID);
	m_lock.Unlock();
	if (pSkeletonTrack == nullptr) {
		return nullptr;
	}

	if (strcmp((*pSkeletonTrack)->GetFileName(), fileName) != 0) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackMan::FindSkeletonTrack,  different filename have the same file id: [%s].", fileName);
		TAssert(0);
		return nullptr;
	}
	return *pSkeletonTrack;
}

