#include "TMatrix3.h"

//	* operator
TVector3 operator * (const TVector3& v, const TMatrix3& mat)
{
	return TVector3(v.m_x * mat.m_mat[0][0] + v.m_y * mat.m_mat[1][0] + v.m_z * mat.m_mat[2][0],
		v.m_x * mat.m_mat[0][1] + v.m_y * mat.m_mat[1][1] + v.m_z * mat.m_mat[2][1],
		v.m_x * mat.m_mat[0][2] + v.m_y * mat.m_mat[1][2] + v.m_z * mat.m_mat[2][2]);
}

TVector3 operator * (const TMatrix3& mat, const TVector3& v)
{
	return v*mat;
}


//	Clear all elements to zero
void TMatrix3::Clear()
{
	m_mat[0][0] = m_mat[0][1] = m_mat[0][2] = 0.0f;
	m_mat[1][0] = m_mat[1][1] = m_mat[1][2] = 0.0f;
	m_mat[2][0] = m_mat[2][1] = m_mat[2][2] = 0.0f;
}

//	Set matrix to identity matrix
void TMatrix3::Identity()
{
	m_mat[0][1] = m_mat[0][2] = 0.0f;
	m_mat[1][0] = m_mat[1][2] = 0.0f;
	m_mat[2][0] = m_mat[2][1] = 0.0f;
	m_mat[0][0] = m_mat[1][1] = m_mat[2][2] = 1.0f;
}


void TMatrix3::Translate(float x, float y)
{
	Identity();
	m_mat[2][0] = x;
	m_mat[2][1] = y;
}

void TMatrix3::Rotate(float fRad)
{
	Identity();
	m_mat[1][1] = m_mat[0][0] = (float)cos(fRad);
	m_mat[0][1] = (float)sin(fRad);
	m_mat[1][0] = -m_mat[0][1];
}


float TMatrix3::Determinant()
{
	float cofactor00 = m_mat[1][1] * m_mat[2][2] - m_mat[1][2] * m_mat[2][1];
	float cofactor10 = m_mat[1][2] * m_mat[2][0] - m_mat[1][0] * m_mat[2][2];
	float cofactor20 = m_mat[1][0] * m_mat[2][1] - m_mat[1][1] * m_mat[2][0];

	return m_mat[0][0] * cofactor00 + m_mat[0][1] * cofactor10 + m_mat[0][2] * cofactor20;
}


void TMatrix3::InverseTM()
{
	float matInverse[3][3];

	matInverse[0][0] = m_mat[1][1] * m_mat[2][2] -
		m_mat[1][2] * m_mat[2][1];
	matInverse[0][1] = m_mat[0][2] * m_mat[2][1] -
		m_mat[0][1] * m_mat[2][2];
	matInverse[0][2] = m_mat[0][1] * m_mat[1][2] -
		m_mat[0][2] * m_mat[1][1];
	matInverse[1][0] = m_mat[1][2] * m_mat[2][0] -
		m_mat[1][0] * m_mat[2][2];
	matInverse[1][1] = m_mat[0][0] * m_mat[2][2] -
		m_mat[0][2] * m_mat[2][0];
	matInverse[1][2] = m_mat[0][2] * m_mat[1][0] -
		m_mat[0][0] * m_mat[1][2];
	matInverse[2][0] = m_mat[1][0] * m_mat[2][1] -
		m_mat[1][1] * m_mat[2][0];
	matInverse[2][1] = m_mat[0][1] * m_mat[2][0] -
		m_mat[0][0] * m_mat[2][1];
	matInverse[2][2] = m_mat[0][0] * m_mat[1][1] -
		m_mat[0][1] * m_mat[1][0];

	float fDet = Determinant();
	float fInvDet = 1.0f / fDet;
	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
			m_mat[iRow][iCol] = matInverse[iRow][iCol] * fInvDet;
	}
}

