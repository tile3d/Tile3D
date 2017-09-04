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
		}
	};
}