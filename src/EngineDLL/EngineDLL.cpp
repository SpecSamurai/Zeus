#include "framework.h"
#include "EngineDLL.h"

ENGINEDLL_API int nEngineDLL=0;

ENGINEDLL_API int fnEngineDLL(void)
{
    return 42;
}

CEngineDLL::CEngineDLL()
{
    return;
}
