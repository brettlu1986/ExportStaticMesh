#pragma once

#include "GraphicRender.h"

class GraphicRender_ClearWindow : public GraphicRender
{
public:
	GraphicRender_ClearWindow();
	~GraphicRender_ClearWindow();

	virtual void OnInit() ;
	virtual bool Render() ;
	virtual void Update();
	virtual void Destroy();

private:
	static const UINT FrameCount = 2;

	// Pipeline objects.
	ComPtr<IDXGISwapChain3> SwapChain;//
	ComPtr<ID3D12Device> Device;//
	ComPtr<ID3D12Resource> RenderTargets[FrameCount];//
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12CommandQueue> CommandQueue;//
	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12PipelineState> PipelineState;//
	ComPtr<ID3D12GraphicsCommandList> CommandList;//
	UINT RtvDescriptorSize;

	// Synchronization objects.
	UINT FrameIndex;
	HANDLE FenceEvent;
	ComPtr<ID3D12Fence> Fence;//
	UINT64 FenceValue;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

};