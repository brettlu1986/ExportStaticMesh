
#include "stdafx.h"
#include "FD3D12Resource.h"
#include "FD3D12Helper.h"
#include "FDDSTextureLoader.h"
#include "FRenderResource.h"
#include <stdlib.h>

FD3D12IndexBuffer::FD3D12IndexBuffer()
:FIndexBuffer(E_FRESOURCE_TYPE::F_TYPE_INDEX_BUFFER)
{

}

FD3D12IndexBuffer::~FD3D12IndexBuffer()
{
	Destroy();
}

void FD3D12IndexBuffer::Destroy()
{
	if(IndexBuffer)
	{
		IndexBuffer.Reset();
	}
	
	if(IndexBufferUpload)
	{
		IndexBufferUpload.Reset();
	}
}

void FD3D12IndexBuffer::InitGPUIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	CreateBuffer(Device, CommandList, IndicesData, IndicesByteSize, IndexBuffer, IndexBufferUpload);
	NAME_D3D12_OBJECT(IndexBuffer);
	NAME_D3D12_OBJECT(IndexBufferUpload);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = IndicesType == E_INDEX_TYPE::TYPE_UINT_16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndicesByteSize;
}

void FD3D12IndexBuffer::InitGPUIndexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, LIndexBuffer& IndexBufferData)
{
	CreateBuffer(Device, CommandList, IndexBufferData.GetIndicesData(), IndexBufferData.GetIndicesDataSize(), IndexBuffer, IndexBufferUpload);
	NAME_D3D12_OBJECT(IndexBuffer);
	NAME_D3D12_OBJECT(IndexBufferUpload);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = IndexBufferData.GetIndicesType() == E_INDEX_TYPE::TYPE_UINT_16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndexBufferData.GetIndicesDataSize();
}

void FD3D12IndexBuffer::Initialize()
{

}



/// //////////////////////////////////
FD3D12VertexBuffer::FD3D12VertexBuffer()
:FVertexBuffer(E_FRESOURCE_TYPE::F_TYPE_VERTEX_BUFFER)
,VertexBufferView(D3D12_VERTEX_BUFFER_VIEW())
{

}

FD3D12VertexBuffer::~FD3D12VertexBuffer()
{
	Destroy();
}

void FD3D12VertexBuffer::Destroy()
{
	if(VertexBuffer)
	{
		VertexBuffer.Reset();
	}
	
	if(VertexUploadBuffer)
	{
		VertexUploadBuffer.Reset();
	}
	
}

void FD3D12VertexBuffer::Initialize()
{

}

void FD3D12VertexBuffer::InitGPUVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	CreateBuffer(Device, CommandList, VertexByteData, VertexDataSize, VertexBuffer, VertexUploadBuffer);
	NAME_D3D12_OBJECT(VertexBuffer);
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexDataSize / VertexCount;
	VertexBufferView.SizeInBytes = VertexDataSize;
}

void FD3D12VertexBuffer::InitGPUVertexBufferView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, LVertexBuffer& VertexBufferData)
{
	CreateBuffer(Device, CommandList, VertexBufferData.GetVertexByteData(), VertexBufferData.GetVertexDataSize(), VertexBuffer, VertexUploadBuffer);
	NAME_D3D12_OBJECT(VertexBuffer);
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexBufferData.GetVertexDataSize() / VertexBufferData.GetVertexCount();
	VertexBufferView.SizeInBytes = VertexBufferData.GetVertexDataSize();
}

/////////////////////////

FD3D12Texture::FD3D12Texture(ID3D12Device* Device)
	:FTexture(E_FRESOURCE_TYPE::F_TYPE_TEXTURE)
	,ParentDevice(Device)
{

}

FD3D12Texture::~FD3D12Texture()
{
}

void FD3D12Texture::Destroy()
{
	if(TextureResource)
	{
		TextureResource.Reset();
	}
	if(TextureResourceUpload)
	{
		TextureResourceUpload.Reset();
	}
	
}

void FD3D12Texture::Initialize()
{

}

void FD3D12Texture::InitializeTexture(FTextureInitializer Initializer) 
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = Initializer.Width;
	texDesc.Height = Initializer.Height;
	texDesc.DepthOrArraySize = Initializer.DepthOrArraySize;
	texDesc.MipLevels = Initializer.MipLevels;
	texDesc.Format = static_cast<DXGI_FORMAT>(Initializer.Format);
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(Initializer.Flags);

	D3D12_CLEAR_VALUE ClearValues;
	if(Initializer.ClearValue != nullptr)
	{	
		if(Initializer.ClearValue->ClearColor != nullptr && Initializer.ClearValue->ClearDepth != nullptr)
		{
			assert(!"the d3d clear value is union, only set one, can not set both!");
		}

		ClearValues.Format = static_cast<DXGI_FORMAT>(Initializer.ClearValue->Format);
		if(Initializer.ClearValue->ClearColor)
		{
			FClearColor* Colors = Initializer.ClearValue->ClearColor;
			ClearValues.Color[0] = Colors->Color[0];
			ClearValues.Color[1] = Colors->Color[1];
			ClearValues.Color[2] = Colors->Color[2];
			ClearValues.Color[3] = Colors->Color[3];
		}

		if(Initializer.ClearValue->ClearDepth)
		{
			ClearValues.DepthStencil.Depth = Initializer.ClearValue->ClearDepth->Depth;
			ClearValues.DepthStencil.Stencil = Initializer.ClearValue->ClearDepth->Stencil;
		}
	}

	CD3DX12_HEAP_PROPERTIES DefaultPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(ParentDevice->CreateCommittedResource(
		&DefaultPro,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		static_cast<D3D12_RESOURCE_STATES>(Initializer.ResourceState),
		Initializer.ClearValue == nullptr? nullptr : &ClearValues,
		IID_PPV_ARGS(&TextureResource)
	));

}

void FD3D12Texture::InitializeTexture(const string& Name)
{
	size_t Len = strlen(Name.c_str()) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, Name.c_str(), _TRUNCATE);

	DirectX::LoadTextureDataFromFile(WStr, DdsData, &Header, &BitData, &BitSize);
	delete[] WStr;

}

void FD3D12Texture::InitGPUTextureView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, DXGI_FORMAT ViewFormat)
{
	if(BitSize != 0)
	{
		DirectX::CreateTextureFromDDS12(Device, CommandList, Header, BitData, BitSize, 0, false, TextureResource, TextureResourceUpload);
		NAME_D3D12_OBJECT(TextureResource);
		NAME_D3D12_OBJECT(TextureResourceUpload);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = BitSize != 0 ? TextureResource->GetDesc().Format : ViewFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = TextureResource->GetDesc().MipLevels;
	Device->CreateShaderResourceView(TextureResource.Get(), &srvDesc, CpuHandle);
	
}

FD3DGraphicPipline::FD3DGraphicPipline()
	:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_PIPLINE)
{

}
FD3DGraphicPipline::~FD3DGraphicPipline()
{
	
}

void FD3DGraphicPipline::Destroy()
{
	PipelineState.Reset();
	RootSignature.Reset();
}

void FD3DGraphicPipline::Initialize()
{

}
