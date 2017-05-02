#include "CppUnitTest.h"
#include <Math/TPlane.h>
#include <Math/TMathUtil.h>
#include "TestMathComon.h"
#include <d3dx9math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace TestPlane
{
	TEST_CLASS(TestTPlane)
	{
	public:
		TEST_METHOD(Plane_Ctor)
		{
			TVector3 v3(1.0f, 1.0f, -1.0f), v2(2.0f, 3.0f, 0.0f), v1(1.5f, 4.0f, -2.0f);
			TPlane p1(v1, v2, v3);

			D3DXPLANE p2;
			D3DXPlaneFromPoints(&p2, (D3DXVECTOR3*)&v1, (D3DXVECTOR3*)&v2, (D3DXVECTOR3*)&v3);

			Assert::AreEqual(p1.m_distance, -p2.d);
			Assert::AreEqual(p1.m_normal.m_x, p2.a);
			Assert::AreEqual(p1.m_normal.m_y, p2.b);
			Assert::AreEqual(p1.m_normal.m_z, p2.c);
		}

		TEST_METHOD(Plane_InPlane)
		{
			TVector3 v1(1.0f, 1.0f, -1.0f), v2(2.0f, 3.0f, 0.0f), v3(1.5f, 4.0f, -2.0f);
			TPlane p1(v1, v2, v3);
			bool is_inPlane = p1.InPlane(v2);
			Assert::IsTrue(is_inPlane);
		}

		TEST_METHOD(Plane_GetDistance)
		{
			TVector3 v(0, 1, 0);
			TPlane p1(v, 5);
			TVector3 v2(0, 2, 0);

			float dis = p1.GetDistance(v2);
			Assert::AreEqual(dis, -3.0f);
		}


		TEST_METHOD(Plane_Intersect)
		{
			TVector3 v(0, 1, 0);
			TPlane p1(v, 5);
			
			TVector3 v1(0, 2, 0);
			TVector3 v2(0, 7, 0);

			TVector3 v3 = p1.Intersect(v1, v2);
			TVector3 v4(0, 5, 0);
			Assert::AreEqual(v4, v3);

		}

		TEST_METHOD(Plane_IsIntersect)
		{
			TVector3 v(0, 1, 0);
			TPlane p1(v, 5);

			TVector3 v1(0, 2, 0);
			TVector3 v2(0, 7, 0);
			TVector3 v3(0, 4, 0);

			bool is_intersect1 = p1.IsIntersect(v1, v2);
			bool is_intersect2 = p1.IsIntersect(v1, v3);

			Assert::IsTrue(is_intersect1);
			Assert::IsFalse(is_intersect2);
		}

		TEST_METHOD(Plane_Transform)
		{
			TPlane p(0, 1, 1, 0);
			p.Normalize();

			TMatrix4 m(TMatrix4::IDENTITY);
			m.Scale(1.0f, 2.0f, 3.0f);
			m.Inverse();
			m.Transpose();
			TPlane p2;
			D3DXPlaneTransform((D3DXPLANE*)&p2, (D3DXPLANE*)&p, (D3DXMATRIX*)&m);
			p2.Normalize();

			TMatrix4 m2(TMatrix4::IDENTITY);
			m2.Scale(1.0f, 2.0f, 3.0f);
			p.Transform(m2);
			p.Normalize();

			Assert::AreEqual(p2, p);
		}
	};
}