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
			int * p = (int*)(TMemory::Alloc(12));
			int * q = p;

			*p = 4;
			*(p + 1) = 5;
			*(p + 2) = 6;

			printf("value=%d\n", *q);
		}
	};
}

