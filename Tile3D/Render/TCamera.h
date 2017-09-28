#pragma once

#include <Math/TVector3.h>
#include <Math/TMatrix4.h>

class TCamera
{
public:
	//ctor
	TCamera(TVector3 & camera_pos, TVector3 & camera_target, int width, int height);

	//update the view and projection matrix
	void Update();

	//Rotate on right vector(x axis)
	void Pitch(float angle);

	//Rotate on up vector(y axis)
	void Yaw(float angle);

	//Rotate on look vector(z axis)
	void Roll(float angle);

	//left/right (x axis)
	void Strafe(float units);

	//up down (y axis)
	void Fly(float units);

	//forward/backward(z axis)
	void Walk(float units);

	//zoom in or out
	void Zoom(float units);

	TMatrix4 & GetProjectionMatrix();
	TMatrix4 & GetViewMatrix();

private:
	void CalculateProjectionMatrix();

	//update the frustum
	void CalculateViewArea();

private:
	TVector3 m_pos;			//camera pos (translation)
	TVector3 m_look;		//camera look direction (aim to target)
	TVector3 m_up;			//camera up direction, usually start (0, 1, 0)
	TVector3 m_right;		//camera right direction

	float m_fovy;			//field of view
	float m_aspect;			//width/height aspection
	float m_zNear;			//near surface
	float m_zFar;			//far surface

	float m_totalPitch;		//record total pitch value
	float m_totalYaw;		//record total yaw value 
	float m_totalRoll;		//record total roll value

	bool m_needUpdate;		

	TMatrix4 m_viewMatrix;			//World -> View Matrix
	TMatrix4 m_projectionMatrix;	//Projection View Matrix

};
