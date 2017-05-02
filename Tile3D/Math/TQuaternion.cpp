#include "TQuaternion.h"
#include "TMathUtil.h"


const TQuaternion TQuaternion::IDENTITY(0, 0, 0, 1);
const float TQuaternion::SLERP_EPSILON = 0.25f;

bool TQuaternion::operator == (const TQuaternion & q) const
{
	return m_x == q.m_x && m_y == q.m_y && m_z == q.m_z && m_w == q.m_w;
}

bool TQuaternion::operator != (const TQuaternion & q) const
{
	return m_x != q.m_x || m_y != q.m_y || m_z != q.m_z || m_w != q.m_w;
}


TQuaternion TQuaternion::operator + (const TQuaternion& q) const
{
	return TQuaternion(this->m_x + q.m_x, this->m_y + q.m_y, this->m_z + q.m_z, this->m_w + q.m_w);
}

TQuaternion TQuaternion::operator - (const TQuaternion& q) const
{
	return TQuaternion(this->m_x - q.m_x, this->m_y - q.m_y, this->m_z - q.m_z, this->m_w - q.m_w);
}

TQuaternion& TQuaternion::operator += (const TQuaternion& q)
{
	this->m_x += q.m_x;
	this->m_y += q.m_y;
	this->m_z += q.m_z;
	this->m_w += q.m_w;
	return *this;
}

TQuaternion& TQuaternion::operator -= (const TQuaternion& q)
{
	this->m_x -= q.m_x;
	this->m_y -= q.m_y;
	this->m_z -= q.m_z;
	this->m_w -= q.m_w;
	return *this;
}

TQuaternion& TQuaternion::operator - ()
{
	this->m_x -= -m_x;
	this->m_y -= -m_y;
	this->m_z -= -m_z;
	this->m_w -= -m_w;
	return *this;
}

TQuaternion TQuaternion::operator * (float f) const
{
	return TQuaternion(f*m_x, f*m_y, f*m_z, f*m_w);
}

TQuaternion TQuaternion::operator * (int f) const
{
	return (*this * (float)f);
}


TQuaternion& TQuaternion::operator *= (float f)
{
	this->m_x *= f;
	this->m_y *= f;
	this->m_z *= f;
	this->m_w *= f;
	return *this;
}

TQuaternion& TQuaternion::operator *= (int f)
{
	return *this *= (float)f;
}

TQuaternion TQuaternion::operator * (const TQuaternion &  q) const
{
	TQuaternion r;
	r.m_x = (q.m_x * this->m_w) + (q.m_w * this->m_x) + (q.m_z * this->m_y) - (q.m_y * m_z);
	r.m_y = (q.m_y * this->m_w) + (q.m_w * this->m_y) + (q.m_x * this->m_z) - (q.m_z * m_x);
	r.m_z = (q.m_z * this->m_w) + (q.m_w * this->m_z) + (q.m_y * this->m_x) - (q.m_x * m_y);
	r.m_w = (q.m_w * this->m_w) - (q.m_x * this->m_x) - (q.m_y * this->m_y) - (q.m_z * m_z);
	return r;
}


TQuaternion& TQuaternion::operator *= (const TQuaternion &  q)
{
	*this = *this * q;
	return *this;
}


TQuaternion TQuaternion::operator / (float f) const
{
	f = 1.0f / f;
	return TQuaternion(f*m_x, f*m_y, f*m_z, f*m_w);
}

TQuaternion TQuaternion::operator / (int f) const
{
	return (*this) / (float)f;
}


TQuaternion& TQuaternion::operator /= (float f)
{
	f = 1.0f / f;
	this->m_x *= f;
	this->m_y *= f;
	this->m_z *= f;
	this->m_w *= f;
	return *this;
}

TQuaternion& TQuaternion::operator /= (int f)
{
	return *this /= (float)f;
}



TVector3 TQuaternion::operator * (const TVector3 & v) const
{
	TVector3  uv, uuv;
	TVector3	qvec(m_x, m_y, m_z);

	uv = qvec.CrossProduct(v);
	uuv = qvec.CrossProduct(uv);
	uv = uv * (2.0f * m_w);
	uuv = uuv * 2.0f;

	return v + uv + uuv;
}


