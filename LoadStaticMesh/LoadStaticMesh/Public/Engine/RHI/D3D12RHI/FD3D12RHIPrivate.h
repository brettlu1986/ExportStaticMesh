#pragma once

#include "FRHI.h"
#include "stdafx.h"
#include "FD3D12CommandList.h"
#include "FD3D12CommandListManager.h"
#include "FD3D12Resource.h"
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

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) override;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) override;

	virtual void RHICreateSrvAndCbvs(FCbvSrvDesc Desc) override;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData) override;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) override;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey, bool bDefaultPso = false) override;

	virtual FIndexBuffer* RHICreateIndexBuffer() override;
	virtual FVertexBuffer* RHICreateVertexBuffer() override;
	virtual FTexture* RHICreateTexture() override;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) override;
	virtual void RHIDrawMesh(FMesh* Mesh) override;

private:
	void FindAdapter();

	void InitWindow(UINT Width, UINT Height, void* Window);
	void InitializeDevices();
	void CreateDescriptorHeaps();

	void CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type,
		D3D12_DESCRIPTOR_HEAP_FLAGS, UINT NodeMask, REFIID riid, _COM_Outptr_  void** ppvHeap);

	void CreateSignature();
	void CreateSampler();
	void CreateSwapChain();
	void CreateRenderTargets();

	void SetFenceValue(UINT64 Value) { FenceValue = Value; }
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
	ComPtr<ID3D12CommandAllocator> CommandAllocator;

	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

	ComPtr<ID3D12RootSignature> RootSignature;
	std::map<std::string, ComPtr<ID3D12PipelineState>> PiplelineStateCache;
	ComPtr <ID3D12PipelineState> DefaultPso;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	ComPtr<ID3D12Resource> RenderTargets[RENDER_TARGET_COUNT];
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	ComPtr<ID3D12Fence> GpuFence;
	HANDLE FenceEvent;
	UINT64 FenceValue;

	FCbvSrvDesc CurrentCbvSrvDesc;
	std::map<E_CONSTANT_BUFFER_TYPE, FD3DConstantBuffer*> ConstantBuffers;
	UINT CbvSrvDescriptorSize;

	UINT WndWidth;
	UINT WndHeight;
	HWND Window;
};
