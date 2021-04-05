
#include "FD3D12CommandList.h"
#include "FD3D12Device.h"
#include "FD3D12Helper.h"
#include "FD3D12Resource.h"

FD3D12CommandList::FD3D12CommandList(FD3D12Device* ParentDevice, UINT InCommandListIndex)
:ParentDevice(ParentDevice)
{
	CommandListIndex = InCommandListIndex;

	ThrowIfFailed(ParentDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	NAME_D3D12_OBJECT_WITHINDEX(CommandAllocator, CommandListIndex);

	ParentDevice->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&CommandList));
	NAME_D3D12_OBJECT_WITHINDEX(CommandList, CommandListIndex);
}

FD3D12CommandList::~FD3D12CommandList()
{
}

void FD3D12CommandList::Excute()
{
	ID3D12CommandQueue* CommandQueue = ParentDevice->GetD3DCommandQueue();
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
}

void FD3D12CommandList::SetViewPort()
{
	CommandList->RSSetViewports(1, &(ParentDevice->GetParentAdapter()->ViewPort));
}

void FD3D12CommandList::SetScissorRect()
{
	CommandList->RSSetScissorRects(1, &(ParentDevice->GetParentAdapter()->ScissorRect));
}


void FD3D12CommandList::SetGraphicRootDescripterTable()
{
	FD3D12Adapter* Adapter = ParentDevice->GetParentAdapter();
	FD3DConstantBuffer* ConstantBuffer = Adapter->GetConstantBuffer();
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
	CommandList->Reset(CommandAllocator.Get(), ParentDevice->GetParentAdapter()->GetDefaultPiplineState());
}