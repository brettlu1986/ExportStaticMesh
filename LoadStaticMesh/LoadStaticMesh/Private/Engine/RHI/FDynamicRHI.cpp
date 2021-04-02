
#include "FRHI.h"
#include "FDynamicRHI.h"

FDynamicRHI* GDynamicRHI = nullptr;

void RHIInit()
{
	if(!GDynamicRHI)
	{
		GDynamicRHI = PlatformCreateDynamicRHI();
	}

	if(GDynamicRHI)
		GDynamicRHI->Init();
}

void RHIExit()
{
	GDynamicRHI->ShutDown();
	delete GDynamicRHI;
	GDynamicRHI = nullptr;
}