/*
TVector3 TQuaternion::operator * (const TVector3 & v) const
{
TQuaternion	vq = TQuaternion(1.0f, v);
TQuaternion	qi = TQuaternion(m_w, -m_x, -m_y, -m_z);
TQuaternion	qvq = (*this) * vq * qi;

return TVector3(qvq.m_x, qvq.m_y, qvq.m_z);
}
*/

void TQuaternion::Identity()
{
	m_x = m_y = m_z = 0.0f;
	m_w = 1.0f;
}

void TQuaternion::Clear()
{
	m_x = m_y = m_z = m_w = 0.0f;
}

void TQuaternion::Set(float x, float y, float z, float w)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

const TQuaternion & TQuaternion::Normalize()
{
	float tolerance = 0.0001f;
	const float squareSum = m_x * m_x + m_y * m_y + m_z*m_z + m_w * m_w;
	if (squareSum >= tolerance)
	{
		const float scale = TMathUtil::InvSqrt(squareSum);
		m_x *= scale;
		m_y *= scale;
		m_z *= scale;
		m_w *= scale;
	}
	else
	{
		*this = TQuaternion::IDENTITY;
	}
	return *this;
}

bool TQuaternion::IsNormalized()
{
	return fabs(1.0 - SizeSquared()) < 0.01f;
}

const TQuaternion & TQuaternion::Inverse()
{
	if (!IsNormalized()) {
		Normalize();
	}

	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
	return *this;
}

const TQuaternion & TQuaternion::Conjugate()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
	return *this;
}

float TQuaternion::DotProduct(const TQuaternion & q) const
{
	return m_w * q.m_w + m_x * q.m_x + m_y * q.m_y + m_z * q.m_z;
}


///////////////////////////////////////////////////////////////////////////////////////
//
// MatrixToQuad()
//
//	construct a quaternion with a rotating matrix. The matrix is not required to be a 
// pure rotation matrix, it can contains translation and scale factor, but the output
// quaternion will not be unit quaternion if it contains scale factor.
//
///////////////////////////////////////////////////////////////////////////////////////
void TQuaternion::ConvertFromMatrix(const TMatrix4 & m)
{
	float tr, s;

	// calculating the trace of the matrix, it is equal to 4(1 - x*x - y*y - z*z)=4w*w if it is a unit quaternion
	tr = m.m_mat[0][0] + m.m_mat[1][1] + m.m_mat[2][2] + 1.0f;
	// check the diagonal
	if (tr > 0.36f) // we can calculate out w directly
	{
		s = (float)sqrt(tr); // s is 2w
		this->m_w = s * 0.5f;
		s = 0.5f / s;	// now s is 1/4w
		this->m_x = (m.m_mat[1][2] - m.m_mat[2][1]) * s;
		this->m_y = (m.m_mat[2][0] - m.m_mat[0][2]) * s;
		this->m_z = (m.m_mat[0][1] - m.m_mat[1][0]) * s;
	}
	else
	{
		// we have to calculate x, y or z first
		if( m.m_mat[0][0] >= m.m_mat[1][1] && m.m_mat[0][0] >= m.m_mat[2][2] )
		{
			s = (float)sqrt(1.0f + m.m_mat[0][0] - m.m_mat[1][1] - m.m_mat[2][2]); // s is 2x
			this->m_x = s *0.5f;
			s = 0.5f / s;
			this->m_y = (m.m_mat[0][1] + m.m_mat[1][0]) * s;
			this->m_z = (m.m_mat[0][2] + m.m_mat[2][0]) * s;
			this->m_w = (m.m_mat[1][2] - m.m_mat[2][1]) * s;
		}
		else if( m.m_mat[1][1] >= m.m_mat[0][0] && m.m_mat[1][1] >= m.m_mat[2][2] )
		{
			s = (float)sqrt(1.0f + m.m_mat[1][1] - m.m_mat[0][0] - m.m_mat[2][2]); // s is 2y
			this->m_y = s *0.5f;
			s = 0.5f / s;
			this->m_x = (m.m_mat[0][1] + m.m_mat[1][0]) * s;
			this->m_z = (m.m_mat[1][2] + m.m_mat[2][1]) * s;
			this->m_w = (m.m_mat[2][0] - m.m_mat[0][2]) * s;
		}
		else // m.m_mat[2][2] is maximum
		{
			s = (float)sqrt(1.0f + m.m_mat[2][2] - m.m_mat[0][0] - m.m_mat[1][1]); // s is 2z
			this->m_z = s *0.5f;
			s = 0.5f / s;
			this->m_x = (m.m_mat[0][2] + m.m_mat[2][0]) * s;
			this->m_y = (m.m_mat[1][2] + m.m_mat[2][1]) * s;
			this->m_w = (m.m_mat[0][1] - m.m_mat[1][0]) * s;
		}
	}

}


