
#include "D3D12Device.h"
#include "D3D12Helper.h"
#include "D3D12CommandListManager.h"

#define COMMAND_LIST_NUM 3 // pre/ render/ post

D3D12Device::D3D12Device()
{

}

D3D12Device::D3D12Device(D3D12Adapter* InAdapter)
:ParentAdapter(InAdapter)
,CommandListManager(nullptr)
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

	ID3D12Device* Device = GetDevice();
	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}