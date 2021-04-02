#pragma once

#include "FRHIResource.h"
#include "FD3D12Device.h"
#include "d3dx12.h"

using namespace Microsoft::WRL;

class FD3DResource : public FRHIResource
{
public: 
	FD3DResource();
	FD3DResource(FD3D12Device* InDevice);
	virtual ~FD3DResource();

	virtual void Destroy() = 0;
	virtual void Initialize() = 0;
	
protected:
	FD3D12Device* ParentDevice;
};

class D3DRenderTarget : public FD3DResource
{
public: 
	D3DRenderTarget();
	D3DRenderTarget(FD3D12Device* InDevice);
	virtual ~D3DRenderTarget();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetRenderTargetCount(UINT Count);

	ID3D12Resource* GetRenderTarget(UINT TargetIndex)
	{
		return RenderTargets[TargetIndex].Get();
	}

	UINT GetRtvDescriptorSize() const
	{
		return RtvDescriptorSize;
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(UINT TargetIndex);
	
private:
	std::vector<ComPtr<ID3D12Resource>> RenderTargets;

	UINT RtvDescriptorSize;
	UINT TargetCount;
};


class D3DConstantBuffer :public FD3DResource
{
public:
	D3DConstantBuffer();
	D3DConstantBuffer(FD3D12Device* InDevice);
	virtual ~D3DConstantBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetConstantBufferInfo(UINT BufferSize, void* pDataFrom, UINT DataSize);
	void UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize);
	ID3D12Resource* GetD3DConstantBuffer() const
	{
		return ConstantBuffer.Get();
	}

	UINT GetBufferSize() const
	{
		return BufferSize;
	}

	UINT GetCbvSrvUavDescriptorSize() const 
	{
		return CbvSrvUavDescriptorSize;
	}
private: 
	UINT BufferSize;

	UINT8* pCbvDataBegin;
	UINT CbvSrvUavDescriptorSize;
	ComPtr<ID3D12Resource> ConstantBuffer;
};


class D3DDepthStencilBuffer :public FD3DResource
{
public:
	D3DDepthStencilBuffer();
	D3DDepthStencilBuffer(FD3D12Device* InDevice);
	virtual ~D3DDepthStencilBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetDepthStencilSize(UINT Width, UINT Height);

	ID3D12Resource* GetD3DDepthStencilBuffer() const
	{
		return DepthStencilBuffer.Get();
	}
private:

	UINT DsvDescriptorSize;
	ComPtr<ID3D12Resource> DepthStencilBuffer;
};

class D3DVertexBuffer :public FD3DResource
{
public:
	D3DVertexBuffer();
	D3DVertexBuffer(FD3D12Device* InDevice, const void* InitData, UINT StrideInByte, UINT ByteSize);
	virtual ~D3DVertexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	UINT GetStrideInByte() const {
		return StrideInByte;
	}

	UINT GetByteSize() const {
		return ByteSize;
	}

	ID3D12Resource* GetD3DVertexBuffer()
	{
		return VertexBuffer.Get();
	}
private: 
	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> VertexUploadBuffer;

	UINT StrideInByte;
	UINT ByteSize;
};

class D3DIndexBuffer :public FD3DResource
{
public:
	D3DIndexBuffer();
	D3DIndexBuffer(FD3D12Device* InDevice, const void* InitData, UINT ByteSize,UINT IndicesCount, bool bUseHalfInt32);
	virtual ~D3DIndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	UINT GetDataSize() const {
		return ByteSize;
	}

	UINT GetIndicesCount() const 
	{
		return IndicesCount;
	}

	bool UseHalfInt32() const {
		return bUseHalfInt32;
	}

	ID3D12Resource* GetD3DIndexBuffer()
	{
		return IndexBuffer.Get();
	}
private:
	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> IndexBufferUpload;

	bool bUseHalfInt32;
	UINT ByteSize;
	UINT IndicesCount;
};


class D3DShaderResource :public FD3DResource
{
public:
	D3DShaderResource();
	D3DShaderResource(FD3D12Device* InDevice, std::wstring TextureName);
	virtual ~D3DShaderResource();

	virtual void Destroy() override;
	virtual void Initialize() override;
	
	ID3D12Resource* GetD3DShaderResource()
	{
		return TextureResource.Get();
	}

	UINT GetSrvDescripterSize() const
	{
		return CbvSrvUavDescriptorSize;
	}
private:
	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> TextureResourceUpload;
	UINT CbvSrvUavDescriptorSize;
};





/// ////////////
class D3DView : public FRHIView
{
public:
	D3DView();
	D3DView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes);
	virtual ~D3DView();
protected:
	FD3D12Device* ParentDevice;
};

class D3DConstantBufferView : public D3DView
{
public:
	D3DConstantBufferView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes);
	virtual ~D3DConstantBufferView();

};

class D3DDepthStencilView : public D3DView
{
public:
	D3DDepthStencilView(FD3D12Device* InDevice, ID3D12Resource* DepthStencilBuffer);
	virtual ~D3DDepthStencilView();
};

class D3DVertexBufferView : public D3DView
{
public:
	D3DVertexBufferView(FD3D12Device* InDevice, D3DVertexBuffer* InVertexBuffer);
	virtual ~D3DVertexBufferView();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const
	{
		return VertexBufferView;
	}

	virtual void Clear() override;
private: 
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3DVertexBuffer* VertexBuffer;
};

class D3DIndexBufferView : public D3DView
{
public:
	D3DIndexBufferView(FD3D12Device* InDevice, D3DIndexBuffer* InIndexBuffer);
	virtual ~D3DIndexBufferView();

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const
	{
		return IndexBufferView;
	}

	UINT GetIndicesCount()
	{
		return IndexBuffer->GetIndicesCount();
	}
	virtual void Clear() override;
private:
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	D3DIndexBuffer* IndexBuffer;
};


class D3DShaderResourceView : public D3DView
{
public:
	D3DShaderResourceView(FD3D12Device* InDevice, D3DShaderResource* InShaderResource);
	virtual ~D3DShaderResourceView();

	virtual void Clear() override;
private:
	D3DShaderResource* ShaderResource;
};