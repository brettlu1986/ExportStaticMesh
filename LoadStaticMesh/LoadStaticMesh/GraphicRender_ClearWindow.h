#pragma once

#include "GraphicRender.h"

class GraphicRender_ClearWindow : public GraphicRender
{
public:
	GraphicRender_ClearWindow();
	~GraphicRender_ClearWindow();

	virtual void OnInit() ;
	virtual bool Render() ;
	virtual void Destroy();

private:
	static const UINT FrameCount = 2;

	// Pipeline objects.
	ComPtr<IDXGISwapChain3> m_swapChain;//
	ComPtr<ID3D12Device> m_device;//
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];//
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;//
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState;//
	ComPtr<ID3D12GraphicsCommandList> m_commandList;//
	UINT m_rtvDescriptorSize;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;//
	UINT64 m_fenceValue;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

};