//construct a rotation matrix with a quaternion
void TQuaternion::ConvertToMatrix(TMatrix4 & m) const
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	
	// calculate coefficients
	x2 = m_x + m_x;
	y2 = m_y + m_y;
	z2 = m_z + m_z;
	xx = m_x * x2;
	xy = m_x * y2;
	xz = m_x * z2;
	yy = m_y * y2;
	yz = m_y * z2;
	zz = m_z * z2;
	wx = m_w * x2;
	wy = m_w * y2;
	wz = m_w * z2;

	m.m_mat[0][0] = 1.0f - (yy + zz);
	m.m_mat[1][0] = xy - wz;
	m.m_mat[2][0] = xz + wy;
	m.m_mat[3][0] = 0.0f;
	m.m_mat[0][1] = xy + wz;
	m.m_mat[1][1] = 1.0f - (xx + zz);
	m.m_mat[2][1] = yz - wx;
	m.m_mat[3][1] = 0.0f;
	m.m_mat[0][2] = xz - wy;
	m.m_mat[1][2] = yz + wx;
	m.m_mat[2][2] = 1.0f - (xx + yy);
	m.m_mat[3][2] = 0.0f;
	m.m_mat[0][3] = 0.0f;
	m.m_mat[1][3] = 0.0f;
	m.m_mat[2][3] = 0.0f;
	m.m_mat[3][3] = 1.0f;
}

void TQuaternion::ConvertFromAxisAngle(const TVector3 & vecAxis, float vAngle)
{
	float sine, cosine;

	vAngle *= 0.5f;
	sine = (float)sin(vAngle);
	cosine = (float)cos(vAngle);

	m_w = cosine;
	m_x = vecAxis.m_x * sine;
	m_y = vecAxis.m_y * sine;
	m_z = vecAxis.m_z * sine;
}

void TQuaternion::ConvertToAxisAngle(TVector3& vecAxis, float& vAngle) const
{
	float cosine, sine;

	cosine = m_w;
	vAngle = (float)(acos(cosine)) * 2.0f;
	sine = (float)sqrt(1.0f - cosine * cosine);

	if (sine < 0.0005f)
		sine = 1.0f;

	vecAxis.m_x = m_x / sine;
	vecAxis.m_y = m_y / sine;
	vecAxis.m_z = m_z / sine;
}

void TQuaternion::ConvertFromEulerAngle(float pitch, float yaw, float roll)
{
	pitch *= 0.5f;
	yaw *= 0.5f;
	roll *= 0.5f;

	// calculate trig identities
	float cr, cp, cy, sr, sp, sy, cpcy, spsy, cpsy, spcy;
	cr = (float)cos(pitch);
	cp = (float)cos(yaw);
	cy = (float)cos(roll);
	sr = (float)sin(pitch);
	sp = (float)sin(yaw);
	sy = (float)sin(roll);
	cpcy = cp * cy;
	spsy = sp * sy;
	cpsy = cp * sy;
	spcy = sp * cy;
	m_w = cr * cpcy + sr * spsy;
	m_x = sr * cpcy + cr * spsy;
	m_y = cr * spcy - sr * cpsy;
	m_z = cr * cpsy - sr * spcy;
}


