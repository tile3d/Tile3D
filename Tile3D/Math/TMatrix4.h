/*
* FILE: TMatrix4.h
*
* DESCRIPTION: TMatrix4 class
*
* CREATED BY: liansm
*
* HISTORY:
*
* Copyright (c) 2017 Tile3D Ltd, All Rights Reserved.
*/

#pragma once


// NOTE1.  The (x,y,z) coordinate system is assumed to be left-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t)  sin(t)
//           0     -sin(t)  cos(t)
// where t > 0 indicates a clockwise rotation in the yz-plane
//   RY =  cos(t)    0     -sin(t)
//           0       1       0
//        sin(t)    0     cos(t)
// where t > 0 indicates a clockwise rotation in the zx-plane
//   RZ =  cos(t)	sin(t)    0
//        -sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a clockwise rotation in the xy-plane.


/**
* NOTE2.
* Defines a 4 x 4 floating point matrix representing a 3D transformation.
*
* Vectors are treated as rows, resulting in a matrix that is represented as follows,
* where x, y and z are the translation components of the matrix :
*
* 1  0  0  0
* 0  1  0  0
* 0  0  1  0
* x  y  z  1
*
* This matrix class is directly compatible with DirectX since its elements are
* laid out in memory exactly as they are expected by DirectX.
* The matrix uses column - major format such that array indices increase down column first.
* Since matrix multiplication is not commutative, multiplication must be done in the
* correct order when combining transformations.Suppose we have a translation
* matrix T and a rotation matrix R.To first rotate an object around the origin
* and then translate it, you would multiply the two matrices as RT.
*
* Likewise, to first translate the object and then rotate it, you would do TR.
* So generally, matrices must be multiplied in the correct order in which you
* want the transformations to take place(this also applies to
* the scale, rotate, and translate methods below; these methods are convenience
* methods for post - multiplying by a matrix representing a scale, rotation, or translation).
*
*/


#include <memory.h>
#include "TVector3.h"
#include "TVector4.h"
#include "TMatrix3.h"

//
//TBD: 
//1) Implement the Archive
//2) support the SSE
//3) support ToString
//4) handle the differences between DirectX and OpenGL(left hand vs right hand   row vector vs column vector)
//5) Implement Decompose
//6) Implement Reflect
//7) Implement the releated methods with Quaternion
class TMatrix4
{
//	Attributes
public:
	float m_mat[4][4];

public:		
//	Construct flag
	enum CONSTRUCT
	{
		IDENTITY = 0,	//	Construct a identity matrix
		CLEARED,		//	Construct a cleared matrix
	};

	//	Constructions and Destructions
public:		

	TMatrix4() {}

	TMatrix4(float* v)
	{
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
				m_mat[i][j] = v[i * 4 + j];
		}
	}

	TMatrix4(const TMatrix4& mat)
	{
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
				m_mat[i][j] = mat.m_mat[i][j];
		}
	}
	
	TMatrix4(CONSTRUCT c)
	{
		if (c == IDENTITY) 
		{
			Identity();
		}
		else if (c == CLEARED) 
		{
			Clear();
		}
	}
	
	TMatrix4(	float fEntry00, float fEntry01, float fEntry02, float fEntry03,
				float fEntry10, float fEntry11, float fEntry12, float fEntry13,
				float fEntry20, float fEntry21, float fEntry22, float fEntry23,
				float fEntry30, float fEntry31, float fEntry32, float fEntry33)
	{
		m_mat[0][0] = fEntry00; m_mat[0][1] = fEntry01; m_mat[0][2] = fEntry02; m_mat[0][3] = fEntry03;		
		m_mat[1][0] = fEntry10; m_mat[1][1] = fEntry11; m_mat[1][2] = fEntry12; m_mat[1][3] = fEntry13;		
		m_mat[2][0] = fEntry20;	m_mat[2][1] = fEntry21; m_mat[2][2] = fEntry22; m_mat[2][3] = fEntry23;		
		m_mat[3][0] = fEntry30; m_mat[3][1] = fEntry31; m_mat[3][2] = fEntry32;m_mat[3][3] = fEntry33;
	}


