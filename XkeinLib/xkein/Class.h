#pragma once


#include<typeinfo>
#include "tools.h"
#include "Memory.h"

XKEINNAMESPACE_START

class ClassInformation
{
public:
	char* Name;
	MemoryBuffer ClassData;
};

class Object
{
	virtual ~Object() = 0;
	virtual ClassInformation GetClassInformation() = 0;
};


#define EXPORTCLASS




XKEINNAMESPACE_END