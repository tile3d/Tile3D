#include "CppUnitTest.h"
#include <Math/TVector4.h>
#include "TestMathComon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestMath
{
	TEST_CLASS(TestVector4)
	{
	public:
		TEST_METHOD(Vector4_Operation)
		{
			TVector4 v1(2.0, 3.0, 4.0, 5.0);
			TVector4 v2(5.0, 6.0, 7.0, 6.0);
			TVector4 v3 = v1 + v2;
			TVector4 v4(7.0, 9.0, 11.0, 11.0);
			Assert::IsTrue(v3==v4);

			TVector4 v5(-5.4f, 0, 3, 6);
			TVector4 v6 = v5 * -3;
			TVector4 v7(16.2f, 0, -9, -18);
			Assert::AreEqual(v6, v7);

			TVector4 v8(4, 5, 6, 0);
			TVector4 v9(7, -3, 0, 0);
			TVector4 v10(1, 2, 3, 0);
			TVector4 v11 = v8 + v9 - v10;
			TVector4 v12(10, 0, 3, 0);
			Assert::AreEqual(v11, v12);


			TVector4 v13(2, 4, 5, 9);
			TVector4 v14(23, 7, 8, 10);
			TVector4 v15 = v13 * v14;
			TVector4 v16(46, 28, 40, 90);
			Assert::AreEqual(v16, v15);


			bool is_equal = (v13 == v14);
		}

		TEST_METHOD(Vector4_Dot)
		{
			TVector4 p(2, 2, 4, 0);
			TVector4 q(1, -2, 7, 0);
			float dot = p.DotProduct(q);
			Assert::AreEqual(dot, 26.0f);
		}

		TEST_METHOD(Vector4_CrossProduct)
		{
			TVector4 p(1, 3, 4, 0);
			TVector4 q(2, -5, 8, 0);
			TVector4 w = p.CrossProduct(q);
			TVector4 e(44, 0, -11, 0);
			Assert::AreEqual(e, w);
		}


		TEST_METHOD(Vector4_Length)
		{
			TVector4 v1(5, -4, 7, 0);
			float len = v1.Length();
			Assert::AreEqual(len, 9.48683f, 0.0001f);

			float square_len = v1.SquaredLength();
			Assert::AreEqual(square_len, 90.0f);
		}

		TEST_METHOD(Vector4_Normalize)
		{
			TVector4 v1(1, 2, 2, 1);
			TVector4 v2 = v1.Normalize();
			TVector4 v3(1.0f/3, 2.0f/3, 2.0f/3, 1.0f/3);

			Assert::AreEqual(v2, v3);

			TVector4 v4(0.0f, 0.0f, 0.0f, 0.0f);
			v4.Normalize();
		}


		TEST_METHOD(Vector4_Project)
		{
			TVector4 p(4, 3, -1, 0);
			float a = (float)sqrt(2) / 2;
			TVector4 q(a, a, 0, 0);
			TVector4 h = p.Project(q);
			TVector4 e_project(7.0f / 2, 7.0f / 2, 0, 0.0f);
			Assert::AreEqual(h, e_project);

			TVector4 perp = p.Perp(q);
			TVector4 e_perp(0.5f, -0.5f, -1.0f, 0.0f);
			Assert::AreEqual(perp, e_perp);
		}


		TEST_METHOD(Vector4_Clear)
		{
			TVector4 v1(92, 221, 2121, 22);
			v1.Clear();
			TVector4 v2(0, 0, 0, 0);
			Assert::AreEqual(v2, v1);
		}
	};
}
