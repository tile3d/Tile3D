#include "TRotator.h"

const TRotator TRotator::ZeroRotator(0.f, 0.f, 0.f);

TRotator TRotator::operator+(const TRotator & r) const
{
	TRotator ret;
	ret.m_pitch = m_pitch + r.m_pitch;
	ret.m_yaw = m_yaw + r.m_yaw;
	ret.m_roll = m_roll + r.m_roll;
	return ret;
}


TRotator TRotator::operator-(const TRotator & r) const
{
	TRotator ret;
	ret.m_pitch = m_pitch - r.m_pitch;
	ret.m_yaw = m_yaw - r.m_yaw;
	ret.m_roll = m_roll - r.m_roll;
	return ret;
}


TRotator TRotator::operator * (float scale) const
{
	TRotator ret;
	ret.m_pitch = m_pitch * scale;
	ret.m_yaw = m_yaw * scale;
	ret.m_roll = m_roll * scale;
	return ret;
}

TRotator& TRotator::operator *= (float scale)
{
	m_pitch *= scale;
	m_yaw *= scale;
	m_roll *= scale;
	return *this;
}

TRotator& TRotator::operator += (const TRotator & r)
{
	m_pitch += r.m_pitch;
	m_yaw += r.m_yaw;
	m_roll += r.m_roll;
	return *this;
}

TRotator& TRotator::operator -= (const TRotator & r)
{
	m_pitch -= r.m_pitch;
	m_yaw -= r.m_yaw;
	m_roll -= r.m_roll;
	return *this;
}


TQuaternion TRotator::GetQuaternion() const
{
	TQuaternion q;
	q.ConvertFromEulerAngle(m_pitch, m_yaw, m_roll);
	return q;
}

TMatrix4 TRotator::GetRotationMatrix() const
{
	TMatrix4 m;

	m.RotateZ(m_roll);
	m.RotateX(m_pitch);
	m.RotateY(m_yaw);

	return m;
}

