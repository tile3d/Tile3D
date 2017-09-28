#include "TInput.h"
#include "TEngine.h"
#include "TCamera.h"

void TInput::OnKeyDown(int key)
{
	TCamera * pCamera = TEngine::GetInstance()->GetCamera();
	float units = 0.1f;
	switch (key)
	{
		case 'A':
		{
			pCamera->Strafe(-units);
		}
		break;

		case 'D':
		{
			pCamera->Strafe(units);
		}
		break;

		case 'W':
		{
			pCamera->Walk(units);
		}
		break;

		case 'S':
		{
			pCamera->Walk(-units);
		}
		break;

		case 'Q':
		{
			pCamera->Fly(units);
		}
		break;

		case 'E':
		{
			pCamera->Fly(-units);

		}
		break;

		case 'J':
		{
			pCamera->Yaw(units);

		}
		break;

		case 'L':
		{
			pCamera->Yaw(-units);

		}
		break;

		case 'K':
		{
			pCamera->Pitch(units);

		}
		break;

		case 'I':
		{
			pCamera->Pitch(-units);

		}
		break;


		default:
		{
		}
		break;
	}
}

void TInput::OnKeyUp(int key)
{
}

void TInput::OnMouseMove(int param, int x, int y)
{
	if (m_leftMousePressed)
	{
		TCamera * pCamera = TEngine::GetInstance()->GetCamera();
		//	Rotate camera
		float f1 = 0.01f;
		if (x - m_lastMousePosX)
		{
			float fYaw = (x - m_lastMousePosX) * f1;
			pCamera->Yaw(fYaw);
		}

		if (y - m_lastMousePosY)
		{
			float fPitch = (y - m_lastMousePosY) * f1;
			pCamera->Pitch(fPitch);
		}
	}

	m_lastMousePosX = x;
	m_lastMousePosY = y;
}

void TInput::OnMouseLButtonDown(int param, int x, int y)
{
	m_leftMousePressed = true;

	m_lastMousePosX = x;
	m_lastMousePosY = y;
}



void TInput::OnMouseLButtonUp(int param, int x, int y)
{
	if (!m_leftMousePressed) return;
	m_leftMousePressed = false;
}

void TInput::OnMouseRButtonDown(int param, int x, int y)
{
	m_rightMousePressed = true;

	m_lastMousePosX = x;
	m_lastMousePosY = y;
}


void TInput::OnMouseRButtonUp(int param, int x, int y)
{
	if (!m_rightMousePressed) return;
	m_rightMousePressed = false;
}

void TInput::OnMouseWheel(int param, short delta)
{
	TCamera * pCamera = TEngine::GetInstance()->GetCamera();
	float units = 0.1f;
	if (delta > 0) {
		pCamera->Zoom(units);
	}
	else {
		pCamera->Zoom(-units);
	}
}
