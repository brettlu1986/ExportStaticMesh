
#include "D3D12Resource.h"
#include "D3D12Helper.h"

D3DResource::D3DResource()
:ParentDevice(nullptr)
{

}

D3DResource::D3DResource(D3D12Device* InDevice)
:ParentDevice(InDevice)
{
}

D3DResource::~D3DResource()
{
}

D3DRenderTarget::D3DRenderTarget()
:TargetCount(0)
{
	ResourceName = "RenderTarget";
}

D3DRenderTarget::D3DRenderTarget(D3D12Device* InDevice)
:D3DResource(InDevice)
,TargetCount(0)
{
	ResourceName = "RenderTarget";
}

D3DRenderTarget::~D3DRenderTarget()
{

}

void D3DRenderTarget::Initialize()
{
	RtvDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE D3DRenderTarget::GetRtvHandle(UINT TargetIndex)
{
	ComPtr<ID3D12DescriptorHeap> RtvHeap = ParentDevice->GetParentAdapter()->RtvHeap;
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetIndex, RtvDescriptorSize);
}

void D3DRenderTarget::SetRenderTargetCount(UINT Count)
{
	TargetCount = Count;
	ComPtr<ID3D12DescriptorHeap> RtvHeap = ParentDevice->GetParentAdapter()->RtvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	IDXGISwapChain* SwapChain = ParentDevice->GetParentAdapter()->GetSwapChain();
	// Create a RTV for each frame.
	for (UINT n = 0; n < Count; n++)
	{
		ComPtr<ID3D12Resource> Target;
		ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&Target)));
		ParentDevice->GetDevice()->CreateRenderTargetView(Target.Get(), nullptr, RtvHandle);
		RenderTargets.push_back(Target);
		RtvHandle.Offset(1, RtvDescriptorSize);
	}
}

void D3DRenderTarget::Destroy()
{
	for(size_t i = 0; i < RenderTargets.size(); ++i)
	{
		RenderTargets[i].Reset();
	}
	RenderTargets.clear();
}


D3DConstantBuffer::D3DConstantBuffer()
{
	ResourceName = "ConstantBuffer";
}

D3DConstantBuffer::D3DConstantBuffer(D3D12Device* InDevice)
	:D3DResource(InDevice)
{	
	ResourceName = "ConstantBuffer";
}

D3DConstantBuffer::~D3DConstantBuffer()
{

}

void D3DConstantBuffer::Destroy() 
{
	ConstantBuffer->Unmap(0, nullptr);
	ConstantBuffer.Reset();
}

void D3DConstantBuffer::Initialize() 
{
	CbvSrvUavDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3DConstantBuffer::SetConstantBufferInfo(UINT BufferSize, void* pDataFrom, void** pDataMap)
{
	
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ThrowIfFailed(ParentDevice->GetDevice()->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));
	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, pDataMap));
	memcpy(pDataMap, &pDataFrom, sizeof(pDataFrom));
	NAME_D3D12_OBJECT(ConstantBuffer);
}