#pragma once

#include <Render/TStream.h>
#include <d3d9.h>

struct TSkinMeshVertex;
class TD3D9Stream : public TStream
{
public:
	TD3D9Stream() {
		m_pVertexBuffer = nullptr;
		m_pIndexBuffer = nullptr;
		m_vertNum = 0;
		m_vertSize = 0;
		m_indexNum = 0;
		m_indexSize = 0;
		m_FVFFlags = 0;
	}

	virtual void Render();

	virtual bool SetSkinMeshVertexBuffer(TSkinMeshVertex* verts, int vertNum);
	virtual bool SetSkinMeshIndexBuffer(unsigned short * indices, int indiceNum);

private:
	IDirect3DVertexBuffer9 * m_pVertexBuffer;
	IDirect3DIndexBuffer9 * m_pIndexBuffer;
	int m_vertNum;
	int m_vertSize;
	int m_indexNum;
	int m_indexSize;
	int m_FVFFlags;
};

