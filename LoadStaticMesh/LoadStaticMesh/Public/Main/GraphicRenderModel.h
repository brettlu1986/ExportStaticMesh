#pragma once

#include "GraphicRender.h"
#include "Camera.h"
#include "Model.h"
#include "MathHelper.h"

using namespace DirectX;

class GraphicRenderModel : public GraphicRender
{
public:
	GraphicRenderModel();
	~GraphicRenderModel();

	static GraphicRenderModel* Get() { return ThisRenderModel; }

	virtual void OnInit();
	virtual bool Render();
	virtual void Update();
	virtual void Destroy();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

private:

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	struct ThreadParameter
	{
		UINT  ThreadId;
		UINT  FrameIndex;
		HANDLE ThisThread;
		HANDLE RenderBegin;
		HANDLE RenderRun;
		HANDLE RenderEnd;

	};

	typedef enum RENDER_STATE
	{
		RENDER_INIT = 0,
		RENDER_PRE,
		RENDER_POST,
	}RENDER_STATE;

	ThreadParameter ThreadParam;
	static GraphicRenderModel* ThisRenderModel;

	void LoadPipline();
	void LoadAssets();
	void CreateRenderThread();

	// pipline step
	void CreateDxgiObjects();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();

	//create assets step
	void CreateRootSignature();
	void LoadShadersAndCreatePso();

	//use in render thread
	void InitCamera();
	void InitModel();
	UINT RenderThread(void* Param);
	void RenderThreadInit();
	void RenderThreadRun();

	void CreateConstantBufferView();
	void CreateDepthStencilView();
	void CreateVertexBufferView();
	void CreateIndexBufferView();
	void CreateTextureAndSampler();
	
	//fence
	void FlushCommandQueue();

private:
	static const UINT FrameCount = 3;
	static const UINT ThreadCount = 1;

	// Pipeline objects.
	CD3DX12_VIEWPORT Viewport;
	CD3DX12_RECT ScissorRect;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D12Device> Device;
	ComPtr<ID3D12Resource> RenderTargets[FrameCount];


	ComPtr<ID3D12Resource> DepthStencilBuffer;
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12RootSignature> RootSignature;

	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;;
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap;
	ComPtr<ID3D12DescriptorHeap> SamplerHeap;
	//
	ComPtr<ID3D12CommandAllocator> CommandAllocatorPre;
	ComPtr<ID3D12GraphicsCommandList> CommandListPre;

	ComPtr<ID3D12CommandAllocator> CommandAllocatorPost;
	ComPtr<ID3D12GraphicsCommandList> CommandListPost;

	ComPtr<ID3D12CommandAllocator> RenderThreadCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> RenderThreadCmdList;

	std::vector<HANDLE>	HWaitedArr;
	//

	ComPtr<ID3D12PipelineState> PipelineState;

	ComPtr<ID3D12Resource> ConstantBuffer;
	ObjectConstants ObjectConstant;
	UINT8* pCbvDataBegin;
	UINT ConstantBufferSize;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	UINT CbvSrvUavDescriptorSize;

	// Synchronization objects.
	UINT FrameIndex;
	ComPtr<ID3D12Fence> Fence;
	HANDLE FenceEvent;
	UINT64 FenceValue;

	Model ModelGeo;
	Camera Camera;
	
	XMFLOAT4X4 MtProj; 
	POINT LastMousePoint;
	UINT State;
};