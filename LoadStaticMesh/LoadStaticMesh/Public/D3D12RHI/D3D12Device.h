#pragma once
#include "D3D12Adapter.h"

class D3D12Device
{
public:
	D3D12Device();
	D3D12Device(D3D12Adapter* InAdapter);
	virtual ~D3D12Device();

	void Initialize();
private:
	D3D12Adapter* ParentAdapter;
};