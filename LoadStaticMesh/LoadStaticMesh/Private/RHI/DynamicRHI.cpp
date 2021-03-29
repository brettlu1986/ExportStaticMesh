
#include "RHI.h"
#include "DynamicRHI.h"

DynamicRHI* GDynamicRHI = nullptr;

void RHIInit()
{
	if(!GDynamicRHI)
	{
		GDynamicRHI = PlatformCreateDynamicRHI();
	}

	if(GDynamicRHI)
		GDynamicRHI->Init();
}

void RHIPostInit()
{	
	GDynamicRHI->PostInit();
}

void RHIExit()
{
	GDynamicRHI->ShutDown();
	delete GDynamicRHI;
	GDynamicRHI = nullptr;
}