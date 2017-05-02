/*
* FILE: TMatrix3.h
*
* DESCRIPTION: TMatrix3 class
*
* CREATED BY: liansm
*
* HISTORY:
*
* Copyright (c) 2017 Tile3D Ltd, All Rights Reserved.
*/

#pragma once

#include <memory.h>
#include "TVector3.h"


class TMatrix3
{
	//	Attributes
public:
	float m_mat[3][3];

public:
	//	Construct flag
	enum CONSTRUCT
	{
		IDENTITY = 0,	//	Construct a identity matrix
		CLEARED,		//	Construct a cleared matrix
	};

public:		//	Constructions and Destructions

	TMatrix3() {}

	TMatrix3(CONSTRUCT c)
	{
		if (c == IDENTITY) {
			Identity();
		}
		else if (c == CLEARED) {
			Clear();
		}
	}

	TMatrix3(const float arr[3][3])
	{
		memcpy(m_mat, arr, 9 * sizeof(float));
	}

	TMatrix3(const TMatrix3& rkMatrix)
	{
		memcpy(m_mat, rkMatrix.m_mat, 9 * sizeof(float));
	}

	TMatrix3(float fEntry00, float fEntry01, float fEntry02,
		float fEntry10, float fEntry11, float fEntry12,
		float fEntry20, float fEntry21, float fEntry22)
	{
		m_mat[0][0] = fEntry00;
		m_mat[0][1] = fEntry01;
		m_mat[0][2] = fEntry02;
		m_mat[1][0] = fEntry10;
		m_mat[1][1] = fEntry11;
		m_mat[1][2] = fEntry12;
		m_mat[2][0] = fEntry20;
		m_mat[2][1] = fEntry21;
		m_mat[2][2] = fEntry22;
	}


	//	Operations
public:
	TVector3 GetRow(int i) { return TVector3(m_mat[i][0], m_mat[i][1], m_mat[i][2]); }
	TVector3 GetCol(int i) { return TVector3(m_mat[0][i], m_mat[1][i], m_mat[2][i]); }

	//	* operator
	friend TVector3 operator * (const TVector3& v, const TMatrix3& mat);
	friend TVector3 operator * (const TMatrix3& mat, const TVector3& v);

	TMatrix3 operator * (const TMatrix3& mat)
	{
		TMatrix3 matRet;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
					matRet.m_mat[i][j] += m_mat[i][k] * mat.m_mat[k][j];
			}
		}
		return matRet;
	}

	//	*= operator
	TMatrix3& operator *= (const TMatrix3& mat)
	{
		*this = *this * mat;
		return *this;
	}

	//	== operator
	bool operator == (const TMatrix3& m) const
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (m_mat[i][j] != m.m_mat[i][j])
					return false;
			}
		}
		return true;
	}

	//	!= operator
	bool operator != (const TMatrix3& m) const
	{
		return !(*this == m);
	}

	bool Equals(const TMatrix3 & m) const
	{
		float tolerance = 0.00001f;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if(fabs(m_mat[i][j] - m.m_mat[i][j]) > tolerance)
					return false;
			}
		}
		return true;
	}

	void Transpose()
	{
		float t;
		t = m_mat[0][1]; m_mat[0][1] = m_mat[1][0]; m_mat[1][0] = t;
		t = m_mat[0][2]; m_mat[0][2] = m_mat[2][0];	m_mat[2][0] = t;
		t = m_mat[1][2]; m_mat[1][2] = m_mat[2][1]; m_mat[2][1] = t;
	}

	//	Clear all elements to zero
	void Clear();
	//	Set matrix to identity matrix
	void Identity();

	//	Build matrix to be translation and rotation matrix
	void Translate(float x, float y);

	//	Rotate
	void Rotate(float fRad);

	//Det
	float Determinant();

	// Inverse the matrix
	void InverseTM();
};

