// EngineDLL.cpp : Defines the exported functions for the DLL.
//

#include "framework.h"
#include "EngineDLL.h"


// This is an example of an exported variable
ENGINEDLL_API int nEngineDLL=0;

// This is an example of an exported function.
ENGINEDLL_API int fnEngineDLL(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
CEngineDLL::CEngineDLL()
{
    return;
}