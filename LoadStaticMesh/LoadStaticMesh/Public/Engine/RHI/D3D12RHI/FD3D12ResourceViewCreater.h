#pragma once

#include "stdafx.h"
#include "FResourceViewCreater.h"
#include "FDefine.h"

using namespace Microsoft::WRL;

class FD3D12ResourceView : public FResourceView
{
public: 
	UINT GetCount() { return Count;}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpu(UINT i = 0);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpu(UINT i = 0);
	
	void SetResourceView(UINT InCount, UINT DesSize, D3D12_CPU_DESCRIPTOR_HANDLE CpuDes, D3D12_GPU_DESCRIPTOR_HANDLE GpuDes);
protected: 
	
	//resource count, may be more than one
	UINT Count = 0;
	UINT DescriptorSize = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptor;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor;
};

class FD3D12CbvResourceView : public FD3D12ResourceView
{
public: 
	void SetConstantBufferViewInfo(ID3D12Device* Device, UINT InBufferSize);
	void UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize);
private: 
	UINT BufferSize;
	UINT8* pCbvDataBegin;
	ComPtr<ID3D12Resource> ConstantBuffer;
};

class FD3D12ResourceViewHeap
{
public:
	void OnCreate(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, UINT InDescriptorCount);
	void OnDestroy();
	bool AllocDescriptor(UINT Count, FResourceView* Rv);

	ID3D12DescriptorHeap* GetHeap() {  return Heap; }
private:
	void SetNameByType(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);

	UINT Index; 
	UINT DescripterNum;
	UINT DescripterElementSize;

	ID3D12DescriptorHeap* Heap;

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

	FD3D12ResourceViewHeap DsvHeap;
	FD3D12ResourceViewHeap RtvHeap;
	FD3D12ResourceViewHeap SamplerHeap;
	FD3D12ResourceViewHeap CbvSrvUavHeap;
};
