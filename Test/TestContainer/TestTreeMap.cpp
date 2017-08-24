#include "CppUnitTest.h"
#include <Container/TTreeMap.h>
#include <time.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestContainer
{
	TEST_CLASS(TestTreeMap)
	{
	public:
		TEST_METHOD(Map_Insert)
		{
			TTreeMap<int, int> tm;
//			tm.Insert(4, 5);
		}
	};
}