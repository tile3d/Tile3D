#pragma once

#include "TVector3.h"

class TPlane;
class TFrustum;
class TOBBBox;
//Axis-aligned bounding box
class TAABBBox
{
public:
	TVector3 m_min;
	TVector3 m_max;

//ctor
public:
	TAABBBox() {}

	TAABBBox(const TVector3 & min, const TVector3 & max) {
		this->m_min = min;
		this->m_max = max;
	}

//operation
public:
	bool operator==(const TAABBBox & other) const {
		return m_min == other.m_min && m_max == other.m_max;
	}

	TAABBBox& operator+=(const TVector3 & other) {
		AddVertex(other);
	}


//method
public:
	TVector3 GetCenter() const {
		return TVector3(m_min + m_max) * 0.5f;
	}

	TVector3 GetExtent() const {
		return (m_max - m_min) * 0.5f;;
	}

	TVector3 GetSize() const {
		return m_max - m_min;
	}

	float GetVolume() const {
		return ((m_max.m_x - m_min.m_x) * (m_max.m_y - m_min.m_y) * (m_max.m_z - m_min.m_z));
	}


	bool IsInside(const TVector3 & in) const {
		return ((in.m_x > m_min.m_x) && (in.m_x < m_max.m_x) && (in.m_y > m_min.m_y) && (in.m_y < m_max.m_y) && (in.m_z > m_min.m_z) && (in.m_z < m_max.m_z));
	}


	bool IsInside(const TAABBBox & box) const {
		return IsInside(box.m_min) && IsInside(box.m_max);
	}


	void AddVertex(const TVector3 & v);

	void Merge(const TAABBBox & other);

	bool Intersect(const TAABBBox & other) const;

	float Intersect(const TPlane & plane) const;

	bool Intersect(const TFrustum & frustum) const;

	bool Intersect(const TOBBBox & box) const;
};
