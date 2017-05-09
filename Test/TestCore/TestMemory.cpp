#include "CppUnitTest.h"
#include <Core/TMemory.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMemory
{
	TEST_CLASS(TestTMemory)
	{
	public:
		TEST_METHOD(Memory_Alloc)
		{
			int a = 5;
			int c = 6;
			Assert::AreEqual(11, a + c);
		}
	};
}