void TQuaternion::ConvertToEulerAngle(float & pitch, float & yaw, float & roll) const
{
	pitch = (float)(atan2(2 * (m_w*m_x + m_y*m_z), 1 - 2 * (m_x*m_x + m_y*m_y)));
	yaw = (float)(asin(2 * (m_w*m_y - m_z*m_x)));
	roll = (float)(atan2(2 * (m_w*m_z + m_x*m_y), 1 - 2 * (m_y*m_y + m_z*m_z)));
}

void TQuaternion::Lerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst)
{
	if (t == 0.0f)
	{
		memcpy(dst, &q1, sizeof(float) * 4);
		return;
	}
	else if (t == 1.0f)
	{
		memcpy(dst, &q2, sizeof(float) * 4);
		return;
	}

	float t1 = 1.0f - t;

	dst->m_x = t1 * q1.m_x + t * q2.m_x;
	dst->m_y = t1 * q1.m_y + t * q2.m_y;
	dst->m_z = t1 * q1.m_z + t * q2.m_z;
	dst->m_w = t1 * q1.m_w + t * q2.m_w;
}

void TQuaternion::Slerp(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst)
{
	Slerp(q1.m_x, q1.m_y, q1.m_z, q1.m_w, q2.m_x, q2.m_y, q2.m_z, q2.m_w, t, &dst->m_x, &dst->m_y, &dst->m_z, &dst->m_w);
}

void TQuaternion::Squad(const TQuaternion& q1, const TQuaternion& q2, const TQuaternion& s1, const TQuaternion& s2, float t, TQuaternion* dst)
{
	TQuaternion dstQ(0.0f, 0.0f, 0.0f, 1.0f);
	TQuaternion dstS(0.0f, 0.0f, 0.0f, 1.0f);

	Slerp(q1, q2, t, &dstQ);
	Slerp(s1, s2, t, &dstS);
	Slerp(dstQ, dstS, 2.0f * t * (1.0f - t), dst);
}


TQuaternion TQuaternion::SLERPQuad(const TQuaternion& q1, const TQuaternion& q2, float fraction)
{
	float cosine, sign;
	float f1, f2;
	cosine = q1.DotProduct(q2);
	if (cosine < 0.0f)
	{
		cosine = -cosine;
		sign = -1.0f;
	}
	else
		sign = 1.0f;

	if (cosine > 1.0f - SLERP_EPSILON)
	{
		// the from and to value are very close, so use LERP will be ok
		f1 = 1.0f - fraction;
		f2 = fraction * sign;
	}
	else
	{
		float theta;
		theta = (float)acos(cosine);
		float sine;
		sine = (float)sin(theta);
		f1 = (float)(sin((1.0f - fraction) * theta) / sine);
		f2 = (float)(sin(fraction * theta) / sine) * sign;
	}

	return q1 * f1 + q2 * f2;
}

