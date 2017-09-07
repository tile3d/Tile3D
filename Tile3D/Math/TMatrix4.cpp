#include "TMatrix4.h"


TMatrix4 TMatrix4::operator+(const TMatrix4 & t) const
{
	TMatrix4 o;
	o.m_mat[0][0] = m_mat[0][0] + t.m_mat[0][0]; o.m_mat[0][1] = m_mat[0][1] + t.m_mat[0][1]; 
	o.m_mat[0][2] = m_mat[0][2] + t.m_mat[0][2]; o.m_mat[0][3] = m_mat[0][3] + t.m_mat[0][3];

	o.m_mat[1][0] = m_mat[1][0] + t.m_mat[1][0]; o.m_mat[1][1] = m_mat[1][1] + t.m_mat[1][1]; 
	o.m_mat[1][2] = m_mat[1][2] + t.m_mat[1][2]; o.m_mat[1][3] = m_mat[1][3] + t.m_mat[1][3];
	
	o.m_mat[2][0] = m_mat[2][0] + t.m_mat[2][0]; o.m_mat[2][1] = m_mat[2][1] + t.m_mat[2][1]; 
	o.m_mat[2][2] = m_mat[2][2] + t.m_mat[2][2]; o.m_mat[2][3] = m_mat[2][3] + t.m_mat[2][3];
	
	o.m_mat[3][0] = m_mat[3][0] + t.m_mat[3][0]; o.m_mat[3][1] = m_mat[3][1] + t.m_mat[3][1]; 
	o.m_mat[3][2] = m_mat[3][2] + t.m_mat[3][2]; o.m_mat[3][3] = m_mat[3][3] + t.m_mat[3][3];
	return o;
}

TMatrix4 TMatrix4::operator-(const TMatrix4 & t) const
{
	TMatrix4 o;
	o.m_mat[0][0] = m_mat[0][0] - t.m_mat[0][0]; o.m_mat[0][1] = m_mat[0][1] - t.m_mat[0][1]; 
	o.m_mat[0][2] = m_mat[0][2] - t.m_mat[0][2];  o.m_mat[0][3] = m_mat[0][3] - t.m_mat[0][3];
	
	o.m_mat[1][0] = m_mat[1][0] - t.m_mat[1][0]; o.m_mat[1][1] = m_mat[1][1] - t.m_mat[1][1]; 
	o.m_mat[1][2] = m_mat[1][2] - t.m_mat[1][2];	o.m_mat[1][3] = m_mat[1][3] - t.m_mat[1][3];
	
	o.m_mat[2][0] = m_mat[2][0] - t.m_mat[2][0]; o.m_mat[2][1] = m_mat[2][1] - t.m_mat[2][1];
	o.m_mat[2][2] = m_mat[2][2] - t.m_mat[2][2];  o.m_mat[2][3] = m_mat[2][3] - t.m_mat[2][3];
	
	o.m_mat[3][0] = m_mat[3][0] - t.m_mat[3][0]; o.m_mat[3][1] = m_mat[3][1] - t.m_mat[3][1];
	o.m_mat[3][2] = m_mat[3][2] - t.m_mat[3][2];  o.m_mat[3][3] = m_mat[3][3] - t.m_mat[3][3];
	return o;
}


TMatrix4 & TMatrix4::operator+= (const TMatrix4 & t)
{
	m_mat[0][0] += t.m_mat[0][0]; m_mat[0][1] += t.m_mat[0][1]; m_mat[0][2] += t.m_mat[0][2]; m_mat[0][3] += t.m_mat[0][3];
	m_mat[1][0] += t.m_mat[1][0]; m_mat[1][1] += t.m_mat[1][1]; m_mat[1][2] += t.m_mat[1][2]; m_mat[1][3] += t.m_mat[1][3];
	m_mat[2][0] += t.m_mat[2][0]; m_mat[2][1] += t.m_mat[2][1]; m_mat[2][2] += t.m_mat[2][2]; m_mat[2][3] += t.m_mat[2][3];
	m_mat[3][0] += t.m_mat[3][0]; m_mat[3][1] += t.m_mat[3][1]; m_mat[3][2] += t.m_mat[3][2]; m_mat[3][3] += t.m_mat[3][3];
	return *this;
}

