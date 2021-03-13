#pragma once

#include "GraphicRender.h"
#include "GraphicCamera.h"
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

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};
	
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
	UINT8* pCbvDataBegin;

	ComPtr<ID3D12PipelineState> PipelineState;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	UINT CbvSrvUavDescriptorSize;

	// App resources.
	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> VertexUploadBuffer;
	
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> IndexBufferUpload;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	UINT IndiceSize;
	UINT IndicesCount;

	ComPtr<ID3D12Resource> ConstantBuffer;

	// Synchronization objects.
	UINT FrameIndex;
	ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue;

	GraphicCamera Camera;
	ObjectConstants ObjectConstant;

	//
	XMFLOAT4X4 MtWorld; 
	XMFLOAT4X4 MtView; 
	XMFLOAT4X4 MtProj; 

	float Theta;
	float Phi; 
	float Radius;
};