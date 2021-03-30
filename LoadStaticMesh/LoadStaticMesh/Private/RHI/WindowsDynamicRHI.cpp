
#include "RHI.h"
#include "D3D12RHIPrivate.h"

EDynamicModuleType DynamicModuleType = EDynamicModuleType::MODULE_D3D12;

static IDynamicRHIModule* LoadDynamicRHIModule()
{
	IDynamicRHIModule* DynamicRHIModule = nullptr;

	switch(DynamicModuleType)
	{
	case EDynamicModuleType::MODULE_D3D12:
	{
		DynamicRHIModule = new D3D12DynamicRHIModule();
		if (!DynamicRHIModule->IsSupported())
		{
			DynamicRHIModule = nullptr;
		}
	}
	break;
	default:{}
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