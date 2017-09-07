#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>


//Skeleton Cache(TBD)
//1) need test the skeleton cache memory consumption
//2) how to release the skeleton memory(skinmodel or skeletonman?)
//3) lock (how to lock)
class TSkeleton;
class TFile;
class TSkeletonMan
{
public:
	static TSkeletonMan * GetInstance() {
		static TSkeletonMan skeletonMan;
		return &skeletonMan;
	}

	TSkeleton* LoadSkeleton(const char * skeletonFile);
	TSkeleton* LoadSkeleton(TFile * pFile);

private:
	TSkeleton* FindSkeletonByFile(const char * skeletonFile);
	TSkeleton* FindSkeletonByID(int skeletonFileID);
	TSkeleton * CreateSkeleton(TFile * pFile);


private:
	THashMap<int, TSkeleton*> m_skeletons;
	TMutexLock m_lock;
};

