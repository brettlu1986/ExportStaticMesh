#pragma once

#include "FRHI.h"
#include "stdafx.h"
#include "FD3D12Adapter.h"
#include "FD3D12CommandList.h"

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

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) override;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) override;

	virtual void RHICreateSrvAndCbvs(FCbvSrvDesc Desc) override;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) override;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) override;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey, bool bDefaultPso = false) override;

	virtual FIndexBuffer* RHICreateIndexBuffer() override;
	virtual FVertexBuffer* RHICreateVertexBuffer() override;
	virtual FTexture* RHICreateTexture() override;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) override;
	virtual void RHIDrawMesh(FMesh* Mesh) override;

private:
	void RHIInitWindow(UINT Width, UINT Height, void* Window);

	FD3D12Adapter* ChosenAdapter = nullptr;
};
