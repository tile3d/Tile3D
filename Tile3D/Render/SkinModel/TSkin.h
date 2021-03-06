#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Container/TArray.h>

#pragma pack(push, 1)

struct TSkinHeader
{
	int m_flags;
	int m_version;
	int m_skinMeshNum;
	int m_rigidMeshNum;
	int m_morphSkinMeshNum;
	int m_morphRigidMeshNum;
	int m_textureNum;
	int m_materialNum;
	int m_skinBoneNum;
	float m_minWeight;
	int m_skeBoneNum;
	int m_suppleMeshNum;
	int m_muscleMeshNum;
	char m_reserved[52];
};

#pragma pack(pop)

class TFile;
class TSkinMesh;
class TSkinModel;
class TMaterial;
class TSkeleton;
class TTexture;
class TSkin : public TObject
{
public:
	enum TSKIN_CONST
	{
		TSKIN_IDENTIFY = (('A' << 24) | ('S' << 16) | ('K' << 8) | 'I'),
		TSKIN_VERSION = 9
	};

public:
	TSkin();
	~TSkin();

	bool Load(const char * skinFile);
	bool Load(TFile * pFile);
	bool Save(TFile * pFile);
	TSkin * Clone();

	int GetSkinID() const { return m_skinID; }
	int GetSkinBoneNum();
	int GetBoneIndex(int i) {return m_boneIndex[i];}

	bool BindSkeleton(TSkeleton* pSkeleton);

	void ApplyBlendMatrix(TSkinModel* pSkinModel);
	void Render(TSkinModel* pModel);

private:
	int m_version;
	int m_skinID;

	float m_minWeight;
	int m_skeBoneNum;
	int m_skinBoneNum;

	TString m_skinFileName;
	TArray<TString> m_boneNames;
	TArray<int> m_boneIndex;

	TArray<TTexture*> m_textures;
	TArray<TSkinMesh*> m_skinMeshs;
	TArray<TMaterial*> m_materials;
};

