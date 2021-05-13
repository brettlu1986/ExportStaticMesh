
#include "stdafx.h"
#include "FD3D12Resource.h"
#include "FD3D12Helper.h"
#include "FDDSTextureLoader.h"
#include "FRenderResource.h"
#include <stdlib.h>

FD3D12IndexBuffer::FD3D12IndexBuffer()
:FIndexBuffer(E_RESOURCE_TYPE::TYPE_INDEX_BUFFER)
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

void FD3D12IndexBuffer::Initialize()
{

}



/// //////////////////////////////////
FD3D12VertexBuffer::FD3D12VertexBuffer()
:FVertexBuffer(E_RESOURCE_TYPE::TYPE_VERTEX_BUFFER)
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
	if(bSKMVertex)
	{
		CreateBuffer(Device, CommandList, SKMVertexData.data(), VertexDataSize, VertexBuffer, VertexUploadBuffer);
	}
	else 
	{
		CreateBuffer(Device, CommandList, VertexData.data(), VertexDataSize, VertexBuffer, VertexUploadBuffer);
	}
	NAME_D3D12_OBJECT(VertexBuffer);
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = StandardInputStride;
	VertexBufferView.SizeInBytes = VertexDataSize;
}

/////////////////////////

FD3D12Texture::FD3D12Texture()
	:FTexture(E_RESOURCE_TYPE::TYPE_TEXTURE)
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

void FD3D12Texture::InitializeTexture(const std::string& Name)
{
	size_t Len = strlen(Name.c_str()) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, Name.c_str(), _TRUNCATE);

	DirectX::LoadTextureDataFromFile(WStr, DdsData, &Header, &BitData, &BitSize);
	delete[] WStr;
}

void FD3D12Texture::InitGPUTextureView(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, UINT CbvSrvUavDescriptorSize, ID3D12DescriptorHeap* CbvSrvHeap, FCbvSrvDesc& CbvSrvDesc)
{
	DirectX::CreateTextureFromDDS12(Device, CommandList, Header, BitData, BitSize, 0, false, TextureResource, TextureResourceUpload);
	NAME_D3D12_OBJECT(TextureResource);
	NAME_D3D12_OBJECT(TextureResourceUpload);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = TextureResource->GetDesc().MipLevels;

	UINT SrvOffsetFromFirstTex = GetTextureHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvHandle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvDesc.SrvDesc.HeapOffsetStart + SrvOffsetFromFirstTex, CbvSrvUavDescriptorSize);
	Device->CreateShaderResourceView(TextureResource.Get(), &srvDesc, CbvSrvHandle);
}


FD3DConstantBuffer::FD3DConstantBuffer()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_CONSTANT_BUFFER)
	, pCbvDataBegin(nullptr)
{
}

FD3DConstantBuffer::~FD3DConstantBuffer()
{
}

void FD3DConstantBuffer::Destroy()
{
	if(ConstantBuffer)
	{
		ConstantBuffer->Unmap(0, nullptr);
		ConstantBuffer.Reset();
	}
}

void FD3DConstantBuffer::Initialize()
{
}

void FD3DConstantBuffer::SetConstantBufferInfo(ID3D12Device* Device, UINT InBufferSize)
{
	BufferSize = InBufferSize;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ThrowIfFailed(Device->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));
	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin)));
	memset(pCbvDataBegin, 0, InBufferSize);
	NAME_D3D12_OBJECT(ConstantBuffer);
}

void FD3DConstantBuffer::UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize)
{	
	memcpy(pCbvDataBegin, pDataUpdate, BufferSize);
}

FD3DGraphicPipline::FD3DGraphicPipline()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_PIPLINE)
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


FD3DShaderMap::FD3DShaderMap()
	:FRenderResource(E_RESOURCE_TYPE::TYPE_SHADERMAP)
	, ParentDevice(nullptr)
	, Width(0)
	, Height(0)
{

}

FD3DShaderMap::FD3DShaderMap(ID3D12Device* Device, UINT InWidth, UINT InHeight)
	:FRenderResource(E_RESOURCE_TYPE::TYPE_SHADERMAP)
	,ParentDevice(Device)
	,Width(InWidth)
	,Height(InHeight)
{
	Initialize();
}

FD3DShaderMap::~FD3DShaderMap()
{

}

void FD3DShaderMap::Destroy()
{
	ShadowMap.Reset();
}

void FD3DShaderMap::Initialize() 
{
	ViewPort = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
	RScissorRect = { 0, 0, (int)Width, (int)Height };

	D3D12_RESOURCE_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(D3D12_RESOURCE_DESC));
	TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TexDesc.Alignment = 0;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.DepthOrArraySize = 1;
	TexDesc.MipLevels = 1;
	TexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE OptClear;
	OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	OptClear.DepthStencil.Depth = 1.0f;
	OptClear.DepthStencil.Stencil = 0;

	const CD3DX12_HEAP_PROPERTIES Prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(ParentDevice->CreateCommittedResource(
		&Prop,
		D3D12_HEAP_FLAG_NONE,
		&TexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&OptClear,
		IID_PPV_ARGS(&ShadowMap)));
}

void FD3DShaderMap::BuildDescriptors( CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv,
	CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuDsv)
{
	CpuSrv = InCpuSrv;
	GpuSrv = InGpuSrv;
	CpuDsv = InCpuDsv;

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = 1;
	SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	SrvDesc.Texture2D.PlaneSlice = 0;
	ParentDevice->CreateShaderResourceView(ShadowMap.Get(), &SrvDesc, CpuSrv);

	D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
	DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DsvDesc.Texture2D.MipSlice = 0;
	ParentDevice->CreateDepthStencilView(ShadowMap.Get(), &DsvDesc, CpuDsv);
}