#pragma once

#include <math.h>
#include "TVector3.h"
#include "TQuaternion.h"


class TRotator
{
public:
	// Rotation around the right axis (around X axis)
	float m_pitch;

	// Rotation around the up axis (around Y axis)
	float m_yaw;

	// Rotation around the forward axis (around Z axis)
	float m_roll;

public:
	static const TRotator ZeroRotator;


// ctor
public:
	TRotator() {}
	TRotator(float pitch, float yaw, float roll) : m_pitch(pitch), m_yaw(yaw), m_roll(roll) {}


// operation
public:
	// + and -
	TRotator operator+(const TRotator & r) const;
	TRotator operator-(const TRotator & r) const;
	TRotator&  operator+=(const TRotator & r);
	TRotator&  operator-=(const TRotator & r);


	TRotator operator * (float scale) const;
	TRotator& operator *= (float scale);

	// == and !=
	bool operator == (const TRotator & r) const
	{
		return m_pitch == r.m_pitch && m_yaw == r.m_yaw && m_roll == r.m_roll;
	}

	bool operator != (const TRotator & r) const
	{
		return m_pitch != r.m_pitch || m_yaw == r.m_yaw || m_roll == r.m_roll;
	}

	bool Equals(const TRotator & r) const
	{
		float bTolerance = 0.001f;
		return fabs(m_pitch - r.m_pitch) <= bTolerance && fabs(m_yaw - r.m_yaw) <= bTolerance && fabs(m_roll - r.m_roll) <= bTolerance;
	}


public:
	TVector3 Euler() const
	{
		return TVector3(m_pitch, m_yaw, m_roll);
	}

	TQuaternion GetQuaternion() const;

	TMatrix4 GetRotationMatrix() const;

};

