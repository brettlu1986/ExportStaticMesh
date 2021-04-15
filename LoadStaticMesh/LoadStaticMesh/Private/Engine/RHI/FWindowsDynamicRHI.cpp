
#include "FRHI.h"
#include "FD3D12RHIPrivate.h"

EDynamicModuleType DynamicModuleType = EDynamicModuleType::MODULE_D3D12;

static IDynamicRHIModule* LoadDynamicRHIModule()
{
	IDynamicRHIModule* DynamicRHIModule = nullptr;

	switch(DynamicModuleType)
	{
	case EDynamicModuleType::MODULE_D3D12:
	{
		DynamicRHIModule = new FD3D12DynamicRHIModule();
	}
	break;
	default:{}
	}
	return DynamicRHIModule;
}

FDynamicRHI* PlatformCreateDynamicRHI()
{
	FDynamicRHI* DyRHI = nullptr;

	IDynamicRHIModule* DynamicRHIModule = LoadDynamicRHIModule();
	if(DynamicRHIModule)
	{
		DyRHI = DynamicRHIModule->CreateRHI();
	}

	return DyRHI;
}