

#include "D3D12ResourceManager.h"
#include "D3D12Resource.h"

D3D12ResourceManager::D3D12ResourceManager(D3D12Device* InParent)
:ParentDevice(InParent)
, RenderTarget(nullptr)
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
}

void D3D12ResourceManager::CreateRenderTarget(UINT TargetCount)
{
	RenderTarget = new D3DRenderTarget(ParentDevice);
	RenderTarget->Initialize();
	RenderTarget->SetRenderTargetCount(TargetCount);
}

void D3D12ResourceManager::CreateConstantBuffer(UINT BufferSize, void* pDataFrom, void** ppDataMap)
{
	ConstantBuffer = new D3DConstantBuffer(ParentDevice);
	ConstantBuffer->Initialize();
	ConstantBuffer->SetConstantBufferInfo(BufferSize, pDataFrom, ppDataMap);
}