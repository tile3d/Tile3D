#include "TestMathComon.h"

std::wstringstream& operator <<(std::wstringstream& s, const TVector3& t)
{
	s << "Vector3[" << t.m_x << "," << t.m_y << "," << t.m_z << "]";
	return s;
}


std::wstringstream& operator <<(std::wstringstream& s, const TVector4& t)
{
	s << "Vector4[" << t.m_x << "," << t.m_y << "," << t.m_z << "," << t.m_w << "]";
	return s;
}

std::wstringstream& operator <<(std::wstringstream& s, const TMatrix3 & t)
{
	s << "Matrix3[" << "(" << t.m_mat[0][0] << "," << t.m_mat[0][1] << "," << t.m_mat[0][2] << "),"
		<< "(" << t.m_mat[1][0] << "," << t.m_mat[1][1] << "," << t.m_mat[1][2] << "),"
		<< "(" << t.m_mat[2][0] << "," << t.m_mat[2][1] << "," << t.m_mat[2][2] << ")" << "]";
	return s;
}


std::wstringstream& operator <<(std::wstringstream& s, const TMatrix4 & t)
{
	s << "Matrix4[" << "(" << t.m_mat[0][0] << "," << t.m_mat[0][1] << "," << t.m_mat[0][2] <<  "," << t.m_mat[0][3] << "),"
		<< "(" << t.m_mat[1][0] << "," << t.m_mat[1][1] << "," << t.m_mat[1][2]  << "," << t.m_mat[1][3] << "),"
		<< "(" << t.m_mat[2][0] << "," << t.m_mat[2][1] << "," << t.m_mat[2][2]  << "," << t.m_mat[2][3] << ")"
		<< "(" << t.m_mat[3][0] << "," << t.m_mat[3][1] << "," << t.m_mat[3][2] << "," << t.m_mat[3][3] << ")]";

	return s;
}

std::wstringstream& operator <<(std::wstringstream& s, const TQuaternion & t)
{
	s << "Quaternion[" << t.m_x << "," << t.m_y << "," << t.m_z << "," << t.m_w << "]";

	return s;
}

std::wstringstream& operator <<(std::wstringstream& s, const TPlane & p)
{
	s << "Plane[" << p.m_normal.m_x << "," << p.m_normal.m_y  << "," << p.m_normal.m_z << "," << p.m_distance << "]";
	return s;
}

