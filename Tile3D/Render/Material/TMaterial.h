#pragma once

#include <Core/TObject.h>
#include <Math/TColor.h>

struct TMaterialParam
{
	TColor m_diffuse;
	TColor m_ambient;
	TColor m_specular;
	TColor m_emissive;
	float m_power;

};

class TFile;
class TMaterial : public TObject
{
public:
	TMaterial();
	~TMaterial();


	bool Load(TFile * pFile);
	bool Save(TFile * pFile);

	TColor& GetDiffuse() { return m_materialParam.m_diffuse; }
	TColor& GetAmbient() { return m_materialParam.m_ambient; }
	TColor& GetSpecular() { return m_materialParam.m_specular; }
	TColor& GetEmissive() { return m_materialParam.m_emissive; }

	void SetDiffuse(float r, float g, float b, float a) {
		m_materialParam.m_diffuse.SetColor(r, g, b, a);
	}

	void SetAmbient(float r, float g, float b, float a) {
		m_materialParam.m_ambient.SetColor(r, g, b, a);
	}

	void SetSpecular(float r, float g, float b, float a) {
		m_materialParam.m_specular.SetColor(r, g, b, a);
	}

	void SetEmissive(float r, float g, float b, float a) {
		m_materialParam.m_emissive.SetColor(r, g, b, a);
	}

private:
	TMaterialParam m_materialParam;
	bool m_b2Sided;
};

