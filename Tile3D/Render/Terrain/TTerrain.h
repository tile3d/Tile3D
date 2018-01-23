#pragma once

class TTerrain
{
public:
	bool Init(const char* szFileName, bool bEnableLightMap, const TVector3& vLoadCenter, float fLoadDistance, 
		float fViewDistance, float fLodDistance1, float fLodDistance2, bool bMultiThreadLoad, bool bEnableDynamicStream);



private:
	bool m_enableLightMap;
};