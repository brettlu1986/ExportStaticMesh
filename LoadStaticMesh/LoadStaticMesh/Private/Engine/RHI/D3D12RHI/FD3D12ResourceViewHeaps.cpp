
#include "FD3D12ResourceViewHeaps.h"


void FD3D12ResourceViewHeap::OnCreate(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, UINT DescriptorCount)
{

}

void FD3D12ResourceViewHeap::OnDestroy()
{


}

bool FD3D12ResourceViewHeap::AllocDescriptor(UINT Size, FResourceView* Rv)
{

	return true;
}


FD3D12ResourceViewHeaps::FD3D12ResourceViewHeaps()
:ParentDevice(nullptr)
{

}

FD3D12ResourceViewHeaps::FD3D12ResourceViewHeaps(ID3D12Device* Device)
:ParentDevice(Device)
{

}

FD3D12ResourceViewHeaps::~FD3D12ResourceViewHeaps()
{

}

void FD3D12ResourceViewHeaps::OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	DsvHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DsvCount);
	RtvHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RtvCount);
	SamplerHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SamplerCount);
	CbvSrvUavHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CbvCount + SrvCount + UavCount);
}

void FD3D12ResourceViewHeaps::OnDestroy()
{
	DsvHeap.OnDestroy();
	RtvHeap.OnDestroy();
	SamplerHeap.OnDestroy();
	CbvSrvUavHeap.OnDestroy();
}