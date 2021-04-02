
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

D3DRenderTarget::D3DRenderTarget()
:TargetCount(0)
{
	ResourceName = "RenderTarget";
}

D3DRenderTarget::D3DRenderTarget(FD3D12Device* InDevice)
:FD3DResource(InDevice)
,TargetCount(0)
{
	ResourceName = "RenderTarget";
}

D3DRenderTarget::~D3DRenderTarget()
{

}

void D3DRenderTarget::Initialize()
{
	RtvDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE D3DRenderTarget::GetRtvHandle(UINT TargetIndex)
{
	ComPtr<ID3D12DescriptorHeap> RtvHeap = ParentDevice->GetParentAdapter()->RtvHeap;
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetIndex, RtvDescriptorSize);
}

void D3DRenderTarget::SetRenderTargetCount(UINT Count)
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

void D3DRenderTarget::Destroy()
{
	for(size_t i = 0; i < RenderTargets.size(); ++i)
	{
		RenderTargets[i].Reset();
	}
	RenderTargets.clear();
}


D3DConstantBuffer::D3DConstantBuffer()
{
	ResourceName = "ConstantBuffer";
}

D3DConstantBuffer::D3DConstantBuffer(FD3D12Device* InDevice)
	:FD3DResource(InDevice)
	,pCbvDataBegin(nullptr)
{	
	ResourceName = "ConstantBuffer";
}

D3DConstantBuffer::~D3DConstantBuffer()
{

}

void D3DConstantBuffer::Destroy() 
{
	ConstantBuffer->Unmap(0, nullptr);
	ConstantBuffer.Reset();
}

void D3DConstantBuffer::Initialize() 
{
	CbvSrvUavDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3DConstantBuffer::SetConstantBufferInfo(UINT InBufferSize, void* pDataFrom, UINT DataSize)
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

void D3DConstantBuffer::UpdateConstantBufferInfo(void* pDataUpdate, UINT DataSize)
{
	memcpy(pCbvDataBegin, pDataUpdate, DataSize);
}

// dsv buffer
D3DDepthStencilBuffer::D3DDepthStencilBuffer()
{
	ResourceName = "DepthStencilBuffer";
}

D3DDepthStencilBuffer::D3DDepthStencilBuffer(FD3D12Device* InDevice)
	:FD3DResource(InDevice)
{
	ResourceName = "DepthStencilBuffer";
}

D3DDepthStencilBuffer::~D3DDepthStencilBuffer()
{

}

void D3DDepthStencilBuffer::Destroy()
{
	DepthStencilBuffer.Reset();
}

void D3DDepthStencilBuffer::Initialize()
{
	DsvDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void D3DDepthStencilBuffer::SetDepthStencilSize(UINT Width, UINT Height)
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


D3DVertexBuffer::D3DVertexBuffer()
{
	ResourceName = "VertexBuffer";
}

D3DVertexBuffer::D3DVertexBuffer(FD3D12Device* InDevice, const void* InitData,UINT StrideInByte, UINT ByteSize)
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

D3DVertexBuffer::~D3DVertexBuffer()
{
	
}

void D3DVertexBuffer::Destroy()
{
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
}

void D3DVertexBuffer::Initialize() 
{
	
}


D3DIndexBuffer::D3DIndexBuffer()
{
	ResourceName = "IndexBuffer";
}

D3DIndexBuffer::D3DIndexBuffer(FD3D12Device* InDevice, const void* InitData, UINT ByteSize, UINT IndicesCount, bool bUseHalfInt32)
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

D3DIndexBuffer::~D3DIndexBuffer()
{

}

void D3DIndexBuffer::Destroy() 
{
	IndexBuffer.Reset();
	IndexBufferUpload.Reset();
}

void D3DIndexBuffer::Initialize()
{

}


D3DShaderResource::D3DShaderResource()
{
	ResourceName = "ShaderResource";
}

D3DShaderResource::D3DShaderResource(FD3D12Device* InDevice, std::wstring TextureName)
:FD3DResource(InDevice)
{
	ResourceName = "ShaderResource";
	ID3D12GraphicsCommandList* CommandList = ParentDevice->GetCommandListManager()->GetDefaultCommandList();
	DirectX::CreateDDSTextureFromFile12(ParentDevice->GetDevice(), CommandList, TextureName.c_str(), TextureResource, TextureResourceUpload);
	NAME_D3D12_OBJECT(TextureResource);
	NAME_D3D12_OBJECT(TextureResourceUpload);
}

D3DShaderResource::~D3DShaderResource()
{

}

void D3DShaderResource::Destroy()
{
	TextureResource.Reset();
	TextureResourceUpload.Reset();
}

void D3DShaderResource::Initialize()
{
	CbvSrvUavDescriptorSize = ParentDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}







D3DView::D3DView()
:FRHIView()
,ParentDevice(nullptr)
{
}

D3DView::D3DView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
: FRHIView(InLocation, InBytes, InSizeBytes)
, ParentDevice(InDevice)
{
}

D3DView::~D3DView()
{

}


D3DConstantBufferView::D3DConstantBufferView(FD3D12Device* InDevice, UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
:D3DView(InDevice, InLocation, InBytes, InSizeBytes)
{
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap = ParentDevice->GetParentAdapter()->CbvSrvHeap;
	D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
	CbvDesc.BufferLocation = InLocation;
	CbvDesc.SizeInBytes = InSizeBytes;
	ParentDevice->GetDevice()->CreateConstantBufferView(&CbvDesc, CbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
}

D3DConstantBufferView::~D3DConstantBufferView()
{
	
}


D3DDepthStencilView::D3DDepthStencilView(FD3D12Device* InDevice, ID3D12Resource* DepthStencilBuffer)
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

D3DDepthStencilView::~D3DDepthStencilView()
{

}


D3DVertexBufferView::D3DVertexBufferView(FD3D12Device* InDevice, D3DVertexBuffer* InVertexBuffer)
{
	ParentDevice = InDevice;
	VertexBuffer = InVertexBuffer;
	VertexBufferView.BufferLocation = VertexBuffer->GetD3DVertexBuffer()->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = VertexBuffer->GetStrideInByte();
	VertexBufferView.SizeInBytes = VertexBuffer->GetByteSize();
}

D3DVertexBufferView::~D3DVertexBufferView()
{

}

void D3DVertexBufferView::Clear()
{
	if (VertexBuffer)
	{
		VertexBuffer->Destroy();
		delete VertexBuffer;
		VertexBuffer = nullptr;
	}
}


D3DIndexBufferView::D3DIndexBufferView(FD3D12Device* InDevice, D3DIndexBuffer* InIndexBuffer)
{
	ParentDevice = InDevice;
	IndexBuffer = InIndexBuffer;

	IndexBufferView.BufferLocation = IndexBuffer->GetD3DIndexBuffer()->GetGPUVirtualAddress();
	IndexBufferView.Format = IndexBuffer->UseHalfInt32() ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndexBuffer->GetDataSize();
}

D3DIndexBufferView::~D3DIndexBufferView()
{

}

void D3DIndexBufferView::Clear()
{
	if (IndexBuffer)
	{
		IndexBuffer->Destroy();
		delete IndexBuffer;
		IndexBuffer = nullptr;
	}
}

D3DShaderResourceView::D3DShaderResourceView(FD3D12Device* InDevice, D3DShaderResource* InShaderResource)
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

D3DShaderResourceView::~D3DShaderResourceView()
{

}

void D3DShaderResourceView::Clear()
{
	if (ShaderResource)
	{
		ShaderResource->Destroy();
		delete ShaderResource;
		ShaderResource = nullptr;
	}
}