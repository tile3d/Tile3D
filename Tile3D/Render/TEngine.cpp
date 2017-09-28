#include "TEngine.h"
#include <Render/SkinModel/TSkinModel.h>
#include <Render/SkinModel/TSkinModelMan.h>
#include "TCamera.h"
#include "TInput.h"

void TEngine::Init(TApplication * pApplication, TDevice * pDevice) {

	m_pApplication = pApplication;
	m_pDevice = pDevice;

	//create the game carmera
	TVector3 position(0, 0, -5);
	TVector3 target(0.0f, 0.0f, 0.0f);
	m_pCamera = new TCamera(position, target, m_pApplication->GetWidth(), m_pApplication->GetHeight());

	//create the input to handle the input 
	m_pInput = new TInput();

	m_status = TENGINE_STATUS_RUNNING;
}


void TEngine::Run()
{
	if (m_status != TENGINE_STATUS_RUNNING) return;

	//Render
	Render();
}

void TEngine::Render()
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

void TEngine::Pause()
{
	m_status = TENGINE_STATUS_PAUSE;
}

void TEngine::Continue()
{
	m_status = TENGINE_STATUS_RUNNING;
}

void TEngine::ShutDown()
{
	if (m_status == TENGINE_STATUS_SHUTDOWN) return;

	m_status = TENGINE_STATUS_SHUTDOWN;
	m_pApplication->Close();
	m_pDevice->Close();

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

	if (m_pInput != nullptr) {
		delete m_pInput;
	}

	for (int i = 0; i < m_skinModels.Size(); ++i) {
		TSkinModel * pSkinModel = m_skinModels[i];
		TSkinModelMan::GetInstance()->ReleaseSkinModel(pSkinModel);
	}
}

