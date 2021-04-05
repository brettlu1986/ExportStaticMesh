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

class FD3D12DynamicRHI : public FDynamicRHI
{
public:
	FD3D12DynamicRHI(FD3D12Adapter* ChosenAdapterIn);

	virtual void Init() override;
	virtual void ShutDown() override;

	virtual FGenericFence* RHICreateFence(const std::string& Name) override;

	
	virtual void RHICreatePiplineStateObject(FRHIPiplineStateInitializer& Initializer) override;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) override;
	
	
	
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
	virtual void RHIInitWindow(UINT Width, UINT Height, void* Window) override;

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) override;
	virtual void RHIRenderEnd(UINT TargetFrame) override;

	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize) override;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) override;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) override;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps) override;

	virtual FIndexBuffer* RHICreateIndexBuffer() override;
	virtual FVertexBuffer* RHICreateVertexBuffer() override;
	virtual FTexture* RHICreateTexture() override;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) override;
	virtual void RHIDrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* TextureRes) override;

private: 
	FD3D12Adapter* ChosenAdapter = nullptr;

};
