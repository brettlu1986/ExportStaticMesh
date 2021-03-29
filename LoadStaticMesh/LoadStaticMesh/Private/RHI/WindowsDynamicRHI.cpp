
#include "RHI.h"

static bool ShouldPreferD3d12()
{
	return true;
}

static void LoadDynamicRHIModule()
{
	bool bPreferD3d12 = ShouldPreferD3d12();
	IDynamicRHIModule* DynamicRHIModule = nullptr;
	if(bPreferD3d12)
	{
		//DynamicRHIModule
	}
}


DynamicRHI* PlatformCreateDynamicRHI()
{
	DynamicRHI* DyRHI = nullptr;

	return DyRHI;
}