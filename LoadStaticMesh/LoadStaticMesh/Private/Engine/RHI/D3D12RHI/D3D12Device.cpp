
#include "D3D12Device.h"
#include "D3D12Helper.h"
#include "D3D12CommandListManager.h"
#include "D3D12ResourceManager.h"

#define COMMAND_LIST_NUM 1 // pre/ render/ post

D3D12Device::D3D12Device()
{

}

D3D12Device::D3D12Device(D3D12Adapter* InAdapter)
:ParentAdapter(InAdapter)
,CommandListManager(nullptr)
,ResourceManager(nullptr)
{

}

D3D12Device::~D3D12Device()
{

}

void D3D12Device::ShutDown()
{
	if(CommandListManager)
	{
		CommandListManager->Clear();
		CommandListManager = nullptr;
	}

	if (ResourceManager)
	{
		ResourceManager->Clear();
		ResourceManager = nullptr;
	}
}

ID3D12Device* D3D12Device::GetDevice()
{
	return GetParentAdapter()->GetD3DDevice();
}

void D3D12Device::Initialize()
{
	CommandListManager = new D3D12CommandListManager(this);
	CommandListManager->Initialize();
	CommandListManager->CreateCommandLists(COMMAND_LIST_NUM);

	ResourceManager = new D3D12ResourceManager(this);
	ResourceManager->Initialize();
}