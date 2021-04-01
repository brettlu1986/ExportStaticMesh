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

class D3D12ResourceManager;
class D3D12DynamicRHI : public DynamicRHI
{
public:
	D3D12DynamicRHI(D3D12Adapter* ChosenAdapterIn);

	virtual void Init() override;
	virtual void ShutDown() override;

	virtual GenericFence* RHICreateFence(const std::string& Name) override;

	virtual void RHICreateViewPort(RHIViewPort& ViewPort) override;
	virtual void RHICreateSwapObject(RHISwapObjectInfo& SwapInfo) override;
	virtual void RHICreatePiplineStateObject(RHIPiplineStateInitializer& Initializer) override;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) override;
	virtual void RHICreateRenderTarget(UINT TargetCount) override;
	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize) override;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) ;

	virtual void RHICreateDepthStencilBuffer(UINT Width, UINT Height) override;
	virtual RHIView* RHICreateVertexBufferView(const void* InitData, UINT StrideInByte, UINT DataSize) override;
	virtual RHIView* RHICreateIndexBufferView(const void* InitData, UINT DataSize, UINT IndicesCount, bool bUseHalfInt32) override;
	virtual RHIView* RHICreateShaderResourceView(std::wstring TextureName);
	virtual RHICommandList& RHIGetCommandList(UINT Index) ;
	virtual void RHIExcuteCommandList(RHICommandList& CommandList);
	virtual void RHICloseCommandList(RHICommandList& CommandList);
	virtual void RHISignalCurrentFence() ;
	virtual bool RHIIsFenceComplete() ;
	virtual void RHIResetCommandList(RHICommandList& CommandList);
	virtual void RHISetCurrentViewPortAndScissorRect(RHICommandList& CommandList);
	virtual void RHITransitionToState(RHICommandList& CommandList, UINT TargetFrame, ETransitionState State);
	virtual void RHIClearRenderTargetAndDepthStencilView(RHICommandList& CommandList, UINT TargetFrame, RHIColor ClearColor);

	virtual void RHISetGraphicRootDescripterTable(RHICommandList& CommandList);
	virtual void RHIDrawWithVertexAndIndexBufferView(RHICommandList& CommandList, RHIView* VertexBufferView, RHIView* IndexBufferView);
	virtual void RHISwapObjectPresent();

private: 
	D3D12ResourceManager* GetResourceManager();
	D3D12Adapter* ChosenAdapter = nullptr;

};