//	Operaitons
public:		
	//	+ operator
	TMatrix4 operator + (const TMatrix4& t) const;
	//	- operator
	TMatrix4 operator - (const TMatrix4& t) const;
	//	+= operator
	TMatrix4& operator += (const TMatrix4& mat);
	//	-= operator
	TMatrix4& operator -= (const TMatrix4& mat);

	//	* operator
	friend TVector3 operator * (const TVector3& v, const TMatrix4& mat);
	friend TVector3 operator * (const TMatrix4& mat, const TVector3& v);
	friend TVector4 operator * (const TVector4& v, const TMatrix4& mat);
	friend TVector4 operator * (const TMatrix4& mat, const TVector4& v);
	
	TMatrix4 operator * (const TMatrix4& t) const;
	TMatrix4& operator *= (const TMatrix4& mat)
	{
		*this = *this * mat;
		return *this;
	}

	//	Scale matrix
	TMatrix4 operator * (float s) const;
	friend TMatrix4 operator * (float s, const TMatrix4& mat) { return mat * s; }
	friend TMatrix4 operator / (const TMatrix4& mat, float s) { return mat * (1.0f / s); }

	TMatrix4& operator *= (float s);
	//	/= operator
	TMatrix4& operator /= (float s) { return *this *= (1.0f / s); }
	
	//	== operator
	bool operator == (const TMatrix4& m) const
	{
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
			{
				if (m_mat[i][j] != m.m_mat[i][j])
					return false;
			}
		}
		return true;
	}

	//	!= operator
	bool operator != (const TMatrix4& m) const
	{ 
		return !(*this == m);
	}

	bool Equals(const TMatrix4 & m) const
	{
		float tolerance = 0.0001f;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (fabs(m_mat[i][j] - m.m_mat[i][j]) > tolerance)
					return false;
			}
		}
		return true;
	}

//Member functions
public:
	//	Get row and column
	TVector3 GetRow(int i) const { return TVector3(m_mat[i][0], m_mat[i][1], m_mat[i][2]); }
	TVector3 GetCol(int i) const { return TVector3(m_mat[0][i], m_mat[1][i], m_mat[2][i]); }
	//	Set row and column
	void SetRow(int i, const TVector3& v) { m_mat[i][0] = v.m_x; m_mat[i][1] = v.m_y; m_mat[i][2] = v.m_z; }
	void SetCol(int i, const TVector3& v) { m_mat[0][i] = v.m_x; m_mat[1][i] = v.m_y; m_mat[2][i] = v.m_z; }

	//	Clear all elements to zero
	void Clear();
	//	Set matrix to identity matrix
	void Identity();
	//	Transpose matrix
	void Transpose();

	//	Get transpose matrix of this matrix
	TMatrix4 GetTranspose() const;
	//	Inverse matrix (used only by transform matrix)
	void Inverse() { *this = GetInverseTM(); }
	//	Get inverse matrix (used only by transform matrix)
	TMatrix4 GetInverseTM() const;

	//	Get determinant of this matrix
	float Determinant() const;

	//	Build matrix to be translation and rotation matrix
	void Translate(float x, float y, float z);
	void RotateX(float fRad);
	void RotateY(float fRad);
	void RotateZ(float fRad);

	TMatrix4&  RotateAxis(const TVector3& v, float fRad);
	TMatrix4&  RotateAxis(const TVector3& vPos, const TVector3& vAxis, float fRad);
	
	void Scale(float sx, float sy, float sz);

    //	Set/Get rotation part of transform matrix
	void SetRotatePart(const TMatrix3 & mat);
	TMatrix3 GetRotatePart() const;
    
	//	Set/Get translate part of transform matrix
	void SetTransPart(const TVector3& vTrans) { SetRow(3, vTrans); }
	TVector3 GetTransPart() const { return GetRow(3); };

	//Build the Camera Look at Matrix(left hand)
	TMatrix4 & BuildCameraLookAtMatrix(const TVector3 & position, const TVector3 & look, const TVector3 & up);

	//Build the Perspective Matrix(left hand)
	TMatrix4 & BuildPerspectiveMatrix(float fieldOfViewRadians, float aspectRatio, float near, float far);

protected:	
	//	Calcualte determinant of a 3x3 matrix
	float Det3(float a11, float a12, float a13, float a21, float a22, float a23,
				 float a31, float a32, float a33) const
	{
		return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
			   a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
	}	
};


