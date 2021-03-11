
#include "stdafx.h"
#include <dxgidebug.h>
#include "ApplicationMain.h"
#include "GraphicRender_ClearWindow.h"


GraphicRender_ClearWindow::GraphicRender_ClearWindow()
	:FrameIndex(0)
	,RtvDescriptorSize(0)
	, FenceEvent(nullptr)
	, FenceValue(0)
{
}

GraphicRender_ClearWindow::~GraphicRender_ClearWindow()
{

}

void GraphicRender_ClearWindow::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// Load the rendering pipeline dependencies.
void GraphicRender_ClearWindow::LoadPipeline()
{
	UINT DxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();

			// Enable additional debug layers.
			DxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

	}
#endif

	ComPtr<IDXGIFactory4> Factory;
	ThrowIfFailed(CreateDXGIFactory2(DxgiFactoryFlags, IID_PPV_ARGS(&Factory)));

	if (UseWarpDevice)
	{
		ComPtr<IDXGIAdapter> WarpAdapter;
		ThrowIfFailed(Factory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			WarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&Device)
		));
	}
	else
	{
		ComPtr<IDXGIAdapter1> HardwareAdapter;
		GetHardwareAdapter(Factory.Get(), &HardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			HardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&Device)
		));
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	SwapChainDesc.BufferCount = FrameCount;
	SwapChainDesc.Width = WndWidth;
	SwapChainDesc.Height = WndHeight;
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> SwapChainSC;
	ThrowIfFailed(Factory->CreateSwapChainForHwnd(
		CommandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		MainApplication->GetHwnd(),
		&SwapChainDesc,
		nullptr,
		nullptr,
		&SwapChainSC
	));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(Factory->MakeWindowAssociation(MainApplication->GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(SwapChainSC.As(&SwapChain));
	FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
		RtvHeapDesc.NumDescriptors = FrameCount;
		RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(Device->CreateDescriptorHeap(&RtvHeapDesc, IID_PPV_ARGS(&RtvHeap)));

		RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n])));
			Device->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RtvHandle);
			RtvHandle.Offset(1, RtvDescriptorSize);
		}
	}

	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));

	
}

// Load the sample assets.
void GraphicRender_ClearWindow::LoadAssets()
{
	// Create the command list.
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(CommandList->Close());

	// Create synchronization objects.
	{
		ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
		FenceValue = 1;

		// Create an event handle to use for frame synchronization.
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

bool GraphicRender_ClearWindow::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(SwapChain->Present(1, 0));

	WaitForPreviousFrame();

	return true;
}

void GraphicRender_ClearWindow::Update()
{

}

void GraphicRender_ClearWindow::Destroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
   // cleaned up by the destructor.
	WaitForPreviousFrame();
	CloseHandle(FenceEvent);

	Fence.Reset();
	PipelineState.Reset();
	CommandList.Reset();
	CommandAllocator.Reset();
	RtvHeap.Reset();
	ResetComPtrArray(&RenderTargets);
	CommandQueue.Reset();
	SwapChain.Reset();
	Device.Reset();

#if defined(_DEBUG)
	{
		ComPtr<IDXGIDebug1> DxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
		{
			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void GraphicRender_ClearWindow::PopulateCommandList()
{
	 //Command list allocators can only be reset when the associated 
	 //command lists have finished execution on the GPU; apps should use 
	 //fences to determine GPU execution progress.
	ThrowIfFailed(CommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), PipelineState.Get()));

	// Indicate that the back buffer will be used as a render target.
	D3D12_RESOURCE_BARRIER Rb = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &Rb);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RtvDescriptorSize);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CommandList->ClearRenderTargetView(RtvHandle, clearColor, 0, nullptr);

	// Indicate that the back buffer will now be used to present.
	D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(1, &Rb2);

	ThrowIfFailed(CommandList->Close());
}

void GraphicRender_ClearWindow::WaitForPreviousFrame()
{
	//WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	//	This is code implemented as such for simplicity.The D3D12HelloFrameBuffering
	//	sample illustrates how to use fences for efficient resource usageand to
	//	maximize GPU utilization.

	//	Signaland increment the fence value.
	const UINT64 Value = FenceValue;
	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), Value));
	FenceValue++;

	// Wait until the previous frame is finished.
	if (Fence->GetCompletedValue() < Value)
	{
		ThrowIfFailed(Fence->SetEventOnCompletion(Value, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}

	FrameIndex = SwapChain->GetCurrentBackBufferIndex();
}