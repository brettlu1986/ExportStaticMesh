#pragma once
#include "FD3D12Adapter.h"
#include "FD3D12CommandListManager.h"
#include "FD3D12ResourceManager.h"

class FD3D12Device
{
public:
	FD3D12Device();
	FD3D12Device(FD3D12Adapter* InAdapter);
	virtual ~FD3D12Device();

	void Initialize();
	void ShutDown();

	FD3D12Adapter* GetParentAdapter() const {
		return ParentAdapter;
	}

	FD3D12CommandListManager* GetCommandListManager() const {
		return CommandListManager;
	}

	FD3D12ResourceManager* GetResourceManager() const 
	{
		return ResourceManager;
	}

	ID3D12CommandQueue* GetD3DCommandQueue() const 
	{
		return GetCommandListManager()->GetD3DCommandQueue();
	}

	ID3D12Device* GetDevice();
	
private:

	FD3D12Adapter* ParentAdapter;
	FD3D12CommandListManager* CommandListManager;
	FD3D12ResourceManager* ResourceManager;
};