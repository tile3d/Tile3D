#include "TRay.h"
#include "TPlane.h"

float TRay::Intersect(TPlane & plane)
{
	float denom = plane.m_normal.DotProduct(m_direction);
	if (fabs(denom) < 0.0001f)
	{
		// Parallel
		return (float)INTERSECTS_NONE;
	}
	else
	{
		float nom = plane.m_normal.DotProduct(m_origin) + plane.m_distance;
		float t = -(nom / denom);

		if (t < 0.0f) {
			return (float)INTERSECTS_NONE;
		}
		else {
			return t;
		}
	}
}
