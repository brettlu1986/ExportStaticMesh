#pragma once

#include "GraphicRender.h"
#include "Camera.h"
#include "Model.h"
#include "MathHelper.h"

using namespace DirectX;

class GraphicRender_LoadModel : public GraphicRender
{
public:
	GraphicRender_LoadModel();
	~GraphicRender_LoadModel();

	virtual void OnInit();
	virtual bool Render();
	virtual void Update();
	virtual void Destroy();

private:
	void InitCamera();
	void InitModel();

	void LoadPipline();
	void LoadAssets();

	// pipline step
	void CreateDxgiObjects();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();

	//create assets step
	void CreateConstantBuffer();
	void CreateRootSignature();
	void LoadShadersAndCreatePso();
	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void OnResize();
	void FlushCommandQueue();

private:
	static const UINT FrameCount = 2;

	// Pipeline objects.
	CD3DX12_VIEWPORT Viewport;
	CD3DX12_RECT ScissorRect;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D12Device> Device;
	ComPtr<ID3D12Resource> RenderTargets[FrameCount];
	ComPtr<ID3D12Resource> DepthStencilBuffer;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12RootSignature> RootSignature;

	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;;
	ComPtr<ID3D12DescriptorHeap> CbvHeap;

	ComPtr<ID3D12PipelineState> PipelineState;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	UINT CbvSrvUavDescriptorSize;

	// Synchronization objects.
	UINT FrameIndex;
	ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue;

	Model ModelGeo;
	Camera Camera;
	
	XMFLOAT4X4 MtWorld; 
	XMFLOAT4X4 MtView; 
	XMFLOAT4X4 MtProj; 
};