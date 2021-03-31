#pragma once
#include "D3D12Adapter.h"
#include "D3D12CommandListManager.h"

class D3D12Device
{
public:
	D3D12Device();
	D3D12Device(D3D12Adapter* InAdapter);
	virtual ~D3D12Device();

	void Initialize();
	void ShutDown();

	D3D12Adapter* GetParentAdapter() const {
		return ParentAdapter;
	}

	D3D12CommandListManager* GetCommandListManager() const {
		return CommandListManager;
	}

	ID3D12CommandQueue* GetD3DCommandQueue() const 
	{
		return GetCommandListManager()->GetD3DCommandQueue();
	}

	ID3D12Device* GetDevice();
	
private:

	D3D12Adapter* ParentAdapter;
	D3D12CommandListManager* CommandListManager;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	UINT CbvSrvUavDescriptorSize;
};