#pragma once

#include "TVector3.h"
#include "TMatrix4.h"

class TQuaternion
{
//variables
public:
	float m_x, m_y, m_z, m_w;

public:
	static const TQuaternion IDENTITY;
	static const float SLERP_EPSILON;

//ctor and dtor
public:
	TQuaternion() {}
	TQuaternion(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
	TQuaternion(float w, const TVector3 & u) : m_x(u.m_x), m_y(u.m_y), m_z(u.m_z), m_w(w) {}
	TQuaternion(const TQuaternion & q) : m_x(q.m_x), m_y(q.m_y), m_z(q.m_z), m_w(q.m_w) {}
	
	TQuaternion(const TMatrix4 & m) {
		ConvertFromMatrix(m);
	}

	TQuaternion(const TVector3 & v, float r) {
		ConvertFromAxisAngle(v, r);
	}

	TQuaternion(float pitch, float yaw, float roll) {
		ConvertFromEulerAngle(pitch, yaw, roll);
	}

//operation
public:
	//== and !=
	bool operator == (const TQuaternion & q) const;
	bool operator != (const TQuaternion & q) const;

	//+ and -
	TQuaternion operator + (const TQuaternion& q) const;
	TQuaternion operator - (const TQuaternion & q) const;
	TQuaternion & operator += (const TQuaternion & q);
	TQuaternion & operator -= (const TQuaternion & q);
	TQuaternion & operator - ();

	//*
	TQuaternion operator * (float f) const;
	TQuaternion operator * (int f) const;
	TQuaternion& operator *= (float f);
	TQuaternion& operator *= (int f);
	TQuaternion operator * (const TQuaternion & q) const;
	TQuaternion& operator *= (const TQuaternion &  q);

	//
	TQuaternion operator / (float f) const;
	TQuaternion operator / (int f) const;
	TQuaternion& operator /= (float f);
	TQuaternion& operator /= (int f);

	//TVector3
	TVector3 operator * (const TVector3 & v) const;
//	TVector3 operator ^ (const TVector3 & v) const;

	bool Equals(const TQuaternion & q) const
	{
		float tolerance = 0.0001f;
		return (fabs(m_x - q.m_x) < tolerance) && (fabs(m_y - q.m_y) < tolerance) && (fabs(m_z - q.m_z) < tolerance) && (fabs(m_w - q.m_w) < tolerance);
	}

//method
public:
	void Identity();
	
	void Clear();

	void Set(float x, float y, float z, float w);

	float SizeSquared() const
	{
		return m_x * m_x + m_y * m_y + m_z*m_z + m_w * m_w;
	}

	const TQuaternion& Normalize();
	bool IsNormalized();

	const TQuaternion& Inverse();

	const TQuaternion& Conjugate();

	float DotProduct(const TQuaternion & q) const;

	//Quaterion <--> Matrix
	void ConvertFromMatrix(const TMatrix4 & m);
	void ConvertToMatrix(TMatrix4 & m) const;

	//Quaterion <--> Axis
	void ConvertFromAxisAngle(const TVector3 & vecAxis, float vAngle);
	void ConvertToAxisAngle(TVector3& vecAxis, float& vAngle) const;
	
	//Quaterion <--> Euler angel
	void ConvertFromEulerAngle(float pitch, float yaw, float roll);
	void ConvertToEulerAngle(float & pitch, float & yaw, float & roll) const;


	/**
	* Interpolates between two quaternions using linear interpolation.
	*
	* The interpolation curve for linear interpolation between
	* quaternions gives a straight line in quaternion space.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param t The interpolation coefficient.
	* @param dst A quaternion to store the result in.
	*/
	static void Lerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst);

	/**
	* Interpolates between two quaternions using spherical linear interpolation.
	*
	* Spherical linear interpolation provides smooth transitions between different
	* orientations and is often useful for animating models or cameras in 3D.
	*
	* Note: For accurate interpolation, the input quaternions must be at (or close to) unit length.
	* This method does not automatically normalize the input quaternions, so it is up to the
	* caller to ensure they call normalize beforehand, if necessary.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param t The interpolation coefficient.
	* @param dst A quaternion to store the result in.
	*/
	static void Slerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst);

	/**
	* Interpolates over a series of quaternions using spherical spline interpolation.
	*
	* Spherical spline interpolation provides smooth transitions between different
	* orientations and is often useful for animating models or cameras in 3D.
	*
	* Note: For accurate interpolation, the input quaternions must be unit.
	* This method does not automatically normalize the input quaternions,
	* so it is up to the caller to ensure they call normalize beforehand, if necessary.
	*
	* @param q1 The first quaternion.
	* @param q2 The second quaternion.
	* @param s1 The first control point.
	* @param s2 The second control point.
	* @param t The interpolation coefficient.
	* @param dst A quaternion to store the result in.
	*/
	static void Squad(const TQuaternion& q1, const TQuaternion& q2, const TQuaternion& s1, const TQuaternion& s2, float t, TQuaternion* dst);


	///////////////////////////////////////////////////////////////////////////////////
	// Interpolate method.
	// 
	// A3DQUATERNION SLERPQuad(const A3DQUATERNION& q1, const A3DQUATERNION&q2, float fraction)
	//	calculate the interpolation of two quaternion using Spherical Linear Interpolation
	// algorithm.
	//
	// q1				IN				the start quaternion
	// q2				IN				the end quaternion
	// fraction			IN				the fraction between q1 and q2, 0.0 means q1 and 1.0 means q2
	//
	///////////////////////////////////////////////////////////////////////////////////
	static TQuaternion SLERPQuad(const TQuaternion& q1, const TQuaternion& q2, float fraction);


	//--------------------------------------------------------------------------------
	// compute the hypersphere-angle between two quaternions, and we can use it as the 
	// difference of these two quaternions.
	//
	// q1,q2 should be normalized
	// 
	// return the angle in unit of degree
	//
	static float GetDiffAngle(const TQuaternion& q1, const TQuaternion& q2);
private:
	/**
	* Interpolates between two quaternions using spherical linear interpolation.
	*
	* Spherical linear interpolation provides smooth transitions between different
	* orientations and is often useful for animating models or cameras in 3D.
	*
	* Note: For accurate interpolation, the input quaternions must be at (or close to) unit length.
	* This method does not automatically normalize the input quaternions, so it is up to the
	* caller to ensure they call normalize beforehand, if necessary.
	*
	* @param q1x The x component of the first quaternion.
	* @param q1y The y component of the first quaternion.
	* @param q1z The z component of the first quaternion.
	* @param q1w The w component of the first quaternion.
	* @param q2x The x component of the second quaternion.
	* @param q2y The y component of the second quaternion.
	* @param q2z The z component of the second quaternion.
	* @param q2w The w component of the second quaternion.
	* @param t The interpolation coefficient.
	* @param dstx A pointer to store the x component of the slerp in.
	* @param dsty A pointer to store the y component of the slerp in.
	* @param dstz A pointer to store the z component of the slerp in.
	* @param dstw A pointer to store the w component of the slerp in.
	*/
	static void Slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw);

	static void SlerpForSquad(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst);
};