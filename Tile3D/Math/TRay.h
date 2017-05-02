#pragma once

#include "TVector3.h"
#include "TMatrix4.h"


class TPlane;
class TAABBBox;
class TOBBBox;
class TFrustum;
class TRay
{
public:
	TVector3 m_origin;
	TVector3 m_direction;

	static const int INTERSECTS_NONE = -1;

//ctor
public:
	TRay() {}
	
	TRay(TVector3 & origin, TVector3 & direction) : m_origin(origin), m_direction(direction){
	}

//method
public:
	const TVector3 & GetOrigin() const { return m_origin; }
	void SetOrigin(TVector3 & origin) { m_origin = origin; }

	const TVector3 & GetDirection() const { return m_direction;  }
	void SetDirection(TVector3 & direction) { m_direction = direction; }

	void Normalize() {
		m_direction.Normalize();
	}

	void Transform(const TMatrix4 & m) {
		m_origin = m_origin * m;

		TVector4 direction(m_direction.m_x, m_direction.m_y, m_direction.m_z, 0);
		direction = direction * m;

		m_direction = TVector3(direction.m_x, direction.m_y, direction.m_z);
		m_direction.Normalize();
	}

	/**
	* Tests whether this ray intersects the specified plane and returns the distance
	* from the origin of the ray to the plane.
	*
	* @param plane The plane to test intersection with.
	*
	* @return The distance from the origin of this ray to the plane or
	*     INTERSECTS_NONE if this ray does not intersect the plane.
	*/
	float Intersect(TPlane & plane);


	/**
	* Tests whether this ray intersects the specified bounding box.
	*
	* @param box The bounding box to test intersection with.
	*
	* @return The distance from the origin of this ray to the bounding object or
	*     INTERSECTS_NONE if this ray does not intersect the bounding object.
	*/
	float Intersect(TAABBBox & box);


	/**
	* Tests whether this ray intersects the specified bounding sphere.
	*
	* @param sphere The bounding sphere to test intersection with.
	*
	* @return The distance from the origin of this ray to the bounding object or
	*     INTERSECTS_NONE if this ray does not intersect the bounding object.
	*/
	float Intersect(TOBBBox & box);


	/**
	* Tests whether this ray intersects the specified frustum.
	*
	* @param frustum The frustum to test intersection with.
	*
	* @return The distance from the origin of this ray to the frustum or
	*     INTERSECTS_NONE if this ray does not intersect the frustum.
	*/
	float Intersect(TFrustum & frustum);
};

