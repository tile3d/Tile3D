#pragma once

#include "TDevice.h"
#include "TApplication.h"

#include <Render/TCamera.h>
#include <Container/TArray.h>

class TSkinModel;
class TEngine
{
public:
	static TEngine * GetInstance() {
		static TEngine engine;
		return &engine;
	}

	void Setup(TApplication * pApplication, TDevice * pDevice);

	void Run();
	void ShutDown();

	void Release();

	void AddSkinModel(TSkinModel * pModel) {
		m_skinModels.Add(pModel);
	}

	TDevice * GetDevice() { return m_pDevice; }
	TApplication * GetApplication() { return m_pApplication; }

private:
	TEngine() {}
	TEngine(TEngine & rhs) {}


private:
	TApplication * m_pApplication;
	TDevice * m_pDevice;
	TCamera * m_pCamera;
	TArray<TSkinModel*> m_skinModels;
};


