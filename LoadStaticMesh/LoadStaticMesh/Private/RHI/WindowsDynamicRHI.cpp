
#include "RHI.h"
#include "D3D12RHIPrivate.h"

static bool ShouldPreferD3d12()
{
	return true;
}

static IDynamicRHIModule* LoadDynamicRHIModule()
{
	//choose platform here
	bool bPreferD3d12 = ShouldPreferD3d12();
	IDynamicRHIModule* DynamicRHIModule = nullptr;

	if(bPreferD3d12)
	{
		DynamicRHIModule = new D3D12DynamicRHIModule();
		if (!DynamicRHIModule->IsSupported())
		{
			DynamicRHIModule = nullptr;
		}
	}
	return DynamicRHIModule;
}

DynamicRHI* PlatformCreateDynamicRHI()
{
	DynamicRHI* DyRHI = nullptr;

	IDynamicRHIModule* DynamicRHIModule = LoadDynamicRHIModule();
	if(DynamicRHIModule)
	{
		DyRHI = DynamicRHIModule->CreateRHI();
	}

	return DyRHI;
}