
#include "FRHI.h"
#include "FDynamicRHI.h"

FDynamicRHI* GDynamicRHI = nullptr;

bool RHICreate()
{
	if (!GDynamicRHI)
	{
		GDynamicRHI = PlatformCreateDynamicRHI();
		return true;
	}
	return false;
}

void RHIInit()
{
	if(GDynamicRHI)
		GDynamicRHI->Init();
}

void RHIExit()
{
	GDynamicRHI->ShutDown();
	delete GDynamicRHI;
	GDynamicRHI = nullptr;
}