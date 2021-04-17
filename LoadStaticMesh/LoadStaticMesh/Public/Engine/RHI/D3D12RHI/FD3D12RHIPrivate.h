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

	virtual void RHIBeginRenderFrame(UINT TargetFrame) override;
	virtual void RHIEndRenderFrame(UINT TargetFrame) override;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) override;
	
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) override;

	virtual FIndexBuffer* RHICreateIndexBuffer() override;
	virtual FVertexBuffer* RHICreateVertexBuffer() override;
	virtual FTexture* RHICreateTexture() override;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) override;
	virtual void RHIDrawMesh(FMesh* Mesh) override;

	virtual void RHICreateFrameResources(FScene* Scene) override;
	virtual void RHIUpdateFrameResources(FScene* Scene, UINT FrameIndex) override;
	virtual void RHIUpdateFrameResource(UINT FrameIndex) override;
	virtual FFrameResource& RHIGetFrameResource(UINT FrameIndex) override;
	virtual void RHIRenderFrameResource(FFrameResource& FrameResource) override;

	virtual void RHIUpdateFrameResourceCamera(LCamera& Camera) override;
private:
	void UpdateFrameResourceMtConstants(FFrameResource& FrameResource);
	void UpdateFrameResourceMatConstants(FFrameResource& FrameResource);
	void UpdateFrameResourcePassConstants(FFrameResource& FrameResource);

	void FindAdapter();
	void InitWindow(UINT Width, UINT Height, void* Window);
	void InitializeDevices();
	void CreateDescriptorHeaps();
	void CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type,
		D3D12_DESCRIPTOR_HEAP_FLAGS, UINT NodeMask, REFIID riid, _COM_Outptr_  void** ppvHeap);
	void CreateSrvAndCbvs(FFrameResource* FrameResource);
	void CreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey);
	void CreateSampler();
	void CreateSwapChain();
	void CreateRenderTargets();

	void WaitForPreviousFrame();

	FD3D12AdapterDesc CurrentAdapterDesc;
	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<IDXGIAdapter1> DxgiAdapter;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGISwapChain> SwapChain;

	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap;
	ComPtr<ID3D12DescriptorHeap> SamplerHeap;

	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	//ComPtr<ID3D12CommandAllocator> CommandAllocator;

	ComPtr<ID3D12CommandAllocator> CommandAllocator[FRAME_COUNT];

	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

	std::map<std::string, FD3DGraphicPipline*> PiplelineStateObjCache;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	ComPtr<ID3D12Resource> RenderTargets[RENDER_TARGET_COUNT];
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	ComPtr<ID3D12Fence> GpuFence;
	HANDLE FenceEvent;

	UINT64 FenceValues[FRAME_COUNT];
	UINT CurrentFrame;

	FCbvSrvDesc CurrentCbvSrvDesc;
	UINT CbvSrvDescriptorSize;

	UINT WndWidth;
	UINT WndHeight;
	HWND Window;

	std::vector<FFrameResource> FrameResources;
	FPassConstants PassConstant;
};
