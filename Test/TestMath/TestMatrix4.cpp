#include "CppUnitTest.h"
#include <Math/TMatrix4.h>
#include <Math/TMathUtil.h>
#include "TestMathComon.h"
#include <d3dx9math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestMath
{
	TEST_CLASS(TestTMatrix4)
	{
	public:
		TEST_METHOD(Matrix4_Operation)
		{
			TMatrix4 m1(TMatrix4::CONSTRUCT::IDENTITY);
			TMatrix4 m2(TMatrix4::CONSTRUCT::CLEARED);

			TMatrix4 m3(m1);
			Assert::AreEqual(m1, m3);
			Assert::AreNotEqual(m1, m2);

			TMatrix4 m4(-2, 0, 3, 1, 5, 7, -6, 2, 1, -4, 2, 5, 4, 5, 6, 8);
			TMatrix4 m5(13, 4, -8, 12, 0, 6, -3, -1, 5, -6, 22, 45, 12, 34, 10, 9);
			TMatrix4 m6 = m4 + m5;			
			TMatrix4 m7(11, 4, -5, 13, 5, 13, -9, 1, 6, -10, 24, 50, 16, 39, 16, 17);
			Assert::AreEqual(m7, m6);
			m4 += m5;
			Assert::AreEqual(m7, m6);

			TMatrix4 m8(-2, 0, 3, 1, 5, 7, -6, 2, 1, -4, 2, 5, 4, 5, 6, 8);
			TMatrix4 m9 = m6 - m8;
			Assert::AreEqual(m5, m9);
			m6 -= m8;
			Assert::AreEqual(m5, m6);


		}

		TEST_METHOD(Matrix4_Multipy)
		{
			TMatrix4 m1(-2, 0, 3, 1, 5, 7, -6, 2, 1, -4, 2, 5, 4, 5, 6, 8);
			TVector4 v1(3, -1, 4, 7);

			TVector4 v2 = v1*m1;
			TVector4 v3(21, 12, 65, 77);
			TVector3 v33(2, 3, 6);

			TVector4 v4 = m1*v1;
			TVector3 v44 = m1*v33;

			Assert::AreEqual(v3, v2);
			Assert::AreEqual(v3, v4);

			TMatrix4 m2(TMatrix4::CONSTRUCT::IDENTITY);
			TVector3 v5(5, -1, 2);
			TVector3 v6 = v5 * m2;
			Assert::AreEqual(v5, v6);

			TMatrix4 m10(-2, 0, 3, 1, 5, 7, -6, 2, 1, -4, 2, 5, 4, 5, 6, 8);
			TMatrix4 m11 = m10 * 3;
			m10 *= 3;
			Assert::AreEqual(m11, m10);

			TMatrix4 m12(-2, 0, 3, 1, 5, 7, -6, 2, 1, -4, 2, 5, 4, 5, 6, 8);
			TMatrix4 m13(13, 4, -8, 12, 0, 6, -3, -1, 5, -6, 22, 45, 12, 34, 10, 9);
			TMatrix4 m14 = m12 * m13;
			TMatrix4 m15(1, 8, 92, 120, 59, 166, -173, -199, 83, 138, 98, 151, 178, 282, 165, 385);
			Assert::AreEqual(m15, m14);

			Assert::IsFalse(m1.Equals(m15));
		}

		TEST_METHOD(Matrix4_Transpose)
		{
			TMatrix4 m1(13, 4, -8, 12, 0, 6, -3, -1, 5, -6, 22, 45, 12, 34, 10, 9);
			TMatrix4 m2(13, 0, 5, 12, 4, 6, -6, 34, -8, -3, 22, 10, 12, -1, 45, 9);
			TMatrix4 m3(13, 4, -8, 12, 0, 6, -3, -1, 5, -6, 22, 45, 12, 34, 10, 9);
			m1.Transpose();
			Assert::AreEqual(m2, m1);
			m1.Transpose();
			Assert::AreEqual(m3, m1);
			TMatrix4 m4 = m3.GetTranspose();
			Assert::AreEqual(m2, m4);
		}

		TEST_METHOD(Matrix4_Translate)
		{
			TMatrix4 m1;
			m1.Translate(4, 6, 7);

			TMatrix4 m2(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 4, 6, 7, 1);
			Assert::AreEqual(m2, m1);
		}

		TEST_METHOD(Matrix4_Determinant)
		{
			TMatrix4 m(1, -1, 3, 2, -2, -3, 3, 2, 6, -1, -1, 2, -4, -1, 3, -1);
			float result = m.Determinant();
			float expect = -70.0f;
			Assert::AreEqual(expect, result);

			float result2 = D3DXMatrixDeterminant((D3DXMATRIX*)&m);
			Assert::AreEqual(expect, result);
		}


		TEST_METHOD(Matrix4_Rotate)
		{
			TMatrix4 m1;
			TMatrix4 m2;
			TMatrix4 m3;

			float fRad = 30 * PI / 180;
			m1.RotateX(fRad);
			m2.RotateY(fRad);
			m3.RotateZ(fRad);

			TMatrix4 em1(1, 0, 0, 0, 0, (float)sqrt(3) / 2.0f, 0.5f, 0, 0, -0.5f, (float)sqrt(3) / 2.0f, 0, 0, 0, 0, 1);
			bool isEqual1 = m1.Equals(em1);
			Assert::IsTrue(isEqual1);

			TMatrix4 dm1;
			D3DXMatrixRotationX((D3DXMATRIX*)&dm1, fRad);
			Assert::IsTrue(m1.Equals(dm1));

			TMatrix4 em2((float)sqrt(3)/2.0f, 0, -0.5f, 0, 0, 1, 0, 0, 0.5f, 0, (float)sqrt(3) / 2.0f,  0, 0, 0, 0, 1);
			bool isEqual2 = m2.Equals(em2);
			Assert::IsTrue(isEqual2);

			TMatrix4 dm2;
			D3DXMatrixRotationY((D3DXMATRIX*)&dm2, fRad);
			Assert::IsTrue(m2.Equals(dm2));


			TMatrix4 em3((float)sqrt(3) / 2.0f, 0.5f, 0, 0, -0.5f, (float)sqrt(3)/2, 0, 0, 0, 0, 1, 0,  0, 0, 0, 1);
			bool isEqual3 = m3.Equals(em3);
			Assert::IsTrue(isEqual3);

			TMatrix4 dm3;
			D3DXMatrixRotationZ((D3DXMATRIX*)&dm3, fRad);
			Assert::IsTrue(m3.Equals(dm3));
		}

		TEST_METHOD(Matrix4_RotateAxis)
		{
			TMatrix4 m1;
			TVector3 v1(-2, 2, 1);
			v1.Normalize();
			m1.RotateAxis(v1, TMathUtil::GetRadByDegree(30.0f));

			TMatrix4 m2;
			D3DXMatrixRotationAxis((D3DXMATRIX*)&m2, (D3DXVECTOR3*)&v1, TMathUtil::GetRadByDegree(30.0f));
			Assert::IsTrue(m1.Equals(m2));
		}

		TEST_METHOD(Matrix4_Inverse)
		{
			TMatrix4 m(1, 0, 0, 4, 0, 1, 0, 3, 0, 0, 1, 7, 0, 0, 0, 1);
			m.Inverse();
			TMatrix4 m2(1, 0, 0, -4, 0, 1, 0, -3, 0, 0, 1, -7, 0, 0, 0, 1);
			Assert::AreEqual(m2, m);

			TMatrix4 m3(1, 3, -5, 7, 0, 1, 2, 3, 0, 0, 1, 2, 0, 0, 0, 1);
			m3.Inverse();

			TMatrix4 m4(1, 3, -5, 7, 0, 1, 2, 3, 0, 0, 1, 2, 0, 0, 0, 1);
			TMatrix4 m5;
			D3DXMatrixInverse((D3DXMATRIX*)&m5, NULL, (D3DXMATRIX*)&m4);
			Assert::IsTrue(m3.Equals(m5));
		}

		TEST_METHOD(Matrix4_Scale)
		{
			TMatrix4 m1;
			m1.Scale(2, 3, 6);
			TMatrix4 m2(2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 6, 0, 0, 0, 0, 1);
			Assert::AreEqual(m2, m1);
		}

		TEST_METHOD(Matrix4_CameraLookMatrix)
		{
			TVector3 target(0, 0, 20);
			TVector3 position(0, 0, -20);
			TVector3 look = target - position;
			TVector3 up(0, 1, 0);

			TMatrix4 cameraMatrix;
			cameraMatrix.BuildCameraLookAtMatrix(position, look, up);

			TMatrix4 e_cameraMatrix;
			D3DXMatrixLookAtLH((D3DXMATRIX*)&e_cameraMatrix, (D3DXVECTOR3*)&position, (D3DXVECTOR3*)&target, (D3DXVECTOR3*)&up);
			bool isEqual = cameraMatrix.Equals(e_cameraMatrix);
			Assert::IsTrue(isEqual);

			TVector3 target2(114.337f, 147.612f, -131.318f);
			TVector3 position2(160.912f, 149.294f, -178.879f);
			TVector3 up2(0, 1, 0);
			TMatrix4 cameraMatrix2;
			cameraMatrix2.BuildCameraLookAtMatrix(position2, target2-position2, up2);

			TMatrix4 e_cameraMatrix2;
			D3DXMatrixLookAtLH((D3DXMATRIX*)&e_cameraMatrix2, (D3DXVECTOR3*)&position2, (D3DXVECTOR3*)&target2, (D3DXVECTOR3*)&up2);
			bool isEqual2 = cameraMatrix2.Equals(e_cameraMatrix2);
//			Assert::AreEqual(e_cameraMatrix2, cameraMatrix2);
			Assert::IsTrue(isEqual2);
		}

		TEST_METHOD(Matrix4_PerspectiveMatrix)
		{
			float fovy = TMathUtil::GetRadByDegree(30.0f);
			TMatrix4 m1;
			float aspectRadio = 1920.0f / 1440.0f;
			float z_near = 1.0f;
			float z_far = 1000.0f;

			m1.BuildPerspectiveMatrix(fovy, aspectRadio, z_near, z_far);

			TMatrix4 m2;
			D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)&m2, fovy, aspectRadio, z_near, z_far);
			Assert::IsTrue(m1.Equals(m2));
		}

	};
}