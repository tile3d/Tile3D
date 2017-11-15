#include "TSkinMesh.h"
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Common/TLog.h>
#include <Math/TVector3.h>
#include <Render/TEngine.h>

TSkinMesh::TSkinMesh()
{
	m_textureIndex = 0;
	m_materialIndex = 0;
	m_vertNums = 0;
	m_indexNums = 0;
	m_skinIndex = 0;
	m_skinMeshID = 0;
	m_verts = nullptr;
	m_tangentVerts = nullptr;
	m_indices = nullptr;
}

TSkinMesh::~TSkinMesh()
{
	Release();
}

void TSkinMesh::Release()
{
	if (m_verts != nullptr) {
		delete[] m_verts;
		m_verts = nullptr;
	}

	if (m_tangentVerts != nullptr) {
		delete[] m_tangentVerts;
		m_tangentVerts = nullptr;
	}


	if (m_indices != nullptr) {
		delete[] m_indices;
		m_indices = nullptr;
	}

}

TSkinMesh * TSkinMesh::Clone()
{
	return this;
}

bool TSkinMesh::Load(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum,  int skinVersion)
{
	if (!pFile->ReadString(m_skinMeshName)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMesh::Load,  Failed to read the skinmesh name: [%s].", pFile->GetRelativeFileName());
		return false;
	}

	//load the skin mesh header
	TSkinMeshHeader header;
	int count;
	if (!pFile->Read(&header, sizeof(header), &count) || count != sizeof(header)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMesh::Load,  Failed to load the skinmesh header, filename=%s.", m_skinMeshName);
		return false;
	}

	m_textureIndex = header.m_textureIndex;
	m_materialIndex = header.m_materialIndex;
	m_vertNums = header.m_vertNums;
	m_indexNums = header.m_indexNums;
	m_skinIndex = skinIndex;
	m_skinMeshID = TFileDir::GetInstance()->GetIDFromFileName(pFile->GetRelativeFileName());

	//create the vertex buffer
	if (m_vertNums > 0) {
		m_verts = new TSkinMeshVertex[m_vertNums];
		m_tangentVerts = new TSkinMeshVertexTangent[m_vertNums];

		if (!pFile->Read(m_verts, sizeof(TSkinMeshVertex)*m_vertNums, &count) || count != sizeof(TSkinMeshVertex)*m_vertNums){
			TLog::Log(LOG_ERR, "SkinModel", "TSkinMesh::Load,  Failed to load the skinmesh vertexs, filename=%s.", m_skinMeshName);
			return false;
		}	

		for (int i = 0; i < m_vertNums; i++) {
			memcpy(&m_tangentVerts[i], &m_verts[i], sizeof(TSkinMeshVertex));

			TVector3 normal(m_verts[i].m_normal[0], m_verts[i].m_normal[1], m_verts[i].m_normal[2]);
			TVector3 binormal = TVector3::CrossProduct(normal, TVector3(1, 0, 0));
			binormal.Normalize();
			//TBD: why
			if (binormal.SquaredLength() < 1e-6) {
				binormal = TVector3::CrossProduct(normal, TVector3(0, 1, 0));
				binormal.Normalize();
			}
			m_tangentVerts[i].m_tangent = TVector4(binormal.m_x, binormal.m_y, binormal.m_z, 1.0f);
		}
	}

	//create the index buffer
	if (m_indexNums > 0) {
		m_indices = new unsigned short[m_indexNums];

		if (!pFile->Read(m_indices, sizeof(unsigned short)*m_indexNums, &count) || count != sizeof(unsigned short)*m_indexNums) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinMesh::Load,  Failed to load the skinmesh indexs, filename=%s.", m_skinMeshName);
			return false;
		}
	}

	TDevice * pDevice = TEngine::GetInstance()->GetDevice();
	m_pStream = pDevice->CreateStream();

	m_pStream->SetSkinMeshVertexBuffer(m_verts, m_vertNums);
	m_pStream->SetSkinMeshIndexBuffer(m_indices, m_indexNums);

	//TBD
	//1) build the tangents
	//2) build the mesh aabb
	return true;
}

void TSkinMesh::Render()
{
	m_pStream->Render();
}

