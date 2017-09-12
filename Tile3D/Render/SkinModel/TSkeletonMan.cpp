#include "TSkeletonMan.h"
#include "TSkeleton.h"
#include <Util/TTypes.h>
#include <Util/TLog.h>
#include <File/TFileDir.h>
#include <File/TFile.h>
#include <File/TFileImage.h>


//TBD: if multithread initiaze this in the same time, how to handle?
TSkeleton* TSkeletonMan::LoadSkeleton(const char * skeletonFile)
{
	TSkeleton * pSkeleton = FindSkeletonByFile(skeletonFile);
	if (pSkeleton != nullptr) {
		return pSkeleton->Clone();
	}

	TFileImage fi;
	if (fi.Open("", skeletonFile, TFile::TFILE_OPENEXIST| TFile::TFILE_BINARY| TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonMan::LoadSkeleton,  Failed to load the skeleton: [%s].", skeletonFile);
		return nullptr;
	}
	TSkeleton* pNewSkeleton = CreateSkeleton(&fi);
	fi.Close();
	return pNewSkeleton;
}

TSkeleton* TSkeletonMan::LoadSkeleton(TFile * pFile)
{
	TSkeleton * pSkeleton = FindSkeletonByFile(pFile->GetRelativeFileName());
	if (pSkeleton != nullptr) {
		return pSkeleton->Clone();
	}
	return CreateSkeleton(pFile);
}

TSkeleton* TSkeletonMan::FindSkeletonByFile(const char * skeletonFile)
{
	char relFile[MAX_PATH];
	relFile[0] = '\0';

	TFileDir::GetInstance()->GetRelativePath(skeletonFile, relFile);
	int skeletonFileID = TFileDir::GetInstance()->GetIDFromFileName(relFile);
	return FindSkeletonByID(skeletonFileID);
}

TSkeleton* TSkeletonMan::FindSkeletonByID(int skeletonFileID)
{
	m_lock.Lock();
	TSkeleton ** pSkeleton = m_skeletons.Find(skeletonFileID);
	m_lock.Unlock();
	if (pSkeleton == nullptr) {
		return nullptr;
	}
	return *pSkeleton;
}

TSkeleton * TSkeletonMan::CreateSkeleton(TFile * pFile)
{
	TSkeleton * pSkeleton = new TSkeleton();

	if (!pSkeleton->Load(pFile)) {
		delete pSkeleton;
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonMan::CreateSkeleton,  Failed to load the skeleton: [%s].", pFile->GetRelativeFileName());
		return nullptr;
	}

	m_lock.Lock();
	m_skeletons.Put(pSkeleton->GetSkeletonID(), pSkeleton);
	m_lock.Unlock();
	return pSkeleton;
}