TMatrix4 & TMatrix4::operator-= (const TMatrix4 & t)
{
	m_mat[0][0] -= t.m_mat[0][0]; m_mat[0][1] -= t.m_mat[0][1]; m_mat[0][2] -= t.m_mat[0][2]; m_mat[0][3] -= t.m_mat[0][3];
	m_mat[1][0] -= t.m_mat[1][0]; m_mat[1][1] -= t.m_mat[1][1]; m_mat[1][2] -= t.m_mat[1][2]; m_mat[1][3] -= t.m_mat[1][3];
	m_mat[2][0] -= t.m_mat[2][0]; m_mat[2][1] -= t.m_mat[2][1]; m_mat[2][2] -= t.m_mat[2][2]; m_mat[2][3] -= t.m_mat[2][3];
	m_mat[3][0] -= t.m_mat[3][0]; m_mat[3][1] -= t.m_mat[3][1]; m_mat[3][2] -= t.m_mat[3][2]; m_mat[3][3] -= t.m_mat[3][3];
	return *this;
}

//vector3 * matrix4
TVector3 operator* (const TVector3 & v, const TMatrix4 & t)
{
	TVector3 result(v.m_x*t.m_mat[0][0] + v.m_y*t.m_mat[1][0] + v.m_z*t.m_mat[2][0] + t.m_mat[0][3],
		v.m_x*t.m_mat[0][1] + v.m_y*t.m_mat[1][1] + v.m_z*t.m_mat[2][1] + t.m_mat[1][3],
		v.m_x*t.m_mat[0][2] + v.m_y*t.m_mat[1][2] + v.m_z*t.m_mat[2][2] + t.m_mat[2][3]);

	float w = 1.0f / (v.m_x * t.m_mat[0][3] + v.m_y * t.m_mat[1][3] + v.m_z * t.m_mat[2][3] + t.m_mat[3][3]);
	return result *= w;

}

//matrix4 * vector3
TVector3 operator* (const TMatrix4 & t, const TVector3 & v)
{
	return v*t;
}

//vector4 * matrix4
TVector4 operator* (const TVector4 & v, const TMatrix4 & t)
{
	return TVector4(v.m_x*t.m_mat[0][0] + v.m_y*t.m_mat[1][0] + v.m_z*t.m_mat[2][0] + v.m_w*t.m_mat[3][0],
		v.m_x*t.m_mat[0][1] + v.m_y*t.m_mat[1][1] + v.m_z*t.m_mat[2][1] + v.m_w*t.m_mat[3][1],
		v.m_x*t.m_mat[0][2] + v.m_y*t.m_mat[1][2] + v.m_z*t.m_mat[2][2] + v.m_w*t.m_mat[3][2],
		v.m_x*t.m_mat[0][3] + v.m_y*t.m_mat[1][3] + v.m_z*t.m_mat[2][3] + v.m_w*t.m_mat[3][3]);
}

//matrix4 * vector4
TVector4 operator* (const TMatrix4 & t, const TVector4 & v)
{
	return v*t;
}


TMatrix4 TMatrix4::operator * (const TMatrix4 & t) const
{
	TMatrix4 r;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			r.m_mat[i][j] = m_mat[i][0] * t.m_mat[0][j] + m_mat[i][1] * t.m_mat[1][j] + m_mat[i][2] * t.m_mat[2][j] + m_mat[i][3] * t.m_mat[3][j];
		}
	}
	return r;
}

