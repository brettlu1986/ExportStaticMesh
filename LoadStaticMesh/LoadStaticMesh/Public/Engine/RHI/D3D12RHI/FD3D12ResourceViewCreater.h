#pragma once

#include "stdafx.h"
#include "FResourceViewCreater.h"
#include "FDefine.h"
#include "FD3D12Resource.h"

using namespace Microsoft::WRL;

class FD3D12ResourceView : public FResourceView
{
public: 
	FD3D12ResourceView()
	:CpuDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE())
	,GpuDescriptor(D3D12_GPU_DESCRIPTOR_HANDLE())
	,ViewResource(nullptr)
	,Type(E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_NONE)
	{};
	virtual ~FD3D12ResourceView() 
	{
		if (ViewResource != nullptr)
		{
			ViewResource->Release();
		}
	};
	UINT GetCount() { return Count;}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpu(UINT i = 0);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpu(UINT i = 0);
	
	void SetResourceView(UINT InCount, UINT DesSize, D3D12_CPU_DESCRIPTOR_HANDLE CpuDes, D3D12_GPU_DESCRIPTOR_HANDLE GpuDes, E_RESOURCE_VIEW_TYPE ViewType);

	E_RESOURCE_VIEW_TYPE GetViewType()
	{
		return Type;
	}

	ID3D12Resource* Resource()
	{
		return ViewResource;
	}

	void SetViewResource(ID3D12Resource* Res)
	{
		ViewResource = Res;
	}
protected: 
	E_RESOURCE_VIEW_TYPE Type;
	ID3D12Resource* ViewResource;
	//resource count, may be more than one
	UINT Count = 0;
	UINT DescriptorSize = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptor;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor;
};

class FD3D12DsvResourceView : public FD3D12ResourceView
{
public:
	FD3D12DsvResourceView(){};
	virtual ~FD3D12DsvResourceView(){};

	void SetDsvViewInfo(ID3D12Device* Device, FD3D12Texture* Tex, DXGI_FORMAT Format);
};

class FD3D12RtvResourceView : public FD3D12ResourceView
{
public:
	FD3D12RtvResourceView() {};
	virtual ~FD3D12RtvResourceView(){};

	void SetRtvViewInfo(ID3D12Device* Device, IDXGISwapChain* SwapChain, DXGI_FORMAT Format, UINT SwapBufferIndex);
	void SetRtvViewInfo(ID3D12Device* Device, FD3D12Texture* Tex, DXGI_FORMAT Format);
};

class FD3D12CbvResourceView : public FD3D12ResourceView
{
public: 
	FD3D12CbvResourceView() 
	:pCbvDataBegin(nullptr)
	,BufferSize(0)
	{};
	virtual ~FD3D12CbvResourceView()
	{
		if (ConstantBuffer)
		{
			ConstantBuffer->Unmap(0, nullptr);
			ConstantBuffer.Reset();
		}
	}
	void SetConstantBufferViewInfo(ID3D12Device* Device, UINT InBufferSize);
	void UpdateConstantBufferInfo(void* pDataUpdate);

	UINT GetBufferSize() { return BufferSize;}
private: 
	UINT BufferSize;
	UINT8* pCbvDataBegin;
	ComPtr<ID3D12Resource> ConstantBuffer;
};

class FD3D12ResourceViewHeap : public FResourceHeap
{
public:
	FD3D12ResourceViewHeap();
	virtual ~FD3D12ResourceViewHeap();

	virtual void OnCreate(ID3D12Device* Device, UINT HeapType, UINT InDescriptorCount) override;
	virtual void OnDestroy() override;
	virtual bool AllocDescriptor(UINT Count, FResourceView* Rv, E_RESOURCE_VIEW_TYPE ViewType) override;

	ID3D12DescriptorHeap* GetHeap() {  return Heap.Get(); }
private:
	D3D12_DESCRIPTOR_HEAP_TYPE GetD3DHeapType(UINT Type);
	void SetNameByType(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);

	UINT Index; 
	UINT DescripterNum;
	UINT DescripterElementSize;

	ComPtr<ID3D12DescriptorHeap> Heap;
};

class FD3D12ResourceViewCreater : public FResourceViewCreater
{
public:
	FD3D12ResourceViewCreater();
	FD3D12ResourceViewCreater(ID3D12Device* Device);
	virtual ~FD3D12ResourceViewCreater();

	virtual void OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) override;
	virtual void OnDestroy() override;

	bool AllocDescriptor(UINT Count, E_RESOURCE_VIEW_TYPE HeapType, FResourceView* ResView);

private:
	ID3D12Device* ParentDevice;
};
