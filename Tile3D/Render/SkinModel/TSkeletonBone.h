#pragma once

#include <Core/TObject.h>
#include <Container/TArray.h>
#include <Container/TString.h>
#include <Math/TMatrix4.h>
#include <Math/TQuaternion.h>

struct TBoneData
{
	char m_flags;
	int m_parentIndex;
	int m_firstJointIndex;
	int m_childNum;
	TMatrix4 m_relativeMat;
	TMatrix4 m_boneInitMat;
};


class TFile;
class TSkeleton;
class TSkeletonBone : public TObject
{
public:
	enum TBONE_FLAGS
	{
		TBONE_FLAGS_FAKEBONE = 0x01,
		TBONE_FLAGS_FLIPPED = 0x02,
	};


public:
	TSkeletonBone();
	~TSkeletonBone();

	void Release();
	void Init(TSkeleton * pSkeleton) {
		m_pSkeleton = pSkeleton;
	}

	bool Load(TFile * pFile);
	bool Save(TFile * pFile);

	TString& GetBoneName() { return m_boneName; }

	void SetRelativeTM(const TMatrix4 & mat);
	void ResetRelativeTM() { SetRelativeTM(m_originRelMat);  }

	bool IsFlipped() { return (m_flags & TBONE_FLAGS_FLIPPED) ? true : false; }

	int GetParentIndex() { return m_parentIndex; }
	const TMatrix4& GetBoneInitTM() { return m_boneInitMat; }

	int GetChildNum() { return m_childrens.Size(); }
	int GetChild(int index) { return m_childrens[index]; }
	TSkeletonBone* GetChildPtr(int index);

private:
	int m_boneID;
	char m_flags;
	int m_parentIndex;
	int m_firstJointIndex;
	TString m_boneName;
	TArray<short> m_childrens;
	TSkeleton * m_pSkeleton;

	TMatrix4 m_relativeMat;			//releative tm to parent
	TMatrix4 m_boneInitMat;			//bone initial matrix which transform skin mesh from
	TMatrix4 m_originRelMat;		//origin relative matrix of bone
	TQuaternion m_relativeQu;	

};
