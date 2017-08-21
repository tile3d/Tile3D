#include "TMemory.h"
#include "Memory/TMemMan.h"

void * TMemory::Alloc(size_t size)
{
	return TMemMan::GetInstance()->Malloc(size);
}

void TMemory::Free(void * p)
{
	TMemMan::GetInstance()->Free(p);
}

void * TMemory::Realloc(void * p, size_t size)
{
	return TMemMan::GetInstance()->Realloc(p, size);
}

void * operator new (size_t size)
{
	return TMemory::Alloc(size);
}

void operator delete(void * p)
{
	TMemory::Free(p);
}

void * operator new[] (size_t size)
{
	return TMemory::Alloc(size);
}

void operator delete[](void * p)
{
	TMemory::Free(p);
}

void * operator new(size_t size, void * p)
{
	return TMemory::Realloc(p, size);
}

void * operator new[](size_t size, void * p)
{
	return TMemory::Realloc(p, size);
}
