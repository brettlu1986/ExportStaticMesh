#pragma once

#include "RHI.h"
#include "stdafx.h"
#include "D3D12Adapter.h"
#include "D3D12Fence.h"

using namespace Microsoft::WRL;


class D3D12DynamicRHIModule : public IDynamicRHIModule
{
public:
	D3D12DynamicRHIModule() {}
	~D3D12DynamicRHIModule() {}

	virtual bool IsSupported() override;
	virtual DynamicRHI* CreateRHI() override;

private:
	D3D12Adapter* ChosenAdapter = nullptr;

	void FindAdapter();
};

class D3D12DynamicRHI : public DynamicRHI
{
public:
	D3D12DynamicRHI(D3D12Adapter* ChosenAdapterIn);

	virtual void Init() override;
	virtual void PostInit() override;
	virtual void ShutDown() override;

	virtual GenericFence* RHICreateFence(const std::string& Name) override;
private: 
	D3D12Adapter* ChosenAdapter = nullptr;

};
