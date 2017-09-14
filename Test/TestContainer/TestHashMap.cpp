#include "CppUnitTest.h"
#include <Container/THashMap.h>
#include <time.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestContainer
{
	TEST_CLASS(TestHashMap)
	{
	public:
		TEST_METHOD(HashMap_Ctor)
		{
			THashMap<int, int> tm;
			tm.Put(2, 5);
			int * pValue = tm.Find(2);
			int value = *pValue;
			Assert::AreEqual(5, value);
		}

		TEST_METHOD(HashMap_Put)
		{
			THashMap<int, int> tm;
			bool p1 = tm.Put(3, 5);
			bool p2 = tm.Put(3, 6);
			Assert::IsTrue(p1);
			Assert::IsFalse(p2);
		}
	};
}