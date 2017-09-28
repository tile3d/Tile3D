#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Math/TVector4.h>
#include <Render/TStream.h>


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
class TStream;
class TSkinMesh : public TObject
{
public:
	TSkinMesh();
	~TSkinMesh();

	void Release();
	bool Load(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion);

	TSkinMesh * Clone();
	int GetSkinMeshID() { return m_skinMeshID; }
	void Render();

private:
	int m_textureIndex;		//Texture index
	int m_materialIndex;	//Material Index
	int m_vertNums;			//vertex numbers
	int m_indexNums;		//index numbers
	int m_skinIndex;		//skin index
	int m_skinMeshID;		//skin mesh ID
	TString m_skinMeshName;

	TSkinMeshVertex * m_verts;
	TSkinMeshVertexTangent * m_tangentVerts;
	TStream * m_pStream;
	unsigned short * m_indices;
};

