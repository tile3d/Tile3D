#pragma once

#include <Render/SkinModel/TSkinMesh.h>

struct TSkinMeshVertex;
class TStream
{
public:
	virtual void Render() {}

	virtual bool SetSkinMeshVertexBuffer(TSkinMeshVertex* verts, int vertNum) { return true; }
	virtual bool SetSkinMeshIndexBuffer(unsigned short * indices, int indiceNum) { return true; }


};