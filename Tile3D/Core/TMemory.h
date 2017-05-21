/*
* FILE: TMemory.h
*
* DESCRIPTION: TMemory class
*
* CREATED BY: liansm
*
* HISTORY:
*
* Copyright (c) 2017 Tile3D Ltd, All Rights Reserved.
*/

#pragma once



//
// Design Goal
// 1) Manager all the memory alloc and release, include other library (STL and third-party)
// 2) Use TLS to reduce the lock race condition (TBD)
// 3) Allocate the memory by the Page to reduce the Page swap 
// 4) Memmory Leak Checker
// 5) Memory Alloc/Release/.. staticts
// 6) Illegae memory access check (double free/access the not alloacted memory) (TBD)
// 7) Support both 32 and 64 bit version (TBD)
// 8) Reduce the memory footprint?(TBD)
// 9) Use TArray instead of TMemVec, Use THashMap instead of std::map(TBD)
// 10) Impelment the TMemStack to alloc and free the memory in the same block (TBD)
//
class TMemory
{
public:
	static void * Alloc(size_t size);
	static void Free(void * p);
};

void * operator new (size_t size);
void operator delete(void * p);
void * operator new[](size_t size);
void operator delete[](void * p);

