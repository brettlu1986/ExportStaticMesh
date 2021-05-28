#pragma once

#include "FRHI.h"
#include "stdafx.h"
#include "FD3D12Resource.h"
#include "FMesh.h"
#include <map>

using namespace Microsoft::WRL;

class FD3D12DynamicRHIModule : public IDynamicRHIModule
{
public:
	FD3D12DynamicRHIModule() {}
	~FD3D12DynamicRHIModule() {}
	virtual FDynamicRHI* CreateRHI() override;
};

class FD3D12DynamicRHI : public FDynamicRHI
{
public:
	FD3D12DynamicRHI();

	virtual void Init() override;
	virtual void ShutDown() override;

	virtual void BeginRenderScene() override;
	virtual void UpdateSceneResources(FScene* RenderScene) override;
	virtual void EndRenderScene() override;
	
	virtual void BeginCreateSceneResource() override;
	virtual void EndCreateSceneResource() override;
	
	virtual FIndexBuffer* RHICreateIndexBuffer() override;
	virtual FVertexBuffer* RHICreateVertexBuffer() override;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) override;
	virtual FTexture* RHICreateTexture() override;
	virtual FTexture* CreateTexture(FTextureInitializer TexInitializer) override;
	
	virtual void CreateResourceViewCreater(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) override;
	virtual void CreateVertexAndIndexBufferView(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer) override;
	virtual FResourceView* CreateResourceView(FResourceViewInfo ViewInfo) override;
	virtual void CreatePipelineStateObject(FPiplineStateInitializer Initializer) override;

	virtual void SetResourceHeaps(vector<FResourceHeap*>& Heaps) override;
	virtual void SetRenderTargets(FResourceView* RtvView, FResourceView* DsvView) override;
	virtual void SetVertexAndIndexBuffers(FVertexBuffer* VertexBuffer, FIndexBuffer* IndexBuffer) override;
	virtual void SetPiplineStateObject(FD3DGraphicPipline* PsoObj) override;
	virtual void SetResourceParams(UINT Index, FResourceView* ResView) override;
	virtual void DrawTriangleList(FIndexBuffer* IndexBuffer) override;
	virtual void ResourceTransition(FResourceView* ResourceView, E_RESOURCE_STATE StateFrom, E_RESOURCE_STATE StateTo) override;

	virtual void BeginEvent(const char* Name) override;
	virtual void EndEvent() override;

	virtual UINT GetFrameIndex() override { return FrameIndex; };
	virtual void SetViewPortInfo(FRHIViewPort ViewPort) override;
	virtual FRHIViewPort GetDefaultViewPort() override { return DefaultViewPort;};

private:
	void UpdateSceneMtConstants(FScene* RenderScene);
	void UpdateScenePassConstants(FScene* RenderScene);
	void UpdateSceneSkeletalConstants(FScene* RenderScene);

	void FindAdapter();
	void InitWindow(UINT Width, UINT Height, void* Window);
	void InitializeDevices();
	void CreateSampler();
	void CreateSwapChain();
	void WaitForPreviousFrame();

	FD3D12AdapterDesc CurrentAdapterDesc;
	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<IDXGIAdapter1> DxgiAdapter;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGISwapChain> SwapChain;

	vector<CD3DX12_STATIC_SAMPLER_DESC> StaticSamplers;

	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;

	ComPtr<ID3D12Fence> GpuFence;
	HANDLE FenceEvent;
	UINT64 FenceValues;

	UINT WndWidth;
	UINT WndHeight;
	HWND Window;

	UINT FrameIndex;
	FRHIViewPort DefaultViewPort;
};
