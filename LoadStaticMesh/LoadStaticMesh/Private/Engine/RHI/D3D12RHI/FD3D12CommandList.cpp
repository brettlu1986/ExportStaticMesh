
#include "FD3D12CommandList.h"
#include "FD3D12Helper.h"
#include "FD3D12Resource.h"
#include "FD3D12Adapter.h"

FD3D12CommandList::FD3D12CommandList(FD3D12Adapter* InAdapter, UINT InCommandListIndex)
:ParentAdapter(InAdapter)
{
	CommandListIndex = InCommandListIndex;

	ThrowIfFailed(ParentAdapter->GetD3DDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	NAME_D3D12_OBJECT_WITHINDEX(CommandAllocator, CommandListIndex);

	ParentAdapter->GetD3DDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&CommandList));
	NAME_D3D12_OBJECT_WITHINDEX(CommandList, CommandListIndex);
}

FD3D12CommandList::~FD3D12CommandList()
{
}

void FD3D12CommandList::Excute()
{
	ID3D12CommandQueue* CommandQueue = ParentAdapter->GetD3DCommandQueue();
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
}

void FD3D12CommandList::SetViewPort()
{
	CommandList->RSSetViewports(1, &(ParentAdapter->ViewPort));
}

void FD3D12CommandList::SetScissorRect()
{
	CommandList->RSSetScissorRects(1, &(ParentAdapter->ScissorRect));
}


void FD3D12CommandList::SetGraphicRootDescripterTable()
{
	FD3DConstantBuffer* ConstantBuffer = ParentAdapter->GetConstantBuffer();
	CommandList->SetGraphicsRootSignature(ParentAdapter->GetRootSignature());
	CommandList->SetPipelineState(ParentAdapter->GetDefaultPiplineState());

	ID3D12DescriptorHeap* ppHeaps[] = { ParentAdapter->CbvSrvHeap.Get(), ParentAdapter->SamplerHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CommandList->SetGraphicsRootDescriptorTable(0, ParentAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(ParentAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(1, ConstantBuffer->GetCbvSrvUavDescriptorSize());
	CommandList->SetGraphicsRootDescriptorTable(1, tex);
	CommandList->SetGraphicsRootDescriptorTable(2, ParentAdapter->SamplerHeap->GetGPUDescriptorHandleForHeapStart());
}

void FD3D12CommandList::Clear()
{
	CommandAllocator.Reset();
	CommandList.Reset();
}

void FD3D12CommandList::Close()
{
	CommandList->Close();
}

void FD3D12CommandList::Reset()
{
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator.Get(), ParentAdapter->GetDefaultPiplineState());
}