#include "TObject.h"
#include "TMemory.h"

TObject * TObject::Clone()
{
	TObject * pObject = new TObject();
	return pObject;
}

void * TObject::operator new(size_t nSize)
{
	return TMemory::Alloc(nSize);
}

void TObject::operator delete(void * pData)
{
	return TMemory::Free(pData);
}

void * TObject::operator new[](size_t nSize)
{
	return TMemory::Alloc(nSize);
}

void TObject::operator delete[](void * pData)
{
	return TMemory::Free(pData);
}

void * TObject::operator new(size_t size, void * p)
{
	return TMemory::Realloc(p, size);
}
