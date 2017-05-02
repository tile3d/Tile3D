#include "TPlane.h"


void TPlane::Transform(const TMatrix4 & m)
{
	TMatrix4 inverted = m.GetInverseTM();

	// Treat the plane as a four-tuple and multiply by the inverse transpose of the matrix to get the transformed plane.
	// Then we normalize the plane by dividing both the normal and the distance by the length of the normal.
	float nx = m_normal.m_x * inverted.m_mat[0][0] + m_normal.m_y * inverted.m_mat[0][1] + m_normal.m_z * inverted.m_mat[0][2] + m_distance * inverted.m_mat[0][3];
	float ny = m_normal.m_x * inverted.m_mat[1][0] + m_normal.m_y * inverted.m_mat[1][1] + m_normal.m_z * inverted.m_mat[1][2] + m_distance * inverted.m_mat[1][3];
	float nz = m_normal.m_x * inverted.m_mat[2][0] + m_normal.m_y * inverted.m_mat[2][1] + m_normal.m_z * inverted.m_mat[2][2] + m_distance * inverted.m_mat[2][3];
	float d =  m_normal.m_x * inverted.m_mat[3][0] + m_normal.m_y * inverted.m_mat[3][1] + m_normal.m_z * inverted.m_mat[3][2] + m_distance * inverted.m_mat[3][3];
	float divisor = (float)sqrt(nx * nx + ny * ny + nz * nz);
	float factor = 1.0f / divisor;

	m_normal.m_x = nx * factor;
	m_normal.m_y = ny * factor;
	m_normal.m_z = nz * factor;
	m_distance = d * factor;
}

