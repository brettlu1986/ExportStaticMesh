
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

FD3DRenderTarget::FD3DRenderTarget()
:TargetCount(0)
{
	ResourceName = "RenderTarget";
}

FD3DRenderTarget::FD3DRenderTarget(FD3D12Device* InDevice)
:FD3DResource(InDevice)
,TargetCount(0)
{
	ResourceName = "RenderTarget";
}

FD3DRenderTarget::~FD3DRenderTarget()
{

}

void FD3DRenderTarget::Initialize()
{
	RtvDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE FD3DRenderTarget::GetRtvHandle(UINT TargetIndex)
{
	ComPtr<ID3D12DescriptorHeap> RtvHeap = ParentDevice->GetParentAdapter()->RtvHeap;
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetIndex, RtvDescriptorSize);
}

void FD3DRenderTarget::SetRenderTargetCount(UINT Count)
{
	TargetCount = Count;
	ComPtr<ID3D12DescriptorHeap> RtvHeap = ParentDevice->GetParentAdapter()->RtvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	IDXGISwapChain* SwapChain = ParentDevice->GetParentAdapter()->GetSwapChain();
	// Create a RTV for each frame.
	for (UINT n = 0; n < Count; n++)
	{
		ComPtr<ID3D12Resource> Target;
		ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&Target)));
		ParentDevice->GetDevice()->CreateRenderTargetView(Target.Get(), nullptr, RtvHandle);
		RenderTargets.push_back(Target);
		RtvHandle.Offset(1, RtvDescriptorSize);
	}
}

void FD3DRenderTarget::Destroy()
{
	for(size_t i = 0; i < RenderTargets.size(); ++i)
	{
		RenderTargets[i].Reset();
	}
	RenderTargets.clear();
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

// dsv buffer
FD3DDepthStencilBuffer::FD3DDepthStencilBuffer()
{
	ResourceName = "DepthStencilBuffer";
}

FD3DDepthStencilBuffer::FD3DDepthStencilBuffer(FD3D12Device* InDevice)
	:FD3DResource(InDevice)
{
	ResourceName = "DepthStencilBuffer";
}

FD3DDepthStencilBuffer::~FD3DDepthStencilBuffer()
{

}

void FD3DDepthStencilBuffer::Destroy()
{
	DepthStencilBuffer.Reset();
}

void FD3DDepthStencilBuffer::Initialize()
{
	DsvDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void FD3DDepthStencilBuffer::SetDepthStencilSize(UINT Width, UINT Height)
{
	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES ProDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Width, Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	ThrowIfFailed(ParentDevice->GetDevice()->CreateCommittedResource(
		&ProDefault,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&DepthStencilBuffer)
	));
	NAME_D3D12_OBJECT(DepthStencilBuffer);
}


FD3DVertexBuffer::FD3DVertexBuffer()
{
	ResourceName = "VertexBuffer";
}

FD3DVertexBuffer::FD3DVertexBuffer(FD3D12Device* InDevice, const void* InitData,UINT StrideInByte, UINT ByteSize)
	:FD3DResource(InDevice)
	,StrideInByte(StrideInByte)
	,ByteSize(ByteSize)
{
	ResourceName = "VertexBuffer";
	
	ID3D12GraphicsCommandList* CommandList = ParentDevice->GetCommandListManager()->GetDefaultCommandList();
	CreateBuffer(ParentDevice->GetDevice(), CommandList, InitData, ByteSize, VertexBuffer, VertexUploadBuffer);
	NAME_D3D12_OBJECT(VertexBuffer);
	NAME_D3D12_OBJECT(VertexUploadBuffer);
}

FD3DVertexBuffer::~FD3DVertexBuffer()
{
	
}

void FD3DVertexBuffer::Destroy()
{
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
}

void FD3DVertexBuffer::Initialize() 
{
	
}


FD3DIndexBuffer::FD3DIndexBuffer()
{
	ResourceName = "IndexBuffer";
}

FD3DIndexBuffer::FD3DIndexBuffer(FD3D12Device* InDevice, const void* InitData, UINT ByteSize, UINT IndicesCount, bool bUseHalfInt32)
:FD3DResource(InDevice)
, bUseHalfInt32(bUseHalfInt32)
, ByteSize(ByteSize)
, IndicesCount(IndicesCount)
{
	ResourceName = "IndexBuffer";
	ID3D12GraphicsCommandList* CommandList = ParentDevice->GetCommandListManager()->GetDefaultCommandList();
	CreateBuffer(ParentDevice->GetDevice(), CommandList, InitData, ByteSize, IndexBuffer, IndexBufferUpload);
	NAME_D3D12_OBJECT(IndexBuffer);
	NAME_D3D12_OBJECT(IndexBufferUpload);
}

FD3DIndexBuffer::~FD3DIndexBuffer()
{

}

void FD3DIndexBuffer::Destroy() 
{
	IndexBuffer.Reset();
	IndexBufferUpload.Reset();
}

void FD3DIndexBuffer::Initialize()
{

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


FD3DDepthStencilView::FD3DDepthStencilView(FD3D12Device* InDevice, ID3D12Resource* DepthStencilBuffer)
{
	ParentDevice = InDevice;

	ComPtr<ID3D12DescriptorHeap> DsvHeap = ParentDevice->GetParentAdapter()->DsvHeap;
	// Create the depth/stencil buffer and view.
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
	ParentDevice->GetDevice()->CreateDepthStencilView(DepthStencilBuffer, &depthStencilDesc, DsvHeap->GetCPUDescriptorHandleForHeapStart());
}

FD3DDepthStencilView::~FD3DDepthStencilView()
{

}


FD3DVertexBufferView::FD3DVertexBufferView(FD3D12Device* InDevice, FD3DVertexBuffer* InVertexBuffer)
{
	ParentDevice = InDevice;
	VertexBuffer = InVertexBuffer;
	VertexBufferView.BufferLocation = VertexBuffer->GetD3DVertexBuffer()->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexBuffer->GetStrideInByte();
	VertexBufferView.SizeInBytes = VertexBuffer->GetByteSize();
}

FD3DVertexBufferView::~FD3DVertexBufferView()
{

}

void FD3DVertexBufferView::Clear()
{
	if (VertexBuffer)
	{
		VertexBuffer->Destroy();
		delete VertexBuffer;
		VertexBuffer = nullptr;
	}
}


FD3DIndexBufferView::FD3DIndexBufferView(FD3D12Device* InDevice, FD3DIndexBuffer* InIndexBuffer)
{
	ParentDevice = InDevice;
	IndexBuffer = InIndexBuffer;

	IndexBufferView.BufferLocation = IndexBuffer->GetD3DIndexBuffer()->GetGPUVirtualAddress();
	IndexBufferView.Format = IndexBuffer->UseHalfInt32() ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndexBuffer->GetDataSize();
}

FD3DIndexBufferView::~FD3DIndexBufferView()
{

}

void FD3DIndexBufferView::Clear()
{
	if (IndexBuffer)
	{
		IndexBuffer->Destroy();
		delete IndexBuffer;
		IndexBuffer = nullptr;
	}
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