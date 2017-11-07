#include "TMaterial.h"
#include <File/TFile.h>
#include <string.h>
#include <Common/TLog.h>
#include <Render/TEngine.h>
#include <Render/TDevice.h>
#include <Render/Windows/TD3D9Device.h>

TMaterial::TMaterial()
{
	memset(&m_materialParam, 0, sizeof(m_materialParam));
	m_b2Sided = false;
}


TMaterial::~TMaterial()
{

}


bool TMaterial::Load(TFile * pFile)
{
	int length;
	char lineBuffer[2048];
//	char result[2048];
	if (pFile->IsBinary()) {
		pFile->ReadString(lineBuffer, 2048, &length);
		pFile->Read(&m_materialParam.m_ambient, sizeof(TColor), &length);
		pFile->Read(&m_materialParam.m_diffuse, sizeof(TColor), &length);
		pFile->Read(&m_materialParam.m_emissive, sizeof(TColor), &length);
		pFile->Read(&m_materialParam.m_specular, sizeof(TColor), &length);
		pFile->Read(&m_materialParam.m_power, sizeof(float), &length);
		pFile->Read(&m_b2Sided, sizeof(bool), &length);

	}
	else {
		//Material:
		pFile->ReadLine(lineBuffer, 2048, &length);

		pFile->ReadLine(lineBuffer, 2048, &length);
		if (strcmp(lineBuffer, "{") != 0) {
			return false;
		}

		pFile->ReadLine(lineBuffer, 2048, &length);
		sscanf(lineBuffer, "Ambient:(%f, %f, %f, %f)", &m_materialParam.m_ambient.m_r, &m_materialParam.m_ambient.m_g, &m_materialParam.m_ambient.m_b, &m_materialParam.m_ambient.m_a);

		pFile->ReadLine(lineBuffer, 2048, &length);
		sscanf(lineBuffer, "Diffuse:(%f, %f, %f, %f)", &m_materialParam.m_diffuse.m_r, &m_materialParam.m_diffuse.m_g, &m_materialParam.m_diffuse.m_b, &m_materialParam.m_diffuse.m_a);

		pFile->ReadLine(lineBuffer, 2048, &length);
		sscanf(lineBuffer, "Emissive:(%f, %f, %f, %f)", &m_materialParam.m_emissive.m_r, &m_materialParam.m_emissive.m_g, &m_materialParam.m_emissive.m_b, &m_materialParam.m_emissive.m_a);

		pFile->ReadLine(lineBuffer, 2048, &length);
		sscanf(lineBuffer, "Specular:(%f, %f, %f, %f)", &m_materialParam.m_specular.m_r, &m_materialParam.m_specular.m_g, &m_materialParam.m_specular.m_b, &m_materialParam.m_specular.m_a);

		pFile->ReadLine(lineBuffer, 2048, &length);
		sscanf(lineBuffer, "Power:(%f)", &m_materialParam.m_power);

		pFile->ReadLine(lineBuffer, 2048, &length);
		int value = 0;
		sscanf(lineBuffer, "2Sided:(%d)", &value);
		m_b2Sided = value ? true : false;

		pFile->ReadLine(lineBuffer, 2048, &length);
		if (strcmp(lineBuffer, "}") != 0) {
			return false;
		}
	}
	return true;
}


void SetColor(D3DCOLORVALUE& color, TColor & tcolor)
{
	color.r = tcolor.m_r;
	color.g = tcolor.m_g;
	color.b = tcolor.m_b;
	color.a = tcolor.m_a;

}


void TMaterial::Render()
{
	TD3D9Device * pDevice = (TD3D9Device*)TEngine::GetInstance()->GetDevice();
	D3DMATERIAL9 mat;
	SetColor(mat.Ambient, this->GetAmbient());
	SetColor(mat.Diffuse, this->GetDiffuse());
	SetColor(mat.Emissive, this->GetEmissive());
	SetColor(mat.Specular, this->GetSpecular());
	mat.Power = this->m_materialParam.m_power;

	HRESULT hresult = pDevice->GetDirect3DDevice()->SetMaterial((D3DMATERIAL9 *)this);
	if (hresult != S_OK) {
		TLog::Log(LOG_ERR, "Texture", "TTexture::Render,  failed to set the texture");
	}
}
