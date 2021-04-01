#pragma once

#include "RHIResource.h"
#include "D3D12Device.h"
#include "d3dx12.h"

using namespace Microsoft::WRL;

class D3DResource : public RHIResource
{
public: 
	D3DResource();
	D3DResource(D3D12Device* InDevice);
	virtual ~D3DResource();

	virtual void Destroy() = 0;
	virtual void Initialize() = 0;
	
protected:
	D3D12Device* ParentDevice;
};

class D3DRenderTarget : public D3DResource
{
public: 
	D3DRenderTarget();
	D3DRenderTarget(D3D12Device* InDevice);
	virtual ~D3DRenderTarget();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetRenderTargetCount(UINT Count);

	ID3D12Resource* GetRenderTarget(UINT TargetIndex)
	{
		return RenderTargets[TargetIndex].Get();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(UINT TargetIndex);
	
private:
	std::vector<ComPtr<ID3D12Resource>> RenderTargets;

	UINT RtvDescriptorSize;
	UINT TargetCount;
};


class D3DConstantBuffer :public D3DResource
{
public:
	D3DConstantBuffer();
	D3DConstantBuffer(D3D12Device* InDevice);
	virtual ~D3DConstantBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetConstantBufferInfo(UINT BufferSize, void* pDataFrom, void** pDataMap);
private: 
	
	UINT BufferSize;
	UINT* BufferData;

	UINT CbvSrvUavDescriptorSize;
	ComPtr<ID3D12Resource> ConstantBuffer;
};
