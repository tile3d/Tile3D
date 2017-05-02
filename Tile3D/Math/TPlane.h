#pragma once

#include "TVector3.h"
#include "TVector4.h"
#include "TMatrix4.h"

//
// p . m_normal = d (different from DirectX which is  p . m_normal + d = 0)
// ax+by+cz=d
//
class TPlane
{
public:
	TVector3 m_normal;	// plane normal
	float m_distance;   // from the origin to the plane

	enum Side
	{
		NO_SIDE,
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
		BOTH_SIDE
	};

//ctor
public:
	TPlane() {}
	
	TPlane(const TPlane & p) : m_normal(p.m_normal), m_distance(p.m_distance){}
	
	TPlane(TVector3 & normal, float d) : m_normal(normal), m_distance(d) {}
	
	TPlane(float normal_x, float normal_y, float normal_z, float d){
		m_normal = TVector3(normal_x, normal_y, normal_z);
		m_distance = d;
	}

	TPlane(TVector3 & pos1, TVector3 & pos2, TVector3 & pos3) {
		TVector3 v1 = pos2 - pos1;
		TVector3 v2 = pos3 - pos1;
		m_normal = v1.CrossProduct(v2);
		m_normal.Normalize();

		m_distance = pos1.DotProduct(m_normal);
	}

	TPlane(TVector3 & normal, TVector3 & point) {
		m_normal = normal;
		m_distance = point.DotProduct(m_normal);
	}


//operator
public:
	bool operator == (const TPlane & p) const {
		return m_normal == p.m_normal && m_distance == p.m_distance;
	}

	bool operator != (const TPlane & p) const {
		return m_normal != p.m_normal || m_distance != p.m_distance;
	}

	bool Equals(const TPlane & p) const {
		float tolerance = 0.0001f;
		return fabs(p.m_normal.m_x - m_normal.m_x) < tolerance &&  fabs(p.m_normal.m_y - m_normal.m_y) < tolerance 
			&& fabs(p.m_normal.m_z - m_normal.m_z) < tolerance && fabs(p.m_distance - m_distance) < tolerance;
	}

	TPlane operator+(const TPlane & p) const {
		return TPlane(p.m_normal + m_normal, p.m_distance + m_distance);
	}

	TPlane operator-(const TPlane & p) const {
		return TPlane(p.m_normal - m_normal, p.m_distance - m_distance);
	}

	TPlane operator*(float scale) const {
		return TPlane(m_normal*scale, m_distance*scale);
	}

	TPlane operator/(float scale) const {
		scale = 1.0f / scale;
		return TPlane(m_normal*scale, m_distance*scale);
	}

	TPlane operator * (const TPlane & p) {
		return TPlane(m_normal*p.m_normal, m_distance*p.m_distance);
		return *this;
	}

	TPlane & operator+=(const TPlane & p)  {
		m_normal += p.m_normal;
		m_distance += p.m_distance;
		return *this;
	}

	TPlane & operator-=(const TPlane & p)  {
		m_normal -= p.m_normal;
		m_distance -= p.m_distance;
		return *this;
	}

	TPlane & operator*=(float scale) {
		m_normal *= scale;
		m_distance *= scale;
		return *this;
	}

	TPlane & operator/=(float scale) {
		scale = 1.0f / scale;
		m_normal *= scale;
		m_distance *= scale;
		return *this;
	}

	TPlane& operator *= (const TPlane & p) {
		m_normal *= p.m_normal;
		m_distance *= p.m_distance;
		return *this;
	}

//method
public:
	TVector3 GetNormal() const { return m_normal; }

	float GetDistance() const { return m_distance; }

	//check if the point in the plane
	bool InPlane(const TVector3 & p) const {
		float d = p.DotProduct(m_normal);
		return fabs(d - m_distance) <= 0.0001f;
	}

	//flip the plane
	void Flip() {
		m_normal = -m_normal;
		m_distance = -m_distance;
	}

	//normalize the plane
	void Normalize() {
		float fLength = m_normal.Length();
		if (fLength > 0.0f) {
			float invLength = 1.0f / fLength;
			m_normal *= invLength;
			m_distance *= invLength;
		}
		return;
	}

	//Computes the dot product of a plane and a 4D vector.
	float DotProduct(const TVector4 & v) {
		return m_normal.m_x * v.m_x + m_normal.m_y * v.m_y + m_normal.m_z * v.m_z + m_distance * v.m_z;
	}

	//Computes the dot product of a plane and a 3D vector. The w parameter of the vector is assumed to be 1.
	float DotProductCoord(const TVector3 & v) {
		return m_normal.m_x * v.m_x + m_normal.m_y * v.m_y + m_normal.m_z * v.m_z + m_distance;
	}

	//Computes the dot product of a plane and a 3D vector. The w parameter of the vector is assumed to be 0.
	float DotProductNormal(const TVector3 & v) {
		return m_normal.m_x * v.m_x + m_normal.m_y * v.m_y + m_normal.m_z * v.m_z;
	}

	//computer the distance from the point to the plane
	float GetDistance(const TVector3 & p) const {
		return p.DotProduct(m_normal) - m_distance;
	}

	//get the side of the v point
	Side GetSide(const TVector3 & v) {
		float fDistance = GetDistance(v);

		if (fDistance < 0.0) {
			return TPlane::NEGATIVE_SIDE;
		}
		else if (fDistance > 0.0) {
			return TPlane::POSITIVE_SIDE;
		}
		return TPlane::NO_SIDE;
	}

	//if the line intersect with the plane
	bool IsIntersect(const TVector3 & v1, const TVector3 & v2) {
		return GetSide(v1) != GetSide(v2);
	}

	//Finds the intersection between a plane and a line.
	//v1: line starting point  v2: line ending point
	TVector3 Intersect(const TVector3 & v1, const TVector3 & v2) const {
		return v1 + (v2 - v1) * (m_distance - v1.DotProduct(m_normal)) / (v2 - v1).DotProduct(m_normal);
	}

	//Transform the plane by the Matrix
	void Transform(const TMatrix4 & m);

};

