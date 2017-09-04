/*
* FILE: TObject.h
*
* DESCRIPTION: TObject class
*
* CREATED BY: liansm
*
* HISTORY:
*
* Copyright (c) 2017 Tile3D Ltd, All Rights Reserved.
*/

#pragma once

#include "TClassID.h"
#include <Container/TString.h>

//Design Goal
//1) Support Reference Count?
//2) Support GC?
//3) RTTI (ClassID/ClassName)
//4) Reflection
//5) Clone
//6) ToString 
//7) How to calculate the actual object size(considering pointer/stl/variable string or array)

class TObject
{
public:
	TObject() {}
	virtual ~TObject() {}
	
	//Create and return a copy of this object
	virtual TObject * Clone();

	virtual int GetClassID() = 0;
	virtual TString GetClassName() = 0;

	//Disable assigment ctor, use clone
	TObject & operator=(TObject& obj) const = delete;

	//Disable copy ctor(need rethink)
	TObject(TObject & obj) = delete;

public:
	void * operator new(size_t nSize);
	void operator delete(void * pData);

	void * operator new[](size_t nSize);
	void operator delete[](void * pData);

	void * operator new(size_t size, void * p);

};


