#pragma once

#include "d3dx12.h"
#include "FDefine.h"
#include "FIndexBuffer.h"
#include "FVertexBuffer.h"
#include "FTexture.h"
#include "FRenderResource.h"
#include "FDDSTextureLoader.h"

using namespace Microsoft::WRL;

class FD3D12IndexBuffer : public FIndexBuffer
{
public: 
	FD3D12IndexBuffer();
	virtual ~FD3D12IndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	void InitGPUIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

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
	
	void InitGPUVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

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
	virtual void InitializeTexture(const std::string& Name) override;
	virtual void InitializeTexture(FTextureInitializer Initializer) override;

	void InitGPUTextureView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, UINT CbvSrvUavDescriptorSize, ID3D12DescriptorHeap* CbvSrvHeap, FCbvSrvDesc& Desc);

	void InitGPUTextureView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, DXGI_FORMAT ViewFormat);

	ID3D12Resource* Resource()
	{
		return TextureResource.Get();
	}
private:

	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> TextureResourceUpload;

	ID3D12Device* ParentDevice;

	DDS_HEADER* Header;
	std::unique_ptr<uint8_t[]> DdsData;
};

class FD3DConstantBuffer :public FRenderResource
{
public:
	FD3DConstantBuffer();
	virtual ~FD3DConstantBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void SetConstantBufferInfo(ID3D12Device* Device, UINT BufferSize);
	void UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize);
	ID3D12Resource* GetD3DConstantBuffer() const
	{
		return ConstantBuffer.Get();
	}

	UINT GetBufferSize() const
	{
		return BufferSize;
	}

private:
	UINT BufferSize;

	UINT8* pCbvDataBegin;
	ComPtr<ID3D12Resource> ConstantBuffer;
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

class FD3DShaderMap : public FRenderResource
{
public:
	FD3DShaderMap();
	FD3DShaderMap(ID3D12Device* Device, UINT InWidth, UINT InHeight);
	virtual ~FD3DShaderMap();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuDsv);

	ID3D12Resource* Resource()
	{
		return ShadowMap.Get();
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const
	{
		return GpuSrv;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() const
	{
		return CpuDsv;
	}

	D3D12_VIEWPORT Viewport() const
	{
		return ViewPort;
	}

	D3D12_RECT ScissorRect() const
	{
		return RScissorRect;
	}

private:
	D3D12_VIEWPORT ViewPort;
	D3D12_RECT RScissorRect;

	ID3D12Device* ParentDevice;
	UINT Width;
	UINT Height;
	ComPtr<ID3D12Resource> ShadowMap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuDsv;
};