TMatrix4 TMatrix4::operator * (float f) const
{
	TMatrix4 o;
	o.m_mat[0][0] = f*m_mat[0][0]; o.m_mat[0][1] = f*m_mat[0][1]; o.m_mat[0][2] = f*m_mat[0][2]; o.m_mat[0][3] = f*m_mat[0][3];
	o.m_mat[1][0] = f*m_mat[1][0]; o.m_mat[1][1] = f*m_mat[1][1]; o.m_mat[1][2] = f*m_mat[1][2]; o.m_mat[1][3] = f*m_mat[1][3];
	o.m_mat[2][0] = f*m_mat[2][0]; o.m_mat[2][1] = f*m_mat[2][1]; o.m_mat[2][2] = f*m_mat[2][2]; o.m_mat[2][3] = f*m_mat[2][3];
	o.m_mat[3][0] = f*m_mat[3][0]; o.m_mat[3][1] = f*m_mat[3][1]; o.m_mat[3][2] = f*m_mat[3][2]; o.m_mat[3][3] = f*m_mat[3][3];
	return o;
}

TMatrix4 & TMatrix4::operator *= (float f)
{
	m_mat[0][0] = f*m_mat[0][0]; m_mat[0][1] = f*m_mat[0][1]; m_mat[0][2] = f*m_mat[0][2]; m_mat[0][3] = f*m_mat[0][3];
	m_mat[1][0] = f*m_mat[1][0]; m_mat[1][1] = f*m_mat[1][1]; m_mat[1][2] = f*m_mat[1][2]; m_mat[1][3] = f*m_mat[1][3];
	m_mat[2][0] = f*m_mat[2][0]; m_mat[2][1] = f*m_mat[2][1]; m_mat[2][2] = f*m_mat[2][2]; m_mat[2][3] = f*m_mat[2][3];
	m_mat[3][0] = f*m_mat[3][0]; m_mat[3][1] = f*m_mat[3][1]; m_mat[3][2] = f*m_mat[3][2]; m_mat[3][3] = f*m_mat[3][3];
	return *this;
}


// Clear all elements to zero
void TMatrix4::Clear()
{
	m_mat[0][0] = m_mat[0][1] = m_mat[0][2] = m_mat[0][3] =  0.0f;
	m_mat[1][0] = m_mat[1][1] = m_mat[1][2] = m_mat[1][3] = 0.0f;
	m_mat[2][0] = m_mat[2][1] = m_mat[2][2] = m_mat[2][3] = 0.0f;
	m_mat[3][0] = m_mat[3][1] = m_mat[3][2] = m_mat[3][3] = 0.0f;
}

//	Set matrix to identity matrix
void TMatrix4::Identity()
{
	m_mat[0][1] = m_mat[0][2] = m_mat[0][3] = 0.0f;
	m_mat[1][0] = m_mat[1][2] = m_mat[1][3] = 0.0f;
	m_mat[2][0] = m_mat[2][1] = m_mat[2][3] = 0.0f;
	m_mat[3][0] = m_mat[3][1] = m_mat[3][2] = 0.0f;
	m_mat[0][0] = m_mat[1][1] = m_mat[2][2] = m_mat[3][3] = 1.0f;
}

bool TMatrix4::IsIdentity() const
{
	TMatrix4 mat;
	mat.Identity();
	return *this == mat;
}

//	Transpose matrix
void TMatrix4::Transpose()
{
	float t;
	t = m_mat[0][1]; m_mat[0][1] = m_mat[1][0]; m_mat[1][0] = t;
	t = m_mat[0][2]; m_mat[0][2] = m_mat[2][0]; m_mat[2][0] = t;
	t = m_mat[0][3]; m_mat[0][3] = m_mat[3][0]; m_mat[3][0] = t;
	t = m_mat[1][2]; m_mat[1][2] = m_mat[2][1]; m_mat[2][1] = t;
	t = m_mat[1][3]; m_mat[1][3] = m_mat[3][1]; m_mat[3][1] = t;
	t = m_mat[2][3]; m_mat[2][3] = m_mat[3][2]; m_mat[3][2] = t;
}

//	Get transpose matrix of this matrix
TMatrix4 TMatrix4::GetTranspose() const
{
	TMatrix4 matRet;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			matRet.m_mat[i][j] = m_mat[j][i];
	}
	return matRet;
}

