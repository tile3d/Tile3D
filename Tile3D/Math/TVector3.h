/*
 * FILE: TVector3.h
 *
 * DESCRIPTION: TVector3 class
 *
 * CREATED BY: liansm
 *
 * HISTORY:
 *
 * Copyright (c) 2017 Tile3D Ltd, All Rights Reserved.
 */

#pragma once

#include <math.h>

//TBD:
// 1) support move sematic
// 2) support SSE instruction
// 3) support archive and serialize
// 4) implement ToString

class TVector3
{
//	Attributes
public:		
	float m_x, m_y, m_z;

public:
	static const TVector3 ZeroVector;
	static const TVector3 UpVector;
	static const TVector3 ForwardVector;
	static const TVector3 RightVector;

//	Constructions and Destructions
public:		
	TVector3() {}
	TVector3(const TVector3& v) : m_x(v.m_x), m_y(v.m_y), m_z(v.m_z) {}
	const TVector3& operator = (const TVector3& v) { m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; return *this; }

	explicit TVector3(float m) : m_x(m), m_y(m), m_z(m) {}
	explicit TVector3(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

//	Operaitons
public:		
	//	+ operator
	TVector3 operator + (const TVector3& v) const{ return TVector3(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z); }
	//	- operator
	TVector3 operator - (const TVector3& v) const{ return TVector3(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z); }

	//	* operator
	TVector3 operator * (float f) const { return TVector3(m_x*f, m_y*f, m_z*f); }
	TVector3 operator * (int f) const { return (*this) * (float)f; }
    TVector3 operator * (const TVector3& v)  const{ return TVector3(m_x*v.m_x, m_y*v.m_y, m_z*v.m_z); }

	//	/ operator
	TVector3 operator / (float f) { f = 1.0f / f; return TVector3(m_x*f, m_y*f, m_z*f); }
	TVector3 operator / (int f) { return (*this) / (float)f; }
	TVector3 operator / (const TVector3& v) { return TVector3(m_x/v.m_x, m_y/v.m_y, m_z/v.m_z); }

	//	== operator
	bool operator == (const TVector3& v) const { return m_x == v.m_x && m_y == v.m_y && m_z == v.m_z; }
	//	!= operator
	bool operator != (const TVector3& v) const { return m_x != v.m_x || m_y != v.m_y || m_z != v.m_z; }

	//	+= operator
	const TVector3& operator += (const TVector3& v) { m_x += v.m_x; m_y += v.m_y; m_z += v.m_z; return *this; }
	//	-= operator
	const TVector3& operator -= (const TVector3& v) { m_x -= v.m_x; m_y -= v.m_y; m_z -= v.m_z; return *this; }
	//	*= operator
	const TVector3& operator *= (float f) { m_x *= f; m_y *= f; m_z *= f; return *this; }
	//	/= operator
	const TVector3& operator /= (float f) { f = 1.0f / f; m_x *= f; m_y *= f; m_z *= f; return *this; }

	const TVector3& operator *= (const TVector3 & v) { m_x *= v.m_x; m_y *= v.m_y; m_z *= v.m_z; return *this; }

	TVector3 operator + () const { return *this; }
	TVector3 operator - () const { return TVector3(-m_x, -m_y, -m_z); }

//	Methods
public:
	void Set(float x, float y, float z) { m_x = x; m_y = y; m_z = z; }
	void Clear() { m_x = m_y = m_z = 0.0f; }
	bool IsZero() const { return m_x == 0.0f && m_y == 0.0f && m_z == 0.0f; }

	//	Length
	float Length() const { return (float)(sqrt(m_x * m_x + m_y * m_y + m_z * m_z)); }
	float LengthH() const { return (float)(sqrt(m_x * m_x + m_z * m_z)); }
	float SquaredLength() const { return m_x * m_x + m_y * m_y + m_z * m_z; }
	float SquaredLengthH() const { return m_x * m_x + m_z * m_z; }

	//	Normalize
	TVector3 & Normalize()
	{
		float f = Length();
		if (f < 1e-6 && f > -1e-6) {
			Clear();
			return *this;
		}

		(*this) *= 1.0f / f;
		return *this;

	}

	//	Get normalize
	static float Normalize(const TVector3& vIn, TVector3& vOut)
	{
		float fMag = vIn.Length();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			vOut.Clear();
			fMag = 0.0f;
		}
		else
		{
			float f = 1.0f / fMag;
			vOut = vIn * f;
		}

		return fMag;
	}
	//	Dot product
	float DotProduct(const TVector3& v) const
	{
		return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
	}

	//	Cross product
	TVector3 CrossProduct(const TVector3& v) const
	{
		return TVector3(m_y * v.m_z - m_z * v.m_y, m_z * v.m_x - m_x * v.m_z, m_x * v.m_y - m_y * v.m_x);
	}

	static TVector3 CrossProduct(const TVector3 & v1, const TVector3& v2)
	{
		return TVector3(v1.m_y * v2.m_z - v1.m_z * v2.m_y, 
						  v1.m_z * v2.m_x - v1.m_x * v2.m_z,
						  v1.m_x * v2.m_y - v1.m_y * v2.m_x);
	}




	// Lerp method.
	static TVector3 Lerp(const TVector3& v1, const TVector3& v2, float fraction)
	{
		return v1 * (1.0f - fraction) + v2 * fraction;
	}

	TVector3 GetInterpolated(const TVector3 v, float d) const
	{
		const float inv = (float)1.0 - d;
		return TVector3((v.m_x*inv + m_x*d), (v.m_y*inv + m_y*d), (v.m_z*inv + m_z*d));
	}

	float GetCosAngel2D(TVector3 v1) const
	{
		TVector3 v2(*this);
		v1.m_z = 0;
		v2.m_z = 0;
		v1.Normalize();
		v2.Normalize();
		return v1.DotProduct(v2);
	}

	// Project on v
	TVector3 Project(const TVector3 & v) const
	{
		float d = DotProduct(v);
		float f = v.DotProduct(v);
		return v * (d/f);
	}

	// Perpendicular
	TVector3 Perp(const TVector3 & v)
	{
		return (*this) - Project(v);
	}

	// Area
	static float Area(const TVector3 & v1, const TVector3 & v2, const TVector3 & v3)
	{
		return 0.5f * (CrossProduct(v2 - v1, v3 - v1)).Length();
	}

	//	Get minimum number
	float MinMember() const
	{ 
		if (m_x < m_y)
			return m_x < m_z ? m_x : m_z;
		else
			return m_y < m_z ? m_y : m_z;
	}

	//	Get maximum member
	float MaxMember() const
	{ 
		if (m_x > m_y)
			return m_x > m_z ? m_x : m_z;
		else
			return m_y > m_z ? m_y : m_z;
	}	
};

