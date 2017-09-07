#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Container/TArray.h>


#pragma pack(push, 1)
struct TSkeletonFileHeader
{
	int m_flags;
	int m_version;
	int m_boneNum;
	int m_jointNum;
	int m_hookNum;
	int m_animStart;
	int m_animEnd;
	int m_animFPS;
	char m_reserved[64];
};
#pragma pack(pop)


class TSkinModel;
class TFile;
class TSkeletonBone;
class TSkeletonJoint;
class THook;
class TSkeleton : public TObject
{
public:
	enum TSKELETON_CONST
	{
		TSKELETON_IDENTIFY = (('A' << 24) | ('S' << 16) | ('M' << 8) | 'E'),
		TSKELETON_VERSION = 6
	};

public:
	TSkeleton();
	~TSkeleton();

	bool Load(TFile * pFile);
	bool Save(TFile * pFile);

	int AddBone(TSkeletonBone* pBone);
	int AddJoint(TSkeletonJoint* pJoint);

	void SetSkinModel(TSkinModel * pSkinModel) {
		m_pSkinModel = pSkinModel;
	}

	int GetSkeletonID() const { return m_skeletonID; }
	
	TSkeleton * Clone();


private:
	void Release();

private:
	int m_version;
	int m_animFPS;
	int m_skeletonID;
	TString m_filename;
	TSkinModel * m_pSkinModel;

	TArray<TSkeletonBone*> m_bones;
	TArray<TSkeletonJoint*> m_joints;
	TArray<THook*> m_hooks;
	TArray<int> m_rootBones;


};

