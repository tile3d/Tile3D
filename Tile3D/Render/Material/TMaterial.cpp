#include "TMaterial.h"
#include <File/TFile.h>
#include <string.h>

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

