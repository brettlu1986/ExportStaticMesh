
#include "FD3D12Device.h"
#include "FD3D12Helper.h"
#include "FD3D12CommandListManager.h"

#define COMMAND_LIST_NUM 1 // pre/ render/ post

FD3D12Device::FD3D12Device()
{

}

FD3D12Device::FD3D12Device(FD3D12Adapter* InAdapter)
:ParentAdapter(InAdapter)
,CommandListManager(nullptr)
{

}

FD3D12Device::~FD3D12Device()
{

}

void FD3D12Device::ShutDown()
{
	if(CommandListManager)
	{
		CommandListManager->Clear();
		CommandListManager = nullptr;
	}

}

ID3D12Device* FD3D12Device::GetDevice()
{
	return GetParentAdapter()->GetD3DDevice();
}

void FD3D12Device::Initialize()
{
	CommandListManager = new FD3D12CommandListManager(this);
	CommandListManager->Initialize();
	CommandListManager->CreateCommandLists(COMMAND_LIST_NUM);
}