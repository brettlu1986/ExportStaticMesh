#pragma once

#include "d3dx12.h"
#include "FDefine.h"
#include "FIndexBuffer.h"
#include "FVertexBuffer.h"
#include "FTexture.h"
#include "FRenderResource.h"
#include "FDDSTextureLoader.h"

#include "LVertexBuffer.h"
#include "LIndexBuffer.h"

using namespace Microsoft::WRL;

class FD3D12IndexBuffer : public FIndexBuffer
{
public: 
	FD3D12IndexBuffer();
	virtual ~FD3D12IndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	void InitGPUIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, LIndexBuffer& IndexBufferData);

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

	void InitGPUVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, LVertexBuffer& VertexBufferData);

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
	FD3D12Texture(ID3D12Device* Device);
	virtual ~FD3D12Texture();

	virtual void Destroy() override;
	virtual void Initialize() override;
	virtual void InitializeTexture(FTextureInitializer Initializer) override;
	void InitGPUTextureView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, DXGI_FORMAT ViewFormat);

	ID3D12Resource* Resource()
	{
		return TextureResource.Get();
	}
private:
	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> TextureResourceUpload;
	ID3D12Device* ParentDevice;
};

class FD3DGraphicPipline : public FRenderResource
{
public:
	FD3DGraphicPipline();
	virtual ~FD3DGraphicPipline();

	virtual void Destroy() override;
	virtual void Initialize() override;

	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12PipelineState> PipelineState;
};