void TQuaternion::Slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz, float* dstw)
{
	// Fast slerp implementation by kwhatmough:
	// It contains no division operations, no trig, no inverse trig
	// and no sqrt. Not only does this code tolerate small constraint
	// errors in the input quaternions, it actually corrects for them.

	if (t == 0.0f)
	{
		*dstx = q1x;
		*dsty = q1y;
		*dstz = q1z;
		*dstw = q1w;
		return;
	}
	else if (t == 1.0f)
	{
		*dstx = q2x;
		*dsty = q2y;
		*dstz = q2z;
		*dstw = q2w;
		return;
	}

	if (q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w)
	{
		*dstx = q1x;
		*dsty = q1y;
		*dstz = q1z;
		*dstw = q1w;
		return;
	}

	float halfY, alpha, beta;
	float u, f1, f2a, f2b;
	float ratio1, ratio2;
	float halfSecHalfTheta, versHalfTheta;
	float sqNotU, sqU;

	float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

	// As usual in all slerp implementations, we fold theta.
	alpha = cosTheta >= 0 ? 1.0f : -1.0f;
	halfY = 1.0f + alpha * cosTheta;

	// Here we bisect the interval, so we need to fold t as well.
	f2b = t - 0.5f;
	u = f2b >= 0 ? f2b : -f2b;
	f2a = u - f2b;
	f2b += u;
	u += u;
	f1 = 1.0f - u;

	// One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
	halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
	halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
	versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

	// Evaluate series expansions of the coefficients.
	sqNotU = f1 * f1;
	ratio2 = 0.0000440917108f * versHalfTheta;
	ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
	ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
	ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
	ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

	sqU = u * u;
	ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
	ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
	ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
	ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

	// Perform the bisection and resolve the folding done earlier.
	f1 *= ratio1 * halfSecHalfTheta;
	f2a *= ratio2;
	f2b *= ratio2;
	alpha *= f1 + f2a;
	beta = f1 + f2b;

	// Apply final coefficients to a and b as usual.
	float w = alpha * q1w + beta * q2w;
	float x = alpha * q1x + beta * q2x;
	float y = alpha * q1y + beta * q2y;
	float z = alpha * q1z + beta * q2z;

	// This final adjustment to the quaternion's length corrects for
	// any small constraint error in the inputs q1 and q2 But as you
	// can see, it comes at the cost of 9 additional multiplication
	// operations. If this error-correcting feature is not required,
	// the following code may be removed.
	f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
	*dstw = w * f1;
	*dstx = x * f1;
	*dsty = y * f1;
	*dstz = z * f1;
}

void TQuaternion::SlerpForSquad(const TQuaternion& q1, const TQuaternion& q2, float t, TQuaternion* dst)
{
	// cos(omega) = q1 * q2;
	// slerp(q1, q2, t) = (q1*sin((1-t)*omega) + q2*sin(t*omega))/sin(omega);
	// q1 = +- q2, slerp(q1,q2,t) = q1.
	// This is a straight-forward implementation of the formula of slerp. It does not do any sign switching.
	float c = q1.m_x * q2.m_x + q1.m_y * q2.m_y+ q1.m_z * q2.m_z + q1.m_w * q2.m_w;

	if (fabs(c) >= 1.0f)
	{
		dst->m_x = q1.m_x;
		dst->m_y = q1.m_y;
		dst->m_z = q1.m_z;
		dst->m_w = q1.m_w;
		return;
	}

	float omega = (float)acos(c);
	float s = (float)(sqrt(1.0f - c * c));
	if (fabs(s) <= 0.00001f)
	{
		dst->m_x = q1.m_x;
		dst->m_y = q1.m_y;
		dst->m_z = q1.m_z;
		dst->m_w = q1.m_w;
		return;
	}

	float r1 = (float)(sin((1 - t) * omega) / s);
	float r2 = (float)(sin(t * omega) / s);
	dst->m_x = (q1.m_x * r1 + q2.m_x * r2);
	dst->m_y = (q1.m_y * r1 + q2.m_y* r2);
	dst->m_z = (q1.m_z * r1 + q2.m_z * r2);
	dst->m_w = (q1.m_w * r1 + q2.m_w * r2);
}

float TQuaternion::GetDiffAngle(const TQuaternion& q1, const TQuaternion& q2)
{
	// we assume q can rotate q1 to q2, so we get
	// q1 * q = q2
	// q =  q1.Inverse() * q2
	// q.w = q1.w * q2.w - (-q1.x) * q2.x - (-q1.y) * q2.y - (-q1.z) * q2.z

	float fW = q1.m_w * q2.m_w + q1.m_x * q2.m_x + q1.m_y * q2.m_y + q1.m_z * q2.m_z;

	if (fW > 1.0f || fW < -1.0f) return 0.0f;

	float fDiffAngle = (float)(acos(fW) * 2.0f);
	if (fDiffAngle > TMathUtil::PI)
		return 360 - fDiffAngle * 180 / TMathUtil::PI;
	else
		return fDiffAngle * 180 / TMathUtil::PI;
}

