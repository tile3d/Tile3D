#include "CppUnitTest.h"
#include <Math/TVector3.h>
#include "TestMathComon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMath
{
	TEST_CLASS(TestVector3)
	{
	public:
		TEST_METHOD(Vector3_Operation)
		{
			TVector3 v1(2.0, 3.0, 4.0);
			TVector3 v2(5.0, 6.0, 7.0);
			TVector3 v3 = v1 + v2;
			TVector3 v4(7.0, 9.0, 11.0);
			Assert::IsTrue(v3 == v4);

			TVector3 v5(-5.4f, 0, 3);
			TVector3 v6 = v5 * -3;
			TVector3 v7(16.2f, 0, -9);
			Assert::AreEqual(v6, v7);

			TVector3 v8(4, 5, 6);
			TVector3 v9(7, -3, 0);
			TVector3 v10(1, 2, 3);
			TVector3 v11 = v8 + v9 - v10;
			TVector3 v12(10, 0, 3);
			Assert::AreEqual(v11, v12);


			TVector3 v13(2, 4, 5);
			TVector3 v14(23, 7, 8);
			TVector3 v15 = v13 * v14;
			TVector3 v16(46, 28, 40);
			Assert::AreEqual(v16, v15);

			bool is_equal = (v15 == v16);
			bool is_equal2 = (v13 == v14);
			Assert::IsTrue(is_equal);
		}

		TEST_METHOD(Vector3_Dot)
		{
			TVector3 p(2, 2, 1);
			TVector3 q(1, -2, 0);
			float dot = p.DotProduct(q);
			Assert::AreEqual(dot, -2.0f);

			TVector3 x(-5, 1, 3);
			TVector3 y(4, -13, 9);
			float z = 10 + x.DotProduct(y);
			Assert::AreEqual(z, 4.0f);
		}

		TEST_METHOD(Vector3_CrossProduct)
		{
			TVector3 p(1, 3, 4);
			TVector3 q(2, -5, 8);
			TVector3 w = p.CrossProduct(q);
			TVector3 e(44, 0, -11);
			Assert::AreEqual(e, w);

			TVector3 v1(8, -7, 4);
			TVector3 v2(3, 10, 7);
			TVector3 v3 = v1.CrossProduct(v2);
			TVector3 v4(-89, -44, 101);
			Assert::AreEqual(v4, v3);

			TVector3 v5 = TVector3::CrossProduct(v1, v2);
			Assert::AreEqual(v5, v4);

			TVector3 v6(8, -7, 4);
			TVector3 v7(3, 10, 7);
			float v8 = v6.CrossProduct(v7).DotProduct(v7);
			Assert::AreEqual(0.0f, v8);
		}


		TEST_METHOD(Vector3_Length)
		{
			TVector3 v1(5, -4, 7);
			float len = v1.Length();
			Assert::AreEqual(len, 9.48683f, 0.0001f);

			float square_len = v1.SquaredLength();
			Assert::AreEqual(square_len, 90.0f);
		}

		TEST_METHOD(Vector3_GetCosAngel2D)
		{
			TVector3 v1(4, 0, 7);
			TVector3 v2(0, 4, 7);
			float v3 = v1.GetCosAngel2D(v2);
			Assert::AreEqual(0.0f, v3);
		}

		TEST_METHOD(Vector3_Normalize)
		{
			TVector3 v1(1, 2, 2);
			TVector3 v2 = v1.Normalize();
			TVector3 v3(1.0f / 3, 2.0f / 3, 2.0f / 3);

			Assert::AreEqual(v2, v3);

			TVector3 v4(0.0f, 0.0f, 0.0f);
			v4.Normalize();
		}


		TEST_METHOD(Vector3_Project)
		{
			TVector3 p(4, 3, -1);
			float a = (float)sqrt(2) / 2;
			TVector3 q(a, a, 0);
			TVector3 h = p.Project(q);
			TVector3 e_project(7.0f / 2, 7.0f / 2, 0);
			Assert::AreEqual(h, e_project);

			TVector3 perp = p.Perp(q);
			TVector3 e_perp(0.5f, -0.5f, -1.0f);
			Assert::AreEqual(perp, e_perp);
		}

		TEST_METHOD(Vector3_Clear)
		{
			TVector3 v1(92, 221, 2121);
			v1.Clear();
			Assert::AreEqual(TVector3::ZeroVector, v1);
		}


	};
}
