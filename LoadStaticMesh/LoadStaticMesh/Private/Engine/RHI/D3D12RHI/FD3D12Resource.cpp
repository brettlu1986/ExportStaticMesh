
#include "FD3D12Resource.h"
#include "FD3D12Helper.h"
#include "FDDSTextureLoader.h"
#include "FRenderResource.h"
#include "FD3D12Adapter.h"
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

void FD3D12IndexBuffer::InitGPUIndexBufferView(FD3D12Adapter* Adapter)
{
	ID3D12GraphicsCommandList* CommandList = Adapter->GetCommandListManager()->GetDefaultCommandList();
	CreateBuffer(Adapter->GetD3DDevice(), CommandList, IndicesData, IndicesByteSize, IndexBuffer, IndexBufferUpload);
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

void FD3D12VertexBuffer::InitGPUVertexBufferView(FD3D12Adapter* Adapter)
{
	ID3D12GraphicsCommandList* CommandList = Adapter->GetCommandListManager()->GetDefaultCommandList();
	CreateBuffer(Adapter->GetD3DDevice(), CommandList, VertexData.data(), VertexDataSize, VertexBuffer, VertexUploadBuffer);
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

void FD3D12Texture::InitGPUTextureView(FD3D12Adapter* Adapter)
{
	ID3D12GraphicsCommandList* CommandList = Adapter->GetCommandListManager()->GetDefaultCommandList();

	DirectX::CreateTextureFromDDS12(Adapter->GetD3DDevice(), CommandList, Header, BitData, BitSize, 0, false, TextureResource, TextureResourceUpload);
	NAME_D3D12_OBJECT(TextureResource);
	NAME_D3D12_OBJECT(TextureResourceUpload);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = TextureResource->GetDesc().MipLevels;

	UINT CbvSrvUavDescriptorSize = Adapter->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap = Adapter->CbvSrvHeap;
	UINT SrvOffsetInHeap = Adapter->GetSrvOffsetInHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvHandle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), SrvOffsetInHeap, CbvSrvUavDescriptorSize);
	Adapter->GetD3DDevice()->CreateShaderResourceView(TextureResource.Get(), &srvDesc, CbvSrvHandle);
}

/// ////////////////////////////////////



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

void FD3DConstantBuffer::SetConstantBufferInfo(FD3D12Adapter* Adapter, UINT InBufferSize)
{
	CbvSrvUavDescriptorSize = Adapter->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	BufferSize = InBufferSize;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ThrowIfFailed(Adapter->GetD3DDevice()->CreateCommittedResource(
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
	/*int8_t* pData = (int8_t*)pDataUpdate;
	UINT ConstantBufferSingleSize = (sizeof(ObjectConstants) + 255) & ~255;
	XMFLOAT4X4 WVP[2];
	for (UINT i = 0; i < 2; i++)
	{
		memcpy(&WVP[i], pData + i * ConstantBufferSingleSize, sizeof(XMFLOAT4X4));
	}*/
	memcpy(pCbvDataBegin, pDataUpdate, BufferSize);
}

