#include "CppUnitTest.h"
#include <Math/TMatrix3.h>
#include "TestMathComon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMath
{		
	TEST_CLASS(TestTMatrix3)
	{
	public:	
		TEST_METHOD(Matrix3_Operation)
		{
			TMatrix3 m1(TMatrix3::CONSTRUCT::IDENTITY);
			TMatrix3 m2(TMatrix3::CONSTRUCT::CLEARED);

			TMatrix3 m3(m1);
			Assert::AreEqual(m1, m3);
			Assert::AreNotEqual(m1, m2);
		}

		TEST_METHOD(Matrix3_MultipyVector)
		{
			TMatrix3 m1(-2, 0, 3, 5, 7, -6, 1, -4, 2);
			TVector3 v1(3, -1, 4);
		
			TVector3 v2 = v1*m1;
			TVector3 v3(-7, -23, 23);
			TVector3 v4 = m1*v1;

			Assert::AreEqual(v3, v2);
			Assert::AreEqual(v3, v4);

			TMatrix3 m2(TMatrix3::CONSTRUCT::IDENTITY);
			TVector3 v5(5, -1, 2);
			TVector3 v6 = v5 * m2;
			Assert::AreEqual(v5, v6);
		}

		TEST_METHOD(Matrix3_Transpose)
		{
			TMatrix3 m1(13, 4, -8, 12, 0, 6, -3 , -1 , 5);
			TMatrix3 m2(13, 12, -3, 4, 0, -1, -8, 6, 5);
			TMatrix3 m3(13, 4, -8, 12, 0, 6, -3, -1, 5);
			m1.Transpose();
			Assert::AreEqual(m2, m1);
			m1.Transpose();
			Assert::AreEqual(m3, m1);
		}

		TEST_METHOD(Matrix3_Translate)
		{
			TMatrix3 m1;
			m1.Translate(4, 6);

			TMatrix3 m2(1, 0, 0, 0, 1, 0, 4, 6, 1);
			Assert::AreEqual(m2, m1);
		}

		TEST_METHOD(Matrix3_Determinant)
		{
			float f[3][3] = { -4, -3, 3, 0, 2, -2, 1, 4, -1 };
			TMatrix3 m(f);
			float result = m.Determinant();
			float expect = -24.0f;
			Assert::AreEqual(result, expect);
		}

		#define PI 3.1415926535f
		TEST_METHOD(Matrix3_Rotate)
		{
			TMatrix3 m;
			float fRad = 22 * PI / 180;
			m.Rotate(fRad);
		}

		TEST_METHOD(Matrix3_Inverse)
		{
			TMatrix3 m(3, -2, 0, 1, 4, 0, 0, 0, 2);
			TMatrix3 e(2.0f / 7, 1.0f / 7, 0, -1.0f / 14, 3.0f / 14, 0, 0, 0, 1.0f / 2);
			TMatrix3 e2(1.0f / 7, 1.0f / 7, 0, -1.0f / 14, 3.0f / 14, 0, 0, 0, 1.0f / 2);
			m.InverseTM();
			bool is_equal = m.Equals(e);
			bool not_equal = m.Equals(e2);

			Assert::IsTrue(is_equal);
			Assert::IsFalse(not_equal);
		}
	};
}