//	Set translation matrix
void TMatrix4::Translate(float x, float y, float z)
{
	Identity();
	m_mat[3][0] = x;
	m_mat[3][1] = y;
	m_mat[3][2] = z;
}

//	fRad rotate radian
void TMatrix4::RotateX(float fRad)
{
	Identity();
	m_mat[2][2] = m_mat[1][1] = (float)cos(fRad);
	m_mat[1][2] = (float)sin(fRad);
	m_mat[2][1] = -m_mat[1][2];
}

//	fRad rotate radian
void TMatrix4::RotateY(float fRad)
{
	Identity();
	m_mat[2][2] = m_mat[0][0] = (float)cos(fRad);
	m_mat[2][0] = (float)sin(fRad);
	m_mat[0][2] = -m_mat[2][0];
}

//	fRad rotate radian
void TMatrix4::RotateZ(float fRad)
{
	Identity();
	m_mat[1][1] = m_mat[0][0] = (float)cos(fRad);
	m_mat[0][1] = (float)sin(fRad);
	m_mat[1][0] = -m_mat[0][1];
}

//Rotate Matrix around the Axis 
//fRad: rotate radian
//v: normalized axis
//fRad: rotation radian
TMatrix4& TMatrix4::RotateAxis(const TVector3 & v, float fRad)
{
	const double c = cos(fRad);
	const double s = sin(fRad);
	const double t = 1.0 - c;

	const double tx = t * v.m_x;
	const double ty = t * v.m_y;
	const double tz = t * v.m_z;

	const double sx = s * v.m_x;
	const double sy = s * v.m_y;
	const double sz = s * v.m_z;

	m_mat[0][0] = (float)(tx * v.m_x + c);
	m_mat[0][1] = (float)(tx * v.m_y + sz);
	m_mat[0][2] = (float)(tx * v.m_z - sy);
	m_mat[0][3] = 0;

	m_mat[1][0] = (float)(ty * v.m_x - sz);
	m_mat[1][1] = (float)(ty * v.m_y + c);
	m_mat[1][2] = (float)(ty * v.m_z + sx);
	m_mat[1][3] = 0;


	m_mat[2][0] = (float)(tz * v.m_x + sy);
	m_mat[2][1] = (float)(tz * v.m_y - sx);
	m_mat[2][2] = (float)(tz * v.m_z + c);
	m_mat[2][3] = 0;

	m_mat[3][0] = m_mat[3][1] = m_mat[3][2] = 0;
	m_mat[3][3] = 1.0f;
	return *this;
}

/*	Rotate camera round an axis

vPos: start position of rotate axis
vAxis: rotate axis
fRad: rotate radian
*/
TMatrix4& TMatrix4::RotateAxis(const TVector3& vPos, const TVector3& vAxis, float fRad)
{
	Translate(-vPos.m_x, -vPos.m_y, -vPos.m_z);

	TMatrix4 mat;
	mat.RotateAxis(vAxis, fRad);
	*this *= mat;
	mat.Translate(vPos.m_x, vPos.m_y, vPos.m_z);
	*this *= mat;
	return *this;
}

//	Build scale matrix
void TMatrix4::Scale(float sx, float sy, float sz)
{
	Clear();
	m_mat[0][0] = sx;
	m_mat[1][1] = sy;
	m_mat[2][2] = sz;
	m_mat[3][3] = 1.0f;
}

