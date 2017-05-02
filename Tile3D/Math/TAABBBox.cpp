#include "TAABBBox.h"
#include <algorithm>

void TAABBBox::AddVertex(const TVector3 & v)
{
	if (v.m_x < m_min.m_x) {
		m_min.m_x = v.m_x;
	}
	else if (v.m_x > m_max.m_x) {
		m_max.m_x = v.m_x;
	}

	if (v.m_y < m_min.m_y) {
		m_min.m_y = v.m_y;
	}
	else if (v.m_y > m_max.m_y) {
		m_max.m_y = v.m_y;
	}

	if (v.m_z < m_min.m_z) {
		m_min.m_z = v.m_z;
	}
	else if (v.m_z > m_max.m_z) {
		m_max.m_z = v.m_z;
	}
}

void TAABBBox::Merge(const TAABBBox & other)
{
	m_min.m_x = std::min(m_min.m_x, other.m_min.m_x);
	m_min.m_y = std::min(m_min.m_y, other.m_min.m_y);
	m_min.m_z = std::min(m_min.m_z, other.m_min.m_z);

	m_max.m_x = std::min(m_max.m_x, other.m_max.m_x);
	m_max.m_y = std::min(m_max.m_y, other.m_max.m_y);
	m_max.m_z = std::min(m_max.m_z, other.m_max.m_z);
}

