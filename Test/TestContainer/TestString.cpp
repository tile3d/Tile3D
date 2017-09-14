#include "CppUnitTest.h"
#include <Container/TString.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


class TestData
{
public:
	TString hookdata;
};

namespace TestContainer
{
	TEST_CLASS(TestHashMap)
	{
	public:
		TEST_METHOD(String_Ctor)
		{
		}

		TEST_METHOD(String_Dtor)
		{
			TestData * pTestData = new TestData();
			const char* name = "hello";
			pTestData->hookdata = name;
			delete pTestData;
		}
	};
}