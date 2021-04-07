#pragma once

#include "d3dx12.h"
#include "FDefine.h"
#include "FIndexBuffer.h"
#include "FVertexBuffer.h"
#include "FTexture.h"
#include "FRenderResource.h"
#include "FD3D12Adapter.h"
#include "FTexture.h"

using namespace Microsoft::WRL;

class FD3D12IndexBuffer : public FIndexBuffer
{
public: 
	FD3D12IndexBuffer();
	virtual ~FD3D12IndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	void InitGPUIndexBufferView(FD3D12Adapter* Adapter);

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const
	{
		return IndexBufferView;
	}

private: 
	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> IndexBufferUpload;

	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
};

class FD3D12VertexBuffer : public FVertexBuffer
{
public:
	FD3D12VertexBuffer();
	virtual ~FD3D12VertexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	
	void InitGPUVertexBufferView(FD3D12Adapter* Adapter);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()
	{
		return VertexBufferView;
	}
private: 

	ComPtr<ID3D12Resource> VertexBuffer;
	ComPtr<ID3D12Resource> VertexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
};

class FD3D12Texture : public FTexture
{
public:
	FD3D12Texture();
	virtual ~FD3D12Texture();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void InitGPUTextureView(FD3D12Adapter* Adapter);
private:

	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> TextureResourceUpload;

};

class FD3D12Adapter;
class FD3DConstantBuffer :public FRenderResource
{
public:
	FD3DConstantBuffer();
	virtual ~FD3DConstantBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetConstantBufferInfo(FD3D12Adapter* Adapter, UINT BufferSize,  UINT DataSize);
	void UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize);
	ID3D12Resource* GetD3DConstantBuffer() const
	{
		return ConstantBuffer.Get();
	}

	UINT GetBufferSize() const
	{
		return BufferSize;
	}

	UINT GetCbvSrvUavDescriptorSize()
	{
		return CbvSrvUavDescriptorSize;
	}

private:
	UINT BufferSize;

	UINT8* pCbvDataBegin;
	UINT CbvSrvUavDescriptorSize;
	ComPtr<ID3D12Resource> ConstantBuffer;
};