float TMatrix4::Determinant() const
{
	return m_mat[0][0] * (
			m_mat[1][1] * (m_mat[2][2] * m_mat[3][3] - m_mat[2][3] * m_mat[3][2]) -
			m_mat[2][1] * (m_mat[1][2] * m_mat[3][3] - m_mat[1][3] * m_mat[3][2]) +
			m_mat[3][1] * (m_mat[1][2] * m_mat[2][3] - m_mat[1][3] * m_mat[2][2])
		)-
		m_mat[1][0] * (
			m_mat[0][1] * (m_mat[2][2] * m_mat[3][3] - m_mat[2][3] * m_mat[3][2]) -
			m_mat[2][1] * (m_mat[0][2] * m_mat[3][3] - m_mat[0][3] * m_mat[3][2]) +
			m_mat[3][1] * (m_mat[0][2] * m_mat[2][3] - m_mat[0][3] * m_mat[2][2])
		) +
		m_mat[2][0] * (
			m_mat[0][1] * (m_mat[1][2] * m_mat[3][3] - m_mat[1][3] * m_mat[3][2]) -
			m_mat[1][1] * (m_mat[0][2] * m_mat[3][3] - m_mat[0][3] * m_mat[3][2]) +
			m_mat[3][1] * (m_mat[0][2] * m_mat[1][3] - m_mat[0][3] * m_mat[1][2]) 
		) -
		m_mat[3][0] * (
			m_mat[0][1] * (m_mat[1][2] * m_mat[2][3] - m_mat[1][3] * m_mat[2][2]) -
			m_mat[1][1] * (m_mat[0][2] * m_mat[2][3] - m_mat[0][3] * m_mat[2][2]) +
			m_mat[2][1] * (m_mat[0][2] * m_mat[1][3] - m_mat[0][3] * m_mat[1][2])
		);
}

TMatrix4 TMatrix4::GetInverseTM() const
{
	float fDet = 1.0f / Det3(m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[1][0], m_mat[1][1], 
		m_mat[1][2],m_mat[2][0], m_mat[2][1], m_mat[2][2]);

	TMatrix4 mat;
	mat.m_mat[0][0] = fDet * Det3(m_mat[1][1], m_mat[1][2], m_mat[1][3], m_mat[2][1], 
					m_mat[2][2], m_mat[2][3], m_mat[3][1], m_mat[3][2], m_mat[3][3]);
	
	mat.m_mat[0][1] = -fDet * Det3(m_mat[0][1], m_mat[0][2], m_mat[0][3], m_mat[2][1],
					m_mat[2][2], m_mat[2][3], m_mat[3][1], m_mat[3][2], m_mat[3][3]);
	
	mat.m_mat[0][2] = fDet * Det3(m_mat[0][1], m_mat[0][2], m_mat[0][3], m_mat[1][1],
					m_mat[1][2], m_mat[1][3], m_mat[3][1], m_mat[3][2], m_mat[3][3]);

	mat.m_mat[0][3] = -fDet * Det3(m_mat[0][1], m_mat[0][2], m_mat[0][3], m_mat[1][1],
					m_mat[1][2], m_mat[1][3], m_mat[2][1], m_mat[2][2], m_mat[2][3]);

	mat.m_mat[1][0] = -fDet * Det3(m_mat[1][0], m_mat[1][2], m_mat[1][3], m_mat[2][0],
		m_mat[2][2], m_mat[2][3], m_mat[3][0], m_mat[3][2], m_mat[3][3]);

	mat.m_mat[1][1] = fDet * Det3(m_mat[0][0], m_mat[0][2], m_mat[0][3], m_mat[2][0],
		m_mat[2][2], m_mat[2][3], m_mat[3][0], m_mat[3][2], m_mat[3][3]);

	mat.m_mat[1][2] = -fDet * Det3(m_mat[0][0], m_mat[0][2], m_mat[0][3], m_mat[1][0],
		m_mat[1][2], m_mat[1][3], m_mat[3][0], m_mat[3][2], m_mat[3][3]);

	mat.m_mat[1][3] = fDet * Det3(m_mat[0][0], m_mat[0][2], m_mat[0][3], m_mat[1][0],
		m_mat[1][2], m_mat[1][3], m_mat[2][0], m_mat[2][2], m_mat[2][3]);

	mat.m_mat[2][0] = fDet * Det3(m_mat[1][0], m_mat[1][1], m_mat[1][3], m_mat[2][0],
		m_mat[2][1], m_mat[2][3], m_mat[3][0], m_mat[3][1], m_mat[3][3]);

	mat.m_mat[2][1] = -fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][3], m_mat[2][0],
		m_mat[2][1], m_mat[2][3], m_mat[3][0], m_mat[3][1], m_mat[3][3]);

	mat.m_mat[2][2] = fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][3], m_mat[1][0],
		m_mat[1][1], m_mat[1][3], m_mat[3][0], m_mat[3][1], m_mat[3][3]);

	mat.m_mat[2][3] = -fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][3], m_mat[1][0],
		m_mat[1][1], m_mat[1][3], m_mat[2][0], m_mat[2][1], m_mat[2][3]);

	mat.m_mat[3][0] = -fDet * Det3(m_mat[1][0], m_mat[1][1], m_mat[1][2], m_mat[2][0],
		m_mat[2][1], m_mat[2][2], m_mat[3][0], m_mat[3][1], m_mat[3][2]);

	mat.m_mat[3][1] = fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[2][0],
		m_mat[2][1], m_mat[2][2], m_mat[3][0], m_mat[3][1], m_mat[3][2]);

	mat.m_mat[3][2] = -fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[2][0],
		m_mat[2][1], m_mat[2][2], m_mat[3][0], m_mat[3][1], m_mat[3][2]);

	mat.m_mat[3][3] = fDet * Det3(m_mat[0][0], m_mat[0][1], m_mat[0][2], m_mat[1][0],
		m_mat[1][1], m_mat[1][2], m_mat[2][0], m_mat[2][1], m_mat[2][2]);

	return mat;
}

