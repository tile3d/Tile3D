#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Math/TVector4.h>

struct TSkinMeshHeader
{
	int m_textureIndex;		//Texture index
	int m_materialIndex;	//Material Index
	int m_vertNums;			//vertex numbers
	int m_indexNums;		//index numbers
};

struct TSkinMeshVertex
{
	float m_pos[3];
	float m_weight[3];
	int m_matIndices;
	float m_normal[3];
	float m_tu;
	float m_tv;
};

struct TSkinMeshVertexTangent
{
	float m_pos[3];
	float m_weight[3];
	int m_matIndices;
	float m_normal[3];
	float m_tu;
	float m_tv;
	TVector4 m_tangent;
};

class TFile;
class TSkin;
class TSkinMesh : public TObject
{
public:
	TSkinMesh();
	~TSkinMesh();

	void Release();
	bool Load(TFile * pFile, TSkin* pSkin, int skinVersion);

private:
	TString m_skinMeshName;

	int m_textureIndex;		//Texture index
	int m_materialIndex;	//Material Index
	int m_vertNums;			//vertex numbers
	int m_indexNums;		//index numbers

	TSkinMeshVertex * m_verts;
	TSkinMeshVertexTangent * m_tangentVerts;
	unsigned short * m_indices;
};

