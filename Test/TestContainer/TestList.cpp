#include "CppUnitTest.h"
#include <Container/TList.h>
#include <list>
#include <time.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


template<typename T> bool IsSame(std::list<T> & list1, TList<T> & list2)
{
	if (list1.size() != list2.Size()) return false;

	typename TList<T>::TListNode * pNode = list2.GetHead();
	typename std::list<T>::iterator iter = list1.begin();

	for (; iter != list1.end(); ++iter) {
		if (*iter != pNode->m_value) {
			return false;
		}
		pNode = pNode->m_pNext;
	}
	return true;
}

namespace TestContainer
{
	TEST_CLASS(TestArray)
	{
	public:
		TEST_METHOD(List_Add)
		{
			TList<int> list1;
			std::list<int> list2;

			list1.AddTail(2);
			list1.AddTail(3);
			list1.AddTail(6);
			list1.AddTail(89);
			list1.AddTail(39);
			list1.AddTail(59);
			list1.AddTail(79);
			Assert::AreEqual(7, list1.Size());


			list2.push_back(2);
			list2.push_back(3);
			list2.push_back(6);
			list2.push_back(89);
			list2.push_back(39);
			list2.push_back(59);
			list2.push_back(79);

			bool check = IsSame(list2, list1);
			Assert::IsTrue(check);

			list1.RemoveHead();
			Assert::AreEqual(6, list1.Size());

			list1.RemoveTail();
			Assert::AreEqual(5, list1.Size());

			list2.pop_front();
			list2.pop_back();
			bool check2 = IsSame(list2, list1);
			Assert::IsTrue(check2);


			TList<int>::TListNode * pNode = list1.Find(39);
			if (pNode) {
				list1.Remove(pNode);
			}

		}


	};
}
