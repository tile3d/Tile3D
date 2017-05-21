#include "CppUnitTest.h"
#include <Core/TMemory.h>
#include <Core/TObject.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMemory
{
	TEST_CLASS(TestTMemory)
	{
	public:
		TEST_METHOD(Memory_Alloc)
		{
			int * p = new int[12];
			int * q = p;

			*p = 4;
			*(p + 1) = 5;
			*(p + 2) = 6;

			printf("value=%d\n", *q);
		}

		class Foo
		{
		public:
			int x;
			int y;
			char name[20];
		};

		class TBar : public TObject
		{
		public:
			TBar() {
				bar_id = 0;
				printf("TBar ctor\n");
			}

			~TBar() {
				printf("TBar dtor\n");
			}

		private:
			int bar_id;
		};

		TBar *  test_bar()
		{
			TBar * pBar = new TBar();
			return pBar;
		}

		TEST_METHOD(Memory_Leak)
		{
			Foo * pFoo = new Foo();
			pFoo->x = 3;
			pFoo->y = 4;
//			delete pFoo;

			Foo ** pFoo2 = new Foo*[1024];
//			delete[] pFoo2;

			TBar * pBar = test_bar();
//			delete pBar;

			TBar * pBar2 = new TBar[36]();
//			delete[] pBar2;
		}
	};
}

