#pragma once

#include "FRHI.h"
#include "stdafx.h"
#include "FD3D12Adapter.h"
#include "FD3D12Fence.h"

using namespace Microsoft::WRL;


class FD3D12DynamicRHIModule : public IDynamicRHIModule
{
public:
	FD3D12DynamicRHIModule() {}
	~FD3D12DynamicRHIModule() {}

	virtual bool IsSupported() override;
	virtual FDynamicRHI* CreateRHI() override;

private:
	FD3D12Adapter* ChosenAdapter = nullptr;

	void FindAdapter();
};

class FD3D12ResourceManager;
class FD3D12DynamicRHI : public FDynamicRHI
{
public:
	FD3D12DynamicRHI(FD3D12Adapter* ChosenAdapterIn);

	virtual void Init() override;
	virtual void ShutDown() override;

	virtual FGenericFence* RHICreateFence(const std::string& Name) override;

	virtual void RHICreateViewPort(FRHIViewPort& ViewPort) override;
	virtual void RHICreateSwapObject(FRHISwapObjectInfo& SwapInfo) override;
	virtual void RHICreatePiplineStateObject(FRHIPiplineStateInitializer& Initializer) override;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) override;
	

	
	
	virtual FRHIView* RHICreateShaderResourceView(std::wstring TextureName);
	virtual FRHICommandList& RHIGetCommandList(UINT Index) ;
	virtual void RHIExcuteCommandList(FRHICommandList& CommandList);
	virtual void RHICloseCommandList(FRHICommandList& CommandList);
	virtual void RHISignalCurrentFence() ;
	virtual bool RHIIsFenceComplete() ;
	virtual void RHIResetCommandList(FRHICommandList& CommandList);
	virtual void RHISetCurrentViewPortAndScissorRect(FRHICommandList& CommandList);

	virtual void RHISetGraphicRootDescripterTable(FRHICommandList& CommandList);
	virtual void RHISwapObjectPresent();

	/// ////////////////////////////////
	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color);
	virtual void RHICreateRenderTarget(UINT Width, UINT Height);
	virtual void RHIRenderEnd(UINT TargetFrame);

	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize) override;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize);

	virtual FIndexBuffer* RHICreateIndexBuffer() ;
	virtual FVertexBuffer* RHICreateVertexBuffer();
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer) ;
	virtual void RHIDrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer);

private: 
	FD3D12ResourceManager* GetResourceManager();
	FD3D12Adapter* ChosenAdapter = nullptr;

};
