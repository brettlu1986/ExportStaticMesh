

#include "FD3D12ResourceManager.h"
#include "FD3D12Resource.h"
#include "FDefine.h"

FD3D12ResourceManager::FD3D12ResourceManager(FD3D12Device* InParent)
:ParentDevice(InParent)
, ConstantBuffer(nullptr)
{

}

FD3D12ResourceManager::~FD3D12ResourceManager()
{

}

void FD3D12ResourceManager::Initialize()
{

}

void FD3D12ResourceManager::Clear()
{
	if(ConstantBuffer)
	{	
		ConstantBuffer->Destroy();
		delete ConstantBuffer;
		ConstantBuffer = nullptr;
	}


	for (size_t i = 0; i < ShaderResourceViews.size(); ++i)
	{
		ShaderResourceViews[i]->Clear();
	}
	ShaderResourceViews.clear();
	
}


void FD3D12ResourceManager::CreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
{
	if(!ConstantBuffer)
	{
		ConstantBuffer = new FD3DConstantBuffer(ParentDevice);
		ConstantBuffer->Initialize();
		ConstantBuffer->SetConstantBufferInfo(BufferSize, pDataFrom, DataSize);
	}

	CreateConstantBufferView();
}

void FD3D12ResourceManager::UpdateConstantBufferData(void* pUpdateData, UINT DataSize)
{
	if (ConstantBuffer)
	{
		ConstantBuffer->UpdateConstantBufferInfo(pUpdateData, DataSize);
	}
}

void FD3D12ResourceManager::CreateConstantBufferView()
{
	if(ConstantBuffer)
	{
		ID3D12Resource* FD3DConstantBuffer = ConstantBuffer->GetD3DConstantBuffer();
		ConstantBufferView = new FD3DConstantBufferView(ParentDevice, FD3DConstantBuffer->GetGPUVirtualAddress(), 0, ConstantBuffer->GetBufferSize());
	}
}


FRHIView* FD3D12ResourceManager::CreateShaderResourceView(std::wstring TextureName)
{
	FD3DShaderResource* ShaderResource = new FD3DShaderResource(ParentDevice, TextureName);
	ShaderResource->Initialize();
	FRHIView* View = new FD3DShaderResourceView(ParentDevice, ShaderResource);
	ShaderResourceViews.push_back(View);
	return View;
}