#include <Container/TArray.h>
#include <Core/TMemory.h>

struct Student
{
	int number;
	int sid;

	Student(int n, int s) : number(n), sid(s) {}
};

void TestArrayLeak1()
{
	TArray<int> a;
	a.Add(2);
	a.Add(3);
	a.Add(100);

	a.Clear();
}

void TestArrayLeak2()
{
	TArray<Student*> ta;

	ta.Add(new Student(2, 2));
	ta.Add(new Student(3, 3));
	
	ta.ClearAll();
}



void TestArrayLeak()
{
	TestArrayLeak1();
	TestArrayLeak2();
}

