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

	virtual void RHICreateViewPort(RHIViewPort& ViewPort) override;
	virtual void RHICreateSwapObject(RHISwapObjectInfo& SwapInfo) override;
	virtual void RHICreatePiplineStateObject(RHIPiplineStateInitializer& Initializer) override;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) override;
	virtual void RHICreateRenderTarget(UINT TargetCount) override;
	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, void** pDataMap) override;
private: 
	D3D12Adapter* ChosenAdapter = nullptr;

};
