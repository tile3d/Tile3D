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
class TSkeletonHook;
class TSkeleton : public TObject
{
public:
	enum TSKELETON_CONST
	{
		TSKELETON_IDENTIFY = (('A' << 24) | ('S' << 16) | ('K' << 8) | 'E'),
		TSKELETON_VERSION = 6
	};

public:
	TSkeleton();
	~TSkeleton();

	bool Load(const char * pFile);
	bool Load(TFile * pFile);
	bool Save(TFile * pFile);

	int AddBone(TSkeletonBone* pBone);
	int AddJoint(TSkeletonJoint* pJoint);
	int AddHook(TSkeletonHook* pHook);

	TSkeletonBone* GetBone(int index) { return m_bones[index]; }
	TSkeletonJoint* GetJoint(int index) { return m_joints[index]; }
	TSkeletonHook* GetHook(int index) { return m_hooks[index]; }

	void SetSkinModel(TSkinModel * pSkinModel) {
		m_pSkinModel = pSkinModel;
	}

	int GetSkeletonID() const { return m_skeletonID; }
	TString & GetFileName() { return m_fileName; }
	bool FindRefBone();

	TSkeleton * Clone();


private:
	int m_version;
	int m_animFPS;
	int m_skeletonID;
	int m_refBone;				//	Reference bone

	TString m_fileName;
	TSkinModel * m_pSkinModel;

	TArray<TSkeletonBone*> m_bones;
	TArray<TSkeletonJoint*> m_joints;
	TArray<TSkeletonHook*> m_hooks;

	TArray<int> m_rootBones;
};



