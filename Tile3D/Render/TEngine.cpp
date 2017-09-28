#include "TEngine.h"
#include <Render/SkinModel/TSkinModel.h>
#include <Render/SkinModel/TSkinModelMan.h>

void TEngine::Setup(TApplication * pApplication, TDevice * pDevice) {
	m_pApplication = pApplication;
	m_pDevice = pDevice;

	TVector3 position(0, 0, -5);
	TVector3 target(0.0f, 0.0f, 0.0f);
	m_pCamera = new TCamera(position, target, m_pApplication->GetWidth(), m_pApplication->GetHeight());
}


void TEngine::Run()
{
	m_pDevice->Clear();
	m_pDevice->BeginRender();

	m_pCamera->Update();
	//Render all the skin models
	for (int i = 0; i < m_skinModels.Size(); ++i) {
		TSkinModel * pSkinModel = m_skinModels[i];
		pSkinModel->Render();
	}

	m_pDevice->EndRender();
	m_pDevice->Present();
}

void TEngine::ShutDown()
{
	Release();
}


void TEngine::Release()
{
	if (m_pApplication != nullptr) {
		delete m_pApplication;
	}

	if (m_pDevice != nullptr) {
		delete m_pDevice;
	}

	if (m_pCamera != nullptr) {
		delete m_pCamera;
	}

	for (int i = 0; i < m_skinModels.Size(); ++i) {
		TSkinModel * pSkinModel = m_skinModels[i];
		TSkinModelMan::GetInstance()->ReleaseSkinModel(pSkinModel);
	}
}

