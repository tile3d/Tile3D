#include "TCamera.h"
#include <Common/TTypes.h>
#include <Render/TEngine.h>
#include <Render/TDevice.h>

TCamera::TCamera(TVector3 & camera_pos, TVector3 & camera_target, int width, int height)
{
	m_pos = camera_pos;
	m_look = camera_target - camera_pos;
	m_look.Normalize();

	m_up = TVector3(0.0f, 1.0f, 0.0f);

	m_right = TVector3::CrossProduct(m_up, m_look);
	m_right.Normalize();

	m_zNear = 1.0f;
	m_zFar = 3000.0f;

	m_fovy = PI / 2.5f;
	m_aspect = (float)width / (float)height;

	m_needUpdate = false;

	m_totalPitch = 0;
	m_totalYaw = 0;
	m_totalRoll = 0;

	CalculateProjectionMatrix();
	CalculateViewArea();
}


void TCamera::Update()
{
	if (m_needUpdate) {
		CalculateProjectionMatrix();
		CalculateViewArea();
		m_needUpdate = false;
	}

	TDevice * pDevice = TEngine::GetInstance()->GetDevice();
	pDevice->SetTransform(TS_VIEW, m_viewMatrix);
	pDevice->SetTransform(TS_PROJECTION, m_projectionMatrix);
}

void TCamera::CalculateProjectionMatrix()
{
	m_viewMatrix.BuildCameraLookAtMatrix(m_pos, m_look, m_up);
	m_projectionMatrix.BuildPerspectiveMatrix(m_fovy, m_aspect, m_zNear, m_zFar);
}

void TCamera::CalculateViewArea()
{

}

TMatrix4 & TCamera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

TMatrix4 & TCamera::GetViewMatrix()
{
	return m_viewMatrix;
}

//Rotate on right vector (x)
void TCamera::Pitch(float angle)
{
	if (m_totalPitch + angle > PI / 2 || m_totalPitch + angle < -PI / 2) return;
	m_totalPitch += angle;

	TMatrix4 mt;
	mt.RotateAxis(m_right, angle);

	m_look = m_look*mt;
	m_up = m_up*mt;

	m_look.Normalize();
	m_up.Normalize();
	m_needUpdate = true;
}

//Rotate on up vector (y)
void TCamera::Yaw(float angle)
{
	if (m_totalYaw + angle > PI || m_totalYaw + angle < -PI) return;
	m_totalYaw += angle;

	TMatrix4 mt;
	mt.RotateAxis(m_up, angle);

	m_look = m_look*mt;
	m_right = m_right*mt;

	m_look.Normalize();
	m_right.Normalize();
	m_needUpdate = true;
}

//Rotate on look vector (z)
void TCamera::Roll(float angle)
{
	if (m_totalRoll + angle > PI / 2 || m_totalRoll + angle < -PI / 2) return;
	m_totalRoll += angle;

	TMatrix4 mt;
	mt.RotateAxis(m_look, angle);

	m_right = m_right*mt;
	m_up = m_up*mt;

	m_up.Normalize();
	m_right.Normalize();
	m_needUpdate = true;
}


//left and right (x)
void TCamera::Strafe(float units)
{
	m_pos += m_right * units;
	m_needUpdate = true;
}

//up and down (y)
void TCamera::Fly(float units)
{
	m_pos += m_up * units;
	m_needUpdate = true;
}

//forward and backward (z)
void TCamera::Walk(float units)
{
	m_pos += m_look * units;
	m_needUpdate = true;
}

void TCamera::Zoom(float units)
{
	if (m_fovy + units > PI / 2 || m_fovy + units < PI / 6) return;
	m_fovy += units;
	m_needUpdate = true;
}

