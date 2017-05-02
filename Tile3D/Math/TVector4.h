/*
* FILE: TVector4.h
*
* DESCRIPTION: TVector4 class
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

class TVector4
{
//	Attributes
public:
	float m_x, m_y, m_z, m_w;

//	Constructions and Destructions
public:
	TVector4() {}
	TVector4(const TVector4& v) : m_x(v.m_x), m_y(v.m_y), m_z(v.m_z), m_w(v.m_w) {}
	const TVector4& operator = (const TVector4& v) { m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; m_w = v.m_w;  return *this; }

	explicit TVector4(float m) : m_x(m), m_y(m), m_z(m), m_w(m) {}
	explicit TVector4(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

//	Operaitons
public:
	//	+ operator
	TVector4 operator + (const TVector4& v) const { return TVector4(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z, m_w + v.m_w); }
	//	- operator
	TVector4 operator - (const TVector4& v) const { return TVector4(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z, m_w + v.m_w); }

	//	* operator
	TVector4 operator * (float f) const { return TVector4(m_x*f, m_y*f, m_z*f, m_w*f); }
	TVector4 operator * (int f) const { return (*this) * (float)f; }
	TVector4 operator * (const TVector4& v)  const { return TVector4(m_x*v.m_x, m_y*v.m_y, m_z*v.m_z, m_w*v.m_w); }

	//	/ operator
	TVector4 operator / (float f) { f = 1.0f / f; return TVector4(m_x*f, m_y*f, m_z*f, m_w*f); }
	TVector4 operator / (int f) { return (*this) / (float)f; }
	TVector4 operator / (const TVector4& v) { return TVector4(m_x / v.m_x, m_y / v.m_y, m_z / v.m_z, m_w / v.m_w); }

	//	== operator
	bool operator == (const TVector4& v) const{ return m_x == v.m_x && m_y == v.m_y && m_z == v.m_z && m_w == v.m_w; }
	//	!= operator
	bool operator != (const TVector4& v) const { return m_x != v.m_x || m_y != v.m_y || m_z != v.m_z || m_w != v.m_w; }

	//	+= operator
	const TVector4& operator += (const TVector4& v) { m_x += v.m_x; m_y += v.m_y; m_z += v.m_z; m_w += v.m_w; return *this; }
	//	-= operator
	const TVector4& operator -= (const TVector4& v) { m_x -= v.m_x; m_y -= v.m_y; m_z -= v.m_z; m_w -= v.m_w; return *this; }
	//	*= operator
	const TVector4& operator *= (float f) { m_x *= f; m_y *= f; m_z *= f; m_w *= f; return *this; }
	//	/= operator
	const TVector4& operator /= (float f) { f = 1.0f / f; m_x *= f; m_y *= f; m_z *= f; m_w *= f; return *this; }

	TVector4 operator + () const { return *this; }
	TVector4 operator - () const { return TVector4(-m_x, -m_y, -m_z, -m_w); }

//	Methods
public:
	void Set(float x, float y, float z, float w) { m_x = x; m_y = y; m_z = z; m_w = w; }
	void Clear() { m_x = m_y = m_z = m_w = 0.0f; }
	bool IsZero() const { return m_x == 0.0f && m_y == 0.0f && m_z == 0.0f && m_w == 0.0f; }

	//	Length
	float Length() const { return (float)(sqrt(m_x * m_x + m_y * m_y + m_z * m_z)); }
	float LengthH() const { return (float)(sqrt(m_x * m_x + m_z * m_z)); }
	float SquaredLength() const { return m_x * m_x + m_y * m_y + m_z * m_z; }
	float SquaredLengthH() const { return m_x * m_x + m_z * m_z; }

	//	Normalize
	TVector4 & Normalize()
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
	static float Normalize(const TVector4& vIn, TVector4& vOut)
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
	float DotProduct(const TVector4& v) const
	{
		return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
	}

	//	Cross product
	TVector4 CrossProduct(const TVector4& v)
	{
		return TVector4(m_y * v.m_z - m_z * v.m_y, m_z * v.m_x - m_x * v.m_z, m_x * v.m_y - m_y * v.m_x, 0.0f);
	}

	static TVector4 CrossProduct(const TVector4 & v1, const TVector4& v2)
	{
		return TVector4(v1.m_y * v2.m_z - v1.m_z * v2.m_y,
			v1.m_z * v2.m_x - v1.m_x * v2.m_z,
			v1.m_x * v2.m_y - v1.m_y * v2.m_x, 0.0f);
	}

	// Lerp method.
	static TVector4 Lerp(const TVector4& v1, const TVector4& v2, float fraction)
	{
		return v1 * (1.0f - fraction) + v2 * fraction;
	}

	TVector4 GetInterpolated(const TVector4 v, float d) const
	{
		const float inv = (float)1.0 - d;
		return TVector4((v.m_x*inv + m_x*d), (v.m_y*inv + m_y*d), (v.m_z*inv + m_z*d), (v.m_w*inv + m_w*d));
	}

	// Project on v
	TVector4 Project(const TVector4 & v)
	{
		float d = DotProduct(v);
		float f = v.DotProduct(v);
		return v * (d / f);
	}


	//	Perp
	TVector4 Perp(const TVector4 & v)
	{
		return (*this) - Project(v);
	}

	// Area
	static float Area(const TVector4 & v1, const TVector4 & v2, const TVector4 & v3)
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

