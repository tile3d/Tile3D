#pragma once

#include "TDevice.h"
#include "TApplication.h"
#include <Container/TArray.h>

class TSkinModel;
class TCamera;
class TInput;
class TEngine
{
public:
	enum TENGINE_DRIVER_TYPE
	{
		TENGINE_DRIVER_NONE,
		TENGINE_DRIVER_D3D9RIVER,
		TENGINE_DRIVER_D3D11DRIVER,
		TENGINE_DRIVER_OPENGLES
	};

	enum TENGINE_STATUS
	{
		TENGINE_STATUS_NONE,
		TENGINE_STATUS_RUNNING,
		TENGINE_STATUS_PAUSE,
		TENGINE_STATUS_SHUTDOWN
	};

	static TEngine * GetInstance() {
		static TEngine engine;
		return &engine;
	}

	void Init(TApplication * pApplication, TDevice * pDevice);
	void Run();
	void Pause();
	void Continue();
	void ShutDown();

	void Render();
	void Release();

	TDevice * GetDevice() { return m_pDevice; }
	TApplication * GetApplication() { return m_pApplication; }
	TCamera * GetCamera() { return m_pCamera; }
	TInput * GetInput() { return m_pInput; }

	void AddSkinModel(TSkinModel * pModel) {
		m_skinModels.Add(pModel);
	}
private:
	TEngine() {}
	TEngine(TEngine & rhs) {}

private:
	TApplication * m_pApplication;
	TDevice * m_pDevice;
	TCamera * m_pCamera;
	TInput * m_pInput;
	TArray<TSkinModel*> m_skinModels;
	TENGINE_STATUS m_status;
};


