#include "CppUnitTest.h"
#include <Container/TArray.h>
#include <vector>
#include <time.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template<typename T> bool IsSame(std::vector<T> & vec, TArray<T> & arr)
{
	if (vec.size() != arr.Size()) return false;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec[i] != arr[i]) {
			return false;
		}
	}
	return true;
}

namespace TestContainer
{
	TEST_CLASS(TestArray)
	{
	public:
		TEST_METHOD(Array_Add)
		{
			TArray<int> arr1;
			std::vector<int> vec1;

			arr1.Add(2);
			arr1.Add(3);
			arr1.Add(6);
			arr1.Add(89);
			arr1.Add(39);
			arr1.Add(59);
			arr1.Add(79);

			vec1.push_back(2);
			vec1.push_back(3);
			vec1.push_back(6);
			vec1.push_back(89);
			vec1.push_back(39);
			vec1.push_back(59);
			vec1.push_back(79);

			int v = arr1[4];
			Assert::AreEqual(39, v);

			bool check = IsSame(vec1, arr1);
			Assert::IsTrue(check);
		}

		TEST_METHOD(Array_Add_Performace)
		{
			TArray<int> arr1;
			std::vector<int> vec1;
			srand((int)(time(NULL)));

			time_t t1 = time(NULL);
			for (size_t i = 0; i < 10000000; ++i) {
				int value = rand() % 100000;
				arr1.Add(value);
			}
			time_t t2 = time(NULL);
			printf("time consumed1: %d\n", (int)(t2 - t1));

/*
			for (size_t i = 0; i < 10000000; ++i) {
				int value = rand() % 100000;
				vec1.push_back(value);
			}
			time_t t3 = time(NULL);
			printf("time consumed2: %d\n", (int)(t3 - t2));
*/
		}

		TEST_METHOD(Array_Add_Pointer)
		{
			class ArrayFoo
			{
			public:
				ArrayFoo(int x, int y, int z) {
					m_x = x;
					m_y = y;
					m_z = z;
				}

			private:
				int m_x;
				int m_y;
				int m_z;
			};

			TArray<ArrayFoo*> arr1;
			ArrayFoo * f1 = new ArrayFoo(2, 4, 6);
			ArrayFoo * f2 = new ArrayFoo(5, 7, 9);
			ArrayFoo * f3 = new ArrayFoo(7, 9, 10);

			arr1.Add(f1);
			arr1.Add(f2);
			arr1.Add(f3);

			size_t count = arr1.Size();
			Assert::AreEqual(3, (int)count);
		}

		TArray<int> TestArrayMoveCtor()
		{
			TArray<int> arr;
			arr.Add(3);
			arr.Add(5);
			return arr;
		}

		TEST_METHOD(Array_Ctor_Move)
		{
			TArray<int> arr = TestArrayMoveCtor();
			TArray<int> arr2;
			arr2.Add(3);
			arr2.Add(5);
			bool isEqual = arr.Equal(arr2);
			Assert::IsTrue(isEqual);
		}

	};
}
