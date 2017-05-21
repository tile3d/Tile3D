#include <Core/TMemory.h>

class Foo
{
public:
	int x;
	int y;
	char name[20];
};

int main()
{
	Foo * pFoo = (Foo*)TMemory::Alloc(sizeof(Foo));
	pFoo->x = 3;
	pFoo->y = 4;
	delete pFoo;

	return 0;
}

