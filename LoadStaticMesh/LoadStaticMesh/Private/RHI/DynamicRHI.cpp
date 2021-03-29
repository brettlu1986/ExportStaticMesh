
#include "RHI.h"
#include "DynamicRHI.h"

DynamicRHI* GDynamicRHI = nullptr;

void RHIInit()
{
	if(!GDynamicRHI)
	{
		GDynamicRHI = PlatformCreateDynamicRHI();
	}
}

void RHIPostInit()
{

}

void RHIExit()
{

}