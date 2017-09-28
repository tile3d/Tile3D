#include "TD3D9Stream.h"
#include "TD3D9Device.h"
#include <Render/TEngine.h>
#include <Render/SkinModel/TSkinMesh.h>
#include <Util/TLog.h>

void TD3D9Stream::Render()
{
	TD3D9Device * pDevice = dynamic_cast<TD3D9Device*>(TEngine::GetInstance()->GetDevice());
	IDirect3DDevice9Ex * pD3DDevice = pDevice->GetDirect3DDevice();

	pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_vertSize);
	pD3DDevice->SetIndices(m_pIndexBuffer);
	pD3DDevice->SetFVF(m_FVFFlags);
	pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertNum, 0, m_indexNum/3);
}


bool TD3D9Stream::SetSkinMeshVertexBuffer(TSkinMeshVertex* verts, int vertNum)
{
	TD3D9Device * pDevice = dynamic_cast<TD3D9Device*>(TEngine::GetInstance()->GetDevice());
	IDirect3DDevice9Ex * pD3DDevice = pDevice->GetDirect3DDevice();

	m_vertNum = vertNum;
	m_vertSize = sizeof(TSkinMeshVertex);
	m_FVFFlags = D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1;
	if (pD3DDevice->CreateVertexBuffer(m_vertNum*m_vertSize, D3DUSAGE_WRITEONLY, m_FVFFlags, D3DPOOL_DEFAULT, &m_pVertexBuffer, 0) != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshVertexBuffer,  Failed to create the vertex buffer");
		return false;
	}

	char * pVerts;
	if (m_pVertexBuffer->Lock(0, m_vertNum*m_vertSize, (void**)&pVerts, 0) != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshVertexBuffer,  Failed to lock the vertex buffer");
		return false;
	}

	memcpy(pVerts, verts, m_vertNum*m_vertSize);
	if (m_pVertexBuffer->Unlock() != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshVertexBuffer,  Failed to unlock the vertex buffer");
		return false;
	}
	return true;
}

bool TD3D9Stream::SetSkinMeshIndexBuffer(unsigned short * indices, int indiceNum)
{
	TD3D9Device * pDevice = dynamic_cast<TD3D9Device*>(TEngine::GetInstance()->GetDevice());
	IDirect3DDevice9Ex * pD3DDevice = pDevice->GetDirect3DDevice();

	m_indexNum = indiceNum;
	m_indexSize = sizeof(short);
	if (pD3DDevice->CreateIndexBuffer(m_indexNum*m_indexSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, 0) != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshIndexBuffer,  Failed to create the index buffer");
		return false;
	}

	short * pIndexs;
	if (m_pIndexBuffer->Lock(0, m_indexNum*m_indexSize, (void**)&pIndexs, 0) != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshIndexBuffer,  Failed to lock the index buffer");
		return false;
	}
	memcpy(pIndexs, indices, m_indexNum*m_indexSize);

	if (m_pIndexBuffer->Unlock() != D3D_OK) {
		TLog::Log(LOG_ERR, "D3D9Stream", "TD3D9Stream::SetSkinMeshIndexBuffer,  Failed to unlock the index buffer");
		return false;
	}

	return true;
}

