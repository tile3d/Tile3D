#pragma once

#include <d3d9.h>

class TVertexDecl
{
public:
	TVertexDecl() {
		m_pVertDecl = nullptr;
	}

	~TVertexDecl() {

	}

	bool Init(const D3DVERTEXELEMENT9* decl);
	void Release();

	bool Appear();

	IDirect3DVertexDeclaration9* GetVertDecl() { return m_pVertDecl; }

private:
	IDirect3DVertexDeclaration9 * m_pVertDecl;
};

