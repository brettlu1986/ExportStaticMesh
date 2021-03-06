#include "pch.h"
#include "FD3D12ResourceViewCreater.h"
#include "FD3D12Helper.h"

//Resource View
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

void FD3D12ResourceView::SetResourceView(UINT InCount, UINT DesSize, D3D12_CPU_DESCRIPTOR_HANDLE CpuDes, D3D12_GPU_DESCRIPTOR_HANDLE GpuDes, E_RESOURCE_VIEW_TYPE ViewType)
{
	Count = InCount;
	DescriptorSize = DesSize;
	CpuDescriptor = CpuDes;
	GpuDescriptor = GpuDes;
	Type = ViewType;
}

//RtvView
void FD3D12RtvResourceView::SetRtvViewInfo(ID3D12Device* Device, IDXGISwapChain* SwapChain, DXGI_FORMAT Format, UINT SwapBufferIndex)
{
	ThrowIfFailed(SwapChain->GetBuffer(SwapBufferIndex, IID_PPV_ARGS(&ViewResource)));

	D3D12_RENDER_TARGET_VIEW_DESC ColorDesc = {};
	ColorDesc.Format = Format;
	ColorDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	ColorDesc.Texture2D.MipSlice = 0;
	ColorDesc.Texture2D.PlaneSlice = 0;

	Device->CreateRenderTargetView(ViewResource.Get(), &ColorDesc, GetCpu());
}

void FD3D12RtvResourceView::SetRtvViewInfo(ID3D12Device* Device, FD3D12Texture* Tex, DXGI_FORMAT Format)
{
	ViewResource = Tex->Resource();

	D3D12_RENDER_TARGET_VIEW_DESC ColorDesc = {};
	ColorDesc.Format = Format;
	ColorDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	ColorDesc.Texture2D.MipSlice = 0;
	ColorDesc.Texture2D.PlaneSlice = 0;
	Device->CreateRenderTargetView(ViewResource.Get(), &ColorDesc, GetCpu());
}

//Dsv View
void FD3D12DsvResourceView::SetDsvViewInfo(ID3D12Device* Device, FD3D12Texture* Tex, DXGI_FORMAT Format)
{
	ViewResource = Tex->Resource();
	D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
	DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DsvDesc.Format = Format;
	DsvDesc.Texture2D.MipSlice = 0;
	Device->CreateDepthStencilView(ViewResource.Get(), &DsvDesc, GetCpu());
}

//Cbv View
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

void FD3D12CbvResourceView::UpdateConstantBufferInfo(void* pDataUpdate)
{
	memcpy(pCbvDataBegin, pDataUpdate, BufferSize);
}

//View Heap
FD3D12ResourceViewHeap::FD3D12ResourceViewHeap()
:Index(0)
,DescripterNum(0)
,DescripterElementSize(0)
{
}

FD3D12ResourceViewHeap::~FD3D12ResourceViewHeap()
{
}

void FD3D12ResourceViewHeap::OnCreate(ID3D12Device* Device, UINT HeapType, UINT InDescriptorCount)
{
	DescripterNum = InDescriptorCount;
	Index = 0;

	D3D12_DESCRIPTOR_HEAP_TYPE D3DHeapType = GetD3DHeapType(HeapType);
	DescripterElementSize = Device->GetDescriptorHandleIncrementSize(D3DHeapType);

	D3D12_DESCRIPTOR_HEAP_DESC DescHeap;
	DescHeap.NumDescriptors = DescripterNum;
	DescHeap.Type = D3DHeapType;
	DescHeap.Flags = ((D3DHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) || (D3DHeapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)) ?
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescHeap.NodeMask = 0;

	ThrowIfFailed(Device->CreateDescriptorHeap(&DescHeap, IID_PPV_ARGS(&Heap)));

	SetNameByType(D3DHeapType);
}

D3D12_DESCRIPTOR_HEAP_TYPE FD3D12ResourceViewHeap::GetD3DHeapType(UINT Type)
{
	if (Type == static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV))
		return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	else if (Type == static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV))
		return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	else if (Type == static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SAMPLER))
		return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	else if (Type == (static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV) |
		static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV) | static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_UAV)))
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	assert(!"invalid heap type");
	return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
}

void FD3D12ResourceViewHeap::SetNameByType(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
	if(HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		SetName(Heap.Get(), L"HeapDsv");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		SetName(Heap.Get(), L"HeapRTV");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		SetName(Heap.Get(), L"HeapSampler");
	else if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		SetName(Heap.Get(), L"HeapCbvSrvUav");
}

void FD3D12ResourceViewHeap::OnDestroy()
{
	Heap.Reset();
}

bool FD3D12ResourceViewHeap::AllocDescriptor(UINT Count, FResourceView* Rv, E_RESOURCE_VIEW_TYPE ViewType)
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
	D3DResView->SetResourceView(Count, DescripterElementSize, CpuView, GpuView, ViewType);
	return true;
}

//View Creater
FD3D12ResourceViewCreater::FD3D12ResourceViewCreater()
:ParentDevice(nullptr)
{

}

FD3D12ResourceViewCreater::FD3D12ResourceViewCreater(ID3D12Device* Device)
:ParentDevice(Device)
{

}

FD3D12ResourceViewCreater::~FD3D12ResourceViewCreater()
{
}

void FD3D12ResourceViewCreater::OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	DsvHeap = new FD3D12ResourceViewHeap();
	DsvHeap->OnCreate(ParentDevice, static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV), DsvCount);

	RtvHeap = new FD3D12ResourceViewHeap();
	RtvHeap->OnCreate(ParentDevice, static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV), RtvCount);

	SamplerHeap = new FD3D12ResourceViewHeap();
	SamplerHeap->OnCreate(ParentDevice, static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SAMPLER), SamplerCount);

	CbvSrvUavHeap = new FD3D12ResourceViewHeap();
	CbvSrvUavHeap->OnCreate(ParentDevice, static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV) | 
		static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV) | static_cast<UINT>(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_UAV)
		, CbvCount + SrvCount + UavCount);
}

bool FD3D12ResourceViewCreater::AllocDescriptor(UINT Count, E_RESOURCE_VIEW_TYPE HeapType, FResourceView* ResView)
{
	if (HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV)
	{
		return DsvHeap->AllocDescriptor(Count, ResView, HeapType);
	}
	else if (HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV)
	{
		return RtvHeap->AllocDescriptor(Count, ResView, HeapType);
	}
	else if (HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV || HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV ||
		HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_UAV)
	{
		return CbvSrvUavHeap->AllocDescriptor(Count, ResView, HeapType);
	}
	else if (HeapType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SAMPLER)
	{
		return SamplerHeap->AllocDescriptor(Count, ResView, HeapType);
	}
	return false;
}

void FD3D12ResourceViewCreater::OnDestroy()
{
	if(DsvHeap)
	{	
		DsvHeap->OnDestroy();
		delete DsvHeap; 
		DsvHeap = nullptr;
	}

	if (RtvHeap)
	{
		RtvHeap->OnDestroy();
		delete RtvHeap;
		RtvHeap = nullptr;
	}

	if (SamplerHeap)
	{
		SamplerHeap->OnDestroy();
		delete SamplerHeap;
		SamplerHeap = nullptr;
	}

	if (CbvSrvUavHeap)
	{
		CbvSrvUavHeap->OnDestroy();
		delete CbvSrvUavHeap;
		CbvSrvUavHeap = nullptr;
	}
}