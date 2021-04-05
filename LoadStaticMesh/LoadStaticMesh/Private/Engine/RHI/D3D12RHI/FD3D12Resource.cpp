
#include "FD3D12Resource.h"
#include "FD3D12Helper.h"
#include "FDDSTextureLoader.h"

FD3DResource::FD3DResource()
:ParentDevice(nullptr)
{

}

FD3DResource::FD3DResource(FD3D12Device* InDevice)
:ParentDevice(InDevice)
{
}

FD3DResource::~FD3DResource()
{
}



FD3DConstantBuffer::FD3DConstantBuffer()
{
	ResourceName = "ConstantBuffer";
}

FD3DConstantBuffer::FD3DConstantBuffer(FD3D12Device* InDevice)
	:FD3DResource(InDevice)
	,pCbvDataBegin(nullptr)
{	
	ResourceName = "ConstantBuffer";
}

FD3DConstantBuffer::~FD3DConstantBuffer()
{

}

void FD3DConstantBuffer::Destroy() 
{
	ConstantBuffer->Unmap(0, nullptr);
	ConstantBuffer.Reset();
}

void FD3DConstantBuffer::Initialize() 
{
	CbvSrvUavDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FD3DConstantBuffer::SetConstantBufferInfo(UINT InBufferSize, void* pDataFrom, UINT DataSize)
{
	BufferSize = InBufferSize;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ThrowIfFailed(ParentDevice->GetDevice()->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));
	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin)));
	memcpy(pCbvDataBegin, pDataFrom, DataSize);
	NAME_D3D12_OBJECT(ConstantBuffer);
}

void FD3DConstantBuffer::UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize)
{
	memcpy(pCbvDataBegin, pDataUpdate, DataSize);
}

FD3DShaderResource::FD3DShaderResource()
{
	ResourceName = "ShaderResource";
}

FD3DShaderResource::FD3DShaderResource(FD3D12Device* InDevice, std::wstring TextureName)
:FD3DResource(InDevice)
{
	ResourceName = "ShaderResource";
	ID3D12GraphicsCommandList* CommandList = ParentDevice->GetCommandListManager()->GetDefaultCommandList();
	DirectX::CreateDDSTextureFromFile12(ParentDevice->GetDevice(), CommandList, TextureName.c_str(), TextureResource, TextureResourceUpload);
	NAME_D3D12_OBJECT(TextureResource);
	NAME_D3D12_OBJECT(TextureResourceUpload);
}

FD3DShaderResource::~FD3DShaderResource()
{

}

void FD3DShaderResource::Destroy()
{
	TextureResource.Reset();
	TextureResourceUpload.Reset();
}

void FD3DShaderResource::Initialize()
{
	CbvSrvUavDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}







FD3DView::FD3DView()
:FRHIView()
,ParentDevice(nullptr)
{
}

FD3DView::FD3DView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
: FRHIView(InLocation, InBytes, InSizeBytes)
, ParentDevice(InDevice)
{
}

FD3DView::~FD3DView()
{

}


FD3DConstantBufferView::FD3DConstantBufferView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
:FD3DView(InDevice, InLocation, InBytes, InSizeBytes)
{
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap = ParentDevice->GetParentAdapter()->CbvSrvHeap;
	D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
	CbvDesc.BufferLocation = InLocation;
	CbvDesc.SizeInBytes = InSizeBytes;
	ParentDevice->GetDevice()->CreateConstantBufferView(&CbvDesc, CbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
}

FD3DConstantBufferView::~FD3DConstantBufferView()
{
	
}


FD3DShaderResourceView::FD3DShaderResourceView(FD3D12Device* InDevice, FD3DShaderResource* InShaderResource)
{
	ParentDevice = InDevice;
	ShaderResource = InShaderResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = ShaderResource->GetD3DShaderResource()->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = ShaderResource->GetD3DShaderResource()->GetDesc().MipLevels;

	UINT CbvSrvUavDescriptorSize = ShaderResource->GetSrvDescripterSize();
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap = ParentDevice->GetParentAdapter()->CbvSrvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvHandle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), 1, CbvSrvUavDescriptorSize);
	ParentDevice->GetDevice()->CreateShaderResourceView(ShaderResource->GetD3DShaderResource(), &srvDesc, CbvSrvHandle);
}

FD3DShaderResourceView::~FD3DShaderResourceView()
{

}

void FD3DShaderResourceView::Clear()
{
	if (ShaderResource)
	{
		ShaderResource->Destroy();
		delete ShaderResource;
		ShaderResource = nullptr;
	}
}



/// ///////////////////////////////////
#include "FRenderResource.h"

FD3D12IndexBuffer::FD3D12IndexBuffer()
:FIndexBuffer(E_RESOURCE_TYPE::TYPE_INDEX_BUFFER)
{

}

FD3D12IndexBuffer::~FD3D12IndexBuffer()
{
	IndexBuffer.Reset();
	IndexBufferUpload.Reset();
}

void FD3D12IndexBuffer::Destroy()
{
	IndexBuffer.Reset(); 
	IndexBufferUpload.Reset();
}

void FD3D12IndexBuffer::InitGPUIndexBuffer(FD3D12Adapter* Adapter)
{
	ID3D12GraphicsCommandList* CommandList = Adapter->GetDevice()->GetCommandListManager()->GetDefaultCommandList();
	CreateBuffer(Adapter->GetD3DDevice(), CommandList, GetIndicesData(), GetIndicesDataSize(), IndexBuffer, IndexBufferUpload);
	NAME_D3D12_OBJECT(IndexBuffer);
	NAME_D3D12_OBJECT(IndexBufferUpload);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = GetIndicesType() == E_INDEX_TYPE::TYPE_UINT_16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = GetIndicesDataSize();
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
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
}

void FD3D12VertexBuffer::Destroy()
{
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
}

void FD3D12VertexBuffer::Initialize()
{

}

void FD3D12VertexBuffer::InitGPUVertexBufferView(FD3D12Adapter* Adapter)
{
	ID3D12GraphicsCommandList* CommandList = Adapter->GetDevice()->GetCommandListManager()->GetDefaultCommandList();
	std::vector<FVertex_PositionTex0> TriangleVertices;
	GetPositionTex0Input(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(FVertex_PositionTex0));
	const UINT StrideInByte = sizeof(FVertex_PositionTex0);
	CreateBuffer(Adapter->GetD3DDevice(), CommandList, TriangleVertices.data(), VertexBufferSize, VertexBuffer, VertexUploadBuffer);
	NAME_D3D12_OBJECT(VertexBuffer);
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = StrideInByte;
	VertexBufferView.SizeInBytes = VertexBufferSize;
}

