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

class FD3DRenderTarget : public FD3DResource
{
public: 
	FD3DRenderTarget();
	FD3DRenderTarget(FD3D12Device* InDevice);
	virtual ~FD3DRenderTarget();

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


class FD3DConstantBuffer :public FD3DResource
{
public:
	FD3DConstantBuffer();
	FD3DConstantBuffer(FD3D12Device* InDevice);
	virtual ~FD3DConstantBuffer();

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


class FD3DDepthStencilBuffer :public FD3DResource
{
public:
	FD3DDepthStencilBuffer();
	FD3DDepthStencilBuffer(FD3D12Device* InDevice);
	virtual ~FD3DDepthStencilBuffer();

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

class FD3DVertexBuffer :public FD3DResource
{
public:
	FD3DVertexBuffer();
	FD3DVertexBuffer(FD3D12Device* InDevice, const void* InitData, UINT StrideInByte, UINT ByteSize);
	virtual ~FD3DVertexBuffer();

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

class FD3DIndexBuffer :public FD3DResource
{
public:
	FD3DIndexBuffer();
	FD3DIndexBuffer(FD3D12Device* InDevice, const void* InitData, UINT ByteSize,UINT IndicesCount, bool bUseHalfInt32);
	virtual ~FD3DIndexBuffer();

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


class FD3DShaderResource :public FD3DResource
{
public:
	FD3DShaderResource();
	FD3DShaderResource(FD3D12Device* InDevice, std::wstring TextureName);
	virtual ~FD3DShaderResource();

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
class FD3DView : public FRHIView
{
public:
	FD3DView();
	FD3DView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes);
	virtual ~FD3DView();
protected:
	FD3D12Device* ParentDevice;
};

class FD3DConstantBufferView : public FD3DView
{
public:
	FD3DConstantBufferView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes);
	virtual ~FD3DConstantBufferView();

};

class FD3DDepthStencilView : public FD3DView
{
public:
	FD3DDepthStencilView(FD3D12Device* InDevice, ID3D12Resource* DepthStencilBuffer);
	virtual ~FD3DDepthStencilView();
};

class FD3DVertexBufferView : public FD3DView
{
public:
	FD3DVertexBufferView(FD3D12Device* InDevice, FD3DVertexBuffer* InVertexBuffer);
	virtual ~FD3DVertexBufferView();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const
	{
		return VertexBufferView;
	}

	virtual void Clear() override;
private: 
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	FD3DVertexBuffer* VertexBuffer;
};

class FD3DIndexBufferView : public FD3DView
{
public:
	FD3DIndexBufferView(FD3D12Device* InDevice, FD3DIndexBuffer* InIndexBuffer);
	virtual ~FD3DIndexBufferView();

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
	FD3DIndexBuffer* IndexBuffer;
};


class FD3DShaderResourceView : public FD3DView
{
public:
	FD3DShaderResourceView(FD3D12Device* InDevice, FD3DShaderResource* InShaderResource);
	virtual ~FD3DShaderResourceView();

	virtual void Clear() override;
private:
	FD3DShaderResource* ShaderResource;
};