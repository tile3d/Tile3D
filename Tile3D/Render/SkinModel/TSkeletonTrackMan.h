#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>
#include "TSkeletonTrack.h"

class TSkeletonTrackMan
{
public:
	static TSkeletonTrackMan * GetInstance() {
		static TSkeletonTrackMan skeletonTrackMan;
		return &skeletonTrackMan;
	}

	TSkeletonTrack * LoadSkeletonTrack(int fileNameID, const char * fileName, bool oldBoneFile);

private:
	TSkeletonTrack * FindSkeletonTrack(int fileNameID, const char * fileName);

private:
	THashMap<int, TSkeletonTrack*> m_skeletonTracks;
	TMutexLock m_lock;
};

