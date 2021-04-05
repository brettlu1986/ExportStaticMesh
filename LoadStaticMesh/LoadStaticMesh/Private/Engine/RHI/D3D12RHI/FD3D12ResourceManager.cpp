

#include "FD3D12ResourceManager.h"
#include "FD3D12Resource.h"
#include "FDefine.h"

FD3D12ResourceManager::FD3D12ResourceManager(FD3D12Device* InParent)
:ParentDevice(InParent)
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
	for (size_t i = 0; i < ShaderResourceViews.size(); ++i)
	{
		ShaderResourceViews[i]->Clear();
	}
	ShaderResourceViews.clear();
}

FRHIView* FD3D12ResourceManager::CreateShaderResourceView(std::wstring TextureName)
{
	FD3DShaderResource* ShaderResource = new FD3DShaderResource(ParentDevice, TextureName);
	ShaderResource->Initialize();
	FRHIView* View = new FD3DShaderResourceView(ParentDevice, ShaderResource);
	ShaderResourceViews.push_back(View);
	return View;
}