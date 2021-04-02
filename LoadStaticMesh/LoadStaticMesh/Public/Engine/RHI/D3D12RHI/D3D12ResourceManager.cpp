

#include "D3D12ResourceManager.h"
#include "D3D12Resource.h"

D3D12ResourceManager::D3D12ResourceManager(D3D12Device* InParent)
:ParentDevice(InParent)
, RenderTarget(nullptr)
, ConstantBuffer(nullptr)
{

}

D3D12ResourceManager::~D3D12ResourceManager()
{

}

void D3D12ResourceManager::Initialize()
{

}

void D3D12ResourceManager::Clear()
{
	if(RenderTarget)
	{
		RenderTarget->Destroy();
		delete RenderTarget;
		RenderTarget = nullptr;
	}

	if(ConstantBuffer)
	{	
		ConstantBuffer->Destroy();
		delete ConstantBuffer;
		ConstantBuffer = nullptr;
	}

	if(DepthStencilBuffer)
	{
		DepthStencilBuffer->Destroy();
		delete DepthStencilBuffer;
		DepthStencilBuffer = nullptr;
	}

	for (size_t i = 0; i < VertexBufferViews.size(); ++i)
	{
		VertexBufferViews[i]->Clear();
	}
	VertexBufferViews.clear();

	for (size_t i = 0; i < IndexBufferViews.size(); ++i)
	{
		IndexBufferViews[i]->Clear();
	}
	IndexBufferViews.clear();

	for (size_t i = 0; i < ShaderResourceViews.size(); ++i)
	{
		ShaderResourceViews[i]->Clear();
	}
	ShaderResourceViews.clear();
	
}

void D3D12ResourceManager::CreateRenderTarget(UINT TargetCount)
{
	if(!RenderTarget)
	{
		RenderTarget = new D3DRenderTarget(ParentDevice);
		RenderTarget->Initialize();
		RenderTarget->SetRenderTargetCount(TargetCount);
	}
}

void D3D12ResourceManager::CreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
{
	if(!ConstantBuffer)
	{
		ConstantBuffer = new D3DConstantBuffer(ParentDevice);
		ConstantBuffer->Initialize();
		ConstantBuffer->SetConstantBufferInfo(BufferSize, pDataFrom, DataSize);
	}

	CreateConstantBufferView();
}

void D3D12ResourceManager::UpdateConstantBufferData(void* pUpdateData, UINT DataSize)
{
	if (ConstantBuffer)
	{
		ConstantBuffer->UpdateConstantBufferInfo(pUpdateData, DataSize);
	}
}

void D3D12ResourceManager::CreateDepthStencilBuffer(UINT Width, UINT Height)
{
	if(!DepthStencilBuffer)
	{
		DepthStencilBuffer = new D3DDepthStencilBuffer(ParentDevice);
		DepthStencilBuffer->Initialize();
		DepthStencilBuffer->SetDepthStencilSize(Width, Height);
	}
	CreateDepthStencilView();
}

void D3D12ResourceManager::CreateConstantBufferView()
{
	if(ConstantBuffer)
	{
		ID3D12Resource* D3dConstantBuffer = ConstantBuffer->GetD3DConstantBuffer();
		ConstantBufferView = new D3DConstantBufferView(ParentDevice, D3dConstantBuffer->GetGPUVirtualAddress(), 0, ConstantBuffer->GetBufferSize());
	}
}

void D3D12ResourceManager::CreateDepthStencilView()
{
	if (DepthStencilBuffer)
	{
		ID3D12Resource* D3dDepthStencilBuffer = DepthStencilBuffer->GetD3DDepthStencilBuffer();
		DepthStencilView = new D3DDepthStencilView(ParentDevice, D3dDepthStencilBuffer);
	}
}

RHIView* D3D12ResourceManager::CreateVertexBufferView(const void* InitData, UINT StrideInByte, UINT DataSize)
{
	D3DVertexBuffer* VertexBuffer = new D3DVertexBuffer(ParentDevice, InitData, StrideInByte, DataSize);
	VertexBuffer->Initialize();
	RHIView* View = new D3DVertexBufferView(ParentDevice, VertexBuffer);
	VertexBufferViews.push_back(View);
	return View;
}

RHIView* D3D12ResourceManager::CreateIndexBufferView(const void* InitData, UINT DataSize, UINT IndicesCount, bool bUseHalfInt32)
{
	D3DIndexBuffer* IndexBuffer = new D3DIndexBuffer(ParentDevice, InitData, DataSize, IndicesCount, bUseHalfInt32);
	IndexBuffer->Initialize();
	RHIView* View = new D3DIndexBufferView(ParentDevice, IndexBuffer);
	IndexBufferViews.push_back(View);
	return View;
}

RHIView* D3D12ResourceManager::CreateShaderResourceView(std::wstring TextureName)
{
	D3DShaderResource* ShaderResource = new D3DShaderResource(ParentDevice, TextureName);
	ShaderResource->Initialize();
	RHIView* View = new D3DShaderResourceView(ParentDevice, ShaderResource);
	ShaderResourceViews.push_back(View);
	return View;
}