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

	struct ObjectConstants
	{
		XMFLOAT4X4 worldViewProj = MathHelper::Identity4x4();
	};
	
	static const UINT FrameCount = 2;

	// Pipeline objects.
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	UINT8* m_pCbvDataBegin;

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources.
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	UINT m_indiceSize;

	ComPtr<ID3D12Resource> m_constantBuffer;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	GraphicCamera m_camera;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

};