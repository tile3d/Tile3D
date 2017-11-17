#include "TEngine.h"
#include <Render/SkinModel/TSkinModel.h>
#include <Render/SkinModel/TSkinModelMan.h>
#include "TCamera.h"
#include "TInput.h"
#include <Common/TLog.h>
#include <File/TFileDir.h>

bool TEngine::Init(TApplication * pApplication, TDevice * pDevice) {
	//Init the log 
	if (!TLog::Init("game.log", LOG_DEBUG, false)) {
		printf("init log file failed\n");
		return false;
	}


	if (!TFileDir::GetInstance()->Init("D:\\engine\\Tile3D\\Demo\\SkinMesh", "", "", "")) {
		TLog::Log(LOG_ERR, "Engine", " TEngine::Init,  failed to init the working dir");
		return false;
	}


	if (!TSkinModelMan::GetInstance()->LoadSkinMeshShader()) {
		TLog::Log(LOG_ERR, "Engine", " TEngine::Init,  failed to load the skin mesh shaders");
		return false;
	}

	m_pApplication = pApplication;
	m_pDevice = pDevice;

	//create the game carmera
	TVector3 position(0, 0, -5);
	TVector3 target(0.0f, 0.0f, 0.0f);
	m_pCamera = new TCamera(position, target, m_pApplication->GetWidth(), m_pApplication->GetHeight());

	//create the input to handle the input 
	m_pInput = new TInput();

	m_status = TENGINE_STATUS_RUNNING;
	return true;
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

	TSkinModelMan::GetInstance()->Render();

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

	TSkinModelMan::GetInstance()->ReleaseSkinModel();
}

