#include "CppUnitTest.h"
#include <Math/TQuaternion.h>
#include "TestMathComon.h"
#include <d3dx9math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestMath
{
	TEST_CLASS(TestQuaternion)
	{
	public:
		TEST_METHOD(Quaternion_AddOperation)
		{
			TQuaternion q1(0.060f, -0.257f, 0.935f, 0.233f);
			TQuaternion q2(-0.752f, 0.374f, 0.495f, 0.280f);

			TQuaternion q3 = q1 + q2;
			TQuaternion q4(-0.692f, 0.117f, 1.43f, 0.513f);
			Assert::IsTrue(q3.Equals(q4));

			q1 += q2;
			Assert::IsTrue(q1.Equals(q4));

			TQuaternion q5(0.060f, -0.257f, 0.935f, 0.233f);
			TQuaternion q6 = q1 - q2;
			Assert::IsTrue(q6.Equals(q5));

			q1 -= q2;
			Assert::IsTrue(q1.Equals(q5));
			Assert::IsFalse(q1.Equals(q2));
		}

		TEST_METHOD(Quaternion_MultipyOperation)
		{
			TQuaternion q1(4, 6, 7, 2);
			TQuaternion q2 = q1 * 3;
			TQuaternion q3 = q1 * 2.0f;

			TQuaternion q4(12, 18, 21, 6);
			TQuaternion q5(8.0, 12.0, 14.0, 4.0);

			Assert::IsTrue(q2.Equals(q4));
			Assert::IsTrue(q3.Equals(q5));

			q1 *= 4;
			TQuaternion q6(16, 24, 28, 8);
			Assert::AreEqual(q1, q6);

			q1 *= 5.0f;
			TQuaternion q7(80.0f, 120.0f, 140.0f, 40.0f);
			Assert::AreEqual(q1, q7);
		}

		TEST_METHOD(Quaternion_DivideOperation)
		{
			TQuaternion q1(4, 6, 7, 2);
			TQuaternion q4(12, 18, 21, 6);
			TQuaternion q5(8.0, 12.0, 14.0, 4.0);

			TQuaternion q6 = q4 / 3;
			TQuaternion q7 = q5 / 2.0f;

			Assert::IsTrue(q1.Equals(q6));
			Assert::IsTrue(q1.Equals(q7));

			TQuaternion q8(80.0f, 120.0f, 140.0f, 40.0f);
			q8 /= 5.0f;		
			TQuaternion q9(16.0f, 24.0f, 28.0f, 8.0f);
			Assert::AreEqual(q9, q8);

			q8 /= 4;
			TQuaternion q10(4, 6, 7, 2);
			Assert::AreEqual(q10, q8);
		}

		TEST_METHOD(Quaternion_Multipy2Operation)
		{
			TQuaternion q1(4, 6, 7, 2);
			TQuaternion q2(12, 18, 21, 6);
			TQuaternion q3(8.0, 12.0, 14.0, 4.0);

			TQuaternion q4 = q1 * q2;

			TQuaternion q5;
			D3DXQuaternionMultiply((D3DXQUATERNION*)&q5, (D3DXQUATERNION*)&q1, (D3DXQUATERNION*)&q2);
			Assert::AreEqual(q5, q4);

			q2 *= q3;
			TQuaternion q6(12, 18, 21, 6);
			TQuaternion q7 = q6 * q3;
			TQuaternion q8;
			D3DXQuaternionMultiply((D3DXQUATERNION*)&q8, (D3DXQUATERNION*)&q6, (D3DXQUATERNION*)&q3);

			bool is_equal = q7 == q8;
			Assert::AreEqual(q7, q2);
			Assert::AreEqual(q8, q2);
		}

		TEST_METHOD(Quaternion_SizeSquare)
		{
			TQuaternion q1(12, 18, 21, 6);
			float f1 = q1.SizeSquared();
			
			float f2 = D3DXQuaternionLengthSq((D3DXQUATERNION*)&q1);
			Assert::AreEqual(f2, f1);
		}

		TEST_METHOD(Quaternion_Normalize)
		{
			TQuaternion q1(12, 18, 21, 6);
			q1.Normalize();

			TQuaternion q2(12, 18, 21, 6);
			TQuaternion q3;
			D3DXQuaternionNormalize((D3DXQUATERNION*)&q3, (D3DXQUATERNION*)&q2);
			Assert::AreEqual(q3, q1);
		}

		TEST_METHOD(Quaternion_Inverse)
		{
			TQuaternion q1(12, 18, 21, 6);
			q1.Normalize();
			q1.Inverse();

			TQuaternion q2(12, 18, 21, 6);
			
			TQuaternion q3;
			q2.Normalize();
			D3DXQuaternionInverse((D3DXQUATERNION*)&q3, (D3DXQUATERNION*)&q2);
			Assert::IsTrue(q1.Equals(q3));

			TQuaternion q4(12, 18, 21, 6);
			q4.Inverse();
			Assert::IsTrue(q4.Equals(q3));
		}

		TEST_METHOD(Quaternion_Conjugate)
		{
			TQuaternion q1(12, 18, 21, 6);
			TQuaternion q2(-12, -18, -21, 6);
			TQuaternion q3(-12, -18, -21, -6);

			TQuaternion q4(-12, -18, -21, 6);
			D3DXQuaternionConjugate((D3DXQUATERNION*)&q4, (D3DXQUATERNION*)&q1);
			
			q1.Conjugate();
		
			Assert::AreEqual(q4, q1);
			Assert::AreEqual(q2, q1);
			Assert::IsTrue(q1 != q3);
		}

		TEST_METHOD(Quaternion_DotProduct)
		{
			TQuaternion q1(4, 6, 7, 2);
			TQuaternion q2(12, 18, 21, 6);
			float f1 = q1.DotProduct(q2);
			float f2 = D3DXQuaternionDot((D3DXQUATERNION*)&q1, (D3DXQUATERNION*)&q2);
			Assert::AreEqual(f2, f1);
		}

		TEST_METHOD(Quaternion_AxisAngle)
		{
			TQuaternion q1, q2;
			TVector3 v1(2, 3, 3);
			v1.Normalize();

			float angel = TMathUtil::GetRadByDegree(30);
			D3DXQuaternionRotationAxis((D3DXQUATERNION*)&q2, (D3DXVECTOR3*)&v1, angel);

			q1.ConvertFromAxisAngle(v1, angel);
			Assert::IsTrue(q1.Equals(q2));

			TQuaternion q3, q4, q5;
			TVector3 v2(0, 1, 0);
			float angel2 = TMathUtil::GetRadByDegree(60);
			D3DXQuaternionRotationAxis((D3DXQUATERNION*)&q3, (D3DXVECTOR3*)&v2, angel2);

			q4.ConvertFromAxisAngle(v2, angel2);
			Assert::IsTrue(q3.Equals(q4));
		}

		TEST_METHOD(Quaternion_ConvertMatrix)
		{
			TMatrix4 m1;
			m1.RotateX(TMathUtil::GetRadByDegree(60));

			TQuaternion q1, q2;
			q1.ConvertFromMatrix(m1);

			D3DXQuaternionRotationMatrix((D3DXQUATERNION*)&q2, (D3DXMATRIX*)&m1);
			Assert::AreEqual(q1, q2);

			TMatrix4 m2;
			q1.ConvertToMatrix(m2);
			Assert::IsTrue(m1.Equals(m2));
		}

		TEST_METHOD(Quaternion_ConvertEulerAngle)
		{
			TQuaternion q1, q2;

			float r1 = TMathUtil::GetRadByDegree(60);
			float r2 = TMathUtil::GetRadByDegree(30);
			float r3 = TMathUtil::GetRadByDegree(0);

			q1.ConvertFromEulerAngle(r1, r2, r3);
			D3DXQuaternionRotationYawPitchRoll((D3DXQUATERNION*)&q2, r2, r1, r3);

			Assert::AreEqual(q1, q2);

			float pitch, yaw, roll;
			q1.ConvertToEulerAngle(pitch, yaw, roll);
			Assert::AreEqual(r1, pitch, 0.001f);
			Assert::AreEqual(r2, yaw, 0.001f);
			Assert::AreEqual(r3, roll, 0.001f);
		}

		TEST_METHOD(Quaternion_Lerp)
		{
			TQuaternion q1(6, 8, 10, 4);
			TQuaternion q2(4, 12, 18, 24);
			TQuaternion q3;
			TQuaternion::Lerp(q1, q2, 0.5, &q3);
			TQuaternion q4(5, 10, 14, 14);

			Assert::AreEqual(q3, q4);
		}

		TEST_METHOD(Quaternion_SLerp)
		{
			TQuaternion q1(6, 8, 10, 4);
			TQuaternion q2(4, 12, 18, 24);
			q1.Normalize();
			q2.Normalize();

			TQuaternion q3, q4;
			TQuaternion::Slerp(q1, q2, 0.3f, &q3);
			D3DXQuaternionSlerp((D3DXQUATERNION*)&q4, (D3DXQUATERNION*)&q1,  (D3DXQUATERNION*)&q2, 0.3f);

			Assert::IsTrue(q3.Equals(q4));
		}

		TEST_METHOD(Quaternion_Squad)
		{
			TQuaternion q1(6, 8, 10, 4);
			TQuaternion q2(4, 12, 18, 24);
			TQuaternion q3(24, -12, 16, 34);
			TQuaternion q4(7, -8, 18, 45);
			TQuaternion q5, q6, q7, q8;

			q1.Normalize();
			q2.Normalize();
			q3.Normalize();
			q4.Normalize();

			TQuaternion::Slerp(q1, q2, 0.3f, &q5);
			D3DXQuaternionSlerp((D3DXQUATERNION*)&q6, (D3DXQUATERNION*)&q1, (D3DXQUATERNION*)&q2, 0.3f);
			Assert::IsTrue(q5.Equals(q6));

			TQuaternion::Slerp(q3, q4, 0.3f, &q7);
			D3DXQuaternionSlerp((D3DXQUATERNION*)&q8, (D3DXQUATERNION*)&q3, (D3DXQUATERNION*)&q4, 0.3f);
			Assert::IsTrue(q7.Equals(q8));

			TQuaternion q9, q10;
			TQuaternion::Squad(q1, q2, q3, q4, 0.3f, &q9);
			q9.Normalize();
			
			D3DXQuaternionSquad((D3DXQUATERNION*)&q10, (D3DXQUATERNION*)&q1, (D3DXQUATERNION*)&q3, (D3DXQUATERNION*)&q4, (D3DXQUATERNION*)&q2, 0.3f);
			q10.Normalize();
			Assert::IsTrue(q9.Equals(q10));
		}

		TEST_METHOD(Quaternion_GetAngel)
		{
			TQuaternion q1, q2;
			TVector3 v1(0, 1, 0);
			v1.Normalize();

			float angel1 = TMathUtil::GetRadByDegree(30);
			float angel2 = TMathUtil::GetRadByDegree(70);

			q1.ConvertFromAxisAngle(v1, angel1);
			q2.ConvertFromAxisAngle(v1, angel2);

			float angel3 = TQuaternion::GetDiffAngle(q1, q2);
			Assert::AreEqual(40.0f, angel3, 0.0001f);
		}
	};
}
