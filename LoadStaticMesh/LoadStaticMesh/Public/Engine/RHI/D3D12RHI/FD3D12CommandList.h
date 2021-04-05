#pragma once
#include "stdafx.h"
#include "FRHIDef.h"

using namespace Microsoft::WRL;

class FD3D12Device;
class FD3D12CommandListAllocator;
class FD3D12CommandList : public FRHICommandList
{
public:
	FD3D12CommandList(FD3D12Device* ParentDevice, FD3D12CommandListAllocator* CommandAllocator,UINT InCommandListIndex);

	~FD3D12CommandList();
	ID3D12GraphicsCommandList* GetD3DCommandList() 
	{
		return CommandList.Get();
	}

	virtual void Clear() override;
	virtual void Close() override;
	virtual void Reset() override;
	virtual void Excute() override;
	virtual void SetViewPort() override;
	virtual void SetScissorRect() override;
	virtual void SetGraphicRootDescripterTable() override;

private: 
	
	FD3D12Device* ParentDevice;
	FD3D12CommandListAllocator* CurrentCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
};
