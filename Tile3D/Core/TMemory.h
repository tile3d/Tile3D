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

