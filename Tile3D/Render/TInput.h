#pragma once

//
//Support keyboard/mouse/joystick/gesture
//
class TInput
{
public:
	//keyboard
	virtual void OnKeyDown(int key);
	virtual void OnKeyUp(int key);

	//mouse
	virtual void OnMouseMove(int param, int x, int y);
	virtual void OnMouseLButtonDown(int param, int x, int y);
	virtual void OnMouseLButtonUp(int param, int x, int y);
	virtual void OnMouseRButtonDown(int param, int x, int y);
	virtual void OnMouseRButtonUp(int param, int x, int y);
	virtual void OnMouseWheel(int param, short delta);

	//TBD: joystick


	//TBD: gesture

private:
	bool m_rightMousePressed;	//if right mouse button pressed
	bool m_leftMousePressed; //if left mouse button pressed

	int m_lastMousePosX;
	int m_lastMousePosY;
};

