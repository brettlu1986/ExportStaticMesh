
#include "FD3D12ResourceViewCreater.h"
#include "FD3D12Helper.h"

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12ResourceView::GetCpu(UINT i)
{
	if (i + 1 > Count)
	{
		assert(!"i index can not > current handle count");
	}
	D3D12_CPU_DESCRIPTOR_HANDLE CpuDes = CpuDescriptor;
	CpuDes.ptr += i * DescriptorSize;
	return CpuDes;
}

D3D12_GPU_DESCRIPTOR_HANDLE FD3D12ResourceView::GetGpu(UINT i)
{
	if (i + 1 > Count)
	{
		assert(!"i index can not > current handle count");
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GpuDes = GpuDescriptor;
	GpuDes.ptr += i * DescriptorSize;
	return GpuDes;
}

void FD3D12ResourceView::SetResourceView(UINT InCount, UINT DesSize, D3D12_CPU_DESCRIPTOR_HANDLE CpuDes, D3D12_GPU_DESCRIPTOR_HANDLE GpuDes)
{
	Count = InCount;
	DescriptorSize = DesSize;
	CpuDescriptor = CpuDes;
	GpuDescriptor = GpuDes;
}

void FD3D12CbvResourceView::SetConstantBufferViewInfo(ID3D12Device* Device, UINT InBufferSize)
{
	BufferSize = InBufferSize;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ThrowIfFailed(Device->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));
	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin)));
	memset(pCbvDataBegin, 0, InBufferSize);
	SetName(ConstantBuffer.Get(), L"FD3D12CbvResourceView");

	UINT64 CbOffset = 0;
	for (UINT i = 0; i < Count; i++)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
		CbvDesc.BufferLocation = ConstantBuffer->GetGPUVirtualAddress() + CbOffset;
		CbvDesc.SizeInBytes = BufferSize / Count;
		CbOffset += CbvDesc.SizeInBytes;
		Device->CreateConstantBufferView(&CbvDesc, GetCpu(i));
	}
}

void FD3D12CbvResourceView::UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize)
{
	memcpy(pCbvDataBegin, pDataUpdate, BufferSize);
}
/// //
void FD3D12ResourceViewHeap::OnCreate(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, UINT InDescriptorCount)
{
	DescripterNum = InDescriptorCount;
	Index = 0;
	DescripterElementSize = Device->GetDescriptorHandleIncrementSize(HeapType);

	D3D12_DESCRIPTOR_HEAP_DESC DescHeap;
	DescHeap.NumDescriptors = DescripterNum;
	DescHeap.Type = HeapType;
	DescHeap.Flags = ( (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) || (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) ) ?
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescHeap.NodeMask = 0;

	ThrowIfFailed(Device->CreateDescriptorHeap(&DescHeap, IID_PPV_ARGS(&Heap)));

	SetNameByType(HeapType);
}

void FD3D12ResourceViewHeap::SetNameByType(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
	if(HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		SetName(Heap, L"HeapDsv");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		SetName(Heap, L"HeapRTV");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		SetName(Heap, L"HeapSampler");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		SetName(Heap, L"HeapCbvSrvUav");
}

void FD3D12ResourceViewHeap::OnDestroy()
{
	Heap->Release();
}

bool FD3D12ResourceViewHeap::AllocDescriptor(UINT Count, FResourceView* Rv)
{
	if(Index + Count > DescripterNum)
	{
		assert(!"heap count ran out of memory, increase its count");
		return false;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CpuView = Heap->GetCPUDescriptorHandleForHeapStart();
	CpuView.ptr += Index * DescripterElementSize;

	D3D12_GPU_DESCRIPTOR_HANDLE GpuView = Heap->GetGPUDescriptorHandleForHeapStart();
	GpuView.ptr += Index *DescripterElementSize;

	Index += Count;

	FD3D12ResourceView* D3DResView = dynamic_cast<FD3D12ResourceView*>(Rv);
	D3DResView->SetResourceView(Count, DescripterElementSize, CpuView, GpuView);
	return true;
}

/// 
FD3D12ResourceViewCreater::FD3D12ResourceViewCreater()
:ParentDevice(nullptr)
, DsvHeap(FD3D12ResourceViewHeap())
, RtvHeap(FD3D12ResourceViewHeap())
, SamplerHeap(FD3D12ResourceViewHeap())
, CbvSrvUavHeap(FD3D12ResourceViewHeap())
{

}

FD3D12ResourceViewCreater::FD3D12ResourceViewCreater(ID3D12Device* Device)
:ParentDevice(Device)
, DsvHeap(FD3D12ResourceViewHeap())
, RtvHeap(FD3D12ResourceViewHeap())
, SamplerHeap(FD3D12ResourceViewHeap())
, CbvSrvUavHeap(FD3D12ResourceViewHeap())
{

}

FD3D12ResourceViewCreater::~FD3D12ResourceViewCreater()
{
	OnDestroy();
}

void FD3D12ResourceViewCreater::OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	DsvHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DsvCount);
	RtvHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RtvCount);
	SamplerHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SamplerCount);
	CbvSrvUavHeap.OnCreate(ParentDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CbvCount + SrvCount + UavCount);
}

bool FD3D12ResourceViewCreater::AllocDescriptor(UINT Count, E_RESOURCE_VIEW_TYPE HeapType, FResourceView* ResView)
{
	if(HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV)
	{
		return DsvHeap.AllocDescriptor(Count, ResView);
	}
	else if(HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV)
	{
		return RtvHeap.AllocDescriptor(Count, ResView);
	}
	else if(HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV || HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV ||
		HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_UAV)
	{
		return CbvSrvUavHeap.AllocDescriptor(Count, ResView);
	}
	else if(HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SAMPLER)
	{
		return SamplerHeap.AllocDescriptor(Count, ResView);
	}
	return false;
}

void FD3D12ResourceViewCreater::OnDestroy()
{
	DsvHeap.OnDestroy();
	RtvHeap.OnDestroy();
	SamplerHeap.OnDestroy();
	CbvSrvUavHeap.OnDestroy();
}