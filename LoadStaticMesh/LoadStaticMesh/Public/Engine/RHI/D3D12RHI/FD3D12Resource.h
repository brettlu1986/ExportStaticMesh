#pragma once

#include "FRHIResource.h"
#include "FD3D12Device.h"
#include "d3dx12.h"
#include "FDefine.h"

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



class FD3DShaderResourceView : public FD3DView
{
public:
	FD3DShaderResourceView(FD3D12Device* InDevice, FD3DShaderResource* InShaderResource);
	virtual ~FD3DShaderResourceView();

	virtual void Clear() override;
private:
	FD3DShaderResource* ShaderResource;
};


////////////////
#include "FIndexBuffer.h"
#include "FVertexBuffer.h"
#include "FRenderResource.h"
#include "FD3D12Adapter.h"

class FD3D12IndexBuffer : public FIndexBuffer
{
public: 
	FD3D12IndexBuffer();
	virtual ~FD3D12IndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	void InitGPUIndexBuffer(FD3D12Adapter* Adapter);

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

