#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <Core/TMemory.h>

class TestCtor
{

public:
	TestCtor()
	{
		printf("enter TestCtor ctor\n");
		x = 1;
		y = 2;
	}


	~TestCtor()
	{
		printf("enter TestCtor dtor\n");
		x = 3;
		y = 4;
	}
private:
	int x;
	int y;

};


int TestMemory()
{
	TestCtor * p1 = new TestCtor;
	delete p1;

	TestCtor * p2 = new TestCtor[5];
	delete[] p2;

	char * p3 = new char[32];
	p3 = new(p3) char[64];

	*(p3 + 33) = 'a';
	delete[] p3;
	return 0;
}