TMatrix4 & TMatrix4::BuildCameraLookAtMatrix(const TVector3 & position, const TVector3 & look, const TVector3 & up)
{
	TVector3 zaxis = look;
	zaxis.Normalize();

	TVector3 xaxis = up.CrossProduct(zaxis);
	xaxis.Normalize();

	TVector3 yaxis = zaxis.CrossProduct(xaxis);
	yaxis.Normalize();

	m_mat[0][0] = xaxis.m_x;
	m_mat[0][1] = yaxis.m_x;
	m_mat[0][2] = zaxis.m_x;
	m_mat[0][3] = 0;

	m_mat[1][0] = xaxis.m_y;
	m_mat[1][1] = yaxis.m_y;
	m_mat[1][2] = zaxis.m_y;
	m_mat[1][3] = 0;

	m_mat[2][0] = xaxis.m_z;
	m_mat[2][1] = yaxis.m_z;
	m_mat[2][2] = zaxis.m_z;
	m_mat[2][3] = 0;

	m_mat[3][0] = -xaxis.DotProduct(position);
	m_mat[3][1] = -yaxis.DotProduct(position);
	m_mat[3][2] = -zaxis.DotProduct(position);
	m_mat[3][3] = 1;
	return *this;
}

TMatrix4 & TMatrix4::BuildPerspectiveMatrix(float fieldOfViewRadians, float aspectRatio, float z_near, float z_far)
{
	const double  h = 1.0 / (tan(fieldOfViewRadians*0.5));
	const float w = static_cast<float>(h / aspectRatio);

	m_mat[0][0] = w;
	m_mat[0][1] = 0;
	m_mat[0][2] = 0;
	m_mat[0][3] = 0;

	m_mat[1][0] = 0;
	m_mat[1][1] = (float)h;
	m_mat[1][2] = 0;
	m_mat[1][3] = 0;

	m_mat[2][0] = 0;
	m_mat[2][1] = 0;
	m_mat[2][2] = (float)(z_far / (z_far - z_near));
	m_mat[2][3] = 1;

	m_mat[3][0] = 0;
	m_mat[3][1] = 0;
	m_mat[3][2] = (float)(-z_near*z_far / (z_far - z_near));;
	m_mat[3][3] = 0;
	return *this;
}

//	Get rotation part of transform matrix
TMatrix3 TMatrix4::GetRotatePart() const
{
	TMatrix3 ret;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			ret.m_mat[i][j] = m_mat[i][j];
	}

	return ret;
}

//	Set rotation part of transform matrix
void TMatrix4::SetRotatePart(const TMatrix3 & mat)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			m_mat[i][j] = mat.m_mat[i][j];
	}
}

