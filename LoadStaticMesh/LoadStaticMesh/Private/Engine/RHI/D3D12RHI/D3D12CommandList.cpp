
#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12CommandListAllocator.h"
#include "D3D12Helper.h"
#include "D3D12ResourceManager.h"
#include "D3D12Resource.h"

D3D12CommandList::D3D12CommandList(D3D12Device* ParentDevice, D3D12CommandListAllocator* CommandAllocator, UINT InCommandListIndex)
:ParentDevice(ParentDevice)
,CurrentCommandAllocator(CommandAllocator)
{
	CommandListIndex = InCommandListIndex;
	ParentDevice->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CurrentCommandAllocator->GetD3DCommandAllocator(),
		nullptr, IID_PPV_ARGS(&CommandList));
	NAME_D3D12_OBJECT_WITHINDEX(CommandList, CommandListIndex);
}

D3D12CommandList::~D3D12CommandList()
{
}

void D3D12CommandList::Excute()
{
	ID3D12CommandQueue* CommandQueue = ParentDevice->GetD3DCommandQueue();
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
}

void D3D12CommandList::SetViewPort()
{
	CommandList->RSSetViewports(1, &(ParentDevice->GetParentAdapter()->ViewPort));
}

void D3D12CommandList::SetScissorRect()
{
	CommandList->RSSetScissorRects(1, &(ParentDevice->GetParentAdapter()->ScissorRect));
}

void D3D12CommandList::TransitionToState(UINT TargetFrame, ETransitionState State)
{
	D3D12ResourceManager* SourceManager = ParentDevice->GetResourceManager();
	switch (State)
	{
	case ETransitionState::STATE_RENDER:
	{
		const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(SourceManager->GetRenderTarget()->GetRenderTarget(TargetFrame), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		CommandList->ResourceBarrier(1, &Rb1);
	}
	break;
	case ETransitionState::STATE_PRESENT:
	{
		const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(SourceManager->GetRenderTarget()->GetRenderTarget(TargetFrame), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		CommandList->ResourceBarrier(1, &Rb2);
	}
	break;
	default:
		break;
	}
}

void D3D12CommandList::ClearRenderTargetAndDepthStencilView(UINT TargetFrame, RHIColor Color)
{
	D3D12Adapter* Adapter = ParentDevice->GetParentAdapter();
	D3D12ResourceManager* SourceManager = ParentDevice->GetResourceManager();
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(Adapter->RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetFrame, SourceManager->GetRenderTarget()->GetRtvDescriptorSize());
	float ClearColor[4] = { Color.R, Color.G, Color.B, Color.A };
	CommandList->ClearRenderTargetView(RtvHandle, ClearColor, 0, nullptr);
	CommandList->ClearDepthStencilView(Adapter->DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = Adapter->DsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);
}

void D3D12CommandList::SetGraphicRootDescripterTable()
{
	D3D12Adapter* Adapter = ParentDevice->GetParentAdapter();
	D3D12ResourceManager* SourceManager = ParentDevice->GetResourceManager();
	D3DConstantBuffer* ConstantBuffer = SourceManager->GetConstantBuffer();
	CommandList->SetGraphicsRootSignature(Adapter->GetRootSignature());
	CommandList->SetPipelineState(Adapter->GetDefaultPiplineState());

	ID3D12DescriptorHeap* ppHeaps[] = { Adapter->CbvSrvHeap.Get(), Adapter->SamplerHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CommandList->SetGraphicsRootDescriptorTable(0, Adapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(Adapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(1, ConstantBuffer->GetCbvSrvUavDescriptorSize());
	CommandList->SetGraphicsRootDescriptorTable(1, tex);
	CommandList->SetGraphicsRootDescriptorTable(2, Adapter->SamplerHeap->GetGPUDescriptorHandleForHeapStart());
}

void D3D12CommandList::DrawWithVertexAndIndexBufferView(RHIView* VertexBufferView, RHIView* IndexBufferView)
{
	D3DVertexBufferView* VBufferView = (D3DVertexBufferView*)VertexBufferView;
	D3DIndexBufferView* IBufferView = (D3DIndexBufferView*)IndexBufferView;
	CommandList->IASetVertexBuffers(0, 1, &(VBufferView->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(IBufferView->GetIndexBufferView()));
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawIndexedInstanced(IBufferView->GetIndicesCount(), 1, 0, 0, 0);
}

void D3D12CommandList::Clear()
{
	if(CurrentCommandAllocator)
	{
		CurrentCommandAllocator->Reset();
		delete CurrentCommandAllocator; 
		CurrentCommandAllocator = nullptr;
	}
	CommandList.Reset();
}

void D3D12CommandList::Close()
{
	CommandList->Close();
}

void D3D12CommandList::Reset()
{
	CurrentCommandAllocator->Reset();
	CommandList->Reset(CurrentCommandAllocator->GetD3DCommandAllocator(), ParentDevice->GetParentAdapter()->GetDefaultPiplineState());
}